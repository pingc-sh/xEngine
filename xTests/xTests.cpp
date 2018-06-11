///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include "xLog.hpp"
#include "xTimeNow.hpp"
#include "xThread.hpp"
#include "xMsgQueue.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::xLog;
using xEngine::xUtils::TimeNow;
using xEngine::xUtils::ManagedThread;
using xEngine::xUtils::xThreadManager;
using xEngine::xUtils::MsgQueue;

///////////////////////////////////////////////////////////////////////////////

class TestThread1 : public ManagedThread
{
protected:
  bool preWork() { xLog.info("TestThread1 pre-work"); return true; }
  void run() {
    xLog.info("TestThread1 run");
    int* i = new int(100);
    msgqueue->send(i);
    xLog.info("TestThread1 send: %d", *i);
  }
  void postWork() { xLog.info("TestThread1 post-work"); }

public:
  MsgQueue<int>* msgqueue;
};

class TestThread2 : public ManagedThread
{
protected:
  bool preWork() { xLog.info("TestThread2 pre-work"); return true; }
  void run() { 
    xLog.info("TestThread2 run");
    int* i = msgqueue->recv();
    xLog.info("TestThread2 recv: %d", *i);
    delete i;
  }
  void postWork() { xLog.info("TestThread2 post-work"); }

public:
  MsgQueue<int> * msgqueue;
};

///////////////////////////////////////////////////////////////////////////////

int main()
{
    xLog.start("xEngine", "./");
    MsgQueue<int>* msgqueue = new MsgQueue<int>;

    TestThread1 t1; t1.msgqueue = msgqueue;
    TestThread2 t2; t2.msgqueue = msgqueue;
    xLog.info("Two thread objects created");
      
    xThreadManager.run(&t1);
    xThreadManager.run(&t2);

    xThreadManager.wait();
    xLog.info("Two thread objects joined");

    xLog.stop();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////
