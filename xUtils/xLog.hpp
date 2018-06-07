///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <mutex>
#include <cstdio>
#include <cstdarg>

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {
namespace xLog {

///////////////////////////////////////////////////////////////////////////////

class Log
{
private:
  bool        mIsStarted; // avoid multiple start of log instance
  std::mutex  mMutex;     // mutex to make thread safe on logging
  std::string mHost;      // hostname that applications runs on
  std::string mName;      // application name
  std::string mPath;      // path for log files if log to file
  std::FILE*  mFp;        // current log file fp if log to file
  unsigned    mMaxLines;  // max lines for a log file
  unsigned    mLines;     // current log file lines if log to file

private:
  Log();
  Log(const Log&) = delete;
  Log& operator=(const Log&) = delete;

private:
  void openLogFile();
  void closeLogFile(bool writedisk);
  std::string get_log_head(const char* type);
  void log(std::string& head, const char* format, std::va_list& args);

public:
  ~Log();
  static Log& instance();
  void start(std::string name, std::string path="", unsigned maxlines=512*1024);
  void stop(bool writedisk = true);

public:
  void info(const char* format, ...);
  void warn(const char* format, ...);
  void error(const char* format, ...);
  void debug(const char* format, ...);
};

///////////////////////////////////////////////////////////////////////////////

static Log& xLog = Log::instance();

///////////////////////////////////////////////////////////////////////////////

}   // end of namespace Log
}   // end of namespace xUtils
}   // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
