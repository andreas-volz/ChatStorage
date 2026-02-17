/*
 * UserRow.h
 *
 *      Author: Andreas Volz
 */

#ifndef USERROW_H_
#define USERROW_H_

// system
#include <cstdint>
#include <string>

struct UserRow
{
  int64_t user_id = 0;
  int64_t account_id = 0;
  std::string name;
  int64_t is_system = 0;
};

#endif /* USERROW_H_ */
