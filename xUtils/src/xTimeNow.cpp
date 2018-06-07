///////////////////////////////////////////////////////////////////////////////

#include <chrono>
#include <sstream> 
#include <iomanip>
#include "xTimeNow.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::TimeNow;

///////////////////////////////////////////////////////////////////////////////

TimeNow::TimeNow()
{
  auto ms_duration = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();

  mSeconds      = ms_duration / 1000000;
  mMicroseconds = ms_duration % 1000000;

  std::time_t tt = mSeconds;
  std::tm* t_utc = std::gmtime(&tt);

  mYear   = t_utc->tm_year + 1900;
  mMonth  = t_utc->tm_mon + 1;
  mDay    = t_utc->tm_mday;
  mHour   = t_utc->tm_hour;
  mMinute = t_utc->tm_min;
  mSecond = t_utc->tm_sec;
}

///////////////////////////////////////////////////////////////////////////////

std::string TimeNow::s_str()
{
  std::stringstream time;
  time << mSeconds
       << "." 
       << std::right << std::setfill('0') << std::setw(6) << mMicroseconds
       << "+00";
  return time.str();
}

///////////////////////////////////////////////////////////////////////////////

std::string TimeNow::f_str()
{
  std::stringstream time;
  time << mYear
       << std::right << std::setfill('0') << std::setw(2) << mMonth
       << std::right << std::setfill('0') << std::setw(2) << mDay
       << "-"
       << std::right << std::setfill('0') << std::setw(2) << mHour
       << std::right << std::setfill('0') << std::setw(2) << mMinute
       << std::right << std::setfill('0') << std::setw(2) << mSecond
       << "."
       << std::right << std::setfill('0') << std::setw(6) << mMicroseconds
       << "+00";
  return time.str();
}

///////////////////////////////////////////////////////////////////////////////
