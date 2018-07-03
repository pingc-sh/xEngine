///////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////////////

namespace xEngine {
namespace xUtils {

///////////////////////////////////////////////////////////////////////////////

class Codec
{
private:
  bool mIsCpuNetworkOrder;  // network order is big endian, x86 is little endian

private:
  Codec();
  Codec(const Codec&) = delete;
  Codec& operator=(const Codec&) = delete;

public:
  ~Codec() {}
  static Codec& instance();

public:
  bool isCpuNetworkOrder() { return mIsCpuNetworkOrder; }

public:
  char* pack8byte(char* buf, unsigned long long data);
  char* pack4byte(char* buf, unsigned int data);
  char* pack2byte(char* buf, unsigned short data);
  char* pack1byte(char* buf, unsigned char data);

  char* unpack8byte(char* buf, unsigned long long* data);
  char* unpack4byte(char* buf, unsigned int* data);
  char* unpack2byte(char* buf, unsigned short* data);
  char* unpack1byte(char* buf, unsigned char* data);
};

///////////////////////////////////////////////////////////////////////////////

static Codec& xCodec = Codec::instance();

///////////////////////////////////////////////////////////////////////////////

}  // end of namespace xUtils
}  // end of namespace xEngine

///////////////////////////////////////////////////////////////////////////////
