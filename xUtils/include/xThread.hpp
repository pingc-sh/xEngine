///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <thread>
#include <list>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

class Thread
{
private:
  std::thread* mThread;
  void init();
  void done();

protected:
  int mState;  // thread state: 

  // preWork must return ture to get thread going on
  virtual bool preWork() { return true; }
  virtual void run() {}
  virtual void postWork() {}

public:
  Thread();
  virtual ~Thread();

public:
  inline int  state() { return mState; }
  inline void executor(std::thread* t) { mThread = t; }

  void start();
  void join();
};

///////////////////////////////////////////////////////////////////////////////

class ThreadManager
{
private:
  std::list<Thread*> mThreads;
  std::mutex mMutex;
  bool mIsWaiting;
  bool mIsAllDone;
  int  mThreadNum;

private:
  ThreadManager() : mIsWaiting(false), mIsAllDone(false), mThreadNum(0) {}
  ThreadManager(const ThreadManager&) = delete;
  ThreadManager& operator=(const ThreadManager&) = delete;

public:
  ~ThreadManager() {}
  static ThreadManager& instance();

public:
  void run(Thread* thread);  // start a thread
  void wait();  // called after all threads spawned
};

///////////////////////////////////////////////////////////////////////////////

static ThreadManager& xThreadManager = ThreadManager::instance();

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
