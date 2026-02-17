/*
 * platform.h
 *
 *      Author: Andreas Volz
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include <vector>
#include <filesystem>

// define a shortcut as it's now used everywhere in my code
namespace fs = std::filesystem;

namespace platform
{
enum class OutputMode { IGNORE, CAPTURE };

char *strdup(const char *s);

#if defined(_MSC_VER) || defined(WIN32)
typedef int mode_t;
#endif

int mkdir(const std::string &pathname, mode_t mode = 0);

int executeProcess(const std::string& program, const std::vector<std::string>& args,
                   OutputMode mode = OutputMode::IGNORE, std::string* output = nullptr);

} /* namespace platform */

#endif /* PLATFORM_H */
