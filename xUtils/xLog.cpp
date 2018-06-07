///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream> 
#include <thread>
#include <unistd.h>
#include "xLog.hpp"

///////////////////////////////////////////////////////////////////////////////

using namespace xEngine::xUtils::xLog;

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
  auto ms_duration = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
  auto sec = ms_duration / 1000000;
  auto ms  = ms_duration % 1000000;
  std::time_t tt = sec;
  std::tm* t_gmt = std::gmtime(&tt); 

  std::stringstream filename;
  filename << mPath.c_str() << "/" << mName.c_str()
           << "_" 
           << mHost.c_str() 
           << "_"
           << t_gmt->tm_year + 1900
           << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_mon + 1
           << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_mday
           << "-" 
           << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_hour
           << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_min
           << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_sec
           << "." 
           << std::right << std::setfill('0') << std::setw(6) << ms
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

std::string Log::get_log_head(const char* type)
{
  auto ms_duration = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
  auto sec = ms_duration / 1000000;
  auto ms  = ms_duration % 1000000;
  std::time_t tt = sec;
  std::tm* t_gmt = std::gmtime(&tt); 

  std::stringstream head;
  head << t_gmt->tm_year + 1900
       << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_mon + 1
       << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_mday
       << "-" 
       << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_hour
       << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_min
       << std::right << std::setfill('0') << std::setw(2) << t_gmt->tm_sec
       << "." 
       << std::right << std::setfill('0') << std::setw(6) << ms
       << " "
       << mHost.c_str()
       << " "
       << mName.c_str() 
       << " " 
       << std::this_thread::get_id() 
       << " " 
       << type << ": ";
  return head.str();
}

///////////////////////////////////////////////////////////////////////////////

void Log::log(std::string& head, const char* format, std::va_list& args)
{
  if(mFp == nullptr) {
    return;
  }

  if(mFp != stdout) {
    if(mLines >= mMaxLines) {  // avoid log file too large 
      closeLogFile(true);  // close current log file and wirte to disk
      openLogFile();  // create a new log file
    }
    mLines++;
  }

  std::fprintf(mFp, "%s", head.c_str());
  std::vfprintf(mFp, format, args);
  std::fprintf(mFp, "\n");
  std::fflush(mFp);  // flush user space buffer into kernel
}

///////////////////////////////////////////////////////////////////////////////

void Log::info(const char* format, ...)
{
  std::string head = get_log_head("info ");
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  std::va_list args;
  va_start(args, format);
  log(head, format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::warn(const char* format, ...)
{
  std::string head = get_log_head("warn ");
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  std::va_list args;
  va_start(args, format);
  log(head, format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::error(const char* format, ...)
{
  std::string head = get_log_head("error");
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  std::va_list args;
  va_start(args, format);
  log(head, format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////

void Log::debug(const char* format, ...)
{
  std::string head = get_log_head("debug");
  std::lock_guard<std::mutex> guard(mMutex);  // make the guard
  std::va_list args;
  va_start(args, format);
  log(head, format, args);
  va_end(args);
}

///////////////////////////////////////////////////////////////////////////////
