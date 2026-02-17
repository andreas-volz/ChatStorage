/*
 * MessageRow.h
 *
 *      Author: Andreas Volz
 */

#ifndef MESSAGEROW_H_
#define MESSAGEROW_H_

// system
#include <cstdint>
#include <string>

struct MessageRow
{
  int64_t message_id = 0;
  int64_t account_id = 0;
  int64_t chat_id = 0;
  int64_t sender_id = 0;
  int64_t media_id;
  int64_t timestamp = 0;
  std::string text;
};

#endif /* MESSAGEROW_H_ */
