/*
 * stringUtil.cpp
 *
 *      Author: Andreas Volz
 */

/* project */
#include "StringUtil.h"
#include "Logger.h"

/* system */
#include <algorithm>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <bits/stdc++.h>

using namespace std;

static Logger logger = Logger("ChatStorage.StringUtil");

namespace StringUtil
{

  int replaceString(const string &match, const string &replace, string &str)
  {
    int i = 0;

    if (str.find(match) == std::string::npos)
      return false;

    std::string::size_type start = 0;

    while ((start = str.find(match)) != std::string::npos)
    {
      str.replace(start, match.size(), replace);

      i++;
    }

    return i;
  }

  bool hasFileEnding(const std::string &filename, const std::string &ending)
  {
    const size_t loc = filename.find(ending, filename.length() - ending.length());

    if (loc != string::npos)
    {
      return true;
    }

    return false;
  }

  std::string cutFileEnding(std::string filename, const std::string &ending)
  {
    if (ending == "")
    {
      const size_t loc = filename.find_last_of('.', filename.length());

      if (loc != string::npos)
      {
        filename.erase(loc);
        return filename;
      }
    }
    else
    {
      const size_t loc = filename.find(ending, filename.length() - ending.length());

      if (loc != string::npos)
      {
        filename.erase(loc);
        return filename;
      }
    }

    return filename;
  }

  std::string to_lower(std::string line)
  {
    std::for_each(line.begin(), line.end(), [](char &c)
    {
      c = ::tolower(c);
    });

    return line;
  }

  std::vector<std::string> splitString(const std::string &inputStr, const std::string &delim)
  {
    vector<string> string_container;
    regex delim_regex(delim);

    // Create a regex_token_iterator to split the string
    sregex_token_iterator it(inputStr.begin(), inputStr.end(), delim_regex, -1);

    // End iterator for the regex_token_iterator
    sregex_token_iterator end;

    // Iterating through each token
    while (it != end)
    {
      //cout << "\"" << *it << "\"" << " ";
      string_container.push_back(*it);
      ++it;
    }

    return string_container;
  }

  std::string normalize_whitespace(const std::string &input)
  {
    std::string out;

    for (size_t i = 0; i < input.size();)
    {

      unsigned char c = static_cast<unsigned char>(input[i]);

      // U+202F → UTF-8: E2 80 AF
      if (i + 2 < input.size() && c == 0xE2 && static_cast<unsigned char>(input[i + 1]) == 0x80
          && static_cast<unsigned char>(input[i + 2]) == 0xAF)
      {

        out += ' ';
        i += 3;
        continue;
      }

      // U+00A0 → UTF-8: C2 A0
      if (i + 1 < input.size() && c == 0xC2 && static_cast<unsigned char>(input[i + 1]) == 0xA0)
      {

        out += ' ';
        i += 2;
        continue;
      }

      // if not just copy
      out += input[i];
      i += 1;
    }

    return out;
  }

  std::string normalize_newlines(const std::string &input)
  {
    std::string s = input;

    // Windows CRLF -> LF
    std::string::size_type pos = 0;
    while ((pos = s.find("\r\n", pos)) != std::string::npos)
    {
      s.replace(pos, 2, "\n");
    }

    // Old Mac CR -> LF
    pos = 0;
    while ((pos = s.find('\r', pos)) != std::string::npos)
    {
      s[pos] = '\n';
    }
    return s;
  }

  bool starts_with(const std::string &str, const std::string &prefix)
  {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
  }

  bool ends_with(const std::string &str, const std::string &suffix)
  {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
  }

} // end namespace StringUtil

