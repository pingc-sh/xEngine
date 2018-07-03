///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

#include <cstdint>
#include <netinet/in.h>

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

class IPv6
{
private:
  struct sockaddr_in6 mAddr;
  bool mSinAddrStrDone;             // if mSAddr to mSAddrStr is done
  char mAddrStr[INET6_ADDRSTRLEN];  // 46 bytes, ipv6 max 45 digits + null ending

public:
  IPv6();                           // init with empty value
  IPv6(const IPv6& rhs);            // init from other IPv6 object
  IPv6& operator=(const IPv6&);     // copy from other IPv6 object
  ~IPv6() {}
  bool operator==(IPv6& rhs);       // check if equal
  bool operator!=(IPv6& rhs);       // check if not equal

public:
  static const socklen_t SockLen;   // sockaddr_in6 length, used in msg sendto

public:
  int init(const char* ipv6, unsigned short port);
  unsigned short port();            // get port, ntohs
  void port(unsigned short port);   // set port, htons
  struct sockaddr_in6* addr();      // get ipv6 sock addr
  const char* addrStr();            // get ipv6 string
};

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
