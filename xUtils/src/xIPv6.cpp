///////////////////////////////////////////////////////////////////////////////

#include <cstring>
#include <arpa/inet.h>
#include "xIPv6.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::IPv6;

///////////////////////////////////////////////////////////////////////////////

const socklen_t IPv6::SockLen = sizeof(struct sockaddr_in6);

///////////////////////////////////////////////////////////////////////////////

IPv6::IPv6()
{
  std::memset(&mAddr, 0, IPv6::SockLen);
  std::memset(&mAddrStr, 0, INET6_ADDRSTRLEN);
  mAddr.sin6_family = AF_INET6;
  mSinAddrStrDone = false;
}

///////////////////////////////////////////////////////////////////////////////

IPv6::IPv6(const IPv6& rhs)
{
  memcpy(this, &rhs, sizeof(rhs));
}

///////////////////////////////////////////////////////////////////////////////

IPv6& IPv6::operator=(const IPv6& rhs)
{
  if(this != &rhs) {
    memcpy(this, &rhs, sizeof(rhs));
  }
  return *this;
}

///////////////////////////////////////////////////////////////////////////////

bool IPv6::operator==(IPv6& rhs)
{
  // if same object return true
  if(this == &rhs) {
    return true;
  }

  // if port is not same then return false
  if(port() != rhs.port()) {
    return false;
  }
  // if 16 bytes ipv6(s6_addr) data is not same then return false
  if(memcmp(mAddr.sin6_addr.s6_addr, rhs.addr()->sin6_addr.s6_addr, 16) != 0) {
    return false;
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////

bool IPv6::operator!=(IPv6& rhs) 
{
  return !this->operator==(rhs);
}

///////////////////////////////////////////////////////////////////////////////

int IPv6::init(const char* ipv6, unsigned short port)
{
  if(ipv6 == nullptr) {
    return -1;
  }

  // even if inet_pton failed, mSAddr may get changed, in this case,
  // we take mSAddrStr in invalid state
  mSinAddrStrDone = false;

  // inet_pton returns 1 means success
  if(inet_pton(AF_INET6, ipv6, &mAddr.sin6_addr) != 1) {
    return -1;
  }
  mAddr.sin6_port = htons(port);
  return 0;
}

///////////////////////////////////////////////////////////////////////////////

struct sockaddr_in6* IPv6::addr()
{
  return &mAddr;
}

///////////////////////////////////////////////////////////////////////////////

const char* IPv6::addrStr()
{
  if(mSinAddrStrDone) {
    return mAddrStr;
  }

  // inet_ntop returns nullptr means failed
  if(inet_ntop(AF_INET6, &mAddr.sin6_addr, mAddrStr, INET6_ADDRSTRLEN) == nullptr) {
    return nullptr;
  }
  mSinAddrStrDone = true;
  return mAddrStr;
}

///////////////////////////////////////////////////////////////////////////////

unsigned short IPv6::port()
{
  return ntohs(mAddr.sin6_port);
}

///////////////////////////////////////////////////////////////////////////////

void IPv6::port(unsigned short port)
{
  mAddr.sin6_port = htons(port);
}

///////////////////////////////////////////////////////////////////////////////
