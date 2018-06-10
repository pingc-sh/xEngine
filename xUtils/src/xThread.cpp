///////////////////////////////////////////////////////////////////////////////

#include <condition_variable>
#include "xThread.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::Thread;
using xEngine::xUtils::ThreadManager;

///////////////////////////////////////////////////////////////////////////////

// thread states
static const int THREAD_STATE_UNKNOWN = 0;
static const int THREAD_STATE_CREATED = 1;
static const int THREAD_STATE_STARTED = 2;
static const int THREAD_STATE_STOPPED = 3;

///////////////////////////////////////////////////////////////////////////////

class ThreadNotify
{
private:
  std::condition_variable mCondVar;
  std::mutex mMutex;
  int mNotify;

private:
  ThreadNotify() : mNotify(0) {}
  ThreadNotify(const ThreadNotify&) = delete;
  ThreadNotify& operator=(const ThreadNotify&) = delete;

public:
  ~ThreadNotify() {}
  static ThreadNotify& instance();

public:
  void notify();  // when a thread is done, notify ThreadManager
  void wait();    // ThreadManager waits for a thread done
};

///////////////////////////////////////////////////////////////////////////////

ThreadNotify& ThreadNotify::instance()
{
  static ThreadNotify _instance;
  return _instance;
}

///////////////////////////////////////////////////////////////////////////////

void ThreadNotify::notify()
{
  { // to make wait efficient on lock
    std::unique_lock<std::mutex> ulock(mMutex);
    mNotify++;
  }
  mCondVar.notify_one();
}

///////////////////////////////////////////////////////////////////////////////

void ThreadNotify::wait()
{
  std::unique_lock<std::mutex> ulock(mMutex);
  mCondVar.wait(ulock, [=](){ return mNotify>0; });
  mNotify--;
}

///////////////////////////////////////////////////////////////////////////////

Thread::Thread()
{
  mThread = nullptr;
  mState  = THREAD_STATE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////

Thread::~Thread()
{
  if(mThread != nullptr) {
    delete mThread;
  }
  mState  = THREAD_STATE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////

void Thread::init()
{
  mState = THREAD_STATE_STARTED;
}

///////////////////////////////////////////////////////////////////////////////

void Thread::done()
{
  mState = THREAD_STATE_STOPPED;
  ThreadNotify::instance().notify();
}

///////////////////////////////////////////////////////////////////////////////

void Thread::start()
{
  init();  // init this thread
  if(preWork()) { // if preWork is good then go on, otherwise not excute
    run();
    postWork();
  }
  done();  // thread done, notify ThreadManager
}

///////////////////////////////////////////////////////////////////////////////

void Thread::join()
{
  mThread->join();
}

///////////////////////////////////////////////////////////////////////////////

ThreadManager& ThreadManager::instance()
{
  static ThreadManager _instance;
  return _instance;
}

///////////////////////////////////////////////////////////////////////////////

void* RunThread(Thread* thread)
{
  thread->start();
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////

void ThreadManager::run(Thread* thread)
{
  if(thread == nullptr) {
    return;
  }

  std::thread* t = new std::thread(RunThread, thread);
  if(t == nullptr) {
    return;
  }
  thread->executor(t);

  std::unique_lock<std::mutex> ulock(mMutex);
  mThreads.push_back(thread);
  mThreadNum++;
}

///////////////////////////////////////////////////////////////////////////////

void ThreadManager::wait()
{
  { // make sure there is only one wait
    std::unique_lock<std::mutex> ulock(mMutex);
    if(mIsWaiting) {
      return;
    }
    mIsWaiting = true;
  }

  // wait and join all other threads, if all done then return.
  while(!mIsAllDone) {
    ThreadNotify::instance().wait();
    std::unique_lock<std::mutex> ulock(mMutex);

    for(auto ite = mThreads.begin(); ite != mThreads.end(); ++ite) {
      if((*ite)->state() != THREAD_STATE_STOPPED) {
        continue;
      }
      (*ite)->join();
      mThreads.erase(ite++);  // std::thread object is deleted in Thread destructor
    }

    if(mThreads.empty()) {
      mIsAllDone = true;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
