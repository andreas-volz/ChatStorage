/*
 * ChatFormatAStreamParser.cpp
 *
 *      Author: Andreas Volz
 */

// project
#include <importer/ChatFormatAStreamParser.h>
#include "ImportUser.h"
#include "ImportMessage.h"
#include "ImportMedia.h"
#include "ChatImportContext.h"
#include "common/StringUtil.h"
#include "chatstorage/ChatSource.h"
#include "common/Logger.h"

// system
#include <iostream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <string>
#include <memory>

using namespace std;

static Logger logger = Logger("ChatStorage.ChatFormatAStreamParser");

// regex_datetime:
// This regex matches a complete date-time string as exported by for Android/iOS.
// This is the documentation for the regex "superset". It is split and matched with alternative combinations below.
//
// Components:
//
// 1. \d{1,2}         -> Day or month (1 or 2 digits, e.g., 1..31 or 1..12)
// 2. \. (or./)       -> Literal dot '.' or '/' separating day and month
// 3. \d{1,2}         -> Month or day (depending on locale, 1 or 2 digits)
// 4. \. (or ./)      -> Literal dot '.' or '/' separating month and year
// 5. \d{2,4}         -> Year (2 or 4 digits, e.g., 23 or 2023)
// 6. ,               -> Literal comma separating date and time
// 7. \d{1,2}:\d{2}   -> Hours and minutes (24h format: HH:MM and 12h format H:MM)
// 8. (?::\d{2})?     -> Optional seconds (e.g., :08)
// 9. (?:AM|PM|am|pm) -> non-capturing 12h time format, note that %m (month) and %d (day) is interchanged in 12h mode for get_time()
//
// Notes:
// - This regex is intentionally flexible to cover multiple locales and optional seconds.
// - It can be used as a building block for detecting timestamps in chat export parsing.
// - No capturing groups are defined here; the entire match can be used directly.
// @formatter:off
static const std::vector<MessageDateTimeCore> message_regex_datetime_cores = {
  {R"date((\d{1,2}\.\d{1,2}\.\d{2,4}, \d{1,2}:\d{2}(?::\d{2})?))date", "%d.%m.%y, %H:%M:%S"}, /* DE */
  {R"date((\d{1,2}\/\d{1,2}\/\d{2,4}, \d{1,2}:\d{2}(?::\d{2})?))date", "%d/%m/%y, %H:%M:%S"}, /* EN 24h */
  {R"date((\d{1,2}\/\d{1,2}\/\d{2,4}, \d{1,2}:\d{2} (?:AM|PM|am|pm)))date", "%m/%d/%y, %I:%M %p"}, /* EN 12h */
  {R"date((\d{1,2}\/\d{1,2}\/\d{2,4}, \d{1,2}:\d{2}:\d{2} (?:AM|PM|am|pm)))date", "%m/%d/%y, %I:%M:%S %p"} /* EN 12h + seconds */
};

// Android and iOS parentheses
static const std::vector<MessagePlatformWrapper> message_regex_platform_wrappers = {
  { "^", " - (.*)" },     // Android
  { "^\\[", "\\] (.*)" }  // iOS
};

/**
 * Parse a chat message line with an optional sender name followed by a payload.
 *
 * Structure:
 * ^                     - Start of line
 * (?: ... )?            - Optional non-capturing group for the sender part
 *   ([^:]+)             - Capturing group 1: sender name (everything up to the first ':')
 *   :                   - Literal colon separating name and message
 * \s*                   - Optional whitespace after the colon
 * (.*)                  - Capturing group 2: message payload (rest of the line)
 * $                     - End of line
 *
  * Notes:
 * - The non-capturing group (?:...) ensures the regex structure does not add
 *   an extra match group.
 * - Unicode control / bidi marks (e.g. LRM, RLM, ZWSP) that may appear directly
 *   after ':' are intentionally matched as part of the payload.
 *   These should be removed in a later payload-cleanup step.
 */
static const std::string message_payload_regex = R"(^(?:([^:]+):)?\s*(.*)$)";

// @formatter:on

bool ChatFormatAStreamParser::parse(std::istream &in_stream, const std::string &chat_name, ChatImportContext &out_ctx)
{
  std::deque<ImportUser> import_users;
  ImportUser *found_user = nullptr;
  ImportUser *system_user = nullptr;

  std::deque<ImportMessage> import_messages;
  ImportMessage *found_message = nullptr;

  std::deque<ImportMedia> import_media_container;

  std::string line;
  int line_count = 0;
  int imported_line_count = 0;
  int user_count = 0;

  while (std::getline(in_stream, line))
  {
    // at very first normalize all strange unicode whitespace
    // those are very bad for structured parsing in date/time.
    // if I later find a case where it's needed for special display that this has to get more work...
    line = StringUtil::normalize_whitespace(line);

    LOG4CXX_TRACE(logger, "parse line: " + line);

    if (line_count == 0)
    {
      // the first line is used to identify the date format by trying out all available ones
      bool regex_found = identifyDateFormat(line);
      if (!regex_found)
      {
        LOG4CXX_ERROR(logger, "File format not supported! No matching RegEx found!");
        return false;
      }
    }

    // regex_datetime
    std::smatch match;

    if (regex_search(line, match, mMessageDateFormat->full_regex))
    {
      // size=3 is a message with timestamp
      // 0: full match
      // 1: date+time part
      // 2: optional name + payload
      if (match.size() == 3)
      {
        const string &datetime_str = match[1];
        const string &line_payload_user = match[2];

        //cout << "line_payload_user:" << line_payload_user << endl;

        std::smatch payload_match;
        if (regex_search(line_payload_user, payload_match, mMessageDateFormat->payload_regex))
        {
          string payload;
          string name_str;

          // only payload message (most likely a system message)
          if (payload_match.size() == 2)
          {
            payload = payload_match[1];
          }
          // user + payload message match (normal user message)
          else if (payload_match.size() == 3)
          {
            name_str = payload_match[1];
            payload = payload_match[2];
          }

          std::tm tm_datetime = {};

          LOG4CXX_TRACE(logger, "Raw DateTime: " + datetime_str);
          std::istringstream datetime_stream(datetime_str);

          datetime_stream >> std::get_time(&tm_datetime, mMessageDateFormat->time_format.c_str());

          if (datetime_stream.fail())
          {
            // break the line parser and continue with the next line - and fix the parser later
            LOG4CXX_ERROR(logger, "DateTime Regex Parser Error!");
            break;
          }

          // create the crono object
          std::time_t message_tt = std::mktime(&tm_datetime);
          std::chrono::system_clock::time_point message_tp = std::chrono::system_clock::from_time_t(message_tt);

          // This is some bare metal debug code that I didn't like to put into trace logs
          /*std::cout << "Year: " << tm_datetime.tm_year + 1900
           << " Month: " << tm_datetime.tm_mon + 1
           << " Day: " << tm_datetime.tm_mday
           << " Hour: " << tm_datetime.tm_hour
           << " Min: " << tm_datetime.tm_min << "\n";*/

          // search if a user with this alias has yet been found
          found_user = nullptr;
          for (auto user_it = import_users.begin(); user_it != import_users.end(); user_it++)
          {
            ImportUser &import_user = *user_it;
            if (import_user.hasNameAlias(name_str))
            {
              // found yet existing local chat user
              found_user = &import_user;
              break; // TODO: for now just take the first user with fitting alias. Border cases are name changes in the same chat...
            }
          }

          // if existing user with same alias is not found
          if (found_user == nullptr && !name_str.empty())
          {
            // create new chat local import user (start with 1)
            int user_id = user_count + 1;

            import_users.emplace_back(user_id);
            ImportUser *new_user = &import_users.back();

            new_user->addNameAlias(name_str);
            found_user = new_user;
            LOG4CXX_INFO(logger, "created user first time: " + name_str + " ID: " + to_string(user_id));
            user_count++;
          }

          if (found_user != nullptr)
          {
            int message_id = import_messages.size();
            import_messages.emplace_back(message_id, message_tp, found_user->getId());
            ImportMessage *import_message = &import_messages.back();

            bool attachement_found = extractAttachement(payload);
            if (attachement_found)
            {
              AttachmentInfo attachment_info = analyzeAttachement(payload);

              import_message->setMediaId(import_media_container.size());
              import_media_container.emplace_back(import_media_container.size());
              ImportMedia *import_media = &import_media_container.back();
              import_media->setAttachmentInfo(attachment_info);
            }
            else
            {
              import_message->addMessageLine(StringUtil::normalize_newlines(payload));
              imported_line_count++;
              found_message = import_message;
            }
          }
          else
          {
            if (!system_user)
            {
              import_users.emplace_back(ImportUser::SYSTEM_USER_ID);
              system_user = &import_users.back();
            }

            // add a system message
            int message_id = import_messages.size();
            import_messages.emplace_back(message_id, message_tp, ImportUser::SYSTEM_USER_ID);
            ImportMessage *import_message = &import_messages.back();
            import_message->addMessageLine(payload);
          }

          LOG4CXX_TRACE(logger, "payload: " + payload);
        }
        else
        {
          // just skip such message if found - could be fixed in the parser later
          LOG4CXX_ERROR(logger, "Parser Error - unknown message type found!");
        }

      }
      else
      {
        // just skip such message if found - could be fixed in the parser later
        LOG4CXX_ERROR(logger, "Parser Error - unknown message type found!");
      }
    }
    else // a message without date/time that is just a line break from the line before
    {
      if (found_user != nullptr)
      {
        if (found_message != nullptr)
        {
          found_message->addMessageLine(StringUtil::normalize_newlines(line));
          LOG4CXX_TRACE(logger, "  to user: " + to_string(found_user->getId()));
          LOG4CXX_TRACE(logger, "belongs to message: " + found_message->getText());
        }
      }
    }
    line_count++;
  }

  LOG4CXX_TRACE(logger, "detected chat lines: " + to_string(line_count));
  LOG4CXX_TRACE(logger, "imported chat lines: " + to_string(imported_line_count));

  out_ctx.chat = std::make_unique<ImportChat>(chat_name, ChatSource::FormatA);
  out_ctx.users = import_users;
  out_ctx.messages = import_messages;
  out_ctx.media = import_media_container;

  return true;
}

bool ChatFormatAStreamParser::extractAttachement(std::string &in_out_payload)
{
  // check attachments are always only single line messages
  size_t multiline = in_out_payload.find('\n');
  if (multiline == std::string::npos)
  {
    // Regex pattern for detecting attachments in a single-line message.
    //
    // Pattern:
    //   ^(.+\.[a-zA-Z0-9]{2,5})\s*(\([^\n]*\))\s*$
    //
    // Explanation:
    //
    // ^                           : Start of the string. Ensures the match begins at the very start.
    //
    // (.+\.[a-zA-Z0-9]{2,5})     : First capturing group (group 1).
    //                               - .+         : Matches one or more characters (any character except newline).
    //                               - \.         : Matches a literal dot, separating the filename from the extension.
    //                               - [a-zA-Z0-9]{2,5} : Matches 2 to 5 alphanumeric characters as the file extension.
    //                               This group captures the core filename with extension.
    //
    // \s*                         : Matches zero or more whitespace characters immediately after the filename.
    //                               This allows for spaces before the suffix without breaking the match.
    //
    // (\([^\n]*\))                : Second capturing group (group 2), the required suffix in parentheses.
    //                               - \( and \)  : Match literal opening and closing parentheses.
    //                               - [^\n]*      : Match any characters except newline inside the parentheses.
    //                               This ensures the suffix is fully contained and at the end of the line.
    //
    // \s*                         : Matches zero or more whitespace characters after the parentheses.
    //                               This allows for trailing spaces at the end of the line.
    //
    // $                           : End of string. Ensures that nothing follows the optional whitespace after the suffix.
    //
    // Notes:
    // - The regex is designed to work on a single line at a time.
    // - Group 1 captures the filename with extension, including spaces, special characters, or Unicode.
    // - Group 2 captures the suffix inside parentheses, which is required for a valid match.
    // - This structure avoids false positives from normal text containing parentheses or dots elsewhere.
    const string regex_attachment_str = R"attachment(^(.+\.[a-zA-Z0-9]{2,5})\s*(\([^\n]*\))\s*$)attachment";

    const std::regex att_regex(regex_attachment_str);
    std::smatch match;

    if (std::regex_search(in_out_payload, match, att_regex))
    {
      if (match.size() >= 3)
      {
        const std::string &filename_str = match[1];
        const std::string &attachment_cmd_str = match[2];

        in_out_payload = filename_str;
        return true;
      }
    }
  }

  return false;
}

ChatFormatAStreamParser::AttachmentInfo ChatFormatAStreamParser::analyzeAttachement(const std::string &attachement)
{
  AttachmentInfo attachment_info {};

  // preserve filename for later usage
  attachment_info.filename = sanitizeFilename(attachement);

  // cut out file extension for analysis
  std::string ext = attachement.substr(attachement.find_last_of('.') + 1);
  std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

  if (ext == "jpg" or ext == "jpeg")
  {
    attachment_info.mime_type = "image/jpeg";
    attachment_info.type = MediaType::Image;
  }
  else if (ext == "png")
  {
    attachment_info.mime_type = "image/png";
    attachment_info.type = MediaType::Image;
  }
  else if (ext == "webp")
  {
    attachment_info.mime_type = "image/webp";
    attachment_info.type = MediaType::Image;
  }
  else if (ext == "gif")
  {
    attachment_info.mime_type = "image/gif";
    attachment_info.type = MediaType::Image;
  }
  else if (ext == "mp4")
  {
    attachment_info.mime_type = "video/mp4";
    attachment_info.type = MediaType::Video;
  }
  else if (ext == "mov")
  {
    attachment_info.mime_type = "video/quicktime";
    attachment_info.type = MediaType::Video;
  }
  else if (ext == "3gp")
  {
    attachment_info.mime_type = "video/3gpp";
    attachment_info.type = MediaType::Video;
  }
  else if (ext == "opus")
  {
    attachment_info.mime_type = "audio/opus";
    attachment_info.type = MediaType::Audio;
  }
  else if (ext == "m4a")
  {
    attachment_info.mime_type = "audio/mp4";
    attachment_info.type = MediaType::Audio;
  }
  else if (ext == "mp3")
  {
    attachment_info.mime_type = "audio/mpeg";
    attachment_info.type = MediaType::Audio;
  }
  else if (ext == "wav")
  {
    attachment_info.mime_type = "audio/wav";
    attachment_info.type = MediaType::Audio;
  }

  return attachment_info;
}

std::string ChatFormatAStreamParser::sanitizeFilename(const std::string &s)
{
  std::string out;
  for (unsigned char c : s)
  {
    // LRM = e2 80 8e, RLM = e2 80 8f
    if (!(c == 0xE2 || c == 0x80 || c == 0x8E || c == 0x8F))
    {
      out += c;
    }
  }
  return out;
}

bool ChatFormatAStreamParser::isIOSDateFormat(const std::string &line)
{
  bool is_ios_format = false;

  // STEP 1: Check if the line starts with '['
  // iOS exported lines always start with a '[' for the timestamp
  if (!line.empty() && line[0] == '[')
  {
    // STEP 2: Find the closing bracket ']'
    size_t close_bracket = line.find(']');
    if (close_bracket != std::string::npos)
    {
      // STEP 3: Check if there is a comma between '[' and ']'
      // iOS timestamps always contain a comma separating date and time
      size_t comma = line.find(',', 1); // start searching after '['
      if (comma != std::string::npos && comma < close_bracket)
      {
        // If all conditions match, it's likely an iOS export line
        is_ios_format = true;
      }
    }
  }
  return is_ios_format;
}

bool ChatFormatAStreamParser::identifyDateFormat(const std::string &line)
{
  for (auto rdc : message_regex_datetime_cores)
  {
    for (auto mrpw : message_regex_platform_wrappers)
    {
      string full_regex_line = mrpw.prefix + rdc.regex_line + mrpw.suffix;
      LOG4CXX_TRACE(logger, "Full Regex: " + full_regex_line);

      std::smatch match;

      const std::regex full_regex(full_regex_line);
      if (std::regex_search(line, match, full_regex))
      {
        // take the first matching one - only one should match...
        const std::regex payload_regex(message_payload_regex);
        mMessageDateFormat = MessageDateFormat { full_regex, payload_regex, rdc.time_format };
        return true;
      }
    }
  }

  return false;
}

