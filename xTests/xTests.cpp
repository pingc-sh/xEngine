///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "xLog.hpp"
#include "xTimeNow.hpp"
#include "xThread.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::xLog;
using xEngine::xUtils::TimeNow;
using xEngine::xUtils::Thread;
using xEngine::xUtils::xThreadManager;

///////////////////////////////////////////////////////////////////////////////

class TestThread1 : public Thread
{
protected:
  bool preWork() { xLog.info("TestThread1 pre-work"); return true; }
  void run() { xLog.info("TestThread1 run"); }
  void postWork() { xLog.info("TestThread1 post-work"); }
};

class TestThread2 : public Thread
{
protected:
  bool preWork() { xLog.info("TestThread2 pre-work"); return true; }
  void run() { xLog.info("TestThread2 run"); }
  void postWork() { xLog.info("TestThread2 post-work"); }
};

///////////////////////////////////////////////////////////////////////////////

int main()
{
    TimeNow t = TimeNow();
    std::cout << t.s_str() << std::endl;
    std::cout << t.f_str() << std::endl;

    xLog.start("xEngine", "./");
    TestThread1* t1 = new TestThread1();
    TestThread2* t2 = new TestThread2();
    xLog.info("Two thread objects created");
    
    xThreadManager.run(t1);
    xThreadManager.run(t2);
    xThreadManager.wait();
    xLog.info("Two thread objects joined");
    xLog.stop();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
