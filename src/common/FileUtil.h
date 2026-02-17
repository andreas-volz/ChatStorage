/*
 * FileUtil.h
 *
 *      Author: Andreas Volz
 */

#ifndef FILEUTIL_H_
#define FILEUTIL_H_

// project
#include "platform.h"

// system
#include <string>
#include <stdlib.h>
#include <string.h>

namespace FileUtil
{


  /**
   *  Check if path exists, if not make all directories.
   */
  void createPath(const std::string &path_name);

#endif /* FILEUTIL_H_ */

} // namespace FileUtil
