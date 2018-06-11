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

class ManagedThread
{
private:
  int  mState;
  void init();
  void done();

protected:
  // preWork must return ture to get thread going on
  virtual bool preWork() { return true; }
  virtual void run() {}
  virtual void postWork() {}

public:
  ManagedThread();
  virtual ~ManagedThread();

public:
  inline int state() { return mState; }
  void start();
};

///////////////////////////////////////////////////////////////////////////////

class ThreadManager
{
private:
  std::list<std::tuple<ManagedThread*, std::thread*>> mThreads;
  std::mutex mMutex;
  bool mIsWaiting;
  bool mIsAllDone;

private:
  ThreadManager() : mIsWaiting(false), mIsAllDone(false) {}
  ThreadManager(const ThreadManager&) = delete;
  ThreadManager& operator=(const ThreadManager&) = delete;

public:
  ~ThreadManager() {}
  static ThreadManager& instance();

public:
  int  run(ManagedThread* thread);  // start a thread
  void wait();  // called after all threads spawned
};

///////////////////////////////////////////////////////////////////////////////

static ThreadManager& xThreadManager = ThreadManager::instance();

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
