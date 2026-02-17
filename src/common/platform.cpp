/*
 * platform.cpp
 *
 *      Author: Andreas Volz
 */

#include "platform.h"

#ifdef _MSC_VER
#define DEBUG _DEBUG
#define PATH_MAX _MAX_PATH
#include <direct.h>
#include <io.h>
#define dirname(x) PathRemoveFileSpec(x); if (x[strlen(x) - 1] == '\\') x[strlen(x) - 1] = '\0'
#else
#include <libgen.h>
#include <limits.h>
#include <unistd.h>
#endif

#ifdef _MSC_VER
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#endif

#include <cstring>
#include <iostream>

using namespace std;

namespace platform
{

char *strdup(const char *s)
{
#ifdef WIN32
  return ::_strdup(s);
#else
  return ::strdup(s);
#endif
}

int mkdir(const std::string &pathname, mode_t mode)
{
#if defined(_MSC_VER) || defined(WIN32)
  return ::_mkdir(pathname.c_str());
#else
  return ::mkdir(pathname.c_str(), mode);
#endif
}

#ifdef WIN32
int executeWindows(const std::string &program, const std::vector<std::string> &args,
                   OutputMode mode, std::string *output)
{
  std::string commandLine = "\"" + program + "\"";
  for (const auto &arg : args)
  {
    commandLine += " \"" + arg + "\"";
  }
  std::wstring wCommandLine(commandLine.begin(), commandLine.end());

  STARTUPINFO si{};
  PROCESS_INFORMATION pi{};
  si.cb = sizeof(STARTUPINFO);

  HANDLE hRead = NULL, hWrite = NULL;
  if (mode == OutputMode::CAPTURE)
  {
    SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
    CreatePipe(&hRead, &hWrite, &sa, 0);
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);
    si.hStdOutput = hWrite;
    si.hStdError = hWrite;
    si.dwFlags |= STARTF_USESTDHANDLES;
  }
  else if (mode == OutputMode::IGNORE)
  {
    HANDLE hNull = CreateFile("NUL", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    si.hStdOutput = hNull;
    si.hStdError = hNull;
    si.dwFlags |= STARTF_USESTDHANDLES;
  }

  if (!CreateProcess(nullptr, &wCommandLine[0], nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi))
  {
    throw std::runtime_error("Failed to create process on Windows: " + std::to_string(GetLastError()));
  }

  if (mode == OutputMode::CAPTURE)
  {
    CloseHandle(hWrite);
    char buffer[4096];
    DWORD bytesRead;
    while (ReadFile(hRead, buffer, sizeof(buffer), &bytesRead, NULL) && bytesRead > 0)
    {
      if (output) output->append(buffer, bytesRead);
    }
    CloseHandle(hRead);
  }

  WaitForSingleObject(pi.hProcess, INFINITE);
  DWORD exitCode;
  GetExitCodeProcess(pi.hProcess, &exitCode);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return static_cast<int>(exitCode);
}
#else
int executePosix(const std::string &program, const std::vector<std::string> &args,
                 OutputMode mode, std::string *output)
{
  std::vector<char *> cArgs;
  cArgs.push_back(const_cast<char *>(program.c_str()));
  for (const auto &arg : args)
  {
    cArgs.push_back(const_cast<char *>(arg.c_str()));
  }
  cArgs.push_back(nullptr);

  int pipeFd[2];
  if (mode == OutputMode::CAPTURE)
  {
    pipe(pipeFd);
  }

  pid_t pid = fork();
  if (pid < 0)
  {
    throw std::runtime_error("Failed to fork process");
  }

  if (pid == 0)
  {
    if (mode == OutputMode::CAPTURE)
    {
      dup2(pipeFd[1], STDOUT_FILENO);
      dup2(pipeFd[1], STDERR_FILENO);
      close(pipeFd[0]);
      close(pipeFd[1]);
    }
    else if (mode == OutputMode::IGNORE)
    {
      int devNull = open("/dev/null", O_WRONLY);
      dup2(devNull, STDOUT_FILENO);
      dup2(devNull, STDERR_FILENO);
      close(devNull);
    }
    execvp(program.c_str(), cArgs.data());
    exit(EXIT_FAILURE);
  }

  if (mode == OutputMode::CAPTURE)
  {
    close(pipeFd[1]);
  }

  int status;
  if (waitpid(pid, &status, 0) == -1)
  {
    throw std::runtime_error("Error waiting for child process");
  }

  if (mode == OutputMode::CAPTURE)
  {
    char buffer[4096];
    ssize_t bytesRead;
    while ((bytesRead = read(pipeFd[0], buffer, sizeof(buffer))) > 0)
    {
      if (output) output->append(buffer, bytesRead);
    }
    close(pipeFd[0]);
  }

  if (WIFEXITED(status))
  {
    return WEXITSTATUS(status);
  }
  else if (WIFSIGNALED(status))
  {
    return -WTERMSIG(status);
  }
  return -1;
}
#endif

int executeProcess(const std::string &program, const std::vector<std::string> &args,
                   OutputMode mode, std::string *output)
{
#ifdef _WIN32
  return executeWindows(program, args, mode, output);
#else
  return executePosix(program, args, mode, output);
#endif
}


} /* namespace platform */
