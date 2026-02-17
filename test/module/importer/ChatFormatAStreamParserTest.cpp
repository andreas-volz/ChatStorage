#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// project
#include "ChatFormatAStreamParserTest.h"
#include "importer/ChatFormatAStreamParser.h"
#include "importer/ChatImportContext.h"
#include "../TestHelpers.h"

// system

using namespace std;
using time_point = std::chrono::system_clock::time_point;

CPPUNIT_TEST_SUITE_REGISTRATION(ChatFormatAStreamParserTest);


void ChatFormatAStreamParserTest::setUp()
{

}

void ChatFormatAStreamParserTest::tearDown()
{

}



void ChatFormatAStreamParserTest::test_attachment()
{
  ChatFormatAStreamParser chat_parser;

  // there is an often used invisible unicode character between ':' and 'IMG'
  // which is removed with sanitizeFilename()
  string attachment_chat =  "27.10.23, 22:56 - Tom: ‎IMG-20231027-WA0011.jpg (something)";

  istringstream chat_stream(attachment_chat);
  ChatImportContext out_ctx;

  bool parse_result = chat_parser.parse(chat_stream, "", out_ctx);
  ASSERT_MSG(parse_result, "Parser step failed!");

  int message_size = out_ctx.messages.size();
  int media_size = out_ctx.media.size();

  ASSERT_EQUAL_MSG(message_size, 1, "Not exact one message found!");
  ASSERT_EQUAL_MSG(media_size, 1, "Not exact one media attachment!");
  ASSERT_MSG(out_ctx.chat != nullptr, "No Chat parsed!");
}

void ChatFormatAStreamParserTest::test_simple_text_message()
{
  ChatFormatAStreamParser chat_parser;

  string message_payload = "Hello again friend";
  string attachment_chat = "27.10.23, 22:56 - Tom: " + message_payload;

  istringstream chat_stream(attachment_chat);
  ChatImportContext out_ctx;

  bool parse_result = chat_parser.parse(chat_stream, "", out_ctx);
  ASSERT_MSG(parse_result, "Parser step failed!");

  int message_size = out_ctx.messages.size();
  int user_size = out_ctx.users.size();

  ASSERT_EQUAL_MSG(message_size, 1, "Not exact one message found!");
  ASSERT_EQUAL_MSG(user_size, 1, "Not exact one user found!"); // -> 'Tom'
  ASSERT_MSG(out_ctx.chat != nullptr, "No Chat parsed!");

  CPPUNIT_ASSERT_EQUAL(message_payload, out_ctx.messages[0].getText());
}

void ChatFormatAStreamParserTest::test_simple_system_message()
{
  ChatFormatAStreamParser chat_parser;

  string message_payload = "Message is encrypted";
  string attachment_chat = "27.10.23, 22:56 - " + message_payload;

  istringstream chat_stream(attachment_chat);
  ChatImportContext out_ctx;

  bool parse_result = chat_parser.parse(chat_stream, "", out_ctx);
  ASSERT_MSG(parse_result, "Parser step failed!");

  int message_size = out_ctx.messages.size();
  int user_size = out_ctx.users.size();

  ASSERT_EQUAL_MSG(message_size, 1, "Not exact one message line input!");
  ASSERT_EQUAL_MSG(user_size, 1, "Not exact one user found!"); // the '<system>' user that is created for the first time
  ASSERT_MSG(out_ctx.chat != nullptr, "No Chat parsed!");

  CPPUNIT_ASSERT_EQUAL(message_payload, out_ctx.messages[0].getText());
}

void ChatFormatAStreamParserTest::test_timeformat_de_android()
{
  check_one_line_message_date_time("07.12.22, 20:58 - ", {2022, 12, 7, 20, 58});
  check_one_line_message_date_time("07.12.22, 20:58:34 - ", {2022, 12, 7, 20, 58, 34});
}

void ChatFormatAStreamParserTest::test_timeformat_en_24h_android()
{
  check_one_line_message_date_time("25/11/2022, 20:57 - ", {2022, 11, 25, 20, 57});
  check_one_line_message_date_time("25/11/2022, 20:57:22 - ", {2022, 11, 25, 20, 57, 22});
}

void ChatFormatAStreamParserTest::test_timeformat_en_12h_android()
{
  // ATTENTION: between '29' and 'am' is an invisible unicode space that is internal normalized with
  // StringUtil::normalize_whitespace()
  check_one_line_message_date_time("4/11/24, 9:29 pm - ", {2024, 4, 11, 21, 29});
  check_one_line_message_date_time("4/11/24, 9:29:23 pm - ", {2024, 4, 11, 21, 29, 23});

  // looks the same, but now with a normal space
  check_one_line_message_date_time("4/11/24, 9:29 pm - ", {2024, 4, 11, 21, 29});
  check_one_line_message_date_time("4/11/24, 9:29:23 pm - ", {2024, 4, 11, 21, 29, 23});
}

void ChatFormatAStreamParserTest::test_timeformat_de_ios()
{
  check_one_line_message_date_time("[10.12.23, 20:07] ", {2023, 12, 10, 20, 07});
  check_one_line_message_date_time("[10.12.23, 20:07:56] ", {2023, 12, 10, 20, 07, 56});
}

void ChatFormatAStreamParserTest::test_timeformat_en_24h_ios()
{
  check_one_line_message_date_time("[25/11/2022, 20:57] ", {2022, 11, 25, 20, 57});
  check_one_line_message_date_time("[25/11/2022, 20:57:12] ", {2022, 11, 25, 20, 57, 12});
}

void ChatFormatAStreamParserTest::test_timeformat_en_12h_ios()
{
  // ATTENTION: between '29' and 'am' is an invisible unicode space that is internal normalized with
  // StringUtil::normalize_whitespace()
  check_one_line_message_date_time("[4/11/24, 9:29 pm] ", {2024, 4, 11, 21, 29});
  check_one_line_message_date_time("[4/11/24, 9:29:23 pm] ", {2024, 4, 11, 21, 29, 23});

  // looks the same, but now with a normal space
  check_one_line_message_date_time("[4/11/24, 9:29 pm] ", {2024, 4, 11, 21, 29});
  check_one_line_message_date_time("[4/11/24, 9:29:23 pm] ", {2024, 4, 11, 21, 29, 23});
}

void ChatFormatAStreamParserTest::check_one_line_message_date_time(const std::string &chat_line, DateTimeParts dtp)
{
  ChatFormatAStreamParser chat_parser;

  istringstream chat_stream(chat_line);
  ChatImportContext out_ctx;

  time_point reference_tp = tp(dtp);

  bool parse_result = chat_parser.parse(chat_stream, "", out_ctx);
  ASSERT_MSG(parse_result, "Parser step failed!");

  int message_size = out_ctx.messages.size();

  time_point message_tp;

  ASSERT_EQUAL_MSG(message_size, 1, "Not exact one message found!");
  ASSERT_MSG(out_ctx.chat != nullptr, "No Chat parsed!");

  message_tp = out_ctx.messages[0].getTimePoint();
  CPPUNIT_ASSERT_EQUAL(reference_tp.time_since_epoch().count(), message_tp.time_since_epoch().count());
}

std::chrono::system_clock::time_point ChatFormatAStreamParserTest::tp(DateTimeParts dtp)
{
  std::tm t {};
  t.tm_year = dtp.y - 1900;
  t.tm_mon = dtp.m - 1;
  t.tm_mday = dtp.d;
  t.tm_hour = dtp.h;
  t.tm_min = dtp.min;
  t.tm_sec = dtp.s;
  return std::chrono::system_clock::from_time_t(std::mktime(&t));
}

