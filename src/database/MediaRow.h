/*
 * MediaRow.h
 *
 *      Author: Andreas Volz
 */

#ifndef MEDIAROW_H_
#define MEDIAROW_H_

// system
#include <cstdint>
#include <string>

struct MediaRow
{
  int64_t media_id;
  int64_t account_id = 0;
  int64_t type = 0;
  int64_t media_size = 0;
  std::string mime_type;
};

#endif /* MEDIAROW_H_ */
