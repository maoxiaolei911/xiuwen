/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include "XWStringUtil.h"
#include "XWObject.h"
#include "XWStream.h"
#include "XWJArithmeticDecoder.h"


JArithmeticDecoderStats::JArithmeticDecoderStats(int contextSizeA) 
{
    contextSize = contextSizeA;
    cxTab = new uchar[contextSize];
    reset();
}

JArithmeticDecoderStats::~JArithmeticDecoderStats() 
{
    if (cxTab)
        delete [] cxTab;
}

JArithmeticDecoderStats *JArithmeticDecoderStats::copy() 
{
    JArithmeticDecoderStats * stats = new JArithmeticDecoderStats(contextSize);
    memcpy(stats->cxTab, cxTab, contextSize);
    return stats;
}

void JArithmeticDecoderStats::reset() 
{
    memset(cxTab, 0, contextSize);
}

void JArithmeticDecoderStats::copyFrom(JArithmeticDecoderStats *stats) 
{
    memcpy(cxTab, stats->cxTab, contextSize);
}

void JArithmeticDecoderStats::setEntry(uint cx, int i, int mps) 
{
    cxTab[cx] = (i << 1) + mps;
}


uint JArithmeticDecoder::qeTab[47] = {
  0x56010000, 0x34010000, 0x18010000, 0x0AC10000,
  0x05210000, 0x02210000, 0x56010000, 0x54010000,
  0x48010000, 0x38010000, 0x30010000, 0x24010000,
  0x1C010000, 0x16010000, 0x56010000, 0x54010000,
  0x51010000, 0x48010000, 0x38010000, 0x34010000,
  0x30010000, 0x28010000, 0x24010000, 0x22010000,
  0x1C010000, 0x18010000, 0x16010000, 0x14010000,
  0x12010000, 0x11010000, 0x0AC10000, 0x09C10000,
  0x08A10000, 0x05210000, 0x04410000, 0x02A10000,
  0x02210000, 0x01410000, 0x01110000, 0x00850000,
  0x00490000, 0x00250000, 0x00150000, 0x00090000,
  0x00050000, 0x00010000, 0x56010000
};

int JArithmeticDecoder::nmpsTab[47] = {
   1,  2,  3,  4,  5, 38,  7,  8,  9, 10, 11, 12, 13, 29, 15, 16,
  17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
  33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 45, 46
};

int JArithmeticDecoder::nlpsTab[47] = {
   1,  6,  9, 12, 29, 33,  6, 14, 14, 14, 17, 18, 20, 21, 14, 14,
  15, 16, 17, 18, 19, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
  30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 46
};

int JArithmeticDecoder::switchTab[47] = {
  1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};


inline uint JArithmeticDecoder::readByte()
{
    if (limitStream) 
    {
        --dataLen;
        if (dataLen < 0) 
            return 0xff;
    }
    ++nBytesRead;
    return (uint)(str->getChar() & 0xff);
}

JArithmeticDecoder::JArithmeticDecoder()
{
    str = 0;
    dataLen = 0;
    limitStream = false;
    nBytesRead = 0;
}

JArithmeticDecoder::~JArithmeticDecoder()
{
    cleanup();
}

void JArithmeticDecoder::cleanup()
{
    if (limitStream) 
    {
        while (dataLen > 0) 
        {
            buf0 = buf1;
            buf1 = readByte();
        }
    }
}

int JArithmeticDecoder::decodeBit(uint context, JArithmeticDecoderStats *stats)
{
    int iCX = stats->cxTab[context] >> 1;
    int mpsCX = stats->cxTab[context] & 1;
    uint qe = qeTab[iCX];
    a -= qe;
    int bit = 0;
    if (c < a) 
    {
        if (a & 0x80000000) 
            bit = mpsCX;
        else 
        {
            // MPS_EXCHANGE
            if (a < qe) 
            {
	            bit = 1 - mpsCX;
	            if (switchTab[iCX]) 
	                stats->cxTab[context] = (nlpsTab[iCX] << 1) | (1 - mpsCX);
	            else 
	                stats->cxTab[context] = (nlpsTab[iCX] << 1) | mpsCX;
            } 
            else 
            {
	            bit = mpsCX;
	            stats->cxTab[context] = (nmpsTab[iCX] << 1) | mpsCX;
            }
            // RENORMD
            do 
            {
	            if (ct == 0) 
	                byteIn();
	            a <<= 1;
	            c <<= 1;
	            --ct;
            } while (!(a & 0x80000000));
        }
    } 
    else 
    {
        c -= a;
        // LPS_EXCHANGE
        if (a < qe) 
        {
            bit = mpsCX;
            stats->cxTab[context] = (nmpsTab[iCX] << 1) | mpsCX;
        } 
        else 
        {
            bit = 1 - mpsCX;
            if (switchTab[iCX]) 
	            stats->cxTab[context] = (nlpsTab[iCX] << 1) | (1 - mpsCX);
            else 
	            stats->cxTab[context] = (nlpsTab[iCX] << 1) | mpsCX;
        }
        a = qe;
        // RENORMD
        do 
        {
            if (ct == 0) 
	            byteIn();
            a <<= 1;
            c <<= 1;
            --ct;
        } while (!(a & 0x80000000));
    }
    return bit;
}

int JArithmeticDecoder::decodeByte(uint context, JArithmeticDecoderStats *stats)
{
    int byte = 0;
    for (int i = 0; i < 8; ++i) 
        byte = (byte << 1) | decodeBit(context, stats);
    return byte;
}

uint JArithmeticDecoder::decodeIAID(uint codeLen, JArithmeticDecoderStats *stats)
{
    prev = 1;
    for (uint i = 0; i < codeLen; ++i) 
    {
        int bit = decodeBit(prev, stats);
        prev = (prev << 1) | bit;
    }
    return prev - (1 << codeLen);
}

bool  JArithmeticDecoder::decodeInt(int *x, JArithmeticDecoderStats *stats)
{
  int s;
  uint v;
  int i;

  prev = 1;
  s = decodeIntBit(stats);
  if (decodeIntBit(stats)) 
  {
    if (decodeIntBit(stats)) 
    {
      if (decodeIntBit(stats)) 
      {
				if (decodeIntBit(stats)) 
				{
	  			if (decodeIntBit(stats)) 
	  			{
	    			v = 0;
	    			for (i = 0; i < 32; ++i) 
	    			{
	      			v = (v << 1) | decodeIntBit(stats);
	    			}
	    			v += 4436;
	  			} 
	  			else 
	  			{
	    			v = 0;
	    			for (i = 0; i < 12; ++i) 
	    			{
	      			v = (v << 1) | decodeIntBit(stats);
	    			}
	    			v += 340;
	  			}
				} 
				else 
				{
	  			v = 0;
	  			for (i = 0; i < 8; ++i) 
	  			{
	    			v = (v << 1) | decodeIntBit(stats);
	  			}
	  			v += 84;
				}
      } 
      else 
      {
				v = 0;
				for (i = 0; i < 6; ++i) 
				{
	  			v = (v << 1) | decodeIntBit(stats);
				}
				v += 20;
      }
    } 
    else 
    {
      v = decodeIntBit(stats);
      v = (v << 1) | decodeIntBit(stats);
      v = (v << 1) | decodeIntBit(stats);
      v = (v << 1) | decodeIntBit(stats);
      v += 4;
    }
  } 
  else 
  {
    v = decodeIntBit(stats);
    v = (v << 1) | decodeIntBit(stats);
  }

  if (s) {
    if (v == 0) {
      return false;
    }
    *x = -(int)v;
  } else {
    *x = (int)v;
  }
  return true;
}

void JArithmeticDecoder::byteIn()
{
  if (buf0 == 0xff) 
  {
    if (buf1 > 0x8f) 
    {
      if (limitStream) 
      {
				buf0 = buf1;
				buf1 = readByte();
				c = c + 0xff00 - (buf0 << 8);
      }
      ct = 8;
    } 
    else 
    {
      buf0 = buf1;
      buf1 = readByte();
      c = c + 0xfe00 - (buf0 << 9);
      ct = 7;
    }
  } 
  else 
  {
    buf0 = buf1;
    buf1 = readByte();
    c = c + 0xff00 - (buf0 << 8);
    ct = 8;
  }
}

int JArithmeticDecoder::decodeIntBit(JArithmeticDecoderStats *stats)
{
    int bit = decodeBit(prev, stats);
    if (prev < 0x100) 
        prev = (prev << 1) | bit;
    else 
        prev = (((prev << 1) | bit) & 0x1ff) | 0x100;
    return bit;
}

void JArithmeticDecoder::restart(int dataLenA)
{
  uint cAdd;
  bool prevFF;
  int k, nBits;

  if (dataLen >= 0) 
  {
    dataLen = dataLenA;
  } 
  else if (dataLen == -1) 
  {
    dataLen = dataLenA;
    buf1 = readByte();
  } 
  else 
  {
    k = (-dataLen - 1) * 8 - ct;
    dataLen = dataLenA;
    cAdd = 0;
    prevFF = false;
    while (k > 0) 
    {
      buf0 = readByte();
      if (prevFF) 
      {
				cAdd += 0xfe00 - (buf0 << 9);
				nBits = 7;
      } 
      else 
      {
				cAdd += 0xff00 - (buf0 << 8);
				nBits = 8;
      }
      prevFF = buf0 == 0xff;
      if (k > nBits) 
      {
				cAdd <<= nBits;
				k -= nBits;
      } 
      else 
      {
				cAdd <<= k;
				ct = nBits - k;
				k = 0;
      }
    }
    c += cAdd;
    buf1 = readByte();
  }
}

void JArithmeticDecoder::start()
{
    buf0 = readByte();
    buf1 = readByte();

    // INITDEC
    c = (buf0 ^ 0xff) << 16;
    byteIn();
    c <<= 7;
    ct -= 7;
    a = 0x80000000;
}

