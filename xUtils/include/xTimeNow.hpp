///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

class TimeNow
{
private:
  unsigned long mSeconds;      // total seconds from epoch
  unsigned long mMicroseconds; // microseconds left from total seconds

  short mYear;    // convert from mSeconds
  short mMonth;   // convert from mSeconds
  short mDay;     // convert from mSeconds
  short mHour;    // convert from mSeconds
  short mMinute;  // convert from mSeconds
  short mSecond;  // convert from mSeconds

public:
  TimeNow();
  ~TimeNow() {}

public:
  inline unsigned long seconds() { return mSeconds; }
  inline unsigned long microseconds() { return mMicroseconds; }

public:
  inline short year()   { return mYear; }
  inline short month()  { return mMonth; }
  inline short day()    { return mDay; }
  inline short hour()   { return mHour; }
  inline short minute() { return mMinute; }
  inline short second() { return mSecond; }

public:
  std::string s_str();  // seconds.microseconds+00, example: 1528444001.694750+00
  std::string f_str();  // yyyymmdd-hhmmss.ms+00, example: 20180608-073211.940934+00
};

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
