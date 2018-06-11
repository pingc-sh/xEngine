///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip> 
#include <unistd.h>
#include "xLog.hpp"
#include "xTimeNow.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::Log;

///////////////////////////////////////////////////////////////////////////////

Log::Log()
{
  mIsStarted   = false;
  mLogFp       = nullptr;
  mLogMaxLines = 0;
  mLogCurLines = 0;
}

///////////////////////////////////////////////////////////////////////////////

Log::~Log()
{
  if(mLogFp != nullptr && mLogFp != stdout) {
    stop(true);  // stop log, flush buf into kernel and write disk
  }
}

///////////////////////////////////////////////////////////////////////////////

Log& Log::instance()
{
  static Log _instance;
  return _instance;
}

///////////////////////////////////////////////////////////////////////////////

void Log::openLogFile()
{
  std::stringstream filename;
  filename << mLogPath.c_str() << "/" << mAppName.c_str()
           << "_" 
           << mHost.c_str() 
           << "_"
           << TimeNow().f_str().c_str()
           << ".log";

  mLogFp = fopen(filename.str().c_str(), "a");
  if(mLogFp == nullptr) {
    std::cout << "error: cannot create logfile, please check path" << std::endl;
    exit(-1);
  }
}

///////////////////////////////////////////////////////////////////////////////

void Log::closeLogFile(bool writedisk)
{
  if(mLogFp != nullptr && mLogFp != stdout) {
    if(writedisk) {
      fsync(fileno(mLogFp));  // flush kernel space buffer into disk
    }
    std::fclose(mLogFp);
  }

  mLogFp = nullptr;
  mLogCurLines = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Log::start(std::string appname, std::string path, unsigned maxlines)
{
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  if(mIsStarted) {  // to avoid multiple times of start
    return;
  }
  mIsStarted = true;

  char hostname[64] = {0};
  gethostname(hostname, 64);
  mHost = hostname;

  if(!appname.empty()) {  // setup application name
    mAppName = appname;
  }

  if(path.empty()) {  // if no path setup then stdout else create log file
    mLogFp = stdout;
  } else {
    mLogPath = path;
    openLogFile();
  }

  if(maxlines > 0) {
    mLogMaxLines = maxlines;
  }
}

///////////////////////////////////////////////////////////////////////////////

void Log::stop(bool writedisk)
{
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  if(!mIsStarted) {  // to avoid multiple times of stop
    return;
  }
  mIsStarted = false;

  if(mLogFp != stdout) {
    closeLogFile(writedisk);
  }
}

///////////////////////////////////////////////////////////////////////////////

void Log::log(std::string time, const char* type, const char* format, std::va_list& args)
{
  if(mLogFp == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard

  if(mLogFp != stdout) {
    if(mLogCurLines >= mLogMaxLines) {  // avoid log file too large 
      closeLogFile(true);  // close current log file and wirte to disk
      openLogFile();  // create a new log file
    }
    mLogCurLines++;
  }

  std::stringstream log_head;
  log_head << time.c_str()
           << " "
           << mHost.c_str()
           << " "
           << mAppName.c_str() 
           << "." 
           << std::hex << std::this_thread::get_id()
           << " " 
           << type 
           << ": ";

  std::fprintf(mLogFp, "%s", log_head.str().c_str());
  std::vfprintf(mLogFp, format, args);
  std::fprintf(mLogFp, "\n");
  std::fflush(mLogFp);  // flush user space buffer into kernel
}

///////////////////////////////////////////////////////////////////////////////

void Log::info(const char* format, ...)
{
  std::va_list args;
  va_start(args, format);
  log(TimeNow().f_str(), "info ", format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::warn(const char* format, ...)
{
  std::va_list args;
  va_start(args, format);
  log(TimeNow().f_str(), "warn ", format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::error(const char* format, ...)
{
  std::va_list args;
  va_start(args, format);
  log(TimeNow().f_str(), "error", format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::debug(const char* format, ...)
{
  std::va_list args;
  va_start(args, format);
  log(TimeNow().f_str(), "debug", format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
