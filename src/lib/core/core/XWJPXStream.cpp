/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <limits.h>
#include "XWApplication.h"
#include "XWJArithmeticDecoder.h"
#include "XWJPXStream.h"

#define jpxNContexts        19

#define jpxContextSigProp    0	// 0 - 8: significance prop and cleanup
#define jpxContextSign       9	// 9 - 13: sign
#define jpxContextMagRef    14	// 14 -16: magnitude refinement
#define jpxContextRunLength 17	// cleanup: run length
#define jpxContextUniform   18	// cleanup: first signif coeff


#define jpxPassSigProp       0
#define jpxPassMagRef        1
#define jpxPassCleanup       2

static uint sigPropContext[3][3][5][3] = {
  {{{ 0, 0, 0 },   // horiz=0, vert=0, diag=0
    { 1, 1, 3 },   // horiz=0, vert=0, diag=1
    { 2, 2, 6 },   // horiz=0, vert=0, diag=2
    { 2, 2, 8 },   // horiz=0, vert=0, diag=3
    { 2, 2, 8 }},  // horiz=0, vert=0, diag=4
   {{ 5, 3, 1 },   // horiz=0, vert=1, diag=0
    { 6, 3, 4 },   // horiz=0, vert=1, diag=1
    { 6, 3, 7 },   // horiz=0, vert=1, diag=2
    { 6, 3, 8 },   // horiz=0, vert=1, diag=3
    { 6, 3, 8 }},  // horiz=0, vert=1, diag=4
   {{ 8, 4, 2 },   // horiz=0, vert=2, diag=0
    { 8, 4, 5 },   // horiz=0, vert=2, diag=1
    { 8, 4, 7 },   // horiz=0, vert=2, diag=2
    { 8, 4, 8 },   // horiz=0, vert=2, diag=3
    { 8, 4, 8 }}}, // horiz=0, vert=2, diag=4
  {{{ 3, 5, 1 },   // horiz=1, vert=0, diag=0
    { 3, 6, 4 },   // horiz=1, vert=0, diag=1
    { 3, 6, 7 },   // horiz=1, vert=0, diag=2
    { 3, 6, 8 },   // horiz=1, vert=0, diag=3
    { 3, 6, 8 }},  // horiz=1, vert=0, diag=4
   {{ 7, 7, 2 },   // horiz=1, vert=1, diag=0
    { 7, 7, 5 },   // horiz=1, vert=1, diag=1
    { 7, 7, 7 },   // horiz=1, vert=1, diag=2
    { 7, 7, 8 },   // horiz=1, vert=1, diag=3
    { 7, 7, 8 }},  // horiz=1, vert=1, diag=4
   {{ 8, 7, 2 },   // horiz=1, vert=2, diag=0
    { 8, 7, 5 },   // horiz=1, vert=2, diag=1
    { 8, 7, 7 },   // horiz=1, vert=2, diag=2
    { 8, 7, 8 },   // horiz=1, vert=2, diag=3
    { 8, 7, 8 }}}, // horiz=1, vert=2, diag=4
  {{{ 4, 8, 2 },   // horiz=2, vert=0, diag=0
    { 4, 8, 5 },   // horiz=2, vert=0, diag=1
    { 4, 8, 7 },   // horiz=2, vert=0, diag=2
    { 4, 8, 8 },   // horiz=2, vert=0, diag=3
    { 4, 8, 8 }},  // horiz=2, vert=0, diag=4
   {{ 7, 8, 2 },   // horiz=2, vert=1, diag=0
    { 7, 8, 5 },   // horiz=2, vert=1, diag=1
    { 7, 8, 7 },   // horiz=2, vert=1, diag=2
    { 7, 8, 8 },   // horiz=2, vert=1, diag=3
    { 7, 8, 8 }},  // horiz=2, vert=1, diag=4
   {{ 8, 8, 2 },   // horiz=2, vert=2, diag=0
    { 8, 8, 5 },   // horiz=2, vert=2, diag=1
    { 8, 8, 7 },   // horiz=2, vert=2, diag=2
    { 8, 8, 8 },   // horiz=2, vert=2, diag=3
    { 8, 8, 8 }}}  // horiz=2, vert=2, diag=4
};

static uint signContext[5][5][2] = {
  {{ 13, 1 },  // horiz=-2, vert=-2
   { 13, 1 },  // horiz=-2, vert=-1
   { 12, 1 },  // horiz=-2, vert= 0
   { 11, 1 },  // horiz=-2, vert=+1
   { 11, 1 }}, // horiz=-2, vert=+2
  {{ 13, 1 },  // horiz=-1, vert=-2
   { 13, 1 },  // horiz=-1, vert=-1
   { 12, 1 },  // horiz=-1, vert= 0
   { 11, 1 },  // horiz=-1, vert=+1
   { 11, 1 }}, // horiz=-1, vert=+2
  {{ 10, 1 },  // horiz= 0, vert=-2
   { 10, 1 },  // horiz= 0, vert=-1
   {  9, 0 },  // horiz= 0, vert= 0
   { 10, 0 },  // horiz= 0, vert=+1
   { 10, 0 }}, // horiz= 0, vert=+2
  {{ 11, 0 },  // horiz=+1, vert=-2
   { 11, 0 },  // horiz=+1, vert=-1
   { 12, 0 },  // horiz=+1, vert= 0
   { 13, 0 },  // horiz=+1, vert=+1
   { 13, 0 }}, // horiz=+1, vert=+2
  {{ 11, 0 },  // horiz=+2, vert=-2
   { 11, 0 },  // horiz=+2, vert=-1
   { 12, 0 },  // horiz=+2, vert= 0
   { 13, 0 },  // horiz=+2, vert=+1
   { 13, 0 }}, // horiz=+2, vert=+2
};

#define idwtAlpha  -1.586134342059924
#define idwtBeta   -0.052980118572961
#define idwtGamma   0.882911075530934
#define idwtDelta   0.443506852043971
#define idwtKappa   1.230174104914001
#define idwtIKappa  (1.0 / idwtKappa)

#define fracBits 16

//------------------------------------------------------------------------

// floor(x / y)
#define jpxFloorDiv(x, y) ((x) / (y))

// floor(x / 2^y)
#define jpxFloorDivPow2(x, y) ((x) >> (y))

// ceil(x / y)
#define jpxCeilDiv(x, y) (((x) + (y) - 1) / (y))

// ceil(x / 2^y)
#define jpxCeilDivPow2(x, y) (((x) + (1 << (y)) - 1) >> (y))


#if 1 //----- disable coverage tracking

#define cover(idx)

#else //----- enable coverage tracking

class JPXCover {
public:

  JPXCover(int sizeA);
  ~JPXCover();
  void incr(int idx);

private:

  int size, used;
  int *data;
};

JPXCover::JPXCover(int sizeA) {
  size = sizeA;
  used = -1;
  data = (int *)malloc(size * sizeof(int));
  memset(data, 0, size * sizeof(int));
}

JPXCover::~JPXCover() {
  int i;

  printf("JPX coverage:\n");
  for (i = 0; i <= used; ++i) {
    printf("  %4d: %8d\n", i, data[i]);
  }
  free(data);
}

void JPXCover::incr(int idx) {
  if (idx < size) {
    ++data[idx];
    if (idx > used) {
      used = idx;
    }
  }
}

JPXCover jpxCover(150);

#define cover(idx) jpxCover.incr(idx)

#endif //----- coverage tracking



XWJPXStream::XWJPXStream(XWStream *strA)
    :XWFilterStream(strA)
{
  bufStr = new XWBufStream(str, 2);

  nComps = 0;
  bpc = NULL;
  width = height = 0;
  haveCS = false;
  havePalette = false;
  haveCompMap = false;
  haveChannelDefn = false;

  img.tiles = NULL;
  bitBuf = 0;
  bitBufLen = 0;
  bitBufSkip = false;
  byteCount = 0;
}

XWJPXStream::~XWJPXStream()
{
    close();
    if (bufStr)
        delete bufStr;
}

void XWJPXStream::close()
{
  JPXTile *tile;
  JPXTileComp *tileComp;
  JPXResLevel *resLevel;
  JPXPrecinct *precinct;
  JPXSubband *subband;
  JPXCodeBlock *cb;
  uint comp, i, k, r, pre, sb;

  free(bpc);
  bpc = NULL;
  if (havePalette) {
  	if (palette.bpc)
    free(palette.bpc);
    if (palette.c)
    free(palette.c);
    havePalette = false;
  }
  if (haveCompMap) {
  	if (compMap.comp)
    free(compMap.comp);
    if (compMap.type)
    free(compMap.type);
    if (compMap.pComp)
    free(compMap.pComp);
    haveCompMap = false;
  }
  if (haveChannelDefn) {
  	if (channelDefn.idx)
    free(channelDefn.idx);
    if (channelDefn.type)
    free(channelDefn.type);
    if (channelDefn.assoc)
    free(channelDefn.assoc);
    haveChannelDefn = false;
  }

  if (img.tiles) {
    for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
      tile = &img.tiles[i];
      if (tile->tileComps) {
	for (comp = 0; comp < img.nComps; ++comp) {
	  tileComp = &tile->tileComps[comp];
	  if (tileComp->quantSteps)
	  free(tileComp->quantSteps);
	  if (tileComp->data)
	  free(tileComp->data);
	  if (tileComp->buf)
	  free(tileComp->buf);
	  if (tileComp->resLevels) {
	    for (r = 0; r <= tileComp->nDecompLevels; ++r) {
	      resLevel = &tileComp->resLevels[r];
	      if (resLevel->precincts) {
		for (pre = 0; pre < 1; ++pre) {
		  precinct = &resLevel->precincts[pre];
		  if (precinct->subbands) {
		    for (sb = 0; sb < (uint)(r == 0 ? 1 : 3); ++sb) {
		      subband = &precinct->subbands[sb];
		      if (subband->inclusion)
		      free(subband->inclusion);
		      if (subband->zeroBitPlane)
		      free(subband->zeroBitPlane);
		      if (subband->cbs) {
			for (k = 0; k < subband->nXCBs * subband->nYCBs; ++k) {
			  cb = &subband->cbs[k];
			  if (cb->dataLen)
			  free(cb->dataLen);
			  if (cb->touched)
			  free(cb->touched);
			  if (cb->arithDecoder) {
			    delete cb->arithDecoder;
			  }
			  if (cb->stats) {
			    delete cb->stats;
			  }
			}
			if (subband->cbs)
			free(subband->cbs);
		      }
		    }
		    if (precinct->subbands)
		    free(precinct->subbands);
		  }
		}
		if (img.tiles[i].tileComps[comp].resLevels[r].precincts)
		free(img.tiles[i].tileComps[comp].resLevels[r].precincts);
	      }
	    }
	    if (img.tiles[i].tileComps[comp].resLevels)
	    free(img.tiles[i].tileComps[comp].resLevels);
	  }
	}
	if (img.tiles[i].tileComps)
	free(img.tiles[i].tileComps);
      }
    }
    if (img.tiles)
    {
    	free(img.tiles);
    	img.tiles = NULL;
    }
  }
  bufStr->close();
}

int XWJPXStream::getChar()
{
  int c;

  if (readBufLen < 8) {
    fillReadBuf();
  }
  if (readBufLen == 8) {
    c = readBuf & 0xff;
    readBufLen = 0;
  } else if (readBufLen > 8) {
    c = (readBuf >> (readBufLen - 8)) & 0xff;
    readBufLen -= 8;
  } else if (readBufLen == 0) {
    c = EOF;
  } else {
    c = (readBuf << (8 - readBufLen)) & 0xff;
    readBufLen = 0;
  }
  return c;
}

void XWJPXStream::getImageParams(int *bitsPerComponent, int *csMode)
{
  uint boxType, boxLen, dataLen, csEnum;
  uint bpc1, dummy, i;
  int csMeth, csPrec, csPrec1, dummy2;
  int csMode1;
  bool haveBPC, haveCSMode;

  csPrec = 0; // make gcc happy
  haveBPC = haveCSMode = false;
  bufStr->reset();
  if (bufStr->lookChar() == 0xff) {
    getImageParams2(bitsPerComponent, csMode);
  } else {
    while (readBoxHdr(&boxType, &boxLen, &dataLen)) {
      if (boxType == 0x6a703268) { // JP2 header
	cover(0);
	// skip the superbox
      } else if (boxType == 0x69686472) { // image header
	cover(1);
	if (readULong(&dummy) &&
	    readULong(&dummy) &&
	    readUWord(&dummy) &&
	    readUByte(&bpc1) &&
	    readUByte(&dummy) &&
	    readUByte(&dummy) &&
	    readUByte(&dummy)) {
	  *bitsPerComponent = bpc1 + 1;
	  haveBPC = true;
	}
      } else if (boxType == 0x636F6C72) { // color specification
	cover(2);
	if (readByte(&csMeth) &&
	    readByte(&csPrec1) &&
	    readByte(&dummy2)) {
	  if (csMeth == 1) {
	    if (readULong(&csEnum)) {
	      csMode1 = STREAM_CS_NONE;
	      if (csEnum == jpxCSBiLevel ||
		  csEnum == jpxCSGrayscale) {
		csMode1 = STREAM_CS_DEVICEGRAY;
	      } else if (csEnum == jpxCSCMYK) {
		csMode1 = STREAM_CS_DEVICECMYK;
	      } else if (csEnum == jpxCSsRGB ||
			 csEnum == jpxCSCISesRGB ||
			 csEnum == jpxCSROMMRGB) {
		csMode1 = STREAM_CS_DEVICERGB;
	      }
	      if (csMode1 != STREAM_CS_NONE &&
		  (!haveCSMode || csPrec1 > csPrec)) {
		*csMode = csMode1;
		csPrec = csPrec1;
		haveCSMode = true;
	      }
	      for (i = 0; i < dataLen - 7; ++i) {
		bufStr->getChar();
	      }
	    }
	  } else {
	    for (i = 0; i < dataLen - 3; ++i) {
	      bufStr->getChar();
	    }
	  }
	}
      } else if (boxType == 0x6A703263) { // codestream
	cover(3);
	if (!(haveBPC && haveCSMode)) {
	  getImageParams2(bitsPerComponent, csMode);
	}
	break;
      } else {
	cover(4);
	for (i = 0; i < dataLen; ++i) {
	  bufStr->getChar();
	}
      }
    }
  }
  bufStr->close();
}

XWString *XWJPXStream::getPSFilter(int, const char *)
{
    return 0;
}

bool XWJPXStream::isBinary(bool)
{
    return str->isBinary(true);
}

int XWJPXStream::lookChar()
{
  int c;

  if (readBufLen < 8) {
    fillReadBuf();
  }
  if (readBufLen == 8) {
    c = readBuf & 0xff;
  } else if (readBufLen > 8) {
    c = (readBuf >> (readBufLen - 8)) & 0xff;
  } else if (readBufLen == 0) {
    c = EOF;
  } else {
    c = (readBuf << (8 - readBufLen)) & 0xff;
  }
  return c;
}

void XWJPXStream::reset()
{
  bufStr->reset();
  if (readBoxes()) {
    curY = img.yOffset;
  } else {
    // readBoxes reported an error, so we go immediately to EOF
    curY = img.ySize;
  }
  curX = img.xOffset;
  curComp = 0;
  readBufLen = 0;
}

void XWJPXStream::fillReadBuf()
{
  JPXTileComp *tileComp;
  uint tileIdx, tx, ty;
  int pix, pixBits;

  do {
    if (curY >= img.ySize) {
      return;
    }
    tileIdx = ((curY - img.yTileOffset) / img.yTileSize) * img.nXTiles
              + (curX - img.xTileOffset) / img.xTileSize;
#if 1 //~ ignore the palette, assume the PDF ColorSpace object is valid
    tileComp = &img.tiles[tileIdx].tileComps[curComp];
#else
    tileComp = &img.tiles[tileIdx].tileComps[havePalette ? 0 : curComp];
#endif
    tx = jpxCeilDiv((curX - img.xTileOffset) % img.xTileSize, tileComp->hSep);
    ty = jpxCeilDiv((curY - img.yTileOffset) % img.yTileSize, tileComp->vSep);
    pix = (int)tileComp->data[ty * (tileComp->x1 - tileComp->x0) + tx];
    pixBits = tileComp->prec;
#if 1 //~ ignore the palette, assume the PDF ColorSpace object is valid
    if (++curComp == img.nComps) {
#else
    if (havePalette) {
      if (pix >= 0 && pix < palette.nEntries) {
	pix = palette.c[pix * palette.nComps + curComp];
      } else {
	pix = 0;
      }
      pixBits = palette.bpc[curComp];
    }
    if (++curComp == (uint)(havePalette ? palette.nComps : img.nComps)) {
#endif
      curComp = 0;
      if (++curX == img.xSize) {
	curX = img.xOffset;
	++curY;
	if (pixBits < 8) {
	  pix <<= 8 - pixBits;
	  pixBits = 8;
	}
      }
    }
    if (pixBits == 8) {
      readBuf = (readBuf << 8) | (pix & 0xff);
    } else {
      readBuf = (readBuf << pixBits) | (pix & ((1 << pixBits) - 1));
    }
    readBufLen += pixBits;
  } while (readBufLen < 8);
}

uint XWJPXStream::finishBitBuf()
{
  if (bitBufSkip) {
    bufStr->getChar();
    --byteCount;
  }
  return byteCount;
}

void XWJPXStream::getImageParams2(int *bitsPerComponent, int *csMode)
{
  int segType;
  uint segLen, nComps1, bpc1, dummy, i;

  while (readMarkerHdr(&segType, &segLen)) {
    if (segType == 0x51) { // SIZ - image and tile size
      cover(5);
      if (readUWord(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readULong(&dummy) &&
	  readUWord(&nComps1) &&
	  readUByte(&bpc1)) {
	*bitsPerComponent = (bpc1 & 0x7f) + 1;
	// if there's no color space info, take a guess
	if (nComps1 == 1) {
	  *csMode = STREAM_CS_DEVICEGRAY;
	} else if (nComps1 == 3) {
	  *csMode = STREAM_CS_DEVICERGB;
	} else if (nComps1 == 4) {
	  *csMode = STREAM_CS_DEVICECMYK;
	}
      }
      break;
    } else {
      cover(6);
      if (segLen > 2) {
	for (i = 0; i < segLen - 2; ++i) {
	  bufStr->getChar();
	}
      }
    }
  }
}

bool XWJPXStream::inverseMultiCompAndDC(JPXTile *tile)
{
  JPXTileComp *tileComp;
  int coeff, d0, d1, d2, t, minVal, maxVal, zeroVal;
  int *dataPtr;
  uint j, comp, x, y;

  //----- inverse multi-component transform

  if (tile->multiComp == 1) {
    cover(86);
    if (img.nComps < 3 ||
	tile->tileComps[0].hSep != tile->tileComps[1].hSep ||
	tile->tileComps[0].vSep != tile->tileComps[1].vSep ||
	tile->tileComps[1].hSep != tile->tileComps[2].hSep ||
	tile->tileComps[1].vSep != tile->tileComps[2].vSep) {
      return false;
    }

    // inverse irreversible multiple component transform
    if (tile->tileComps[0].transform == 0) {
      cover(87);
      j = 0;
      for (y = 0; y < tile->tileComps[0].y1 - tile->tileComps[0].y0; ++y) {
	for (x = 0; x < tile->tileComps[0].x1 - tile->tileComps[0].x0; ++x) {
	  d0 = tile->tileComps[0].data[j];
	  d1 = tile->tileComps[1].data[j];
	  d2 = tile->tileComps[2].data[j];
	  tile->tileComps[0].data[j] = (int)(d0 + 1.402 * d2 + 0.5);
	  tile->tileComps[1].data[j] =
	      (int)(d0 - 0.34413 * d1 - 0.71414 * d2 + 0.5);
	  tile->tileComps[2].data[j] = (int)(d0 + 1.772 * d1 + 0.5);
	  ++j;
	}
      }

    // inverse reversible multiple component transform
    } else {
      cover(88);
      j = 0;
      for (y = 0; y < tile->tileComps[0].y1 - tile->tileComps[0].y0; ++y) {
	for (x = 0; x < tile->tileComps[0].x1 - tile->tileComps[0].x0; ++x) {
	  d0 = tile->tileComps[0].data[j];
	  d1 = tile->tileComps[1].data[j];
	  d2 = tile->tileComps[2].data[j];
	  tile->tileComps[1].data[j] = t = d0 - ((d2 + d1) >> 2);
	  tile->tileComps[0].data[j] = d2 + t;
	  tile->tileComps[2].data[j] = d1 + t;
	  ++j;
	}
      }
    }
  }

  //----- DC level shift
  for (comp = 0; comp < img.nComps; ++comp) {
    tileComp = &tile->tileComps[comp];

    // signed: clip
    if (tileComp->sgned) {
      cover(89);
      minVal = -(1 << (tileComp->prec - 1));
      maxVal = (1 << (tileComp->prec - 1)) - 1;
      dataPtr = tileComp->data;
      for (y = 0; y < tileComp->y1 - tileComp->y0; ++y) {
	for (x = 0; x < tileComp->x1 - tileComp->x0; ++x) {
	  coeff = *dataPtr;
	  if (tileComp->transform == 0) {
	    cover(109);
	    coeff >>= fracBits;
	  }
	  if (coeff < minVal) {
	    cover(110);
	    coeff = minVal;
	  } else if (coeff > maxVal) {
	    cover(111);
	    coeff = maxVal;
	  }
	  *dataPtr++ = coeff;
	}
      }

    // unsigned: inverse DC level shift and clip
    } else {
      cover(90);
      maxVal = (1 << tileComp->prec) - 1;
      zeroVal = 1 << (tileComp->prec - 1);
      dataPtr = tileComp->data;
      for (y = 0; y < tileComp->y1 - tileComp->y0; ++y) {
	for (x = 0; x < tileComp->x1 - tileComp->x0; ++x) {
	  coeff = *dataPtr;
	  if (tileComp->transform == 0) {
	    cover(112);
	    coeff >>= fracBits;
	  }
	  coeff += zeroVal;
	  if (coeff < 0) {
	    cover(113);
	    coeff = 0;
	  } else if (coeff > maxVal) {
	    cover(114);
	    coeff = maxVal;
	  }
	  *dataPtr++ = coeff;
	}
      }
    }
  }

  return true;
}

void XWJPXStream::inverseTransform(JPXTileComp *tileComp)
{
  JPXResLevel *resLevel;
  JPXPrecinct *precinct;
  JPXSubband *subband;
  JPXCodeBlock *cb;
  int *coeff0, *coeff;
  char *touched0, *touched;
  uint qStyle, guard, eps, shift;
  int shift2;
  double mu;
  int val;
  uint r, cbX, cbY, x, y;

  cover(68);

  //----- (NL)LL subband (resolution level 0)

  resLevel = &tileComp->resLevels[0];
  precinct = &resLevel->precincts[0];
  subband = &precinct->subbands[0];

  // i-quant parameters
  qStyle = tileComp->quantStyle & 0x1f;
  guard = (tileComp->quantStyle >> 5) & 7;
  if (qStyle == 0) {
    cover(69);
    eps = (tileComp->quantSteps[0] >> 3) & 0x1f;
    shift = guard + eps - 1;
    mu = 0; // make gcc happy
  } else {
    cover(70);
    shift = guard - 1 + tileComp->prec;
    mu = (double)(0x800 + (tileComp->quantSteps[0] & 0x7ff)) / 2048.0;
  }
  if (tileComp->transform == 0) {
    cover(71);
    shift += fracBits;
  }

  // do fixed point adjustment and dequantization on (NL)LL
  cb = subband->cbs;
  for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
    for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
      for (y = cb->y0, coeff0 = cb->coeffs, touched0 = cb->touched;
	   y < cb->y1;
	   ++y, coeff0 += tileComp->w, touched0 += tileComp->cbW) {
	for (x = cb->x0, coeff = coeff0, touched = touched0;
	     x < cb->x1;
	     ++x, ++coeff, ++touched) {
	  val = *coeff;
	  if (val != 0) {
	    shift2 = shift - (cb->nZeroBitPlanes + cb->len + *touched);
	    if (shift2 > 0) {
	      cover(94);
	      if (val < 0) {
		val = (val << shift2) - (1 << (shift2 - 1));
	      } else {
		val = (val << shift2) + (1 << (shift2 - 1));
	      }
	    } else {
	      cover(95);
	      val >>= -shift2;
	    }
	    if (qStyle == 0) {
	      cover(96);
	      if (tileComp->transform == 0) {
		cover(97);
		val &= -1 << fracBits;
	      }
	    } else {
	      cover(98);
	      val = (int)((double)val * mu);
	    }
	  }
	  *coeff = val;
	}
      }
      ++cb;
    }
  }

  //----- IDWT for each level

  for (r = 1; r <= tileComp->nDecompLevels; ++r) {
    resLevel = &tileComp->resLevels[r];

    // (n)LL is already in the upper-left corner of the
    // tile-component data array -- interleave with (n)HL/LH/HH
    // and inverse transform to get (n-1)LL, which will be stored
    // in the upper-left corner of the tile-component data array
    inverseTransformLevel(tileComp, r, resLevel);
  }
}

void XWJPXStream::inverseTransformLevel(JPXTileComp *tileComp,
			                            uint r, 
			                            JPXResLevel *resLevel)
{
  JPXPrecinct *precinct;
  JPXSubband *subband;
  JPXCodeBlock *cb;
  int *coeff0, *coeff;
  char *touched0, *touched;
  uint qStyle, guard, eps, shift, t;
  int shift2;
  double mu;
  int val;
  int *dataPtr, *bufPtr;
  uint nx1, nx2, ny1, ny2, offset;
  uint x, y, sb, cbX, cbY;

  //----- fixed-point adjustment and dequantization

  qStyle = tileComp->quantStyle & 0x1f;
  guard = (tileComp->quantStyle >> 5) & 7;
  precinct = &resLevel->precincts[0];
  for (sb = 0; sb < 3; ++sb) {

    // i-quant parameters
    if (qStyle == 0) {
      cover(100);
      eps = (tileComp->quantSteps[3*r - 2 + sb] >> 3) & 0x1f;
      shift = guard + eps - 1;
      mu = 0; // make gcc happy
    } else {
      cover(101);
      shift = guard + tileComp->prec;
      if (sb == 2) {
	cover(102);
	++shift;
      }
      t = tileComp->quantSteps[qStyle == 1 ? 0 : (3*r - 2 + sb)];
      mu = (double)(0x800 + (t & 0x7ff)) / 2048.0;
    }
    if (tileComp->transform == 0) {
      cover(103);
      shift += fracBits;
    }

    // fixed point adjustment and dequantization
    subband = &precinct->subbands[sb];
    cb = subband->cbs;
    for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
      for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
	for (y = cb->y0, coeff0 = cb->coeffs, touched0 = cb->touched;
	     y < cb->y1;
	     ++y, coeff0 += tileComp->w, touched0 += tileComp->cbW) {
	  for (x = cb->x0, coeff = coeff0, touched = touched0;
	       x < cb->x1;
	       ++x, ++coeff, ++touched) {
	    val = *coeff;
	    if (val != 0) {
	      shift2 = shift - (cb->nZeroBitPlanes + cb->len + *touched);
	      if (shift2 > 0) {
		cover(74);
		if (val < 0) {
		  val = (val << shift2) - (1 << (shift2 - 1));
		} else {
		  val = (val << shift2) + (1 << (shift2 - 1));
		}
	      } else {
		cover(75);
		val >>= -shift2;
	      }
	      if (qStyle == 0) {
		cover(76);
		if (tileComp->transform == 0) {
		  val &= -1 << fracBits;
		}
	      } else {
		cover(77);
		val = (int)((double)val * mu);
	      }
	    }
	    *coeff = val;
	  }
	}
	++cb;
      }
    }
  }

  //----- inverse transform

  // compute the subband bounds:
  //    0   nx1  nx2
  //    |    |    |
  //    v    v    v
  //   +----+----+
  //   | LL | HL | <- 0
  //   +----+----+
  //   | LH | HH | <- ny1
  //   +----+----+
  //               <- ny2
  nx1 = precinct->subbands[1].x1 - precinct->subbands[1].x0;
  nx2 = nx1 + precinct->subbands[0].x1 - precinct->subbands[0].x0;
  ny1 = precinct->subbands[0].y1 - precinct->subbands[0].y0;
  ny2 = ny1 + precinct->subbands[1].y1 - precinct->subbands[1].y0;

  // horizontal (row) transforms
  if (r == tileComp->nDecompLevels) {
    offset = 3 + (tileComp->x0 & 1);
  } else {
    offset = 3 + (tileComp->resLevels[r+1].x0 & 1);
  }
  for (y = 0, dataPtr = tileComp->data; y < ny2; ++y, dataPtr += tileComp->w) {
    if (precinct->subbands[0].x0 == precinct->subbands[1].x0) {
      // fetch LL/LH
      for (x = 0, bufPtr = tileComp->buf + offset;
	   x < nx1;
	   ++x, bufPtr += 2) {
	*bufPtr = dataPtr[x];
      }
      // fetch HL/HH
      for (x = nx1, bufPtr = tileComp->buf + offset + 1;
	   x < nx2;
	   ++x, bufPtr += 2) {
	*bufPtr = dataPtr[x];
      }
    } else {
      // fetch LL/LH
      for (x = 0, bufPtr = tileComp->buf + offset + 1;
	   x < nx1;
	   ++x, bufPtr += 2) {
	*bufPtr = dataPtr[x];
      }
      // fetch HL/HH
      for (x = nx1, bufPtr = tileComp->buf + offset;
	   x < nx2;
	   ++x, bufPtr += 2) {
	*bufPtr = dataPtr[x];
      }
    }
    inverseTransform1D(tileComp, tileComp->buf, offset, nx2);
    for (x = 0, bufPtr = tileComp->buf + offset; x < nx2; ++x, ++bufPtr) {
      dataPtr[x] = *bufPtr;
    }
  }

  // vertical (column) transforms
  if (r == tileComp->nDecompLevels) {
    offset = 3 + (tileComp->y0 & 1);
  } else {
    offset = 3 + (tileComp->resLevels[r+1].y0 & 1);
  }
  for (x = 0, dataPtr = tileComp->data; x < nx2; ++x, ++dataPtr) {
    if (precinct->subbands[1].y0 == precinct->subbands[0].y0) {
      // fetch LL/HL
      for (y = 0, bufPtr = tileComp->buf + offset;
	   y < ny1;
	   ++y, bufPtr += 2) {
	*bufPtr = dataPtr[y * tileComp->w];
      }
      // fetch LH/HH
      for (y = ny1, bufPtr = tileComp->buf + offset + 1;
	   y < ny2;
	   ++y, bufPtr += 2) {
	*bufPtr = dataPtr[y * tileComp->w];
      }
    } else {
      // fetch LL/HL
      for (y = 0, bufPtr = tileComp->buf + offset + 1;
	   y < ny1;
	   ++y, bufPtr += 2) {
	*bufPtr = dataPtr[y * tileComp->w];
      }
      // fetch LH/HH
      for (y = ny1, bufPtr = tileComp->buf + offset;
	   y < ny2;
	   ++y, bufPtr += 2) {
	*bufPtr = dataPtr[y * tileComp->w];
      }
    }
    inverseTransform1D(tileComp, tileComp->buf, offset, ny2);
    for (y = 0, bufPtr = tileComp->buf + offset; y < ny2; ++y, ++bufPtr) {
      dataPtr[y * tileComp->w] = *bufPtr;
    }
  }
}

void XWJPXStream::inverseTransform1D(JPXTileComp *tileComp, 
	                                   int *data,
				                             uint offset, 
				                             uint n)
{
  uint end, i;

  //----- special case for length = 1
  if (n == 1) {
    cover(79);
    if (offset == 4) {
      cover(104);
      *data >>= 1;
    }

  } else {
    cover(80);

    end = offset + n;

    //----- extend right
    data[end] = data[end - 2];
    if (n == 2) {
      cover(81);
      data[end+1] = data[offset + 1];
      data[end+2] = data[offset];
      data[end+3] = data[offset + 1];
    } else {
      cover(82);
      data[end+1] = data[end - 3];
      if (n == 3) {
	cover(105);
	data[end+2] = data[offset + 1];
	data[end+3] = data[offset + 2];
      } else {
	cover(106);
	data[end+2] = data[end - 4];
	if (n == 4) {
	  cover(107);
	  data[end+3] = data[offset + 1];
	} else {
	  cover(108);
	  data[end+3] = data[end - 5];
	}
      }
    }

    //----- extend left
    data[offset - 1] = data[offset + 1];
    data[offset - 2] = data[offset + 2];
    data[offset - 3] = data[offset + 3];
    if (offset == 4) {
      cover(83);
      data[0] = data[offset + 4];
    }

    //----- 9-7 irreversible filter

    if (tileComp->transform == 0) {
      cover(84);
      // step 1 (even)
      for (i = 1; i <= end + 2; i += 2) {
	data[i] = (int)(idwtKappa * data[i]);
      }
      // step 2 (odd)
      for (i = 0; i <= end + 3; i += 2) {
	data[i] = (int)(idwtIKappa * data[i]);
      }
      // step 3 (even)
      for (i = 1; i <= end + 2; i += 2) {
	data[i] = (int)(data[i] - idwtDelta * (data[i-1] + data[i+1]));
      }
      // step 4 (odd)
      for (i = 2; i <= end + 1; i += 2) {
	data[i] = (int)(data[i] - idwtGamma * (data[i-1] + data[i+1]));
      }
      // step 5 (even)
      for (i = 3; i <= end; i += 2) {
	data[i] = (int)(data[i] - idwtBeta * (data[i-1] + data[i+1]));
      }
      // step 6 (odd)
      for (i = 4; i <= end - 1; i += 2) {
	data[i] = (int)(data[i] - idwtAlpha * (data[i-1] + data[i+1]));
      }

    //----- 5-3 reversible filter

    } else {
      cover(85);
      // step 1 (even)
      for (i = 3; i <= end; i += 2) {
	data[i] -= (data[i-1] + data[i+1] + 2) >> 2;
      }
      // step 2 (odd)
      for (i = 4; i < end; i += 2) {
	data[i] += (data[i-1] + data[i+1]) >> 1;
      }
    }
  }
}

bool XWJPXStream::readBits(int nBits, uint *x)
{
  int c;

  while (bitBufLen < nBits) {
    if (byteCount == 0 || (c = bufStr->getChar()) == EOF) {
      return false;
    }
    --byteCount;
    if (bitBufSkip) {
      bitBuf = (bitBuf << 7) | (c & 0x7f);
      bitBufLen += 7;
    } else {
      bitBuf = (bitBuf << 8) | (c & 0xff);
      bitBufLen += 8;
    }
    bitBufSkip = c == 0xff;
  }
  *x = (bitBuf >> (bitBufLen - nBits)) & ((1 << nBits) - 1);
  bitBufLen -= nBits;
  return true;
}

bool XWJPXStream::readBoxes()
{
  uint boxType, boxLen, dataLen;
  uint bpc1, compression, unknownColorspace, ipr;
  uint i, j;

  haveImgHdr = false;

  // check for a naked JPEG 2000 codestream (without the JP2/JPX
  // wrapper) -- this appears to be a violation of the PDF spec, but
  // Acrobat allows it
  if (bufStr->lookChar() == 0xff) {
    cover(7);
    xwApp->error("Naked JPEG 2000 codestream, missing JP2/JPX wrapper");
    if (!readCodestream(0)) {
      return false;
    }
    nComps = img.nComps;
    bpc = (uint *)malloc(nComps * sizeof(uint));
    for (i = 0; i < nComps; ++i) {
      bpc[i] = img.tiles[0].tileComps[i].prec;
    }
    width = img.xSize - img.xOffset;
    height = img.ySize - img.yOffset;
    return true;
  }

  while (readBoxHdr(&boxType, &boxLen, &dataLen)) {
    switch (boxType) {
    case 0x6a703268:		// JP2 header
      // this is a grouping box ('superbox') which has no real
      // contents and doesn't appear to be used consistently, i.e.,
      // some things which should be subboxes of the JP2 header box
      // show up outside of it - so we simply ignore the JP2 header
      // box
      cover(8);
      break;
    case 0x69686472:		// image header
      cover(9);
      if (!readULong(&height) ||
	  !readULong(&width) ||
	  !readUWord(&nComps) ||
	  !readUByte(&bpc1) ||
	  !readUByte(&compression) ||
	  !readUByte(&unknownColorspace) ||
	  !readUByte(&ipr)) {
	xwApp->error("Unexpected EOF in JPX stream");
	return false;
      }
      if (compression != 7) {
	xwApp->error("Unknown compression type in JPX stream");
	return false;
      }
      bpc = (uint *)malloc(nComps * sizeof(uint));
      for (i = 0; i < nComps; ++i) {
	bpc[i] = bpc1;
      }
      haveImgHdr = true;
      break;
    case 0x62706363:		// bits per component
      cover(10);
      if (!haveImgHdr) {
	xwApp->error("Found bits per component box before image header box in JPX stream");
	return false;
      }
      if (dataLen != nComps) {
	xwApp->error("Invalid bits per component box in JPX stream");
	return false;
      }
      for (i = 0; i < nComps; ++i) {
	if (!readUByte(&bpc[i])) {
	  xwApp->error("Unexpected EOF in JPX stream");
	  return false;
	}
      }
      break;
    case 0x636F6C72:		// color specification
      cover(11);
      if (!readColorSpecBox(dataLen)) {
	return false;
      }
      break;
    case 0x70636c72:		// palette
      cover(12);
      if (!readUWord(&palette.nEntries) ||
	  !readUByte(&palette.nComps)) {
	xwApp->error("Unexpected EOF in JPX stream");
	return false;
      }
      palette.bpc = (uint *)malloc(palette.nComps * sizeof(uint));
      palette.c = (int *)malloc(palette.nEntries * palette.nComps * sizeof(int));
      for (i = 0; i < palette.nComps; ++i) {
	if (!readUByte(&palette.bpc[i])) {
	  xwApp->error("Unexpected EOF in JPX stream");
	  return false;
	}
	++palette.bpc[i];
      }
      for (i = 0; i < palette.nEntries; ++i) {
	for (j = 0; j < palette.nComps; ++j) {
	  if (!readNBytes(((palette.bpc[j] & 0x7f) + 7) >> 3,
			  (palette.bpc[j] & 0x80) ? true : false,
			  &palette.c[i * palette.nComps + j])) {
	    xwApp->error("Unexpected EOF in JPX stream");
	    return false;
	  }
	}
      }
      havePalette = true;
      break;
    case 0x636d6170:		// component mapping
      cover(13);
      compMap.nChannels = dataLen / 4;
      compMap.comp = (uint *)malloc(compMap.nChannels * sizeof(uint));
      compMap.type = (uint *)malloc(compMap.nChannels * sizeof(uint));
      compMap.pComp = (uint *)malloc(compMap.nChannels * sizeof(uint));
      for (i = 0; i < compMap.nChannels; ++i) {
	if (!readUWord(&compMap.comp[i]) ||
	    !readUByte(&compMap.type[i]) ||
	    !readUByte(&compMap.pComp[i])) {
	  xwApp->error("Unexpected EOF in JPX stream");
	  return false;
	}
      }
      haveCompMap = true;
      break;
    case 0x63646566:		// channel definition
      cover(14);
      if (!readUWord(&channelDefn.nChannels)) {
	xwApp->error("Unexpected EOF in JPX stream");
	return false;
      }
      channelDefn.idx = (uint *)malloc(channelDefn.nChannels * sizeof(uint));
      channelDefn.type = (uint *)malloc(channelDefn.nChannels * sizeof(uint));
      channelDefn.assoc = (uint *)malloc(channelDefn.nChannels * sizeof(uint));
      for (i = 0; i < channelDefn.nChannels; ++i) {
	if (!readUWord(&channelDefn.idx[i]) ||
	    !readUWord(&channelDefn.type[i]) ||
	    !readUWord(&channelDefn.assoc[i])) {
	  xwApp->error("Unexpected EOF in JPX stream");
	  return false;
	}
      }
      haveChannelDefn = true;
      break;
    case 0x6A703263:		// contiguous codestream
      cover(15);
      if (!bpc) {
	xwApp->error("JPX stream is missing the image header box");
      }
      if (!haveCS) {
	xwApp->error("JPX stream has no supported color spec");
      }
      if (!readCodestream(dataLen)) {
	return false;
      }
      break;
    default:
      cover(16);
      for (i = 0; i < dataLen; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Unexpected EOF in JPX stream");
	  return false;
	}
      }
      break;
    }
  }
  return true;
}

bool XWJPXStream::readBoxHdr(uint *boxType, uint *boxLen, uint *dataLen)
{
  uint len, lenH;

  if (!readULong(&len) ||
      !readULong(boxType)) {
    return false;
  }
  if (len == 1) {
    if (!readULong(&lenH) || !readULong(&len)) {
      return false;
    }
    if (lenH) {
      xwApp->error("JPX stream contains a box larger than 2^32 bytes");
      return false;
    }
    *boxLen = len;
    *dataLen = len - 16;
  } else if (len == 0) {
    *boxLen = 0;
    *dataLen = 0;
  } else {
    *boxLen = len;
    *dataLen = len - 8;
  }
  return true;
}

bool XWJPXStream::readByte(int *x)
{
  int c0;

  if ((c0 = bufStr->getChar()) == EOF) {
    return false;
  }
  *x = c0;
  if (c0 & 0x80) {
    *x |= -1 - 0xff;
  }
  return true;
}

bool XWJPXStream::readCodeBlockData(JPXTileComp *tileComp,
			                        JPXResLevel *,
			                        JPXPrecinct *,
			                        JPXSubband *,
			                        uint res, uint sb,
			                        JPXCodeBlock *cb)
{
  int *coeff0, *coeff1, *coeff;
  char *touched0, *touched1, *touched;
  uint horiz, vert, diag, all, cx, xorBit;
  int horizSign, vertSign, bit;
  int segSym;
  uint i, x, y0, y1;

  if (cb->arithDecoder) {
    cover(63);
    cb->arithDecoder->restart(cb->dataLen[0]);
  } else {
    cover(64);
    cb->arithDecoder = new JArithmeticDecoder();
    cb->arithDecoder->setStream(bufStr, cb->dataLen[0]);
    cb->arithDecoder->start();
    cb->stats = new JArithmeticDecoderStats(jpxNContexts);
    cb->stats->setEntry(jpxContextSigProp, 4, 0);
    cb->stats->setEntry(jpxContextRunLength, 3, 0);
    cb->stats->setEntry(jpxContextUniform, 46, 0);
  }

  for (i = 0; i < cb->nCodingPasses; ++i) {
    if ((tileComp->codeBlockStyle & 0x04) && i > 0) {
      cb->arithDecoder->setStream(bufStr, cb->dataLen[i]);
      cb->arithDecoder->start();
    }

    switch (cb->nextPass) {

    //----- significance propagation pass
    case jpxPassSigProp:
      cover(65);
      for (y0 = cb->y0, coeff0 = cb->coeffs, touched0 = cb->touched;
	   y0 < cb->y1;
	   y0 += 4, coeff0 += 4 * tileComp->w,
	     touched0 += 4 << tileComp->codeBlockW) {
	for (x = cb->x0, coeff1 = coeff0, touched1 = touched0;
	     x < cb->x1;
	     ++x, ++coeff1, ++touched1) {
	  for (y1 = 0, coeff = coeff1, touched = touched1;
	       y1 < 4 && y0+y1 < cb->y1;
	       ++y1, coeff += tileComp->w, touched += tileComp->cbW) {
	    if (!*coeff) {
	      horiz = vert = diag = 0;
	      horizSign = vertSign = 2;
	      if (x > cb->x0) {
		if (coeff[-1]) {
		  ++horiz;
		  horizSign += coeff[-1] < 0 ? -1 : 1;
		}
		if (y0+y1 > cb->y0) {
		  diag += coeff[-(int)tileComp->w - 1] ? 1 : 0;
		}
		if (y0+y1 < cb->y1 - 1 &&
		    (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		  diag += coeff[tileComp->w - 1] ? 1 : 0;
		}
	      }
	      if (x < cb->x1 - 1) {
		if (coeff[1]) {
		  ++horiz;
		  horizSign += coeff[1] < 0 ? -1 : 1;
		}
		if (y0+y1 > cb->y0) {
		  diag += coeff[-(int)tileComp->w + 1] ? 1 : 0;
		}
		if (y0+y1 < cb->y1 - 1 &&
		    (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		  diag += coeff[tileComp->w + 1] ? 1 : 0;
		}
	      }
	      if (y0+y1 > cb->y0) {
		if (coeff[-(int)tileComp->w]) {
		  ++vert;
		  vertSign += coeff[-(int)tileComp->w] < 0 ? -1 : 1;
		}
	      }
	      if (y0+y1 < cb->y1 - 1 &&
		  (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		if (coeff[tileComp->w]) {
		  ++vert;
		  vertSign += coeff[tileComp->w] < 0 ? -1 : 1;
		}
	      }
	      cx = sigPropContext[horiz][vert][diag][res == 0 ? 1 : sb];
	      if (cx != 0) {
		if (cb->arithDecoder->decodeBit(cx, cb->stats)) {
		  cx = signContext[horizSign][vertSign][0];
		  xorBit = signContext[horizSign][vertSign][1];
		  if (cb->arithDecoder->decodeBit(cx, cb->stats) ^ xorBit) {
		    *coeff = -1;
		  } else {
		    *coeff = 1;
		  }
		}
		*touched = 1;
	      }
	    }
	  }
	}
      }
      ++cb->nextPass;
      break;

    //----- magnitude refinement pass
    case jpxPassMagRef:
      cover(66);
      for (y0 = cb->y0, coeff0 = cb->coeffs, touched0 = cb->touched;
	   y0 < cb->y1;
	   y0 += 4, coeff0 += 4 * tileComp->w,
	     touched0 += 4 << tileComp->codeBlockW) {
	for (x = cb->x0, coeff1 = coeff0, touched1 = touched0;
	     x < cb->x1;
	     ++x, ++coeff1, ++touched1) {
	  for (y1 = 0, coeff = coeff1, touched = touched1;
	       y1 < 4 && y0+y1 < cb->y1;
	       ++y1, coeff += tileComp->w, touched += tileComp->cbW) {
	    if (*coeff && !*touched) {
	      if (*coeff == 1 || *coeff == -1) {
		all = 0;
		if (x > cb->x0) {
		  all += coeff[-1] ? 1 : 0;
		  if (y0+y1 > cb->y0) {
		    all += coeff[-(int)tileComp->w - 1] ? 1 : 0;
		  }
		  if (y0+y1 < cb->y1 - 1 &&
		      (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		    all += coeff[tileComp->w - 1] ? 1 : 0;
		  }
		}
		if (x < cb->x1 - 1) {
		  all += coeff[1] ? 1 : 0;
		  if (y0+y1 > cb->y0) {
		    all += coeff[-(int)tileComp->w + 1] ? 1 : 0;
		  }
		  if (y0+y1 < cb->y1 - 1 &&
		      (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		    all += coeff[tileComp->w + 1] ? 1 : 0;
		  }
		}
		if (y0+y1 > cb->y0) {
		  all += coeff[-(int)tileComp->w] ? 1 : 0;
		}
		if (y0+y1 < cb->y1 - 1 &&
		    (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		  all += coeff[tileComp->w] ? 1 : 0;
		}
		cx = all ? 15 : 14;
	      } else {
		cx = 16;
	      }
	      bit = cb->arithDecoder->decodeBit(cx, cb->stats);
	      if (*coeff < 0) {
		*coeff = (*coeff << 1) - bit;
	      } else {
		*coeff = (*coeff << 1) + bit;
	      }
	      *touched = 1;
	    }
	  }
	}
      }
      ++cb->nextPass;
      break;

    //----- cleanup pass
    case jpxPassCleanup:
      cover(67);
      for (y0 = cb->y0, coeff0 = cb->coeffs, touched0 = cb->touched;
	   y0 < cb->y1;
	   y0 += 4, coeff0 += 4 * tileComp->w,
	     touched0 += 4 << tileComp->codeBlockW) {
	for (x = cb->x0, coeff1 = coeff0, touched1 = touched0;
	     x < cb->x1;
	     ++x, ++coeff1, ++touched1) {
	  y1 = 0;
	  if (y0 + 3 < cb->y1 &&
	      !(*touched1) &&
	      !(touched1[tileComp->cbW]) &&
	      !(touched1[2 * tileComp->cbW]) &&
	      !(touched1[3 * tileComp->cbW]) &&
	      (x == cb->x0 || y0 == cb->y0 ||
	       !coeff1[-(int)tileComp->w - 1]) &&
	      (y0 == cb->y0 ||
	       !coeff1[-(int)tileComp->w]) &&
	      (x == cb->x1 - 1 || y0 == cb->y0 ||
	       !coeff1[-(int)tileComp->w + 1]) &&
	      (x == cb->x0 ||
	       (!coeff1[-1] &&
		!coeff1[tileComp->w - 1] &&
		!coeff1[2 * tileComp->w - 1] && 
		!coeff1[3 * tileComp->w - 1])) &&
	      (x == cb->x1 - 1 ||
	       (!coeff1[1] &&
		!coeff1[tileComp->w + 1] &&
		!coeff1[2 * tileComp->w + 1] &&
		!coeff1[3 * tileComp->w + 1])) &&
	      ((tileComp->codeBlockStyle & 0x08) ||
	       ((x == cb->x0 || y0+4 == cb->y1 ||
		 !coeff1[4 * tileComp->w - 1]) &&
		(y0+4 == cb->y1 ||
		 !coeff1[4 * tileComp->w]) &&
		(x == cb->x1 - 1 || y0+4 == cb->y1 ||
		 !coeff1[4 * tileComp->w + 1])))) {
	    if (cb->arithDecoder->decodeBit(jpxContextRunLength, cb->stats)) {
	      y1 = cb->arithDecoder->decodeBit(jpxContextUniform, cb->stats);
	      y1 = (y1 << 1) |
		   cb->arithDecoder->decodeBit(jpxContextUniform, cb->stats);
	      coeff = &coeff1[y1 * tileComp->w];
	      cx = signContext[2][2][0];
	      xorBit = signContext[2][2][1];
	      if (cb->arithDecoder->decodeBit(cx, cb->stats) ^ xorBit) {
		*coeff = -1;
	      } else {
		*coeff = 1;
	      }
	      ++y1;
	    } else {
	      y1 = 4;
	    }
	  }
	  for (coeff = &coeff1[y1 * tileComp->w],
		 touched = &touched1[y1 << tileComp->codeBlockW];
	       y1 < 4 && y0 + y1 < cb->y1;
	       ++y1, coeff += tileComp->w, touched += tileComp->cbW) {
	    if (!*touched) {
	      horiz = vert = diag = 0;
	      horizSign = vertSign = 2;
	      if (x > cb->x0) {
		if (coeff[-1]) {
		  ++horiz;
		  horizSign += coeff[-1] < 0 ? -1 : 1;
		}
		if (y0+y1 > cb->y0) {
		  diag += coeff[-(int)tileComp->w - 1] ? 1 : 0;
		}
		if (y0+y1 < cb->y1 - 1 &&
		    (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		  diag += coeff[tileComp->w - 1] ? 1 : 0;
		}
	      }
	      if (x < cb->x1 - 1) {
		if (coeff[1]) {
		  ++horiz;
		  horizSign += coeff[1] < 0 ? -1 : 1;
		}
		if (y0+y1 > cb->y0) {
		  diag += coeff[-(int)tileComp->w + 1] ? 1 : 0;
		}
		if (y0+y1 < cb->y1 - 1 &&
		    (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		  diag += coeff[tileComp->w + 1] ? 1 : 0;
		}
	      }
	      if (y0+y1 > cb->y0) {
		if (coeff[-(int)tileComp->w]) {
		  ++vert;
		  vertSign += coeff[-(int)tileComp->w] < 0 ? -1 : 1;
		}
	      }
	      if (y0+y1 < cb->y1 - 1 &&
		  (!(tileComp->codeBlockStyle & 0x08) || y1 < 3)) {
		if (coeff[tileComp->w]) {
		  ++vert;
		  vertSign += coeff[tileComp->w] < 0 ? -1 : 1;
		}
	      }
	      cx = sigPropContext[horiz][vert][diag][res == 0 ? 1 : sb];
	      if (cb->arithDecoder->decodeBit(cx, cb->stats)) {
		cx = signContext[horizSign][vertSign][0];
		xorBit = signContext[horizSign][vertSign][1];
		if (cb->arithDecoder->decodeBit(cx, cb->stats) ^ xorBit) {
		  *coeff = -1;
		} else {
		  *coeff = 1;
		}
	      }
	    } else {
	      *touched = 0;
	    }
	  }
	}
      }
      ++cb->len;
      // look for a segmentation symbol
      if (tileComp->codeBlockStyle & 0x20) {
	segSym = cb->arithDecoder->decodeBit(jpxContextUniform,
					     cb->stats) << 3;
	segSym |= cb->arithDecoder->decodeBit(jpxContextUniform,
					      cb->stats) << 2;
	segSym |= cb->arithDecoder->decodeBit(jpxContextUniform,
					      cb->stats) << 1;
	segSym |= cb->arithDecoder->decodeBit(jpxContextUniform,
					      cb->stats);
	if (segSym != 0x0a) {
	  // in theory this should be a fatal error, but it seems to
	  // be problematic
	  xwApp->error("Missing or invalid segmentation symbol in JPX stream");
	}
      }
      cb->nextPass = jpxPassSigProp;
      break;
    }

    if (tileComp->codeBlockStyle & 0x02) {
      cb->stats->reset();
      cb->stats->setEntry(jpxContextSigProp, 4, 0);
      cb->stats->setEntry(jpxContextRunLength, 3, 0);
      cb->stats->setEntry(jpxContextUniform, 46, 0);
    }

    if (tileComp->codeBlockStyle & 0x04) {
      cb->arithDecoder->cleanup();
    }
  }

  cb->arithDecoder->cleanup();
  return true;
}

bool XWJPXStream::readColorSpecBox(uint dataLen)
{
  JPXColorSpec newCS;
  uint csApprox, csEnum;
  uint i;
  bool ok;

  ok = false;
  if (!readUByte(&newCS.meth) ||
      !readByte(&newCS.prec) ||
      !readUByte(&csApprox)) {
    goto err;
  }
  switch (newCS.meth) {
  case 1:			// enumerated colorspace
    cover(17);
    if (!readULong(&csEnum)) {
      goto err;
    }
    newCS.enumerated.type = (JPXColorSpaceType)csEnum;
    switch (newCS.enumerated.type) {
    case jpxCSBiLevel:
      ok = true;
      break;
    case jpxCSYCbCr1:
      ok = true;
      break;
    case jpxCSYCbCr2:
      ok = true;
      break;
    case jpxCSYCBCr3:
      ok = true;
      break;
    case jpxCSPhotoYCC:
      ok = true;
      break;
    case jpxCSCMY:
      ok = true;
      break;
    case jpxCSCMYK:
      ok = true;
      break;
    case jpxCSYCCK:
      ok = true;
      break;
    case jpxCSCIELab:
      if (dataLen == 7 + 7*4) {
	if (!readULong(&newCS.enumerated.cieLab.rl) ||
	    !readULong(&newCS.enumerated.cieLab.ol) ||
	    !readULong(&newCS.enumerated.cieLab.ra) ||
	    !readULong(&newCS.enumerated.cieLab.oa) ||
	    !readULong(&newCS.enumerated.cieLab.rb) ||
	    !readULong(&newCS.enumerated.cieLab.ob) ||
	    !readULong(&newCS.enumerated.cieLab.il)) {
	  goto err;
	}
      } else if (dataLen == 7) {
	//~ this assumes the 8-bit case
	cover(92);
	newCS.enumerated.cieLab.rl = 100;
	newCS.enumerated.cieLab.ol = 0;
	newCS.enumerated.cieLab.ra = 255;
	newCS.enumerated.cieLab.oa = 128;
	newCS.enumerated.cieLab.rb = 255;
	newCS.enumerated.cieLab.ob = 96;
	newCS.enumerated.cieLab.il = 0x00443530;
      } else {
	goto err;
      }
      ok = true;
      break;
    case jpxCSsRGB:
      ok = true;
      break;
    case jpxCSGrayscale:
      ok = true;
      break;
    case jpxCSBiLevel2:
      ok = true;
      break;
    case jpxCSCIEJab:
      // not allowed in PDF
      goto err;
    case jpxCSCISesRGB:
      ok = true;
      break;
    case jpxCSROMMRGB:
      ok = true;
      break;
    case jpxCSsRGBYCbCr:
      ok = true;
      break;
    case jpxCSYPbPr1125:
      ok = true;
      break;
    case jpxCSYPbPr1250:
      ok = true;
      break;
    default:
      goto err;
    }
    break;
  case 2:			// restricted ICC profile
  case 3: 			// any ICC profile (JPX)
  case 4:			// vendor color (JPX)
    cover(18);
    for (i = 0; i < dataLen - 3; ++i) {
      if (bufStr->getChar() == EOF) {
	goto err;
      }
    }
    break;
  }

  if (ok && (!haveCS || newCS.prec > cs.prec)) {
    cs = newCS;
    haveCS = true;
  }

  return true;

 err:
  xwApp->error("Error in JPX color spec");
  return false;
}

bool XWJPXStream::readCodestream(uint)
{
  JPXTile *tile;
  JPXTileComp *tileComp;
  int segType;
  bool haveSIZ, haveCOD, haveQCD, haveSOT;
  uint precinctSize, style;
  uint segLen, capabilities, comp, i, j, r;

  //----- main header
  haveSIZ = haveCOD = haveQCD = haveSOT = false;
  do {
    if (!readMarkerHdr(&segType, &segLen)) {
      xwApp->error("Error in JPX codestream");
      return false;
    }
    switch (segType) {
    case 0x4f:			// SOC - start of codestream
      // marker only
      cover(19);
      break;
    case 0x51:			// SIZ - image and tile size
      cover(20);
      if (haveSIZ) {
	xwApp->error("Duplicate SIZ marker segment in JPX stream");
	return false;
      }
      if (!readUWord(&capabilities) ||
	  !readULong(&img.xSize) ||
	  !readULong(&img.ySize) ||
	  !readULong(&img.xOffset) ||
	  !readULong(&img.yOffset) ||
	  !readULong(&img.xTileSize) ||
	  !readULong(&img.yTileSize) ||
	  !readULong(&img.xTileOffset) ||
	  !readULong(&img.yTileOffset) ||
	  !readUWord(&img.nComps)) {
	xwApp->error("Error in JPX SIZ marker segment");
	return false;
      }
      if (haveImgHdr && img.nComps != nComps) {
	xwApp->error("Different number of components in JPX SIZ marker segment");
	return false;
      }
      if (img.xSize == 0 || img.ySize == 0 ||
	  img.xOffset >= img.xSize || img.yOffset >= img.ySize ||
	  img.xTileSize == 0 || img.yTileSize == 0 ||
	  img.xTileOffset > img.xOffset ||
	  img.yTileOffset > img.yOffset ||
	  img.xTileSize + img.xTileOffset <= img.xOffset ||
	  img.yTileSize + img.yTileOffset <= img.yOffset) {
	xwApp->error("Error in JPX SIZ marker segment");
	return false;
      }
      img.nXTiles = (img.xSize - img.xTileOffset + img.xTileSize - 1)
	            / img.xTileSize;
      img.nYTiles = (img.ySize - img.yTileOffset + img.yTileSize - 1)
	            / img.yTileSize;
      // check for overflow before allocating memory
      if (img.nXTiles <= 0 || img.nYTiles <= 0 ||
	  img.nXTiles >= INT_MAX / img.nYTiles) {
	xwApp->error("Bad tile count in JPX SIZ marker segment");
	return false;
      }
      img.tiles = (JPXTile *)malloc(img.nXTiles * img.nYTiles * sizeof(JPXTile));
      for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
	img.tiles[i].init = false;
	img.tiles[i].tileComps = (JPXTileComp *)malloc(img.nComps * sizeof(JPXTileComp));
	for (comp = 0; comp < img.nComps; ++comp) {
	  img.tiles[i].tileComps[comp].quantSteps = NULL;
	  img.tiles[i].tileComps[comp].data = NULL;
	  img.tiles[i].tileComps[comp].buf = NULL;
	  img.tiles[i].tileComps[comp].resLevels = NULL;
	}
      }
      for (comp = 0; comp < img.nComps; ++comp) {
	if (!readUByte(&img.tiles[0].tileComps[comp].prec) ||
	    !readUByte(&img.tiles[0].tileComps[comp].hSep) ||
	    !readUByte(&img.tiles[0].tileComps[comp].vSep)) {
	  xwApp->error("Error in JPX SIZ marker segment");
	  return false;
	}
	if (img.tiles[0].tileComps[comp].hSep == 0 ||
	    img.tiles[0].tileComps[comp].vSep == 0) {
	  xwApp->error("Error in JPX SIZ marker segment");
	  return false;
	}
	img.tiles[0].tileComps[comp].sgned =
	    (img.tiles[0].tileComps[comp].prec & 0x80) ? true : false;
	img.tiles[0].tileComps[comp].prec =
	    (img.tiles[0].tileComps[comp].prec & 0x7f) + 1;
	for (i = 1; i < img.nXTiles * img.nYTiles; ++i) {
	  img.tiles[i].tileComps[comp] = img.tiles[0].tileComps[comp];
	}
      }
      haveSIZ = true;
      break;
    case 0x52:			// COD - coding style default
      cover(21);
      if (!haveSIZ) {
	xwApp->error("JPX COD marker segment before SIZ segment");
	return false;
      }
      if (!readUByte(&img.tiles[0].tileComps[0].style) ||
	  !readUByte(&img.tiles[0].progOrder) ||
	  !readUWord(&img.tiles[0].nLayers) ||
	  !readUByte(&img.tiles[0].multiComp) ||
	  !readUByte(&img.tiles[0].tileComps[0].nDecompLevels) ||
	  !readUByte(&img.tiles[0].tileComps[0].codeBlockW) ||
	  !readUByte(&img.tiles[0].tileComps[0].codeBlockH) ||
	  !readUByte(&img.tiles[0].tileComps[0].codeBlockStyle) ||
	  !readUByte(&img.tiles[0].tileComps[0].transform)) {
	xwApp->error("Error in JPX COD marker segment");
	return false;
      }
      if (img.tiles[0].tileComps[0].nDecompLevels > 32 ||
	  img.tiles[0].tileComps[0].codeBlockW > 8 ||
	  img.tiles[0].tileComps[0].codeBlockH > 8) {
	xwApp->error("Error in JPX COD marker segment");
	return false;
      }
      img.tiles[0].tileComps[0].codeBlockW += 2;
      img.tiles[0].tileComps[0].codeBlockH += 2;
      for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
	if (i != 0) {
	  img.tiles[i].progOrder = img.tiles[0].progOrder;
	  img.tiles[i].nLayers = img.tiles[0].nLayers;
	  img.tiles[i].multiComp = img.tiles[0].multiComp;
	}
	for (comp = 0; comp < img.nComps; ++comp) {
	  if (!(i == 0 && comp == 0)) {
	    img.tiles[i].tileComps[comp].style =
	        img.tiles[0].tileComps[0].style;
	    img.tiles[i].tileComps[comp].nDecompLevels =
	        img.tiles[0].tileComps[0].nDecompLevels;
	    img.tiles[i].tileComps[comp].codeBlockW =
	        img.tiles[0].tileComps[0].codeBlockW;
	    img.tiles[i].tileComps[comp].codeBlockH =
	        img.tiles[0].tileComps[0].codeBlockH;
	    img.tiles[i].tileComps[comp].codeBlockStyle =
	        img.tiles[0].tileComps[0].codeBlockStyle;
	    img.tiles[i].tileComps[comp].transform =
	        img.tiles[0].tileComps[0].transform;
	  }
	  img.tiles[i].tileComps[comp].resLevels =
	      (JPXResLevel *)malloc((img.tiles[i].tileComps[comp].nDecompLevels + 1) * sizeof(JPXResLevel));
	  for (r = 0; r <= img.tiles[i].tileComps[comp].nDecompLevels; ++r) {
	    img.tiles[i].tileComps[comp].resLevels[r].precincts = NULL;
	  }
	}
      }
      for (r = 0; r <= img.tiles[0].tileComps[0].nDecompLevels; ++r) {
	if (img.tiles[0].tileComps[0].style & 0x01) {
	  cover(91);
	  if (!readUByte(&precinctSize)) {
	    xwApp->error("Error in JPX COD marker segment");
	    return false;
	  }
	  img.tiles[0].tileComps[0].resLevels[r].precinctWidth = precinctSize & 0x0f;
	  img.tiles[0].tileComps[0].resLevels[r].precinctHeight = (precinctSize >> 4) & 0x0f;
	} else {
	  img.tiles[0].tileComps[0].resLevels[r].precinctWidth = 15;
	  img.tiles[0].tileComps[0].resLevels[r].precinctHeight = 15;
	}
      }
      for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
	for (comp = 0; comp < img.nComps; ++comp) {
	  if (!(i == 0 && comp == 0)) {
	    for (r = 0; r <= img.tiles[i].tileComps[comp].nDecompLevels; ++r) {
	      img.tiles[i].tileComps[comp].resLevels[r].precinctWidth =
		  img.tiles[0].tileComps[0].resLevels[r].precinctWidth;
	      img.tiles[i].tileComps[comp].resLevels[r].precinctHeight =
		  img.tiles[0].tileComps[0].resLevels[r].precinctHeight;
	    }
	  }
	}
      }
      haveCOD = true;
      break;
    case 0x53:			// COC - coding style component
      cover(22);
      if (!haveCOD) {
	xwApp->error("JPX COC marker segment before COD segment");
	return false;
      }
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&style) ||
	  !readUByte(&img.tiles[0].tileComps[comp].nDecompLevels) ||
	  !readUByte(&img.tiles[0].tileComps[comp].codeBlockW) ||
	  !readUByte(&img.tiles[0].tileComps[comp].codeBlockH) ||
	  !readUByte(&img.tiles[0].tileComps[comp].codeBlockStyle) ||
	  !readUByte(&img.tiles[0].tileComps[comp].transform)) {
	xwApp->error("Error in JPX COC marker segment");
	return false;
      }
      if (img.tiles[0].tileComps[comp].nDecompLevels > 32 ||
	  img.tiles[0].tileComps[comp].codeBlockW > 8 ||
	  img.tiles[0].tileComps[comp].codeBlockH > 8) {
	xwApp->error("Error in JPX COC marker segment");
	return false;
      }
      img.tiles[0].tileComps[comp].style = (img.tiles[0].tileComps[comp].style & ~1) | (style & 1);
      img.tiles[0].tileComps[comp].codeBlockW += 2;
      img.tiles[0].tileComps[comp].codeBlockH += 2;
      for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
	if (i != 0) {
	  img.tiles[i].tileComps[comp].style =
	      img.tiles[0].tileComps[comp].style;
	  img.tiles[i].tileComps[comp].nDecompLevels =
	      img.tiles[0].tileComps[comp].nDecompLevels;
	  img.tiles[i].tileComps[comp].codeBlockW =
	      img.tiles[0].tileComps[comp].codeBlockW;
	  img.tiles[i].tileComps[comp].codeBlockH =
	      img.tiles[0].tileComps[comp].codeBlockH;
	  img.tiles[i].tileComps[comp].codeBlockStyle =
	      img.tiles[0].tileComps[comp].codeBlockStyle;
	  img.tiles[i].tileComps[comp].transform =
	      img.tiles[0].tileComps[comp].transform;
	}
	img.tiles[i].tileComps[comp].resLevels =
	    (JPXResLevel *)realloc(
		     img.tiles[i].tileComps[comp].resLevels,
		     (img.tiles[i].tileComps[comp].nDecompLevels + 1) * 
		     sizeof(JPXResLevel));
	for (r = 0; r <= img.tiles[i].tileComps[comp].nDecompLevels; ++r) {
	  img.tiles[i].tileComps[comp].resLevels[r].precincts = NULL;
	}
      }
      for (r = 0; r <= img.tiles[0].tileComps[comp].nDecompLevels; ++r) {
	if (img.tiles[0].tileComps[comp].style & 0x01) {
	  if (!readUByte(&precinctSize)) {
	    xwApp->error("Error in JPX COD marker segment");
	    return false;
	  }
	  img.tiles[0].tileComps[comp].resLevels[r].precinctWidth =
	      precinctSize & 0x0f;
	  img.tiles[0].tileComps[comp].resLevels[r].precinctHeight =
	      (precinctSize >> 4) & 0x0f;
	} else {
	  img.tiles[0].tileComps[comp].resLevels[r].precinctWidth = 15;
	  img.tiles[0].tileComps[comp].resLevels[r].precinctHeight = 15;
	}
      }
      for (i = 1; i < img.nXTiles * img.nYTiles; ++i) {
	for (r = 0; r <= img.tiles[i].tileComps[comp].nDecompLevels; ++r) {
	  img.tiles[i].tileComps[comp].resLevels[r].precinctWidth =
	      img.tiles[0].tileComps[comp].resLevels[r].precinctWidth;
	  img.tiles[i].tileComps[comp].resLevels[r].precinctHeight =
	      img.tiles[0].tileComps[comp].resLevels[r].precinctHeight;
	}
      }
      break;
    case 0x5c:			// QCD - quantization default
      cover(23);
      if (!haveSIZ) {
	xwApp->error("JPX QCD marker segment before SIZ segment");
	return false;
      }
      if (!readUByte(&img.tiles[0].tileComps[0].quantStyle)) {
	xwApp->error("Error in JPX QCD marker segment");
	return false;
      }
      if ((img.tiles[0].tileComps[0].quantStyle & 0x1f) == 0x00) {
	if (segLen <= 3) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
	img.tiles[0].tileComps[0].nQuantSteps = segLen - 3;
	img.tiles[0].tileComps[0].quantSteps =
	    (uint *)realloc(img.tiles[0].tileComps[0].quantSteps, img.tiles[0].tileComps[0].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[0].tileComps[0].nQuantSteps; ++i) {
	  if (!readUByte(&img.tiles[0].tileComps[0].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else if ((img.tiles[0].tileComps[0].quantStyle & 0x1f) == 0x01) {
	img.tiles[0].tileComps[0].nQuantSteps = 1;
	img.tiles[0].tileComps[0].quantSteps = (uint *)realloc(img.tiles[0].tileComps[0].quantSteps,
			       img.tiles[0].tileComps[0].nQuantSteps *  sizeof(uint));
	if (!readUWord(&img.tiles[0].tileComps[0].quantSteps[0])) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
      } else if ((img.tiles[0].tileComps[0].quantStyle & 0x1f) == 0x02) {
	if (segLen < 5) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
	img.tiles[0].tileComps[0].nQuantSteps = (segLen - 3) / 2;
	img.tiles[0].tileComps[0].quantSteps = (uint *)realloc(img.tiles[0].tileComps[0].quantSteps,
			       img.tiles[0].tileComps[0].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[0].tileComps[0].nQuantSteps; ++i) {
	  if (!readUWord(&img.tiles[0].tileComps[0].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else {
	xwApp->error("Error in JPX QCD marker segment");
	return false;
      }
      for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
	for (comp = 0; comp < img.nComps; ++comp) {
	  if (!(i == 0 && comp == 0)) {
	    img.tiles[i].tileComps[comp].quantStyle =
	        img.tiles[0].tileComps[0].quantStyle;
	    img.tiles[i].tileComps[comp].nQuantSteps =
	        img.tiles[0].tileComps[0].nQuantSteps;
	    img.tiles[i].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[i].tileComps[comp].quantSteps,
				   img.tiles[0].tileComps[0].nQuantSteps * sizeof(uint));
	    for (j = 0; j < img.tiles[0].tileComps[0].nQuantSteps; ++j) {
	      img.tiles[i].tileComps[comp].quantSteps[j] =
		  img.tiles[0].tileComps[0].quantSteps[j];
	    }
	  }
	}
      }
      haveQCD = true;
      break;
    case 0x5d:			// QCC - quantization component
      cover(24);
      if (!haveQCD) {
	xwApp->error("JPX QCC marker segment before QCD segment");
	return false;
      }
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&img.tiles[0].tileComps[comp].quantStyle)) {
	xwApp->error("Error in JPX QCC marker segment");
	return false;
      }
      if ((img.tiles[0].tileComps[comp].quantStyle & 0x1f) == 0x00) {
	if (segLen <= (img.nComps > 256 ? 5U : 4U)) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
	img.tiles[0].tileComps[comp].nQuantSteps =
	    segLen - (img.nComps > 256 ? 5 : 4);
	img.tiles[0].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[0].tileComps[comp].quantSteps,
			       img.tiles[0].tileComps[comp].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[0].tileComps[comp].nQuantSteps; ++i) {
	  if (!readUByte(&img.tiles[0].tileComps[comp].quantSteps[i])) {
	    xwApp->error("Error in JPX QCC marker segment");
	    return false;
	  }
	}
      } else if ((img.tiles[0].tileComps[comp].quantStyle & 0x1f) == 0x01) {
	img.tiles[0].tileComps[comp].nQuantSteps = 1;
	img.tiles[0].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[0].tileComps[comp].quantSteps,
			       img.tiles[0].tileComps[comp].nQuantSteps * sizeof(uint));
	if (!readUWord(&img.tiles[0].tileComps[comp].quantSteps[0])) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
      } else if ((img.tiles[0].tileComps[comp].quantStyle & 0x1f) == 0x02) {
	if (segLen < (img.nComps > 256 ? 5U : 4U) + 2) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
	img.tiles[0].tileComps[comp].nQuantSteps =
	    (segLen - (img.nComps > 256 ? 5 : 4)) / 2;
	img.tiles[0].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[0].tileComps[comp].quantSteps,
			       img.tiles[0].tileComps[comp].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[0].tileComps[comp].nQuantSteps; ++i) {
	  if (!readUWord(&img.tiles[0].tileComps[comp].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else {
	xwApp->error("Error in JPX QCC marker segment");
	return false;
      }
      for (i = 1; i < img.nXTiles * img.nYTiles; ++i) {
	img.tiles[i].tileComps[comp].quantStyle =
	    img.tiles[0].tileComps[comp].quantStyle;
	img.tiles[i].tileComps[comp].nQuantSteps =
	    img.tiles[0].tileComps[comp].nQuantSteps;
	img.tiles[i].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[i].tileComps[comp].quantSteps,
			       img.tiles[0].tileComps[comp].nQuantSteps * sizeof(uint));
	for (j = 0; j < img.tiles[0].tileComps[comp].nQuantSteps; ++j) {
	  img.tiles[i].tileComps[comp].quantSteps[j] =
	      img.tiles[0].tileComps[comp].quantSteps[j];
	}
      }
      break;
    case 0x5e:			// RGN - region of interest
      cover(25);
#if 1 //~ ROI is unimplemented
      xwApp->error( "got a JPX RGN segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX RGN marker segment");
	  return false;
	}
      }
#else
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&compInfo[comp].defROI.style) ||
	  !readUByte(&compInfo[comp].defROI.shift)) {
	xwApp->error("Error in JPX RGN marker segment");
	return false;
      }
#endif
      break;
    case 0x5f:			// POC - progression order change
      cover(26);
#if 1 //~ progression order changes are unimplemented
      xwApp->error("got a JPX POC segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX POC marker segment");
	  return false;
	}
      }
#else
      nProgs = (segLen - 2) / (img.nComps > 256 ? 9 : 7);
      progs = (JPXProgOrder *)malloc(nProgs * sizeof(JPXProgOrder));
      for (i = 0; i < nProgs; ++i) {
	if (!readUByte(&progs[i].startRes) ||
	    !(img.nComps > 256 && readUWord(&progs[i].startComp)) ||
	    !(img.nComps <= 256 && readUByte(&progs[i].startComp)) ||
	    !readUWord(&progs[i].endLayer) ||
	    !readUByte(&progs[i].endRes) ||
	    !(img.nComps > 256 && readUWord(&progs[i].endComp)) ||
	    !(img.nComps <= 256 && readUByte(&progs[i].endComp)) ||
	    !readUByte(&progs[i].progOrder)) {
	  xwApp->error("Error in JPX POC marker segment");
	  return false;
	}
      }
#endif
      break;
    case 0x60:			// PPM - packed packet headers, main header
      cover(27);
#if 1 //~ packed packet headers are unimplemented
      xwApp->error("Got a JPX PPM segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX PPM marker segment");
	  return false;
	}
      }
#endif
      break;
    case 0x55:			// TLM - tile-part lengths
      // skipped
      cover(28);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX TLM marker segment");
	  return false;
	}
      }
      break;
    case 0x57:			// PLM - packet length, main header
      // skipped
      cover(29);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX PLM marker segment");
	  return false;
	}
      }
      break;
    case 0x63:			// CRG - component registration
      // skipped
      cover(30);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX CRG marker segment");
	  return false;
	}
      }
      break;
    case 0x64:			// COM - comment
      // skipped
      cover(31);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX COM marker segment");
	  return false;
	}
      }
      break;
    case 0x90:			// SOT - start of tile
      cover(32);
      haveSOT = true;
      break;
    default:
      cover(33);
      xwApp->error("Unknown marker segment {0:02x} in JPX stream");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  break;
	}
      }
      break;
    }
  } while (!haveSOT);

  if (!haveSIZ) {
    xwApp->error("Missing SIZ marker segment in JPX stream");
    return false;
  }
  if (!haveCOD) {
    xwApp->error("Missing COD marker segment in JPX stream");
    return false;
  }
  if (!haveQCD) {
    xwApp->error("Missing QCD marker segment in JPX stream");
    return false;
  }

  //----- read the tile-parts
  while (1) {
    if (!readTilePart()) {
      return false;
    }
    if (!readMarkerHdr(&segType, &segLen)) {
      xwApp->error("Error in JPX codestream");
      return false;
    }
    if (segType != 0x90) {	// SOT - start of tile
      break;
    }
  }

  if (segType != 0xd9) {	// EOC - end of codestream
    xwApp->error("Missing EOC marker in JPX codestream");
    return false;
  }

  //----- finish decoding the image
  for (i = 0; i < img.nXTiles * img.nYTiles; ++i) {
    tile = &img.tiles[i];
    if (!tile->init) {
      xwApp->error("Uninitialized tile in JPX codestream");
      return false;
    }
    for (comp = 0; comp < img.nComps; ++comp) {
      tileComp = &tile->tileComps[comp];
      inverseTransform(tileComp);
    }
    if (!inverseMultiCompAndDC(tile)) {
      return false;
    }
  }

  //~ can free memory below tileComps here, and also tileComp.buf

  return true;
}

int  XWJPXStream::readMarkerHdr(int *segType, uint *segLen)
{
  int c;

  do {
    do {
      if ((c = bufStr->getChar()) == EOF) {
	return false;
      }
    } while (c != 0xff);
    do {
      if ((c = bufStr->getChar()) == EOF) {
	return false;
      }
    } while (c == 0xff);
  } while (c == 0x00);
  *segType = c;
  if ((c >= 0x30 && c <= 0x3f) ||
      c == 0x4f || c == 0x92 || c == 0x93 || c == 0xd9) {
    *segLen = 0;
    return true;
  }
  return readUWord(segLen);
}

bool XWJPXStream::readNBytes(int nBytes, bool signd, int *x)
{
  int y, c, i;

  y = 0;
  for (i = 0; i < nBytes; ++i) {
    if ((c = bufStr->getChar()) == EOF) {
      return false;
    }
    y = (y << 8) + c;
  }
  if (signd) {
    if (y & (1 << (8 * nBytes - 1))) {
      y |= -1 << (8 * nBytes);
    }
  }
  *x = y;
  return true;
}

bool XWJPXStream::readTilePart()
{
  JPXTile *tile;
  JPXTileComp *tileComp;
  JPXResLevel *resLevel;
  JPXPrecinct *precinct;
  JPXSubband *subband;
  JPXCodeBlock *cb;
  int *sbCoeffs;
  bool haveSOD;
  uint tileIdx, tilePartLen, tilePartIdx, nTileParts;
  bool tilePartToEOC;
  uint precinctSize, style;
  uint n, nSBs, nx, ny, sbx0, sby0, comp, segLen;
  uint i, j, k, cbX, cbY, r, pre, sb, cbi, cbj;
  int segType, level;

  // process the SOT marker segment
  if (!readUWord(&tileIdx) ||
      !readULong(&tilePartLen) ||
      !readUByte(&tilePartIdx) ||
      !readUByte(&nTileParts)) {
    xwApp->error("Error in JPX SOT marker segment");
    return false;
  }

  if ((tilePartIdx > 0 && !img.tiles[tileIdx].init) ||
      tileIdx >= img.nXTiles * img.nYTiles) {
    xwApp->error("Weird tile index in JPX stream");
    return false;
  }

  tilePartToEOC = tilePartLen == 0;
  tilePartLen -= 12; // subtract size of SOT segment

  haveSOD = false;
  do {
    if (!readMarkerHdr(&segType, &segLen)) {
      xwApp->error("Error in JPX tile-part codestream");
      return false;
    }
    tilePartLen -= 2 + segLen;
    switch (segType) {
    case 0x52:			// COD - coding style default
      cover(34);
      if (!readUByte(&img.tiles[tileIdx].tileComps[0].style) ||
	  !readUByte(&img.tiles[tileIdx].progOrder) ||
	  !readUWord(&img.tiles[tileIdx].nLayers) ||
	  !readUByte(&img.tiles[tileIdx].multiComp) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[0].nDecompLevels) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[0].codeBlockW) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[0].codeBlockH) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[0].codeBlockStyle) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[0].transform)) {
	xwApp->error("Error in JPX COD marker segment");
	return false;
      }
      if (img.tiles[tileIdx].tileComps[0].nDecompLevels > 32 ||
	  img.tiles[tileIdx].tileComps[0].codeBlockW > 8 ||
	  img.tiles[tileIdx].tileComps[0].codeBlockH > 8) {
	xwApp->error("Error in JPX COD marker segment");
	return false;
      }
      img.tiles[tileIdx].tileComps[0].codeBlockW += 2;
      img.tiles[tileIdx].tileComps[0].codeBlockH += 2;
      for (comp = 0; comp < img.nComps; ++comp) {
	if (comp != 0) {
	  img.tiles[tileIdx].tileComps[comp].style =
	      img.tiles[tileIdx].tileComps[0].style;
	  img.tiles[tileIdx].tileComps[comp].nDecompLevels =
	      img.tiles[tileIdx].tileComps[0].nDecompLevels;
	  img.tiles[tileIdx].tileComps[comp].codeBlockW =
	      img.tiles[tileIdx].tileComps[0].codeBlockW;
	  img.tiles[tileIdx].tileComps[comp].codeBlockH =
	      img.tiles[tileIdx].tileComps[0].codeBlockH;
	  img.tiles[tileIdx].tileComps[comp].codeBlockStyle =
	      img.tiles[tileIdx].tileComps[0].codeBlockStyle;
	  img.tiles[tileIdx].tileComps[comp].transform =
	      img.tiles[tileIdx].tileComps[0].transform;
	}
	img.tiles[tileIdx].tileComps[comp].resLevels =
	    (JPXResLevel *)realloc(
		     img.tiles[tileIdx].tileComps[comp].resLevels,
		     (img.tiles[tileIdx].tileComps[comp].nDecompLevels + 1) * 
		     sizeof(JPXResLevel));
	for (r = 0;
	     r <= img.tiles[tileIdx].tileComps[comp].nDecompLevels;
	     ++r) {
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precincts = NULL;
	}
      }
      for (r = 0; r <= img.tiles[tileIdx].tileComps[0].nDecompLevels; ++r) {
	if (img.tiles[tileIdx].tileComps[0].style & 0x01) {
	  if (!readUByte(&precinctSize)) {
	    xwApp->error("Error in JPX COD marker segment");
	    return false;
	  }
	  img.tiles[tileIdx].tileComps[0].resLevels[r].precinctWidth =
	      precinctSize & 0x0f;
	  img.tiles[tileIdx].tileComps[0].resLevels[r].precinctHeight =
	      (precinctSize >> 4) & 0x0f;
	} else {
	  img.tiles[tileIdx].tileComps[0].resLevels[r].precinctWidth = 15;
	  img.tiles[tileIdx].tileComps[0].resLevels[r].precinctHeight = 15;
	}
      }
      for (comp = 1; comp < img.nComps; ++comp) {
	for (r = 0;
	     r <= img.tiles[tileIdx].tileComps[comp].nDecompLevels;
	     ++r) {
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctWidth =
	      img.tiles[tileIdx].tileComps[0].resLevels[r].precinctWidth;
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctHeight =
	      img.tiles[tileIdx].tileComps[0].resLevels[r].precinctHeight;
	}
      }
      break;
    case 0x53:			// COC - coding style component
      cover(35);
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&style) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].nDecompLevels) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].codeBlockW) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].codeBlockH) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].codeBlockStyle) ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].transform)) {
	xwApp->error("Error in JPX COC marker segment");
	return false;
      }
      if (img.tiles[tileIdx].tileComps[comp].nDecompLevels > 32 ||
	  img.tiles[tileIdx].tileComps[comp].codeBlockW > 8 ||
	  img.tiles[tileIdx].tileComps[comp].codeBlockH > 8) {
	xwApp->error("Error in JPX COD marker segment");
	return false;
      }
      img.tiles[tileIdx].tileComps[comp].style =
	  (img.tiles[tileIdx].tileComps[comp].style & ~1) | (style & 1);
      img.tiles[tileIdx].tileComps[comp].codeBlockW += 2;
      img.tiles[tileIdx].tileComps[comp].codeBlockH += 2;
      img.tiles[tileIdx].tileComps[comp].resLevels =
	  (JPXResLevel *)realloc(
		     img.tiles[tileIdx].tileComps[comp].resLevels,
		     (img.tiles[tileIdx].tileComps[comp].nDecompLevels + 1) * sizeof(JPXResLevel));
      for (r = 0; r <= img.tiles[tileIdx].tileComps[comp].nDecompLevels; ++r) {
	img.tiles[tileIdx].tileComps[comp].resLevels[r].precincts = NULL;
      }
      for (r = 0; r <= img.tiles[tileIdx].tileComps[comp].nDecompLevels; ++r) {
	if (img.tiles[tileIdx].tileComps[comp].style & 0x01) {
	  if (!readUByte(&precinctSize)) {
	    xwApp->error("Error in JPX COD marker segment");
	    return false;
	  }
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctWidth =
	      precinctSize & 0x0f;
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctHeight =
	      (precinctSize >> 4) & 0x0f;
	} else {
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctWidth = 15;
	  img.tiles[tileIdx].tileComps[comp].resLevels[r].precinctHeight = 15;
	}
      }
      break;
    case 0x5c:			// QCD - quantization default
      cover(36);
      if (!readUByte(&img.tiles[tileIdx].tileComps[0].quantStyle)) {
	xwApp->error("Error in JPX QCD marker segment");
	return false;
      }
      if ((img.tiles[tileIdx].tileComps[0].quantStyle & 0x1f) == 0x00) {
	if (segLen <= 3) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
	img.tiles[tileIdx].tileComps[0].nQuantSteps = segLen - 3;
	img.tiles[tileIdx].tileComps[0].quantSteps =
	    (uint *)realloc(img.tiles[tileIdx].tileComps[0].quantSteps,
			       img.tiles[tileIdx].tileComps[0].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[tileIdx].tileComps[0].nQuantSteps; ++i) {
	  if (!readUByte(&img.tiles[tileIdx].tileComps[0].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else if ((img.tiles[tileIdx].tileComps[0].quantStyle & 0x1f) == 0x01) {
	img.tiles[tileIdx].tileComps[0].nQuantSteps = 1;
	img.tiles[tileIdx].tileComps[0].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[0].quantSteps,
			       img.tiles[tileIdx].tileComps[0].nQuantSteps * sizeof(uint));
	if (!readUWord(&img.tiles[tileIdx].tileComps[0].quantSteps[0])) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
      } else if ((img.tiles[tileIdx].tileComps[0].quantStyle & 0x1f) == 0x02) {
	if (segLen < 5) {
	  xwApp->error("Error in JPX QCD marker segment");
	  return false;
	}
	img.tiles[tileIdx].tileComps[0].nQuantSteps = (segLen - 3) / 2;
	img.tiles[tileIdx].tileComps[0].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[0].quantSteps,
			       img.tiles[tileIdx].tileComps[0].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[tileIdx].tileComps[0].nQuantSteps; ++i) {
	  if (!readUWord(&img.tiles[tileIdx].tileComps[0].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else {
	xwApp->error("Error in JPX QCD marker segment");
	return false;
      }
      for (comp = 1; comp < img.nComps; ++comp) {
	img.tiles[tileIdx].tileComps[comp].quantStyle =
	    img.tiles[tileIdx].tileComps[0].quantStyle;
	img.tiles[tileIdx].tileComps[comp].nQuantSteps =
	    img.tiles[tileIdx].tileComps[0].nQuantSteps;
	img.tiles[tileIdx].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[comp].quantSteps,
			       img.tiles[tileIdx].tileComps[0].nQuantSteps * sizeof(uint));
	for (j = 0; j < img.tiles[tileIdx].tileComps[0].nQuantSteps; ++j) {
	  img.tiles[tileIdx].tileComps[comp].quantSteps[j] =
	      img.tiles[tileIdx].tileComps[0].quantSteps[j];
	}
      }
      break;
    case 0x5d:			// QCC - quantization component
      cover(37);
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&img.tiles[tileIdx].tileComps[comp].quantStyle)) {
	xwApp->error("Error in JPX QCC marker segment");
	return false;
      }
      if ((img.tiles[tileIdx].tileComps[comp].quantStyle & 0x1f) == 0x00) {
	if (segLen <= (img.nComps > 256 ? 5U : 4U)) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
	img.tiles[tileIdx].tileComps[comp].nQuantSteps = segLen - (img.nComps > 256 ? 5 : 4);
	img.tiles[tileIdx].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[comp].quantSteps,
			       img.tiles[tileIdx].tileComps[comp].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[tileIdx].tileComps[comp].nQuantSteps; ++i) {
	  if (!readUByte(&img.tiles[tileIdx].tileComps[comp].quantSteps[i])) {
	    xwApp->error("Error in JPX QCC marker segment");
	    return false;
	  }
	}
      } else if ((img.tiles[tileIdx].tileComps[comp].quantStyle & 0x1f)
		 == 0x01) {
	img.tiles[tileIdx].tileComps[comp].nQuantSteps = 1;
	img.tiles[tileIdx].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[comp].quantSteps,
			       img.tiles[tileIdx].tileComps[comp].nQuantSteps * sizeof(uint));
	if (!readUWord(&img.tiles[tileIdx].tileComps[comp].quantSteps[0])) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
      } else if ((img.tiles[tileIdx].tileComps[comp].quantStyle & 0x1f)
		 == 0x02) {
	if (segLen < (img.nComps > 256 ? 5U : 4U) + 2) {
	  xwApp->error("Error in JPX QCC marker segment");
	  return false;
	}
	img.tiles[tileIdx].tileComps[comp].nQuantSteps =
	    (segLen - (img.nComps > 256 ? 5 : 4)) / 2;
	img.tiles[tileIdx].tileComps[comp].quantSteps = (uint *)realloc(img.tiles[tileIdx].tileComps[comp].quantSteps,
			       img.tiles[tileIdx].tileComps[comp].nQuantSteps * sizeof(uint));
	for (i = 0; i < img.tiles[tileIdx].tileComps[comp].nQuantSteps; ++i) {
	  if (!readUWord(&img.tiles[tileIdx].tileComps[comp].quantSteps[i])) {
	    xwApp->error("Error in JPX QCD marker segment");
	    return false;
	  }
	}
      } else {
	xwApp->error("Error in JPX QCC marker segment");
	return false;
      }
      break;
    case 0x5e:			// RGN - region of interest
      cover(38);
#if 1 //~ ROI is unimplemented
      xwApp->error("Got a JPX RGN segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX RGN marker segment");
	  return false;
	}
      }
#else
      if ((img.nComps > 256 && !readUWord(&comp)) ||
	  (img.nComps <= 256 && !readUByte(&comp)) ||
	  comp >= img.nComps ||
	  !readUByte(&compInfo[comp].roi.style) ||
	  !readUByte(&compInfo[comp].roi.shift)) {
	xwApp->error("Error in JPX RGN marker segment");
	return false;
      }
#endif
      break;
    case 0x5f:			// POC - progression order change
      cover(39);
#if 1 //~ progression order changes are unimplemented
      xwApp->error("Got a JPX POC segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX POC marker segment");
	  return false;
	}
      }
#else
      nTileProgs = (segLen - 2) / (img.nComps > 256 ? 9 : 7);
      tileProgs = (JPXProgOrder *)malloc(nTileProgs * sizeof(JPXProgOrder));
      for (i = 0; i < nTileProgs; ++i) {
	if (!readUByte(&tileProgs[i].startRes) ||
	    !(img.nComps > 256 && readUWord(&tileProgs[i].startComp)) ||
	    !(img.nComps <= 256 && readUByte(&tileProgs[i].startComp)) ||
	    !readUWord(&tileProgs[i].endLayer) ||
	    !readUByte(&tileProgs[i].endRes) ||
	    !(img.nComps > 256 && readUWord(&tileProgs[i].endComp)) ||
	    !(img.nComps <= 256 && readUByte(&tileProgs[i].endComp)) ||
	    !readUByte(&tileProgs[i].progOrder)) {
	  xwApp->error("Error in JPX POC marker segment");
	  return false;
	}
      }
#endif
      break;
    case 0x61:			// PPT - packed packet headers, tile-part hdr
      cover(40);
#if 1 //~ packed packet headers are unimplemented
      xwApp->error("Got a JPX PPT segment");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX PPT marker segment");
	  return false;
	}
      }
#endif
    case 0x58:			// PLT - packet length, tile-part header
      // skipped
      cover(41);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX PLT marker segment");
	  return false;
	}
      }
      break;
    case 0x64:			// COM - comment
      // skipped
      cover(42);
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  xwApp->error("Error in JPX COM marker segment");
	  return false;
	}
      }
      break;
    case 0x93:			// SOD - start of data
      cover(43);
      haveSOD = true;
      break;
    default:
      cover(44);
      xwApp->error("Unknown marker segment {0:02x} in JPX tile-part stream");
      for (i = 0; i < segLen - 2; ++i) {
	if (bufStr->getChar() == EOF) {
	  break;
	}
      }
      break;
    }
  } while (!haveSOD);

  //----- initialize the tile, precincts, and code-blocks
  if (tilePartIdx == 0) {
    tile = &img.tiles[tileIdx];
    tile->init = true;
    i = tileIdx / img.nXTiles;
    j = tileIdx % img.nXTiles;
    if ((tile->x0 = img.xTileOffset + j * img.xTileSize) < img.xOffset) {
      tile->x0 = img.xOffset;
    }
    if ((tile->y0 = img.yTileOffset + i * img.yTileSize) < img.yOffset) {
      tile->y0 = img.yOffset;
    }
    if ((tile->x1 = img.xTileOffset + (j + 1) * img.xTileSize) > img.xSize) {
      tile->x1 = img.xSize;
    }
    if ((tile->y1 = img.yTileOffset + (i + 1) * img.yTileSize) > img.ySize) {
      tile->y1 = img.ySize;
    }
    tile->comp = 0;
    tile->res = 0;
    tile->precinct = 0;
    tile->layer = 0;
    tile->maxNDecompLevels = 0;
    for (comp = 0; comp < img.nComps; ++comp) {
      tileComp = &tile->tileComps[comp];
      if (tileComp->nDecompLevels > tile->maxNDecompLevels) {
	tile->maxNDecompLevels = tileComp->nDecompLevels;
      }
      tileComp->x0 = jpxCeilDiv(tile->x0, tileComp->hSep);
      tileComp->y0 = jpxCeilDiv(tile->y0, tileComp->vSep);
      tileComp->x1 = jpxCeilDiv(tile->x1, tileComp->hSep);
      tileComp->y1 = jpxCeilDiv(tile->y1, tileComp->vSep);
      tileComp->w = tileComp->x1 - tileComp->x0;
      tileComp->cbW = 1 << tileComp->codeBlockW;
      tileComp->cbH = 1 << tileComp->codeBlockH;
      tileComp->data = (int *)malloc((tileComp->x1 - tileComp->x0) *
				       (tileComp->y1 - tileComp->y0) * sizeof(int));
      if (tileComp->x1 - tileComp->x0 > tileComp->y1 - tileComp->y0) {
	n = tileComp->x1 - tileComp->x0;
      } else {
	n = tileComp->y1 - tileComp->y0;
      }
      tileComp->buf = (int *)malloc((n + 8) * sizeof(int));
      for (r = 0; r <= tileComp->nDecompLevels; ++r) {
	resLevel = &tileComp->resLevels[r];
	k = r == 0 ? tileComp->nDecompLevels
	           : tileComp->nDecompLevels - r + 1;
	resLevel->x0 = jpxCeilDivPow2(tileComp->x0, k);
	resLevel->y0 = jpxCeilDivPow2(tileComp->y0, k);
	resLevel->x1 = jpxCeilDivPow2(tileComp->x1, k);
	resLevel->y1 = jpxCeilDivPow2(tileComp->y1, k);
	if (r == 0) {
	  resLevel->bx0[0] = resLevel->x0;
	  resLevel->by0[0] = resLevel->y0;
	  resLevel->bx1[0] = resLevel->x1;
	  resLevel->by1[0] = resLevel->y1;
	} else {
	  resLevel->bx0[0] = jpxCeilDivPow2(tileComp->x0 - (1 << (k-1)), k);
	  resLevel->by0[0] = resLevel->y0;
	  resLevel->bx1[0] = jpxCeilDivPow2(tileComp->x1 - (1 << (k-1)), k);
	  resLevel->by1[0] = resLevel->y1;
	  resLevel->bx0[1] = resLevel->x0;
	  resLevel->by0[1] = jpxCeilDivPow2(tileComp->y0 - (1 << (k-1)), k);
	  resLevel->bx1[1] = resLevel->x1;
	  resLevel->by1[1] = jpxCeilDivPow2(tileComp->y1 - (1 << (k-1)), k);
	  resLevel->bx0[2] = jpxCeilDivPow2(tileComp->x0 - (1 << (k-1)), k);
	  resLevel->by0[2] = jpxCeilDivPow2(tileComp->y0 - (1 << (k-1)), k);
	  resLevel->bx1[2] = jpxCeilDivPow2(tileComp->x1 - (1 << (k-1)), k);
	  resLevel->by1[2] = jpxCeilDivPow2(tileComp->y1 - (1 << (k-1)), k);
	}
	resLevel->precincts = (JPXPrecinct *)malloc(sizeof(JPXPrecinct));
	for (pre = 0; pre < 1; ++pre) {
	  precinct = &resLevel->precincts[pre];
	  precinct->x0 = resLevel->x0;
	  precinct->y0 = resLevel->y0;
	  precinct->x1 = resLevel->x1;
	  precinct->y1 = resLevel->y1;
	  nSBs = r == 0 ? 1 : 3;
	  precinct->subbands = (JPXSubband *)malloc(nSBs * sizeof(JPXSubband));
	  for (sb = 0; sb < nSBs; ++sb) {
	    subband = &precinct->subbands[sb];
	    subband->x0 = resLevel->bx0[sb];
	    subband->y0 = resLevel->by0[sb];
	    subband->x1 = resLevel->bx1[sb];
	    subband->y1 = resLevel->by1[sb];
	    subband->nXCBs = jpxCeilDivPow2(subband->x1,
					    tileComp->codeBlockW)
	                     - jpxFloorDivPow2(subband->x0,
					       tileComp->codeBlockW);
	    subband->nYCBs = jpxCeilDivPow2(subband->y1,
					    tileComp->codeBlockH)
	                     - jpxFloorDivPow2(subband->y0,
					       tileComp->codeBlockH);
	    n = subband->nXCBs > subband->nYCBs ? subband->nXCBs
	                                        : subband->nYCBs;
	    for (subband->maxTTLevel = 0, --n;
		 n;
		 ++subband->maxTTLevel, n >>= 1) ;
	    n = 0;
	    for (level = subband->maxTTLevel; level >= 0; --level) {
	      nx = jpxCeilDivPow2(subband->nXCBs, level);
	      ny = jpxCeilDivPow2(subband->nYCBs, level);
	      n += nx * ny;
	    }
	    subband->inclusion = (JPXTagTreeNode *)malloc(n * sizeof(JPXTagTreeNode));
	    subband->zeroBitPlane = (JPXTagTreeNode *)malloc(n * sizeof(JPXTagTreeNode));
	    for (k = 0; k < n; ++k) {
	      subband->inclusion[k].finished = false;
	      subband->inclusion[k].val = 0;
	      subband->zeroBitPlane[k].finished = false;
	      subband->zeroBitPlane[k].val = 0;
	    }
	    subband->cbs = (JPXCodeBlock *)malloc(subband->nXCBs * subband->nYCBs * sizeof(JPXCodeBlock));
	    sbx0 = jpxFloorDivPow2(subband->x0, tileComp->codeBlockW);
	    sby0 = jpxFloorDivPow2(subband->y0, tileComp->codeBlockH);
	    if (r == 0) { // (NL)LL
	      sbCoeffs = tileComp->data;
	    } else if (sb == 0) { // (NL-r+1)HL
	      sbCoeffs = tileComp->data
		         + resLevel->bx1[1] - resLevel->bx0[1];
	    } else if (sb == 1) { // (NL-r+1)LH
	      sbCoeffs = tileComp->data
		         + (resLevel->by1[0] - resLevel->by0[0]) * tileComp->w;
	    } else { // (NL-r+1)HH
	      sbCoeffs = tileComp->data
		         + (resLevel->by1[0] - resLevel->by0[0]) * tileComp->w
		         + resLevel->bx1[1] - resLevel->bx0[1];
	    }
	    cb = subband->cbs;
	    for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
	      for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
		cb->x0 = (sbx0 + cbX) << tileComp->codeBlockW;
		cb->x1 = cb->x0 + tileComp->cbW;
		if (subband->x0 > cb->x0) {
		  cb->x0 = subband->x0;
		}
		if (subband->x1 < cb->x1) {
		  cb->x1 = subband->x1;
		}
		cb->y0 = (sby0 + cbY) << tileComp->codeBlockH;
		cb->y1 = cb->y0 + tileComp->cbH;
		if (subband->y0 > cb->y0) {
		  cb->y0 = subband->y0;
		}
		if (subband->y1 < cb->y1) {
		  cb->y1 = subband->y1;
		}
		cb->seen = false;
		cb->lBlock = 3;
		cb->nextPass = jpxPassCleanup;
		cb->nZeroBitPlanes = 0;
		cb->dataLenSize = 1;
		cb->dataLen = (uint *)malloc(sizeof(uint));
		cb->coeffs = sbCoeffs
		             + (cb->y0 - subband->y0) * tileComp->w
		             + (cb->x0 - subband->x0);
		cb->touched = (char *)malloc((1 << (tileComp->codeBlockW + tileComp->codeBlockH)) * sizeof(char));
		cb->len = 0;
		for (cbj = 0; cbj < cb->y1 - cb->y0; ++cbj) {
		  for (cbi = 0; cbi < cb->x1 - cb->x0; ++cbi) {
		    cb->coeffs[cbj * tileComp->w + cbi] = 0;
		  }
		}
		memset(cb->touched, 0,
		       (1 << (tileComp->codeBlockW + tileComp->codeBlockH)));
		cb->arithDecoder = NULL;
		cb->stats = NULL;
		++cb;
	      }
	    }
	  }
	}
      }
    }
  }

  return readTilePartData(tileIdx, tilePartLen, tilePartToEOC);
}

bool XWJPXStream::readTilePartData(uint tileIdx, uint tilePartLen, bool tilePartToEOC)
{
  JPXTile *tile;
  JPXTileComp *tileComp;
  JPXResLevel *resLevel;
  JPXPrecinct *precinct;
  JPXSubband *subband;
  JPXCodeBlock *cb;
  uint ttVal;
  uint bits, cbX, cbY, nx, ny, i, j, n, sb;
  int level;

  tile = &img.tiles[tileIdx];

  // read all packets from this tile-part
  while (1) {
    if (tilePartToEOC) {
      //~ peek for an EOC marker
      cover(93);
    } else if (tilePartLen == 0) {
      break;
    }

    tileComp = &tile->tileComps[tile->comp];
    resLevel = &tileComp->resLevels[tile->res];
    precinct = &resLevel->precincts[tile->precinct];

    //----- packet header

    // setup
    startBitBuf(tilePartLen);
    if (tileComp->style & 0x02) {
      skipSOP();
    }

    // zero-length flag
    if (!readBits(1, &bits)) {
      goto err;
    }
    if (!bits) {
      // packet is empty -- clear all code-block inclusion flags
      cover(45);
      for (sb = 0; sb < (uint)(tile->res == 0 ? 1 : 3); ++sb) {
	subband = &precinct->subbands[sb];
	for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
	  for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
	    cb = &subband->cbs[cbY * subband->nXCBs + cbX];
	    cb->included = false;
	  }
	}
      }
    } else {

      for (sb = 0; sb < (uint)(tile->res == 0 ? 1 : 3); ++sb) {
	subband = &precinct->subbands[sb];
	for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
	  for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
	    cb = &subband->cbs[cbY * subband->nXCBs + cbX];

	    // skip code-blocks with no coefficients
	    if (cb->x0 >= cb->x1 || cb->y0 >= cb->y1) {
	      cover(46);
	      cb->included = false;
	      continue;
	    }

	    // code-block inclusion
	    if (cb->seen) {
	      cover(47);
	      if (!readBits(1, &cb->included)) {
		goto err;
	      }
	    } else {
	      cover(48);
	      ttVal = 0;
	      i = 0;
	      for (level = subband->maxTTLevel; level >= 0; --level) {
		nx = jpxCeilDivPow2(subband->nXCBs, level);
		ny = jpxCeilDivPow2(subband->nYCBs, level);
		j = i + (cbY >> level) * nx + (cbX >> level);
		if (!subband->inclusion[j].finished &&
		    !subband->inclusion[j].val) {
		  subband->inclusion[j].val = ttVal;
		} else {
		  ttVal = subband->inclusion[j].val;
		}
		while (!subband->inclusion[j].finished &&
		       ttVal <= tile->layer) {
		  if (!readBits(1, &bits)) {
		    goto err;
		  }
		  if (bits == 1) {
		    subband->inclusion[j].finished = true;
		  } else {
		    ++ttVal;
		  }
		}
		subband->inclusion[j].val = ttVal;
		if (ttVal > tile->layer) {
		  break;
		}
		i += nx * ny;
	      }
	      cb->included = level < 0;
	    }

	    if (cb->included) {
	      cover(49);

	      // zero bit-plane count
	      if (!cb->seen) {
		cover(50);
		ttVal = 0;
		i = 0;
		for (level = subband->maxTTLevel; level >= 0; --level) {
		  nx = jpxCeilDivPow2(subband->nXCBs, level);
		  ny = jpxCeilDivPow2(subband->nYCBs, level);
		  j = i + (cbY >> level) * nx + (cbX >> level);
		  if (!subband->zeroBitPlane[j].finished &&
		      !subband->zeroBitPlane[j].val) {
		    subband->zeroBitPlane[j].val = ttVal;
		  } else {
		    ttVal = subband->zeroBitPlane[j].val;
		  }
		  while (!subband->zeroBitPlane[j].finished) {
		    if (!readBits(1, &bits)) {
		      goto err;
		    }
		    if (bits == 1) {
		      subband->zeroBitPlane[j].finished = true;
		    } else {
		      ++ttVal;
		    }
		  }
		  subband->zeroBitPlane[j].val = ttVal;
		  i += nx * ny;
		}
		cb->nZeroBitPlanes = ttVal;
	      }

	      // number of coding passes
	      if (!readBits(1, &bits)) {
		goto err;
	      }
	      if (bits == 0) {
		cover(51);
		cb->nCodingPasses = 1;
	      } else {
		if (!readBits(1, &bits)) {
		  goto err;
		}
		if (bits == 0) {
		  cover(52);
		  cb->nCodingPasses = 2;
		} else {
		  cover(53);
		  if (!readBits(2, &bits)) {
		    goto err;
		  }
		  if (bits < 3) {
		    cover(54);
		    cb->nCodingPasses = 3 + bits;
		  } else {
		    cover(55);
		    if (!readBits(5, &bits)) {
		      goto err;
		    }
		    if (bits < 31) {
		      cover(56);
		      cb->nCodingPasses = 6 + bits;
		    } else {
		      cover(57);
		      if (!readBits(7, &bits)) {
			goto err;
		      }
		      cb->nCodingPasses = 37 + bits;
		    }
		  }
		}
	      }

	      // update Lblock
	      while (1) {
		if (!readBits(1, &bits)) {
		  goto err;
		}
		if (!bits) {
		  break;
		}
		++cb->lBlock;
	      }

	      // one codeword segment for each of the coding passes
	      if (tileComp->codeBlockStyle & 0x04) {
		if (cb->nCodingPasses > cb->dataLenSize) {
		  cb->dataLenSize = cb->nCodingPasses;
		  cb->dataLen = (uint *)realloc(cb->dataLen, cb->dataLenSize * sizeof(uint));
		}

		// read the lengths
		for (i = 0; i < cb->nCodingPasses; ++i) {
		  if (!readBits(cb->lBlock, &cb->dataLen[i])) {
		    goto err;
		  }
		}

	      // one codeword segment for all of the coding passes
	      } else {

		// read the length
		for (n = cb->lBlock, i = cb->nCodingPasses >> 1;
		     i;
		     ++n, i >>= 1) ;
		if (!readBits(n, &cb->dataLen[0])) {
		  goto err;
		}
	      }
	    }
	  }
	}
      }
    }
    if (tileComp->style & 0x04) {
      skipEPH();
    }
    tilePartLen = finishBitBuf();

    //----- packet data

    for (sb = 0; sb < (uint)(tile->res == 0 ? 1 : 3); ++sb) {
      subband = &precinct->subbands[sb];
      for (cbY = 0; cbY < subband->nYCBs; ++cbY) {
	for (cbX = 0; cbX < subband->nXCBs; ++cbX) {
	  cb = &subband->cbs[cbY * subband->nXCBs + cbX];
	  if (cb->included) {
	    if (!readCodeBlockData(tileComp, resLevel, precinct, subband,
				   tile->res, sb, cb)) {
	      return false;
	    }
	    if (tileComp->codeBlockStyle & 0x04) {
	      for (i = 0; i < cb->nCodingPasses; ++i) {
		tilePartLen -= cb->dataLen[i];
	      }
	    } else {
	      tilePartLen -= cb->dataLen[0];
	    }
	    cb->seen = true;
	  }
	}
      }
    }

    //----- next packet

    switch (tile->progOrder) {
    case 0: // layer, resolution level, component, precinct
      cover(58);
      if (++tile->comp == img.nComps) {
	tile->comp = 0;
	if (++tile->res == tile->maxNDecompLevels + 1) {
	  tile->res = 0;
	  if (++tile->layer == tile->nLayers) {
	    tile->layer = 0;
	  }
	}
      }
      break;
    case 1: // resolution level, layer, component, precinct
      cover(59);
      if (++tile->comp == img.nComps) {
	tile->comp = 0;
	if (++tile->layer == tile->nLayers) {
	  tile->layer = 0;
	  if (++tile->res == tile->maxNDecompLevels + 1) {
	    tile->res = 0;
	  }
	}
      }
      break;
    case 2: // resolution level, precinct, component, layer
      //~ this isn't correct -- see B.12.1.3
      cover(60);
      if (++tile->layer == tile->nLayers) {
	tile->layer = 0;
	if (++tile->comp == img.nComps) {
	  tile->comp = 0;
	  if (++tile->res == tile->maxNDecompLevels + 1) {
	    tile->res = 0;
	  }
	}
      }
      break;
    case 3: // precinct, component, resolution level, layer
      //~ this isn't correct -- see B.12.1.4
      cover(61);
      if (++tile->layer == tile->nLayers) {
	tile->layer = 0;
	if (++tile->res == tile->maxNDecompLevels + 1) {
	  tile->res = 0;
	  if (++tile->comp == img.nComps) {
	    tile->comp = 0;
	  }
	}
      }
      break;
    case 4: // component, precinct, resolution level, layer
      //~ this isn't correct -- see B.12.1.5
      cover(62);
      if (++tile->layer == tile->nLayers) {
	tile->layer = 0;
	if (++tile->res == tile->maxNDecompLevels + 1) {
	  tile->res = 0;
	  if (++tile->comp == img.nComps) {
	    tile->comp = 0;
	  }
	}
      }
      break;
    }
  }

  return true;

 err:
  xwApp->error("Error in JPX stream");
  return false;
}

bool XWJPXStream::readUByte(uint *x)
{
  int c0;

  if ((c0 = bufStr->getChar()) == EOF) {
    return false;
  }
  *x = (uint)c0;
  return true;
}

bool XWJPXStream::readULong(uint *x)
{
  int c0, c1, c2, c3;

  if ((c0 = bufStr->getChar()) == EOF ||
      (c1 = bufStr->getChar()) == EOF ||
      (c2 = bufStr->getChar()) == EOF ||
      (c3 = bufStr->getChar()) == EOF) {
    return false;
  }
  *x = (uint)((c0 << 24) | (c1 << 16) | (c2 << 8) | c3);
  return true;
}

bool XWJPXStream::readUWord(uint *x)
{
  int c0, c1;

  if ((c0 = bufStr->getChar()) == EOF ||
      (c1 = bufStr->getChar()) == EOF) {
    return false;
  }
  *x = (uint)((c0 << 8) | c1);
  return true;
}

void XWJPXStream::skipEPH()
{
	int i, k;

  k = bitBufSkip ? 1 : 0;
  if (byteCount >= (uint)(k + 2) &&
      bufStr->lookChar(k) == 0xff &&
      bufStr->lookChar(k + 1) == 0x92) {
    for (i = 0; i < k + 2; ++i) {
      bufStr->getChar();
    }
    byteCount -= k + 2;
    bitBufLen = 0;
    bitBufSkip = false;
  }
}

void XWJPXStream::skipSOP()
{
	int i;

  // SOP occurs at the start of the packet header, so we don't need to
  // worry about bit-stuff prior to it
  if (byteCount >= 6 &&
      bufStr->lookChar(0) == 0xff &&
      bufStr->lookChar(1) == 0x91) {
    for (i = 0; i < 6; ++i) {
      bufStr->getChar();
    }
    byteCount -= 6;
    bitBufLen = 0;
    bitBufSkip = false;
  }
}

void XWJPXStream::startBitBuf(uint byteCountA)
{
  bitBufLen = 0;
  bitBufSkip = false;
  byteCount = byteCountA;
}

