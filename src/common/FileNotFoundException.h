#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

// system
#include <exception>
#include <string>

class FileNotFoundException : public std::exception
{
public:
  FileNotFoundException(const std:: string &file) : txt(file) {}
  virtual ~FileNotFoundException() = default;

  const char *what() const throw();

private:
  const std::string &txt;
};

#endif // FILENOTFOUNDEXCEPTION_H
