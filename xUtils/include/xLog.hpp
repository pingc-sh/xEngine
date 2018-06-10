///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <mutex>
#include <cstdio>
#include <cstdarg>
#include <thread>

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

class Log
{
private:
  bool        mIsStarted;    // avoid multiple start of log instance
  std::mutex  mMutex;        // mutex to make thread safe on logging
  std::string mHost;         // hostname that applications runs on
  std::string mAppName;      // application name
  std::string mLogPath;      // path for log files if log to file
  std::FILE*  mLogFp;        // current log file fp if log to file
  unsigned    mLogMaxLines;  // max lines for a log file
  unsigned    mLogCurLines;  // current log file lines if log to file

private:
  Log();
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

private:
  void openLogFile();
  void closeLogFile(bool writedisk);
  void log(std::string time,     // log time
           const char* type,     // log type: info/warn/error/debug
           const char* format,   // log format
           std::va_list& args);  // log variables

public:
  ~Log();
  static Log& instance();

public:
  void start(std::string appname, std::string path="", unsigned maxlines=512*1024);
  void stop (bool writedisk = true);
  void info (const char* format, ...);
  void warn (const char* format, ...);
  void error(const char* format, ...);
  void debug(const char* format, ...);
};

///////////////////////////////////////////////////////////////////////////////

static Log& xLog = Log::instance();

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
