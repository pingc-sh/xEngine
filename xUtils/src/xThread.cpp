///////////////////////////////////////////////////////////////////////////////

#include <condition_variable>
#include "xThread.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::ManagedThread;
using xEngine::xUtils::ThreadManager;

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

// thread states
static const int THREAD_STATE_UNKNOWN = 0;
static const int THREAD_STATE_CREATED = 1;
static const int THREAD_STATE_STARTED = 2;
static const int THREAD_STATE_STOPPED = 3;

///////////////////////////////////////////////////////////////////////////////

ManagedThread::ManagedThread()
{
  mState = THREAD_STATE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////

ManagedThread::~ManagedThread()
{
  mState = THREAD_STATE_UNKNOWN;
}

///////////////////////////////////////////////////////////////////////////////

void ManagedThread::init()
{
  mState = THREAD_STATE_STARTED;
}

///////////////////////////////////////////////////////////////////////////////

void ManagedThread::done()
{
  mState = THREAD_STATE_STOPPED;
  ThreadNotify::instance().notify();
}

///////////////////////////////////////////////////////////////////////////////

void ManagedThread::start()
{
  init();
  if(preWork()) {  // if preWork is good then go on, otherwise not excute
    run();
    postWork();
  }
  done();  // thread done, notify ThreadManager
}

///////////////////////////////////////////////////////////////////////////////

ThreadManager& ThreadManager::instance()
{
  static ThreadManager _instance;
  return _instance;
}

///////////////////////////////////////////////////////////////////////////////

void* RunThread(ManagedThread* thread)
{
  thread->start();
  return nullptr;
}

///////////////////////////////////////////////////////////////////////////////

int ThreadManager::run(ManagedThread* thread)
{
  if(thread == nullptr) {
    return -1;
  }
  std::thread* t = new std::thread(RunThread, thread);
  if(t == nullptr) {
    return -1;
  }

  std::lock_guard<std::mutex> guard(mMutex);
  mThreads.push_back(std::make_tuple(thread, t));
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

void ThreadManager::wait()
{
  { // make sure there is only one wait
    std::lock_guard<std::mutex> guard(mMutex);;
    if(mIsWaiting) {
      return;
    }
    mIsWaiting = true;
  }

  // wait and join all other threads, if all done then return.
  while(!mIsAllDone) {
    ThreadNotify::instance().wait();
    std::lock_guard<std::mutex> guard(mMutex);

    for(auto ite = mThreads.begin(); ite != mThreads.end(); ++ite) {
      ManagedThread* thread = std::get<0>(*ite);
      if(thread->state() != THREAD_STATE_STOPPED) {
        continue;
      }
      std::thread* t = std::get<1>(*ite);
      t->join();
      delete t;
      mThreads.erase(ite++);
    }
    if(mThreads.empty()) {
      mIsAllDone = true;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
