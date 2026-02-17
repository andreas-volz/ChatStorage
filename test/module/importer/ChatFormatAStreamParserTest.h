#ifndef CHATFORMATASTREAMPARSER_TEST_H
#define CHATFORMATASTREAMPARSER_TEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

// project

// system
#include <string.h>
#include <cstdio>
#include <chrono>

struct DateTimeParts
{
  int y = 0;
  int m = 0;
  int d = 0;
  int h = 0;
  int min = 0;
  int s = 0;
};

class ChatFormatAStreamParserTest: public CPPUNIT_NS::TestFixture
{
CPPUNIT_TEST_SUITE(ChatFormatAStreamParserTest);

  CPPUNIT_TEST(test_timeformat_de_android);
  CPPUNIT_TEST(test_timeformat_en_24h_android);
  CPPUNIT_TEST(test_timeformat_en_12h_android);
  CPPUNIT_TEST(test_timeformat_de_ios);
  CPPUNIT_TEST(test_timeformat_en_24h_ios);
  CPPUNIT_TEST(test_timeformat_en_12h_ios);
  CPPUNIT_TEST(test_simple_text_message);
  CPPUNIT_TEST(test_simple_system_message);
  CPPUNIT_TEST(test_attachment);

  CPPUNIT_TEST_SUITE_END()
  ;

public:
  void setUp();
  void tearDown();

protected:


  void test_attachment();

  void test_simple_text_message();

  void test_simple_system_message();

  /**
   * Android parentheses test functions for each available time format
   */
  void test_timeformat_de_android();
  void test_timeformat_en_24h_android();
  void test_timeformat_en_12h_android();

  /**
   * iOS parentheses test functions for each available time format
   */
  void test_timeformat_de_ios();
  void test_timeformat_en_24h_ios();
  void test_timeformat_en_12h_ios();

private:
  void check_one_line_message_date_time(const std::string &chat_line, DateTimeParts dtp);

  std::chrono::system_clock::time_point tp(DateTimeParts dtp);
};

#endif // CHATFORMATASTREAMPARSER_TEST_H
