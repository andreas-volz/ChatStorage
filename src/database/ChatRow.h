/*
 * ChatRow.h
 *
 *      Author: Andreas Volz
 */

#ifndef CHATROW_H_
#define CHATROW_H_

// system
#include <cstdint>
#include <string>

struct ChatRow
{
  int64_t chat_id = 0;
  int64_t account_id = 0;
  std::string name;
  int64_t source = 0;
};

#endif /* CHATROW_H_ */
