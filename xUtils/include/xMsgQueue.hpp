///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <list>
#include <mutex>
#include <condition_variable>
#include <chrono>

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

template <typename T>
class MsgQueue
{
private:
  std::list<T*> mMessages;           // message list
  std::mutex mMutex;                 // to protect list operations
  bool mMessaging;                   // messaging check
  std::condition_variable mCondVar;  // to sync with different threads

public:
  MsgQueue() : mMessaging(false) {}
  ~MsgQueue() {}
  bool empty();

public:
  void send(T* msg);
  T* recv();
  T* timedRecv(int milliseconds);
};

///////////////////////////////////////////////////////////////////////////////

template<typename T> 
bool MsgQueue<T>::empty()
{
  std::unique_lock<std::mutex> ulock(mMutex);
  return mMessages.empty();
}

///////////////////////////////////////////////////////////////////////////////

template<typename T> 
void MsgQueue<T>::send(T* msg)
{
  std::unique_lock<std::mutex> ulock(mMutex);
  mMessages.push_back(msg);
  mMessaging = true;
  mCondVar.notify_all();  // notify all recvivers
}

///////////////////////////////////////////////////////////////////////////////

template<typename T> 
T* MsgQueue<T>::recv()
{
  std::unique_lock<std::mutex> ulock(mMutex);
  mCondVar.wait(ulock, [=]{ return mMessaging; });
  mMessaging = false;

  T* msg = mMessages.front();
  mMessages.pop_front();
  return msg;
}

///////////////////////////////////////////////////////////////////////////////

template<typename T> 
T* MsgQueue<T>::timedRecv(int milliseconds)
{
  T* msg = nullptr;
  std::unique_lock<std::mutex> ulock(mMutex);
  if(mCondVar.wait_for(ulock, 
                       std::chrono::milliseconds(milliseconds), 
                       [=]{return mMessaging;})) {
    mMessaging = false;
    
    msg = mMessages.front();
    mMessages.pop_front();
  }
  return msg;
}

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
