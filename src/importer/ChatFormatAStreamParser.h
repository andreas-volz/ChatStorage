/*
 * FileBasedFormatAParser.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATFORMATASTREAMPARSER_H_
#define CHATFORMATASTREAMPARSER_H_

// project
#include "AbstractChatParser.h"

// system
#include <regex>

struct MessageDateTimeCore
{
  std::string regex_line;
  std::string time_format;
};

struct MessagePlatformWrapper
{
  std::string prefix;
  std::string suffix;
};

struct MessageDateFormat
{
  std::regex full_regex;
  std::regex payload_regex;
  std::string time_format;
};

class ChatFormatAStreamParser: public AbstractChatParser
{
public:
  ChatFormatAStreamParser() = default;
  virtual ~ChatFormatAStreamParser() = default;

  bool parse(std::istream &in_stream, const std::string &chat_name, ChatImportContext &out_ctx);

private:
  /**
   * Extract the (possible) attachment part from the payload
   *
   * @return true if an attachment was found. In this case in_out_payload holds only this filename
   * @return false if no attachment was found. In this case in_out_payload holds the unmodified payload
   */
  bool extractAttachement(std::string &in_out_payload);

  AttachmentInfo analyzeAttachement(const std::string &attachement);

  /**
   * Removes invisible Unicode bidi control characters (e.g., LRM, RLM)
   * from filenames extracted from export text.
   * Necessary to avoid issues when accessing files via std::filesystem.
   */
  std::string sanitizeFilename(const std::string &s);

  bool isIOSDateFormat(const std::string &line);

  std::string extractIOSDateString(const std::string &line);

  std::string extractAndroidDateString(const std::string &line);

  bool identifyDateFormat(const std::string &line);

  std::optional<MessageDateFormat> mMessageDateFormat;
};

#endif /* CHATFORMATASTREAMPARSER_H_ */
