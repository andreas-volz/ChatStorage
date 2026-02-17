#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// project
#include "FileNotFoundException.h"

// system
#include <string>

using namespace std;

const char *FileNotFoundException::what() const throw()
{
  static string s;
  s = "File not found: '";
  s += txt;
  s += "'.";

  return static_cast <const char *>(s.c_str());
}
