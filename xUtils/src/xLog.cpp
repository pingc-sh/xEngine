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
  mIsStarted = false;
  mFp = nullptr;
  mMaxLines = 0;
  mLines = 0;
}

///////////////////////////////////////////////////////////////////////////////

Log::~Log()
{
  if(mFp != nullptr && mFp != stdout) {
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
  filename << mPath.c_str() << "/" << mName.c_str()
           << "_" 
           << mHost.c_str() 
           << "_"
           << TimeNow().f_str().c_str()
           << ".log";

  mFp = fopen(filename.str().c_str(), "a");
  if(mFp == nullptr) {
    std::cout << "error: cannot create logfile, please check path" << std::endl;
    exit(-1);
  }
}

///////////////////////////////////////////////////////////////////////////////

void Log::closeLogFile(bool writedisk)
{
  if(mFp != nullptr && mFp != stdout) {
    if(writedisk) {
      fsync(fileno(mFp));  // flush kernel space buffer into disk
    }
    std::fclose(mFp);
  }

  mFp = nullptr;
  mLines = 0;
}

///////////////////////////////////////////////////////////////////////////////

void Log::start(std::string name, std::string path, unsigned maxlines)
{
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  if(mIsStarted) {  // to avoid multiple times of start
    return;
  }
  mIsStarted = true;

  char hostname[64] = {0};
  gethostname(hostname, 64);
  mHost = hostname;

  if(!name.empty()) {  // setup application name
    mName = name;
  }

  if(path.empty()) {  // if no path setup then stdout else create log file
    mFp = stdout;
  } else {
    mPath = path;
    openLogFile();
  }

  if(maxlines > 0) {
    mMaxLines = maxlines;
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

  if(mFp != stdout) {
    closeLogFile(writedisk);
  }
}

///////////////////////////////////////////////////////////////////////////////

int Log::getThreadIdx()
{
  std::thread::id tid = std::this_thread::get_id();
  if(mTIDs.find(tid) != mTIDs.end()){
    return mTIDs[tid];
  } else {
    int idx = mTIDs.size() + 1;
    mTIDs.insert(std::make_pair(tid, idx));
    return idx;
  }
}

///////////////////////////////////////////////////////////////////////////////

void Log::log(std::string time, const char* type, const char* format, std::va_list& args)
{
  if(mFp == nullptr) {
    return;
  }
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard

  if(mFp != stdout) {
    if(mLines >= mMaxLines) {  // avoid log file too large 
      closeLogFile(true);  // close current log file and wirte to disk
      openLogFile();  // create a new log file
    }
    mLines++;
  }

  std::stringstream log_head;
  log_head << time.c_str()
           << " "
           << mHost.c_str()
           << " "
           << mName.c_str() 
           << "." 
           << std::right << std::setfill('0') << std::setw(2) << getThreadIdx() 
           << " " 
           << type 
           << ": ";

  std::fprintf(mFp, "%s", log_head.str().c_str());
  std::vfprintf(mFp, format, args);
  std::fprintf(mFp, "\n");
  std::fflush(mFp);  // flush user space buffer into kernel
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
