/*
 * stringUtil.h
 *
 *      Author: Andreas Volz
 */

#ifndef STRINGUTIL_H_
#define STRINGUTIL_H_

// system
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <exception>

namespace StringUtil
{

// some functions that are very usefull for writing files
  bool hasFileEnding(const std::string &filename, const std::string &ending);
  std::string cutFileEnding(std::string filename, const std::string &ending = "");

  /**
   * A helper function to return input string to lower case. Doesn't modify the input string.
   *
   * @return the lower case string variant
   */
  std::string to_lower(std::string line);

  /**
   * An function to replace occurences of substrings in a bigger string. Very basic function without regex.
   * @param match This is the string that is matched to replace.
   * @param replace The string that replaces all found match strings.
   * @param str The string that is matched and replaced.
   *
   * @return number of replaced strings
   */
  int replaceString(const std::string &match, const std::string &replace, std::string &str);

  std::vector<std::string> splitString(const std::string &inputStr, const std::string &delim);

  /**
   * print Vector on std::cout
   */
  template<typename T>
  void printVector(std::vector<T> iv)
  {
    std::cout << "[";
    for (unsigned i = 0; i < iv.size(); i++)
    {
      std::cout << iv[i];
      if (i < iv.size() - 1)
        std::cout << ",";
    }
    std::cout << "]" << std::endl;
  }

  template<typename T>
  void printKV(const std::string &key, const T &value, int keyWidth = 25)
  {
    std::cout << std::left << std::setw(keyWidth) << key << value << "\n";
  }

  template<class T>
  std::string to_hex(const T &value)
  {
    char hex_string[20];
    sprintf(hex_string, "0x%X", value); //convert number to hex
    return hex_string;
  }

  /**
   * Normalizes all known line ending to '\n'
   *
   * Windows CRLF -> LF (\r\n)
   * Old Mac CR -> LF (\r)
   *
   * into -> UNNIX style (\n)
   */
  std::string normalize_newlines(const std::string &input);

  /**
   * Normalizes exported text by replacing problematic Unicode
   * whitespace characters with a standard ASCII space.
   *
   * Specifically handles UTF-8 encoded non-breaking and narrow spaces.
   *
   * The input string is left unmodified; a normalized copy is returned
   * to ensure downstream regex-based parsers work reliably.
   */
  std::string normalize_whitespace(const std::string &input);

  /**
   * C++17 compatible static start_with function
   */
  bool starts_with(const std::string &str, const std::string &prefix);

  /**
   * C++17 compatible static ends_with function
   */
  bool ends_with(const std::string &str, const std::string &suffix);

} // end namespace StringUtil

#endif /* STRINGUTIL_H_ */
