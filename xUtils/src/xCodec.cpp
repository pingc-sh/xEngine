///////////////////////////////////////////////////////////////////////////////

#include <arpa/inet.h> 
#include "xCodec.hpp"

///////////////////////////////////////////////////////////////////////////////

using xEngine::xUtils::Codec;
using xEngine::xUtils::xCodec;

///////////////////////////////////////////////////////////////////////////////

unsigned long long BigLittleSwap64(unsigned long long data)
{
  return ((data & 0xFF00000000000000) >> 56) |
         ((data & 0x00FF000000000000) >> 40) |
         ((data & 0x0000FF0000000000) >> 24) |
         ((data & 0x000000FF00000000) >>  8) |
         ((data & 0x00000000FF000000) <<  8) |
         ((data & 0x0000000000FF0000) << 24) |
         ((data & 0x000000000000FF00) << 40) |
         ((data & 0x00000000000000FF) << 56);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long long htonll(unsigned long long data)
{
  return xCodec.isCpuNetworkOrder() ? data : BigLittleSwap64(data);
}

///////////////////////////////////////////////////////////////////////////////

unsigned long long nlltoh(unsigned long long data)
{
  return xCodec.isCpuNetworkOrder() ? data : BigLittleSwap64(data);
}

///////////////////////////////////////////////////////////////////////////////

Codec::Codec()
{
  union {
    unsigned int i;
    unsigned char s[4];
  } data;
  data.i = 0x12345678;

  mIsCpuNetworkOrder = (0x12 == data.s[0]);
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::pack8byte(char* buf, unsigned long long data)
{
  unsigned long long val = htonll(data);
  buf[0] = val >> 56 & 0xFF;
  buf[1] = val >> 48 & 0xFF;
  buf[2] = val >> 40 & 0xFF;
  buf[3] = val >> 32 & 0xFF;
  buf[4] = val >> 24 & 0xFF;
  buf[5] = val >> 16 & 0xFF;
  buf[6] = val >> 8  & 0xFF;
  buf[7] = val & 0xFF;
  return buf + 8;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::pack4byte(char* buf, unsigned int data)
{
  unsigned int val = htonl(data);
  buf[0] = val >> 24 & 0xFF;
  buf[1] = val >> 16 & 0xFF;
  buf[2] = val >> 8  & 0xFF;
  buf[3] = val & 0xFF;
  return buf + 4;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::pack2byte(char* buf, unsigned short data)
{
  unsigned short val = htons(data);
  buf[0] = val >> 8  & 0xFF;
  buf[1] = val & 0xFF;
  return buf + 2;
}


///////////////////////////////////////////////////////////////////////////////

char* Codec::pack1byte(char* buf, unsigned char data)
{
  buf[0] = data & 0xFF;
  return buf + 1;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::unpack8byte(char* buf, unsigned long long* data)
{
  unsigned long long val = (unsigned long long)buf[0] << 56 | 
                           (unsigned long long)buf[1] << 48 | 
                           (unsigned long long)buf[2] << 40 | 
                           (unsigned long long)buf[3] << 32 |
                           (unsigned long long)buf[4] << 24 | 
                           (unsigned long long)buf[5] << 16 | 
                           (unsigned long long)buf[6] << 8  | 
                           (unsigned long long)buf[7];
  *data = nlltoh(val);
  return buf + 8;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::unpack4byte(char* buf, unsigned int* data)
{
  unsigned int val = (unsigned int)buf[0] << 24 | 
                     (unsigned int)buf[1] << 16 | 
                     (unsigned int)buf[2] << 8  | 
                     (unsigned int)buf[3];
  *data = ntohl(val);
  return buf + 4;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::unpack2byte(char* buf, unsigned short* data)
{
  unsigned short val = (unsigned short)buf[0] << 8 | 
                       (unsigned short)buf[1];
  *data = ntohs(val);
  return buf + 2;
}

///////////////////////////////////////////////////////////////////////////////

char* Codec::unpack1byte(char* buf, unsigned char* data)
{
  *data = buf[0];
  return buf + 1;
}

///////////////////////////////////////////////////////////////////////////////
