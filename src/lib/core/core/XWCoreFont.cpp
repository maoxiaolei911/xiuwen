/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <QByteArray>
#include "XWApplication.h"
#include "XWObject.h"
#include "XWDict.h"
#include "XWFontSetting.h"
#include "XWDocSetting.h"
#include "XWCMap.h"
#include "XWCharCodeToUnicode.h"
#include "FontEncodingTables.h"
#include "BuiltinFontTables.h"
#include "XWFontFileType1.h"
#include "XWFontFileType1C.h"
#include "XWFontFileTrueType.h"
#include "XWCoreFont.h"

struct Base14FontMapEntry {
  const char *altName;
  const char *base14Name;
};

static Base14FontMapEntry base14FontMap[] = {
  { "Arial",                        "Helvetica" },
  { "Arial,Bold",                   "Helvetica-Bold" },
  { "Arial,BoldItalic",             "Helvetica-BoldOblique" },
  { "Arial,Italic",                 "Helvetica-Oblique" },
  { "Arial-Bold",                   "Helvetica-Bold" },
  { "Arial-BoldItalic",             "Helvetica-BoldOblique" },
  { "Arial-BoldItalicMT",           "Helvetica-BoldOblique" },
  { "Arial-BoldMT",                 "Helvetica-Bold" },
  { "Arial-Italic",                 "Helvetica-Oblique" },
  { "Arial-ItalicMT",               "Helvetica-Oblique" },
  { "ArialMT",                      "Helvetica" },
  { "Courier",                      "Courier" },
  { "Courier,Bold",                 "Courier-Bold" },
  { "Courier,BoldItalic",           "Courier-BoldOblique" },
  { "Courier,Italic",               "Courier-Oblique" },
  { "Courier-Bold",                 "Courier-Bold" },
  { "Courier-BoldOblique",          "Courier-BoldOblique" },
  { "Courier-Oblique",              "Courier-Oblique" },
  { "CourierNew",                   "Courier" },
  { "CourierNew,Bold",              "Courier-Bold" },
  { "CourierNew,BoldItalic",        "Courier-BoldOblique" },
  { "CourierNew,Italic",            "Courier-Oblique" },
  { "CourierNew-Bold",              "Courier-Bold" },
  { "CourierNew-BoldItalic",        "Courier-BoldOblique" },
  { "CourierNew-Italic",            "Courier-Oblique" },
  { "CourierNewPS-BoldItalicMT",    "Courier-BoldOblique" },
  { "CourierNewPS-BoldMT",          "Courier-Bold" },
  { "CourierNewPS-ItalicMT",        "Courier-Oblique" },
  { "CourierNewPSMT",               "Courier" },
  { "Helvetica",                    "Helvetica" },
  { "Helvetica,Bold",               "Helvetica-Bold" },
  { "Helvetica,BoldItalic",         "Helvetica-BoldOblique" },
  { "Helvetica,Italic",             "Helvetica-Oblique" },
  { "Helvetica-Bold",               "Helvetica-Bold" },
  { "Helvetica-BoldItalic",         "Helvetica-BoldOblique" },
  { "Helvetica-BoldOblique",        "Helvetica-BoldOblique" },
  { "Helvetica-Italic",             "Helvetica-Oblique" },
  { "Helvetica-Oblique",            "Helvetica-Oblique" },
  { "Symbol",                       "Symbol" },
  { "Symbol,Bold",                  "Symbol" },
  { "Symbol,BoldItalic",            "Symbol" },
  { "Symbol,Italic",                "Symbol" },
  { "Times-Bold",                   "Times-Bold" },
  { "Times-BoldItalic",             "Times-BoldItalic" },
  { "Times-Italic",                 "Times-Italic" },
  { "Times-Roman",                  "Times-Roman" },
  { "TimesNewRoman",                "Times-Roman" },
  { "TimesNewRoman,Bold",           "Times-Bold" },
  { "TimesNewRoman,BoldItalic",     "Times-BoldItalic" },
  { "TimesNewRoman,Italic",         "Times-Italic" },
  { "TimesNewRoman-Bold",           "Times-Bold" },
  { "TimesNewRoman-BoldItalic",     "Times-BoldItalic" },
  { "TimesNewRoman-Italic",         "Times-Italic" },
  { "TimesNewRomanPS",              "Times-Roman" },
  { "TimesNewRomanPS-Bold",         "Times-Bold" },
  { "TimesNewRomanPS-BoldItalic",   "Times-BoldItalic" },
  { "TimesNewRomanPS-BoldItalicMT", "Times-BoldItalic" },
  { "TimesNewRomanPS-BoldMT",       "Times-Bold" },
  { "TimesNewRomanPS-Italic",       "Times-Italic" },
  { "TimesNewRomanPS-ItalicMT",     "Times-Italic" },
  { "TimesNewRomanPSMT",            "Times-Roman" },
  { "TimesNewRomanPSMT,Bold",       "Times-Bold" },
  { "TimesNewRomanPSMT,BoldItalic", "Times-BoldItalic" },
  { "TimesNewRomanPSMT,Italic",     "Times-Italic" },
  { "ZapfDingbats",                 "ZapfDingbats" }
};

//------------------------------------------------------------------------

// index: {fixed:0, sans-serif:4, serif:8} + bold*2 + italic
// NB: must be in same order as psSubstFonts in PSOutputDev.cc
static const char *base14SubstFonts[14] = {
  "Courier",
  "Courier-Oblique",
  "Courier-Bold",
  "Courier-BoldOblique",
  "Helvetica",
  "Helvetica-Oblique",
  "Helvetica-Bold",
  "Helvetica-BoldOblique",
  "Times-Roman",
  "Times-Italic",
  "Times-Bold",
  "Times-BoldItalic",
  // the last two are never used for substitution
  "Symbol",
  "ZapfDingbats"
};

static int readFromStream(void *data) 
{
  return ((XWStream *)data)->getChar();
}

XWFontLoc::XWFontLoc() 
{
  path = NULL;
  fontNum = 0;
  encoding = NULL;
  substIdx = -1;
}

XWFontLoc::~XWFontLoc() 
{
  if (path) {
    delete path;
  }
  if (encoding) {
    delete encoding;
  }
}

XWCoreFont::XWCoreFont(char *tagA, 
                       ObjRef idA, 
                       XWString *nameA,
	  					         CoreFontType typeA, 
	  					         ObjRef embFontIDA)
{
    ok = false;
    tag = new XWString(tagA);
    id = idA;
    name = nameA;
    type = typeA;
    embFontID = embFontIDA;
    embFontName = 0;
    flags = 0;
}

XWCoreFont::~XWCoreFont()
{
    delete tag;
    
    if (name) 
        delete name;
        
    if (embFontName)
        delete embFontName;
}

XWFontLoc * XWCoreFont::locateBase14Font(XWString *base14Name)
{
	XWFontSetting setting;
	XWString * path = setting.findFontFile(base14Name);
  if (!path) {
    return NULL;
  }
  return getExternalFont(path, false);
}

XWFontLoc * XWCoreFont::locateFont(XWRef *xref, bool ps)
{
	XWFontLoc *fontLoc;
  SysFontType sysFontType;
  XWString *path, *base14Name, *substName;
  PSFontParam16 *psFont16;
  XWObject refObj, embFontObj;
  int substIdx, fontNum;
  bool embed;

  if (type == fontType3) 
  {
    return NULL;
  }

	XWDocSetting docsetting;
  //----- embedded font
  if (embFontID.num >= 0) 
  {
    embed = true;
    refObj.initRef(embFontID.num, embFontID.gen);
    refObj.fetch(xref, &embFontObj);
    if (!embFontObj.isStream()) 
    {
      xwApp->error("Embedded font object is wrong type");
      embed = false;
    }
    embFontObj.free();
    refObj.free();
    if (embed) 
    {
      if (ps) 
      {
				switch (type) 
				{
					case fontType1:
					case fontType1C:
					case fontType1COT:
	  				embed = docsetting.getPSEmbedType1();
	  				break;
	  				
					case fontTrueType:
					case fontTrueTypeOT:
	  				embed = docsetting.getPSEmbedTrueType();
	  				break;
	  				
					case fontCIDType0C:
					case fontCIDType0COT:
	  				embed = docsetting.getPSEmbedCIDPostScript();
	  				break;
	  				
					case fontCIDType2:
					case fontCIDType2OT:
	  				embed = docsetting.getPSEmbedCIDTrueType();
	  				break;
	  				
					default:
	  				break;
				}
      }
      if (embed) 
      {
				fontLoc = new XWFontLoc();
				fontLoc->locType = fontLocEmbedded;
				fontLoc->fontType = type;
				fontLoc->embFontID = embFontID;
				return fontLoc;
      }
    }
  }

  //----- PS passthrough
  if (ps && !isCIDFont() && docsetting.getPSFontPassthrough()) 
  {
    fontLoc = new XWFontLoc();
    fontLoc->locType = fontLocResident;
    fontLoc->fontType = fontType1;
    fontLoc->path = name->copy();
    return fontLoc;
  }

  //----- PS resident Base-14 font
  if (ps && !isCIDFont() && ((XWCore8BitFont *)this)->base14) 
  {
    fontLoc = new XWFontLoc();
    fontLoc->locType = fontLocResident;
    fontLoc->fontType = fontType1;
    fontLoc->path = new XWString(((XWCore8BitFont *)this)->base14->base14Name);
    return fontLoc;
  }

	XWFontSetting fontsetting;
  //----- external font file (fontFile, fontDir)
  if ((path = fontsetting.findFontFile(name))) 
  {
    if ((fontLoc = getExternalFont(path, isCIDFont()))) 
    {
      return fontLoc;
    }
  }

  //----- external font file for Base-14 font
  if (!ps && !isCIDFont() && ((XWCore8BitFont *)this)->base14) 
  {
    base14Name = new XWString(((XWCore8BitFont *)this)->base14->base14Name);
    if ((path = fontsetting.findFontFile(base14Name))) 
    {
      if ((fontLoc = getExternalFont(path, false))) 
      {
				delete base14Name;
				return fontLoc;
      }
    }
    delete base14Name;
  }

  //----- system font
  if ((path = fontsetting.findSystemFontFile(name, &sysFontType, &fontNum))) 
  {
    if (isCIDFont()) 
    {
      if (sysFontType == sysFontTTF || sysFontType == sysFontTTC) 
      {
				fontLoc = new XWFontLoc();
				fontLoc->locType = fontLocExternal;
				fontLoc->fontType = fontCIDType2;
				fontLoc->path = path;
				fontLoc->fontNum = fontNum;
				return fontLoc;
      }
    } 
    else 
    {
      if (sysFontType == sysFontTTF || sysFontType == sysFontTTC) 
      {
				fontLoc = new XWFontLoc();
				fontLoc->locType = fontLocExternal;
				fontLoc->fontType = fontTrueType;
				fontLoc->path = path;
				return fontLoc;
      } 
      else if (sysFontType == sysFontPFA || sysFontType == sysFontPFB) 
      {
				fontLoc = new XWFontLoc();
				fontLoc->locType = fontLocExternal;
				fontLoc->fontType = fontType1;
				fontLoc->path = path;
				fontLoc->fontNum = fontNum;
				return fontLoc;
      }
    }
    delete path;
  }

  if (!isCIDFont()) 
  {

    //----- 8-bit PS resident font
    if (ps) 
    {
      if ((path = fontsetting.getPSResidentFont(name))) 
      {
				fontLoc = new XWFontLoc();
				fontLoc->locType = fontLocResident;
				fontLoc->fontType = fontType1;
				fontLoc->path = path;
				return fontLoc;
      }
    }

    //----- 8-bit font substitution
    if (flags & fontFixedWidth) 
    {
      substIdx = 0;
    } 
    else if (flags & fontSerif) 
    {
      substIdx = 8;
    } 
   	else 
   	{
      substIdx = 4;
    }
    if (isBold()) {
      substIdx += 2;
    }
    if (isItalic()) 
    {
      substIdx += 1;
    }
    substName = new XWString(base14SubstFonts[substIdx]);
    if (ps) 
    {
      fontLoc = new XWFontLoc();
      fontLoc->locType = fontLocResident;
      fontLoc->fontType = fontType1;
      fontLoc->path = substName;
      fontLoc->substIdx = substIdx;
      return fontLoc;
    } 
    else 
    {
      path = fontsetting.findFontFile(substName);
      delete substName;
      if (path) 
      {
				if ((fontLoc = getExternalFont(path, false))) 
				{
	  			fontLoc->substIdx = substIdx;
	  			return fontLoc;
				}
      }
    }

    // failed to find a substitute font
    return NULL;
  }

  //----- 16-bit PS resident font
  if (ps && ((psFont16 = fontsetting.getPSResidentFont16(name,((XWCoreCIDFont *)this)->getWMode())))) 
  {
    fontLoc = new XWFontLoc();
    fontLoc->locType = fontLocResident;
    fontLoc->fontType = fontCIDType0; // this is not used
    fontLoc->path = psFont16->psFontName->copy();
    fontLoc->encoding = psFont16->encoding->copy();
    fontLoc->wMode = psFont16->wMode;
    return fontLoc;
  }
  if (ps && ((psFont16 = fontsetting.getPSResidentFontCC(((XWCoreCIDFont *)this)->getCollection(), ((XWCoreCIDFont *)this)->getWMode())))) 
  {
    fontLoc = new XWFontLoc();
    fontLoc->locType = fontLocResident;
    fontLoc->fontType = fontCIDType0; // this is not used
    fontLoc->path = psFont16->psFontName->copy();
    fontLoc->encoding = psFont16->encoding->copy();
    fontLoc->wMode = psFont16->wMode;
    return fontLoc;
  }

  //----- CID font substitution
  if ((path = fontsetting.findCCFontFile(((XWCoreCIDFont *)this)->getCollection()))) 
  {
    if ((fontLoc = getExternalFont(path, true))) 
    {
      return fontLoc;
    }
  }

  // failed to find a substitute font
  return NULL;
}

XWCoreFont * XWCoreFont::makeFont(XWRef *xref, char *tagA, ObjRef idA, XWDict *fontDict)
{
	XWString *nameA;
  ObjRef embFontIDA;
  CoreFontType typeA;
  XWCoreFont *font;
  XWObject obj1;

  // get base font name
  nameA = NULL;
  fontDict->lookup("BaseFont", &obj1);
  if (obj1.isName()) 
  {
    nameA = new XWString(obj1.getName());
  }
  obj1.free();

  // get embedded font ID and font type
  typeA = getFontType(xref, fontDict, &embFontIDA);

  // create the font object
  font = NULL;
  if (typeA < fontCIDType0) 
  {
    font = new XWCore8BitFont(xref, tagA, idA, nameA, typeA, embFontIDA, fontDict);
  } 
  else 
  {
    font = new XWCoreCIDFont(xref, tagA, idA, nameA, typeA, embFontIDA, fontDict);
  }

  return font;
}

char * XWCoreFont::readEmbFontFile(XWRef *xref, int *len)
{
	char *buf;
  XWObject obj1, obj2;
  XWStream *str;
  int c;
  int size, i;

  obj1.initRef(embFontID.num, embFontID.gen);
  obj1.fetch(xref, &obj2);
  if (!obj2.isStream()) {
    xwApp->error("Embedded font file is not a stream");
    obj2.free();
    obj1.free();
    embFontID.num = -1;
    return NULL;
  }
  str = obj2.getStream();

  buf = NULL;
  i = size = 0;
  str->reset();
  while ((c = str->getChar()) != EOF) {
    if (i == size) {
      if (size > INT_MAX - 4096) {
	xwApp->error("Embedded font file is too large");
	break;
      }
      size += 4096;
      buf = (char *)realloc(buf, size * sizeof(char));
    }
    buf[i++] = c;
  }
  *len = i;
  str->close();

  obj2.free();
  obj1.free();

  return buf;
}

void XWCoreFont::readFontDescriptor(XWRef *, XWDict *fontDict)
{
	XWObject obj1, obj2, obj3, obj4;
  double t;
  int i;

  // assume Times-Roman by default (for substitution purposes)
  flags = fontSerif;

  missingWidth = 0;

  if (fontDict->lookup("FontDescriptor", &obj1)->isDict()) 
  {

    // get flags
    if (obj1.dictLookup("Flags", &obj2)->isInt()) 
    {
      flags = obj2.getInt();
    }
    obj2.free();

    // get name
    obj1.dictLookup("FontName", &obj2);
    if (obj2.isName()) 
    {
      embFontName = new XWString(obj2.getName());
    }
    obj2.free();

    // look for MissingWidth
    obj1.dictLookup("MissingWidth", &obj2);
    if (obj2.isNum()) 
    {
      missingWidth = obj2.getNum();
    }
    obj2.free();

    // get Ascent and Descent
    obj1.dictLookup("Ascent", &obj2);
    if (obj2.isNum()) 
    {
      t = 0.001 * obj2.getNum();
      // some broken font descriptors specify a negative ascent
      if (t < 0) 
      {
				t = -t;
      }
      // some broken font descriptors set ascent and descent to 0;
      // others set it to ridiculous values (e.g., 32768)
      if (t != 0 && t < 3) 
      {
				ascent = t;
      }
    }
    obj2.free();
    obj1.dictLookup("Descent", &obj2);
    if (obj2.isNum()) 
    {
      t = 0.001 * obj2.getNum();
      // some broken font descriptors specify a positive descent
      if (t > 0) 
      {
				t = -t;
      }
      // some broken font descriptors set ascent and descent to 0
      if (t != 0 && t > -3) 
      {
				descent = t;
      }
    }
    obj2.free();

    // font FontBBox
    if (obj1.dictLookup("FontBBox", &obj2)->isArray()) 
    {
      for (i = 0; i < 4 && i < obj2.arrayGetLength(); ++i) 
     	{
				if (obj2.arrayGet(i, &obj3)->isNum()) 
				{
	  			fontBBox[i] = 0.001 * obj3.getNum();
				}
				obj3.free();
     	}
    }
    obj2.free();

  }
  obj1.free();
}

void XWCoreFont::setFontBBox(double * bbox)
{
	for (int i = 0; i < 4; i++)
		fontBBox[i] = bbox[i];
}

void XWCoreFont::setFontMatrix(double * m)
{
	for (int i = 0; i < 6; i++)
		fontMat[i] = m[i];
}

XWFontLoc * XWCoreFont::getExternalFont(XWString *path, bool cid)
{
	FoFiIdentifierType fft;
  CoreFontType fontType;
  XWFontLoc *fontLoc;

  fft = XWFontFileIdentifier::identifyFile(path->getCString());
  switch (fft) 
  {
	  case fofiIdType1PFA:
  	case fofiIdType1PFB:
    	fontType = fontType1;
    	break;
    	
  	case fofiIdCFF8Bit:
    	fontType = fontType1C;
    	break;
    	
  	case fofiIdCFFCID:
    	fontType = fontCIDType0C;
    	break;
    	
  	case fofiIdTrueType:
  	case fofiIdTrueTypeCollection:
    	fontType = cid ? fontCIDType2 : fontTrueType;
    	break;
    	
  	case fofiIdOpenTypeCFF8Bit:
    	fontType = fontType1COT;
    	break;
    	
  	case fofiIdOpenTypeCFFCID:
	    fontType = fontCIDType0COT;
  	  break;
  	  
  	case fofiIdUnknown:
  	case fofiIdError:
  	default:
    	fontType = fontUnknownType;
    	break;
  }
  if (fontType == fontUnknownType ||
      (cid ? (fontType < fontCIDType0)
           : (fontType >= fontCIDType0))) 
  {
    delete path;
    return NULL;
  }
  fontLoc = new XWFontLoc();
  fontLoc->locType = fontLocExternal;
  fontLoc->fontType = fontType;
  fontLoc->path = path;
  return fontLoc;
}

CoreFontType XWCoreFont::getFontType(XWRef *xref, XWDict *fontDict, ObjRef *embID)
{
	CoreFontType t, expectedType;
  FoFiIdentifierType fft;
  XWDict *fontDict2;
  XWObject subtype, fontDesc, obj1, obj2, obj3, obj4;
  bool isType0, err;

  t = fontUnknownType;
  embID->num = embID->gen = -1;
  err = false;

  fontDict->lookup("Subtype", &subtype);
  expectedType = fontUnknownType;
  isType0 = false;
  if (subtype.isName("Type1") || subtype.isName("MMType1")) 
  {
    expectedType = fontType1;
  } 
  else if (subtype.isName("Type1C")) 
  {
    expectedType = fontType1C;
  } 
  else if (subtype.isName("Type3")) 
  {
    expectedType = fontType3;
  } 
  else if (subtype.isName("TrueType")) 
  {
    expectedType = fontTrueType;
  } 
  else if (subtype.isName("Type0")) 
  {
    isType0 = true;
  }
  
  subtype.free();

  fontDict2 = fontDict;
  if (fontDict->lookup("DescendantFonts", &obj1)->isArray()) 
  {
    if (obj1.arrayGetLength() == 0) 
    {
      xwApp->error("Empty DescendantFonts array in font");
      obj2.initNull();
    } 
    else if (obj1.arrayGet(0, &obj2)->isDict()) 
    {
      if (!isType0) {
      
				xwApp->error("Non-CID font with DescendantFonts array");
      }
      fontDict2 = obj2.getDict();
      fontDict2->lookup("Subtype", &subtype);
      if (subtype.isName("CIDFontType0")) 
      {
				if (isType0) 
				{
	  			expectedType = fontCIDType0;
				}
      } 
      else if (subtype.isName("CIDFontType2")) 
      {
				if (isType0) 
				{
	  			expectedType = fontCIDType2;
				}
      }
      subtype.free();
    }
  } 
  else 
  {
    obj2.initNull();
  }

  if (fontDict2->lookup("FontDescriptor", &fontDesc)->isDict()) 
  {
    if (fontDesc.dictLookupNF("FontFile", &obj3)->isRef()) 
    {
      *embID = obj3.getRef();
      if (expectedType != fontType1) 
      {
				err = true;
      }
    }
    obj3.free();
    if (embID->num == -1 && fontDesc.dictLookupNF("FontFile2", &obj3)->isRef()) 
    {
      *embID = obj3.getRef();
      if (isType0) 
      {
				expectedType = fontCIDType2;
      } 
      else if (expectedType != fontTrueType) 
      {
				err = true;
      }
    }
    obj3.free();
    if (embID->num == -1 && fontDesc.dictLookupNF("FontFile3", &obj3)->isRef()) 
    {
      *embID = obj3.getRef();
      if (obj3.fetch(xref, &obj4)->isStream()) 
      {
				obj4.streamGetDict()->lookup("Subtype", &subtype);
				if (subtype.isName("Type1")) 
				{
	  			if (expectedType != fontType1) 
	  			{
	    			err = true;
	    			expectedType = isType0 ? fontCIDType0 : fontType1;
	  			}
				} 
				else if (subtype.isName("Type1C")) 
				{
	  			if (expectedType == fontType1) 
	  			{
	    			expectedType = fontType1C;
	  			} 
	  			else if (expectedType != fontType1C) 
	  			{
	    			err = true;
	    			expectedType = isType0 ? fontCIDType0C : fontType1C;
	  			}
				} 
				else if (subtype.isName("TrueType")) 
				{
	  			if (expectedType != fontTrueType) 
	  			{
	    			err = true;
	    			expectedType = isType0 ? fontCIDType2 : fontTrueType;
	  			}
				} 
				else if (subtype.isName("CIDFontType0C")) 
				{
	  			if (expectedType == fontCIDType0) 
	  			{
	    			expectedType = fontCIDType0C;
	  			} 
	  			else 
	  			{
	    			err = true;
	    			expectedType = isType0 ? fontCIDType0C : fontType1C;
	  			}
				} 
				else if (subtype.isName("OpenType")) 
				{
	  			if (expectedType == fontTrueType) 
	  			{
	    			expectedType = fontTrueTypeOT;
	 			 	} 
	 			 	else if (expectedType == fontType1) 
	 			 	{
	    			expectedType = fontType1COT;
	  			} 
	  			else if (expectedType == fontCIDType0) 
	  			{
	    			expectedType = fontCIDType0COT;
	  			} 
	  			else if (expectedType == fontCIDType2) 
	  			{
	    			expectedType = fontCIDType2OT;
	  			} 
	  			else 
	  			{
	    			err = true;
	  			}
				} 
				subtype.free();
      }
      obj4.free();
    }
    obj3.free();
  }
  fontDesc.free();

  t = fontUnknownType;
  if (embID->num >= 0) 
  {
    obj3.initRef(embID->num, embID->gen);
    obj3.fetch(xref, &obj4);
    if (obj4.isStream()) 
    {
      obj4.streamReset();
      fft = XWFontFileIdentifier::identifyStream(&readFromStream, obj4.getStream());
      obj4.streamClose();
      switch (fft) 
      {
      	case fofiIdType1PFA:
      	case fofiIdType1PFB:
					t = fontType1;
					break;
					
      	case fofiIdCFF8Bit:
					t = isType0 ? fontCIDType0C : fontType1C;
					break;
					
      	case fofiIdCFFCID:
					t = fontCIDType0C;
					break;
					
      	case fofiIdTrueType:
      	case fofiIdTrueTypeCollection:
					t = isType0 ? fontCIDType2 : fontTrueType;
					break;
					
      	case fofiIdOpenTypeCFF8Bit:
					t = isType0 ? fontCIDType0COT : fontType1COT;
					break;
					
      	case fofiIdOpenTypeCFFCID:
					t = fontCIDType0COT;
					break;
					
      	default:
				break;
      }
    }
    obj4.free();
    obj3.free();
  }

  if (t == fontUnknownType) 
  {
    t = expectedType;
  }

  if (t != expectedType) 
  {
    err = true;
  }

  if (err) 
  {
    xwApp->error("Mismatch between font type and embedded font file");
  }

  obj2.free();
  obj1.free();

  return t;
}

XWCharCodeToUnicode * XWCoreFont::readToUnicodeCMap(XWDict *fontDict, 
                                                    int nBits,
				                                    XWCharCodeToUnicode *ctu)
{
  XWString *buf;
  XWObject obj1;
  int c;

  if (!fontDict->lookup("ToUnicode", &obj1)->isStream()) {
    obj1.free();
    return NULL;
  }
  buf = new XWString();
  obj1.streamReset();
  while ((c = obj1.streamGetChar()) != EOF) {
    buf->append(c);
  }
  obj1.streamClose();
  obj1.free();
  if (ctu) {
    ctu->mergeCMap(buf, nBits);
  } else {
    ctu = XWCharCodeToUnicode::parseCMap(buf, nBits);
  }
  delete buf;
  return ctu;
}

XWCore8BitFont::XWCore8BitFont(XWRef *xref, 
                  char *tagA, 
                  ObjRef idA, 
                  XWString *nameA,
	     				 		CoreFontType typeA, 
	     				 		ObjRef embFontIDA, 
	     				 		XWDict *fontDict)
	  :XWCoreFont(tagA, idA, nameA, typeA, embFontIDA)
{
  XWString *name2;
  BuiltinFont *builtinFont;
  char **baseEnc;
  bool baseEncFromFontFile;
  char *buf;
  int len;
  XWFontFileType1 *ffT1;
  XWFontFileType1C *ffT1C;
  int code, code2;
  char *charName;
  bool missing, hex;
  uint toUnicode[256];
  XWCharCodeToUnicode *utu, *ctu2;
  uint uBuf[8];
  double mul;
  int firstChar, lastChar;
  ushort w;
  XWObject obj1, obj2, obj3;
  int n, i, a, b, m;

  ctu = NULL;

  // do font name substitution for various aliases of the Base 14 font
  // names
  base14 = NULL;
  if (name) 
  {
    name2 = name->copy();
    i = 0;
    while (i < name2->getLength()) 
    {
      if (name2->getChar(i) == ' ') 
      {
				name2->del(i);
      } 
      else 
      {
				++i;
      }
    }
    a = 0;
    b = sizeof(base14FontMap) / sizeof(Base14FontMapEntry);
    // invariant: base14FontMap[a].altName <= name2 < base14FontMap[b].altName
    while (b - a > 1) 
    {
      m = (a + b) / 2;
      if (name2->cmp(base14FontMap[m].altName) >= 0) 
      {
				a = m;
      } 
      else 
      {
				b = m;
      }
    }
    if (!name2->cmp(base14FontMap[a].altName)) 
    {
      base14 = &base14FontMap[a];
    }
    delete name2;
  }

  // is it a built-in font?
  builtinFont = NULL;
  if (base14) 
  {
    for (i = 0; i < nBuiltinFonts; ++i) 
    {
      if (!strcmp(base14->base14Name, builtinFonts[i].name)) 
      {
				builtinFont = &builtinFonts[i];
				break;
      }
    }
  }

  // default ascent/descent values
  if (builtinFont) 
  {
    ascent = 0.001 * builtinFont->ascent;
    descent = 0.001 * builtinFont->descent;
    fontBBox[0] = 0.001 * builtinFont->bbox[0];
    fontBBox[1] = 0.001 * builtinFont->bbox[1];
    fontBBox[2] = 0.001 * builtinFont->bbox[2];
    fontBBox[3] = 0.001 * builtinFont->bbox[3];
  } 
  else 
  {
    ascent = 0.95;
    descent = -0.35;
    fontBBox[0] = fontBBox[1] = fontBBox[2] = fontBBox[3] = 0;
  }

  // get info from font descriptor
  readFontDescriptor(xref, fontDict);

  // for non-embedded fonts, don't trust the ascent/descent/bbox
  // values from the font descriptor
  if (builtinFont && embFontID.num < 0) 
  {
    ascent = 0.001 * builtinFont->ascent;
    descent = 0.001 * builtinFont->descent;
    fontBBox[0] = 0.001 * builtinFont->bbox[0];
    fontBBox[1] = 0.001 * builtinFont->bbox[1];
    fontBBox[2] = 0.001 * builtinFont->bbox[2];
    fontBBox[3] = 0.001 * builtinFont->bbox[3];
  }

  // get font matrix
  fontMat[0] = fontMat[3] = 1;
  fontMat[1] = fontMat[2] = fontMat[4] = fontMat[5] = 0;
  if (fontDict->lookup("FontMatrix", &obj1)->isArray()) 
  {
    for (i = 0; i < 6 && i < obj1.arrayGetLength(); ++i) 
    {
      if (obj1.arrayGet(i, &obj2)->isNum()) 
      {
				fontMat[i] = obj2.getNum();
      }
      obj2.free();
    }
  }
  obj1.free();

  // get Type 3 bounding box, font definition, and resources
  if (type == fontType3) 
  {
    if (fontDict->lookup("FontBBox", &obj1)->isArray()) 
    {
      for (i = 0; i < 4 && i < obj1.arrayGetLength(); ++i) 
      {
				if (obj1.arrayGet(i, &obj2)->isNum()) 
				{
	  			fontBBox[i] = obj2.getNum();
				}
				obj2.free();
      }
    }
    obj1.free();
    if (!fontDict->lookup("CharProcs", &charProcs)->isDict()) 
    {
      xwApp->error("Missing or invalid CharProcs dictionary in Type 3 font");
      charProcs.free();
    }
    if (!fontDict->lookup("Resources", &resources)->isDict()) 
    {
      resources.free();
    }
  }

  //----- build the font encoding -----

  // Encodings start with a base encoding, which can come from
  // (in order of priority):
  //   1. FontDict.Encoding or FontDict.Encoding.BaseEncoding
  //        - MacRoman / MacExpert / WinAnsi / Standard
  //   2. embedded or external font file
  //   3. default:
  //        - builtin --> builtin encoding
  //        - TrueType --> WinAnsiEncoding
  //        - others --> StandardEncoding
  // and then add a list of differences (if any) from
  // FontDict.Encoding.Differences.

  // check FontDict for base encoding
  hasEncoding = false;
  usesMacRomanEnc = false;
  baseEnc = NULL;
  baseEncFromFontFile = false;
  fontDict->lookup("Encoding", &obj1);
  if (obj1.isDict()) 
  {
    obj1.dictLookup("BaseEncoding", &obj2);
    if (obj2.isName("MacRomanEncoding")) 
    {
      hasEncoding = true;
      usesMacRomanEnc = true;
      baseEnc = macRomanEncoding;
    } 
    else if (obj2.isName("MacExpertEncoding")) 
    {
      hasEncoding = true;
      baseEnc = macExpertEncoding;
    } 
    else if (obj2.isName("WinAnsiEncoding")) 
    {
      hasEncoding = true;
      baseEnc = winAnsiEncoding;
    }
    obj2.free();
  } 
  else if (obj1.isName("MacRomanEncoding")) 
  {
    hasEncoding = true;
    usesMacRomanEnc = true;
    baseEnc = macRomanEncoding;
  } 
  else if (obj1.isName("MacExpertEncoding")) 
  {
    hasEncoding = true;
    baseEnc = macExpertEncoding;
  } 
  else if (obj1.isName("WinAnsiEncoding")) 
  {
    hasEncoding = true;
    baseEnc = winAnsiEncoding;
  }

  // check embedded font file for base encoding
  // (only for Type 1 fonts - trying to get an encoding out of a
  // TrueType font is a losing proposition)
  ffT1 = NULL;
  ffT1C = NULL;
  buf = NULL;
  if (type == fontType1 && embFontID.num >= 0) 
  {
    if ((buf = readEmbFontFile(xref, &len))) 
    {
      if ((ffT1 = XWFontFileType1::make(buf, len))) 
      {
				if (ffT1->getName()) 
				{
	  			if (embFontName) 
	  			{
	    			delete embFontName;
	  			}
	  			embFontName = new XWString(ffT1->getName());
				}
				if (!baseEnc) 
				{
	  			baseEnc = (char **)ffT1->getEncoding();
	  			baseEncFromFontFile = true;
				}
      }
      free(buf);
    }
  } 
  else if (type == fontType1C && embFontID.num >= 0) 
  {
    if ((buf = readEmbFontFile(xref, &len))) 
    {
      if ((ffT1C = XWFontFileType1C::make(buf, len))) 
      {
				if (ffT1C->getName()) 
				{
	  			if (embFontName) 
	  			{
	    			delete embFontName;
	  			}
	  			embFontName = new XWString(ffT1C->getName());
				}
				if (!baseEnc) 
				{
	  			baseEnc = (char **)ffT1C->getEncoding();
	  			baseEncFromFontFile = true;
				}
      }
      free(buf);
    }
  }

  // get default base encoding
  if (!baseEnc) 
  {
    if (builtinFont && embFontID.num < 0) 
    {
      baseEnc = builtinFont->defaultBaseEnc;
      hasEncoding = true;
    } 
    else if (type == fontTrueType) 
    {
      baseEnc = winAnsiEncoding;
    } 
    else 
    {
      baseEnc = standardEncoding;
    }
  }

  // copy the base encoding
  for (i = 0; i < 256; ++i) 
  {
    enc[i] = (char *)baseEnc[i];
    if ((encFree[i] = baseEncFromFontFile) && enc[i]) 
    {
      enc[i] = qstrdup(baseEnc[i]);
    }
  }

  // some Type 1C font files have empty encodings, which can break the
  // T1C->T1 conversion (since the 'seac' operator depends on having
  // the accents in the encoding), so we fill in any gaps from
  // StandardEncoding
  if (type == fontType1C && embFontID.num >= 0 && baseEncFromFontFile) 
  {
    for (i = 0; i < 256; ++i) 
    {
      if (!enc[i] && standardEncoding[i]) 
      {
				enc[i] = (char *)standardEncoding[i];
				encFree[i] = false;
      }
    }
  }

  // merge differences into encoding
  if (obj1.isDict()) 
  {
    obj1.dictLookup("Differences", &obj2);
    if (obj2.isArray()) 
    {
      hasEncoding = true;
      code = 0;
      for (i = 0; i < obj2.arrayGetLength(); ++i) 
      {
				obj2.arrayGet(i, &obj3);
				if (obj3.isInt()) 
				{
	  		code = obj3.getInt();
				} 
				else if (obj3.isName()) 
				{
	  			if (code >= 0 && code < 256) 
	  			{
	    			if (encFree[code]) 
	    			{
	      			delete [] (enc[code]);
	    			}
	    			enc[code] = qstrdup(obj3.getName());
	    			encFree[code] = true;
	  			}
	  			++code;
				} 
				else 
				{
	  			xwApp->error("Wrong type in font encoding resource differences");
				}
				obj3.free();
      }
    }
    obj2.free();
  }
  obj1.free();
  if (ffT1) 
  {
    delete ffT1;
  }
  if (ffT1C) 
  {
    delete ffT1C;
  }

  //----- build the mapping to Unicode -----

  // pass 1: use the name-to-Unicode mapping table
  missing = hex = false;
  XWFontSetting fontsetting;
  for (code = 0; code < 256; ++code) 
  {
    if ((charName = enc[code])) 
    {
      if (!(toUnicode[code] = fontsetting.mapNameToUnicode(charName)) && strcmp(charName, ".notdef")) 
      {
				// if it wasn't in the name-to-Unicode table, check for a
				// name that looks like 'Axx' or 'xx', where 'A' is any letter
				// and 'xx' is two hex digits
				if ((strlen(charName) == 3 &&
	     			isalpha(charName[0]) &&
	    			 isxdigit(charName[1]) && isxdigit(charName[2]) &&
	     			((charName[1] >= 'a' && charName[1] <= 'f') ||
	      		(charName[1] >= 'A' && charName[1] <= 'F') ||
	      		(charName[2] >= 'a' && charName[2] <= 'f') ||
	      		(charName[2] >= 'A' && charName[2] <= 'F'))) ||
	    			(strlen(charName) == 2 &&
	     			isxdigit(charName[0]) && isxdigit(charName[1]) &&
	     			((charName[0] >= 'a' && charName[0] <= 'f') ||
	      		(charName[0] >= 'A' && charName[0] <= 'F') ||
	      		(charName[1] >= 'a' && charName[1] <= 'f') ||
	      		(charName[1] >= 'A' && charName[1] <= 'F')))) 
	      {
	  			hex = true;
				}
				missing = true;
      }
    } 
    else 
    {
      toUnicode[code] = 0;
    }
  }

  // pass 2: try to fill in the missing chars, looking for names of
  // any of the following forms:
  // - 'xx'
  // - 'Axx'
  // - 'nn'
  // - 'Ann'
  // - 'ABnn'
  // - 'unixxxx' (possibly followed by garbage - some Arabic files
  //             use 'uni0628.medi', etc.)
  // where 'A' and 'B' are any letters, 'xx' is two hex digits, 'xxxx'
  // is four hex digits, and 'nn' is 2-4 decimal digits
  XWDocSetting docsetting;
  if (missing && docsetting.getMapNumericCharNames()) 
  {
    for (code = 0; code < 256; ++code) 
    {
      if ((charName = enc[code]) && !toUnicode[code] && strcmp(charName, ".notdef")) 
      {
				n = (int)strlen(charName);
				code2 = -1;
				if (hex && n == 3 && isalpha(charName[0]) && isxdigit(charName[1]) && isxdigit(charName[2])) 
				{
	  			sscanf(charName+1, "%x", &code2);
				} 
				else if (hex && n == 2 && isxdigit(charName[0]) && isxdigit(charName[1])) 
				{
	  			sscanf(charName, "%x", &code2);
				} 
				else if (!hex && n >= 2 && n <= 4 && isdigit(charName[0]) && isdigit(charName[1])) 
				{
	  			code2 = atoi(charName);
				} 
				else if (n >= 3 && n <= 5 && isdigit(charName[1]) && isdigit(charName[2])) 
				{
	  			code2 = atoi(charName+1);
				} 
				else if (n >= 4 && n <= 6 && isdigit(charName[2]) && isdigit(charName[3])) 
				{
	 			 	code2 = atoi(charName+2);
				} 
				else if (n >= 7 && 
					       charName[0] == 'u' && 
					       charName[1] == 'n' && 
					       charName[2] == 'i' &&
		   					 isxdigit(charName[3]) && 
		   					 isxdigit(charName[4]) &&
		   					 isxdigit(charName[5]) && 
		   					 isxdigit(charName[6])) 
		   {
	  			sscanf(charName + 3, "%x", &code2);
				}
				if (code2 >= 0 && code2 <= 0xffff) 
				{
	  			toUnicode[code] = (int)code2;
				}
     	}
   	}

  // if the 'mapUnknownCharNames' flag is set, do a simple pass-through
  // mapping for unknown character names
  } else if (missing && docsetting.getMapUnknownCharNames()) 
  {
    for (code = 0; code < 256; ++code) 
    {
      if (!toUnicode[code]) 
      {
				toUnicode[code] = code;
      }
    }
  }

  // construct the char code -> Unicode mapping object
  ctu = XWCharCodeToUnicode::make8BitToUnicode(toUnicode);

  // merge in a ToUnicode CMap, if there is one -- this overwrites
  // existing entries in ctu, i.e., the ToUnicode CMap takes
  // precedence, but the other encoding info is allowed to fill in any
  // holes
  readToUnicodeCMap(fontDict, 8, ctu);

  // look for a Unicode-to-Unicode mapping
  if (name && (utu = fontsetting.getUnicodeToUnicode(name))) 
  {
    for (i = 0; i < 256; ++i) 
    {
      toUnicode[i] = 0;
    }
    ctu2 = XWCharCodeToUnicode::make8BitToUnicode(toUnicode);
    for (i = 0; i < 256; ++i) 
    {
      n = ctu->mapToUnicode((uint)i, uBuf, 8);
      if (n >= 1) 
      {
				n = utu->mapToUnicode((uint)uBuf[0], uBuf, 8);
				if (n >= 1) 
				{
	  			ctu2->setMapping((uint)i, uBuf, n);
				}
      }
    }
    utu->decRefCnt();
    delete ctu;
    ctu = ctu2;
  }

  //----- get the character widths -----

  // initialize all widths
  for (code = 0; code < 256; ++code) 
 	{
    widths[code] = missingWidth * 0.001;
  }

  // use widths from font dict, if present
  fontDict->lookup("FirstChar", &obj1);
  firstChar = obj1.isInt() ? obj1.getInt() : 0;
  obj1.free();
  if (firstChar < 0 || firstChar > 255) 
  {
    firstChar = 0;
  }
  fontDict->lookup("LastChar", &obj1);
  lastChar = obj1.isInt() ? obj1.getInt() : 255;
  obj1.free();
  if (lastChar < 0 || lastChar > 255) 
  {
    lastChar = 255;
  }
  mul = (type == fontType3) ? fontMat[0] : 0.001;
  fontDict->lookup("Widths", &obj1);
  if (obj1.isArray()) 
  {
    flags |= fontFixedWidth;
    if (obj1.arrayGetLength() < lastChar - firstChar + 1) 
    {
      lastChar = firstChar + obj1.arrayGetLength() - 1;
    }
    for (code = firstChar; code <= lastChar; ++code) 
    {
      obj1.arrayGet(code - firstChar, &obj2);
      if (obj2.isNum()) 
      {
				widths[code] = obj2.getNum() * mul;
				if (fabs(widths[code] - widths[firstChar]) > 0.00001) 
				{
	  			flags &= ~fontFixedWidth;
				}
      }
      obj2.free();
    }

  // use widths from built-in font
  } 
  else if (builtinFont) 
  {
    // this is a kludge for broken PDF files that encode char 32
    // as .notdef
    if (builtinFont->widths->getWidth("space", &w)) 
    {
      widths[32] = 0.001 * w;
    }
    for (code = 0; code < 256; ++code) 
    {
      if (enc[code] && builtinFont->widths->getWidth(enc[code], &w)) 
      {
				widths[code] = 0.001 * w;
      }
    }

  // couldn't find widths -- use defaults 
  } 
  else 
  {
    // this is technically an error -- the Widths entry is required
    // for all but the Base-14 fonts -- but certain PDF generators
    // apparently don't include widths for Arial and TimesNewRoman
    if (isFixedWidth()) 
    {
      i = 0;
    } 
    else if (isSerif()) 
    {
      i = 8;
    } 
    else 
    {
      i = 4;
    }
    if (isBold()) 
    {
      i += 2;
    }
    if (isItalic()) 
    {
      i += 1;
    }
    builtinFont = builtinFontSubst[i];
    // this is a kludge for broken PDF files that encode char 32
    // as .notdef
    if (builtinFont->widths->getWidth("space", &w)) 
    {
      widths[32] = 0.001 * w;
    }
    for (code = 0; code < 256; ++code) 
    {
      if (enc[code] && builtinFont->widths->getWidth(enc[code], &w)) 
      {
				widths[code] = 0.001 * w;
      }
    }
  }
  obj1.free();

  ok = true;
}

XWCore8BitFont::~XWCore8BitFont()
{
    for (int i = 0; i < 256; ++i) 
    {
        if (encFree[i] && enc[i]) 
            delete [] enc[i];
    }
    
    ctu->decRefCnt();
    if (charProcs.isDict()) 
        charProcs.free();
    
    if (resources.isDict()) 
        resources.free();
}

int XWCore8BitFont::getNextChar(char *s, 
                                int , 
                                uint *code,
			                    uint *u, 
			                    int uSize, 
			                    int *uLen,
			                    double *dx, 
			                    double *dy, 
			                    double *ox, 
			                    double *oy)
{
  int c;

  *code = c = (int)(*s & 0xff);
  *uLen = ctu->mapToUnicode(c, u, uSize);
  *dx = widths[c];
  *dy = *ox = *oy = 0;
  return 1;
}

XWObject * XWCore8BitFont::getCharProc(int code, XWObject *proc)
{
    if (enc[code] && charProcs.isDict()) 
        charProcs.dictLookup(enc[code], proc);
    else 
        proc->initNull();
        
    return proc;
}

XWDict * XWCore8BitFont::getCharProcs()
{
    return charProcs.isDict() ? charProcs.getDict() : (XWDict *)NULL;
}

int * XWCore8BitFont::getCodeToGIDMap(XWFontFileTrueType *ff)
{
  int *map;
  int cmapPlatform, cmapEncoding;
  int unicodeCmap, macRomanCmap, msSymbolCmap, cmap;
  bool useMacRoman, useUnicode;
  char *charName;
  uint u;
  int code, i, n;

  map = (int *)malloc(256 * sizeof(int));
  for (i = 0; i < 256; ++i) {
    map[i] = 0;
  }

  // To match up with the Adobe-defined behaviour, we choose a cmap
  // like this:
  // 1. If the PDF font has an encoding:
  //    1a. If the PDF font specified MacRomanEncoding and the
  //        TrueType font has a Macintosh Roman cmap, use it, and
  //        reverse map the char names through MacRomanEncoding to
  //        get char codes.
  //    1b. If the PDF font is not symbolic or the PDF font is not
  //        embedded, and the TrueType font has a Microsoft Unicode
  //        cmap or a non-Microsoft Unicode cmap, use it, and use the
  //        Unicode indexes, not the char codes.
  //    1c. If the PDF font is symbolic and the TrueType font has a
  //        Microsoft Symbol cmap, use it, and use char codes
  //        directly (possibly with an offset of 0xf000).
  //    1d. If the TrueType font has a Macintosh Roman cmap, use it,
  //        as in case 1a.
  // 2. If the PDF font does not have an encoding or the PDF font is
  //    symbolic:
  //    2a. If the TrueType font has a Macintosh Roman cmap, use it,
  //        and use char codes directly (possibly with an offset of
  //        0xf000).
  //    2b. If the TrueType font has a Microsoft Symbol cmap, use it,
  //        and use char codes directly (possible with an offset of
  //        0xf000).
  // 3. If none of these rules apply, use the first cmap and hope for
  //    the best (this shouldn't happen).
  unicodeCmap = macRomanCmap = msSymbolCmap = -1;
  for (i = 0; i < ff->getNumCmaps(); ++i) {
    cmapPlatform = ff->getCmapPlatform(i);
    cmapEncoding = ff->getCmapEncoding(i);
    if ((cmapPlatform == 3 && cmapEncoding == 1) ||
	cmapPlatform == 0) {
      unicodeCmap = i;
    } else if (cmapPlatform == 1 && cmapEncoding == 0) {
      macRomanCmap = i;
    } else if (cmapPlatform == 3 && cmapEncoding == 0) {
      msSymbolCmap = i;
    }
  }
  cmap = 0;
  useMacRoman = false;
  useUnicode = false;
  XWFontSetting fontsetting;
  if (hasEncoding) {
    if (usesMacRomanEnc && macRomanCmap >= 0) {
      cmap = macRomanCmap;
      useMacRoman = true;
    } else if ((!(flags & fontSymbolic) || embFontID.num < 0) &&
	       unicodeCmap >= 0) {
      cmap = unicodeCmap;
      useUnicode = true;
    } else if ((flags & fontSymbolic) && msSymbolCmap >= 0) {
      cmap = msSymbolCmap;
    } else if ((flags & fontSymbolic) && macRomanCmap >= 0) {
      cmap = macRomanCmap;
    } else if (macRomanCmap >= 0) {
      cmap = macRomanCmap;
      useMacRoman = true;
    }
  } else {
    if (msSymbolCmap >= 0) {
      cmap = msSymbolCmap;
    } else if (macRomanCmap >= 0) {
      cmap = macRomanCmap;
    }
  }

  // reverse map the char names through MacRomanEncoding, then map the
  // char codes through the cmap
  if (useMacRoman) {
    for (i = 0; i < 256; ++i) {
      if ((charName = enc[i])) {
	if ((code = fontsetting.getMacRomanCharCode(charName))) {
	  map[i] = ff->mapCodeToGID(cmap, code);
	}
      } else {
	map[i] = -1;
      }
    }

  // map Unicode through the cmap
  } else if (useUnicode) {
    for (i = 0; i < 256; ++i) {
      if (((charName = enc[i]) &&
	   (u = fontsetting.mapNameToUnicode(charName))) ||
	  (n = ctu->mapToUnicode((uint)i, &u, 1))) {
	map[i] = ff->mapCodeToGID(cmap, u);
      } else {
	map[i] = -1;
      }
    }

  // map the char codes through the cmap, possibly with an offset of
  // 0xf000
  } else {
    for (i = 0; i < 256; ++i) {
      if (!(map[i] = ff->mapCodeToGID(cmap, i))) {
	map[i] = ff->mapCodeToGID(cmap, 0xf000 + i);
      }
    }
  }

  // try the TrueType 'post' table to handle any unmapped characters
  for (i = 0; i < 256; ++i) {
    if (map[i] <= 0 && (charName = enc[i])) {
      map[i] = ff->mapNameToGID(charName);
    }
  }

  return map;
}

XWDict * XWCore8BitFont::getResources()
{
    return resources.isDict() ? resources.getDict() : (XWDict *)NULL;
}

XWCharCodeToUnicode * XWCore8BitFont::getToUnicode()
{
    ctu->incRefCnt();
    return ctu;
}

static int cmpWidthExcep(const void *w1, const void *w2) 
{
    return ((CoreFontCIDWidthExcep *)w1)->first - ((CoreFontCIDWidthExcep *)w2)->first;
}

static int cmpWidthExcepV(const void *w1, const void *w2) 
{
    return ((CoreFontCIDWidthExcepV *)w1)->first - ((CoreFontCIDWidthExcepV *)w2)->first;
}

XWCoreCIDFont::XWCoreCIDFont(XWRef *xref, 
                 						char *tagA, 
                 						ObjRef idA, 
                 						XWString *nameA,
	     					 						CoreFontType typeA, 
	     					 						ObjRef embFontIDA, 
	     					 						XWDict *fontDict)
    :XWCoreFont(tagA, idA, nameA, typeA, embFontIDA)
{
  XWDict *desFontDict;
  XWObject desFontDictObj;
  XWObject obj1, obj2, obj3, obj4, obj5, obj6;
  XWCharCodeToUnicode *utu;
  int c;
  uint uBuf[8];
  int c1, c2;
  int excepsSize, i, j, k, n;
  XWFontSetting fontsetting;

  ascent = 0.95;
  descent = -0.35;
  fontBBox[0] = fontBBox[1] = fontBBox[2] = fontBBox[3] = 0;
  collection = NULL;
  cMap = NULL;
  ctu = NULL;
  ctuUsesCharCode = true;
  widths.defWidth = 1.0;
  widths.defHeight = -1.0;
  widths.defVY = 0.880;
  widths.exceps = NULL;
  widths.nExceps = 0;
  widths.excepsV = NULL;
  widths.nExcepsV = 0;
  cidToGID = NULL;
  cidToGIDLen = 0;

  // get the descendant font
  if (!fontDict->lookup("DescendantFonts", &obj1)->isArray() || obj1.arrayGetLength() == 0) 
  {
    xwApp->error("Missing or empty DescendantFonts entry in Type 0 font");
    obj1.free();

    goto err1;
  }
  if (!obj1.arrayGet(0, &desFontDictObj)->isDict()) 
  {
    xwApp->error("Bad descendant font in Type 0 font");
    goto err2;
  }
  obj1.free();
  desFontDict = desFontDictObj.getDict();

  // get info from font descriptor
  readFontDescriptor(xref, desFontDict);

  //----- encoding info -----

  // char collection
  if (!desFontDict->lookup("CIDSystemInfo", &obj1)->isDict()) 
  {
    xwApp->error("Missing CIDSystemInfo dictionary in Type 0 descendant font");
    goto err2;
  }
  obj1.dictLookup("Registry", &obj2);
  obj1.dictLookup("Ordering", &obj3);
  if (!obj2.isString() || !obj3.isString()) 
  {
    xwApp->error("Invalid CIDSystemInfo dictionary in Type 0 descendant font");
    goto err3;
  }
  collection = obj2.getString()->copy()->append('-')->append(obj3.getString());
  obj3.free();
  obj2.free();
  obj1.free();

  // look for a ToUnicode CMap  
  if (!(ctu = readToUnicodeCMap(fontDict, 16, NULL))) 
  {
    ctuUsesCharCode = false;

    // use an identity mapping for the "Adobe-Identity" and
    // "Adobe-UCS" collections
    if (!collection->cmp("Adobe-Identity") || !collection->cmp("Adobe-UCS")) 
    {
      ctu = XWCharCodeToUnicode::makeIdentityMapping();

    // look for a user-supplied .cidToUnicode file
    } 
    else if (!(ctu = fontsetting.getCIDToUnicode(collection))) 
    {
      xwApp->error("Unknown character collection");
    }
  }

  // look for a Unicode-to-Unicode mapping
  if (name && (utu = fontsetting.getUnicodeToUnicode(name))) 
  {
    if (ctu) 
    {    
      for (c = 0; c < ctu->getLength(); ++c) 
      {
				n = ctu->mapToUnicode(c, uBuf, 8);
				if (n >= 1) 
				{
	  			n = utu->mapToUnicode((int)uBuf[0], uBuf, 8);
	  			if (n >= 1) 
	  			{
	    			ctu->setMapping(c, uBuf, n);
	  			}
				}
      }
      utu->decRefCnt();
    } 
    else 
    {
      ctu = utu;
    }
  }

  // encoding (i.e., CMap)
  if (fontDict->lookup("Encoding", &obj1)->isNull()) 
  {
    xwApp->error("Missing Encoding entry in Type 0 font");
    goto err2;
  }
  if (!(cMap = XWCMap::parse(NULL, collection, &obj1))) 
  {
    goto err2;
  }
  obj1.free();

  // CIDToGIDMap
  // (the PDF spec only allows these for TrueType fonts, but Acrobat
  // apparently also allows them for OpenType CFF fonts)
  if (type == fontCIDType2 || type == fontCIDType0COT) 
  {
    desFontDict->lookup("CIDToGIDMap", &obj1);
    if (obj1.isStream()) 
    {
      cidToGIDLen = 0;
      i = 64;
      cidToGID = (int *)malloc(i * sizeof(int));
      obj1.streamReset();
      while ((c1 = obj1.streamGetChar()) != EOF && (c2 = obj1.streamGetChar()) != EOF) 
      {
				if (cidToGIDLen == i) 
				{
	  			i *= 2;
	  			cidToGID = (int *)realloc(cidToGID, i * sizeof(int));
				}
				cidToGID[cidToGIDLen++] = (c1 << 8) + c2;
      }
    } 
    else if (!obj1.isName("Identity") && !obj1.isNull()) 
    {
      xwApp->error("Invalid CIDToGIDMap entry in CID font");
    }
    obj1.free();
  }

  //----- character metrics -----

  // default char width
  if (desFontDict->lookup("DW", &obj1)->isInt()) 
  {
    widths.defWidth = obj1.getInt() * 0.001;
  }
  obj1.free();

  // char width exceptions
  if (desFontDict->lookup("W", &obj1)->isArray()) 
  {
    excepsSize = 0;
    i = 0;
    while (i + 1 < obj1.arrayGetLength()) 
   	{
      obj1.arrayGet(i, &obj2);
      obj1.arrayGet(i + 1, &obj3);
      if (obj2.isInt() && obj3.isInt() && i + 2 < obj1.arrayGetLength()) 
      {
				if (obj1.arrayGet(i + 2, &obj4)->isNum()) 
				{
	  			if (widths.nExceps == excepsSize) 
	  			{
	    			excepsSize += 16;
	    			widths.exceps = (CoreFontCIDWidthExcep *)realloc(widths.exceps, excepsSize * sizeof(CoreFontCIDWidthExcep));
	  			}
	  			widths.exceps[widths.nExceps].first = obj2.getInt();
	  			widths.exceps[widths.nExceps].last = obj3.getInt();
	  			widths.exceps[widths.nExceps].width = obj4.getNum() * 0.001;
	  			++widths.nExceps;
				} 
				else 
				{
	  			xwApp->error("Bad widths array in Type 0 font");
				}
				obj4.free();
				i += 3;
      } 
      else if (obj2.isInt() && obj3.isArray()) 
      {
				if (widths.nExceps + obj3.arrayGetLength() > excepsSize) 
				{
	  			excepsSize = (widths.nExceps + obj3.arrayGetLength() + 15) & ~15;
	  			widths.exceps = (CoreFontCIDWidthExcep *)realloc(widths.exceps, excepsSize * sizeof(CoreFontCIDWidthExcep));
				}
				j = obj2.getInt();
				for (k = 0; k < obj3.arrayGetLength(); ++k) 
				{
	  			if (obj3.arrayGet(k, &obj4)->isNum()) 
	  			{
	    			widths.exceps[widths.nExceps].first = j;
	    			widths.exceps[widths.nExceps].last = j;
	    			widths.exceps[widths.nExceps].width = obj4.getNum() * 0.001;
	    			++j;
	    			++widths.nExceps;
	  			} 
	  			else 
	  			{
	    			xwApp->error("Bad widths array in Type 0 font");
	  			}
	  			obj4.free();
				}
				i += 2;
      } 
      else 
      {
				xwApp->error("Bad widths array in Type 0 font");
				++i;
      }
      obj3.free();
      obj2.free();
    }
    qsort(widths.exceps, widths.nExceps, sizeof(CoreFontCIDWidthExcep),
	  &cmpWidthExcep);
  }
  obj1.free();

  // default metrics for vertical font
  if (desFontDict->lookup("DW2", &obj1)->isArray() && obj1.arrayGetLength() == 2) 
  {
    if (obj1.arrayGet(0, &obj2)->isNum()) 
    {
      widths.defVY = obj2.getNum() * 0.001;
    }
    obj2.free();
    if (obj1.arrayGet(1, &obj2)->isNum()) 
    {
      widths.defHeight = obj2.getNum() * 0.001;
    }
    obj2.free();
  }
  obj1.free();

  // char metric exceptions for vertical font
  if (desFontDict->lookup("W2", &obj1)->isArray()) 
  {
    excepsSize = 0;
    i = 0;
    while (i + 1 < obj1.arrayGetLength()) 
    {
      obj1.arrayGet(i, &obj2);
      obj1.arrayGet(i+ 1, &obj3);
      if (obj2.isInt() && obj3.isInt() && i + 4 < obj1.arrayGetLength()) 
      {
				if (obj1.arrayGet(i + 2, &obj4)->isNum() &&
	    			obj1.arrayGet(i + 3, &obj5)->isNum() &&
	    			obj1.arrayGet(i + 4, &obj6)->isNum()) 
	     {
	  			if (widths.nExcepsV == excepsSize) 
	  			{
	    			excepsSize += 16;
	    			widths.excepsV = (CoreFontCIDWidthExcepV *)realloc(widths.excepsV, excepsSize * sizeof(CoreFontCIDWidthExcepV));
	  			}
	  			widths.excepsV[widths.nExcepsV].first = obj2.getInt();
	  			widths.excepsV[widths.nExcepsV].last = obj3.getInt();
	  			widths.excepsV[widths.nExcepsV].height = obj4.getNum() * 0.001;
	  			widths.excepsV[widths.nExcepsV].vx = obj5.getNum() * 0.001;
	  			widths.excepsV[widths.nExcepsV].vy = obj6.getNum() * 0.001;
	  			++widths.nExcepsV;
				} 
				else 
				{
	  			xwApp->error("Bad widths (W2) array in Type 0 font");
				}
				obj6.free();
				obj5.free();
				obj4.free();
				i += 5;
      } 
      else if (obj2.isInt() && obj3.isArray()) 
      {
				if (widths.nExcepsV + obj3.arrayGetLength() / 3 > excepsSize) 
				{
	  			excepsSize = (widths.nExcepsV + obj3.arrayGetLength() / 3 + 15) & ~15;
	  			widths.excepsV = (CoreFontCIDWidthExcepV *)realloc(widths.excepsV, excepsSize * sizeof(CoreFontCIDWidthExcepV));
				}
				j = obj2.getInt();
				for (k = 0; k < obj3.arrayGetLength(); k += 3) 
				{
	  			if (obj3.arrayGet(k, &obj4)->isNum() &&
	      			obj3.arrayGet(k+1, &obj5)->isNum() &&
	      			obj3.arrayGet(k+2, &obj6)->isNum()) 
	      	{
	    			widths.excepsV[widths.nExcepsV].first = j;
	    			widths.excepsV[widths.nExcepsV].last = j;
	    			widths.excepsV[widths.nExcepsV].height = obj4.getNum() * 0.001;
	    			widths.excepsV[widths.nExcepsV].vx = obj5.getNum() * 0.001;
	    			widths.excepsV[widths.nExcepsV].vy = obj6.getNum() * 0.001;
	    			++j;
	    			++widths.nExcepsV;
	  			} 
	  			else 
	  			{
	    			xwApp->error("Bad widths (W2) array in Type 0 font");
	  			}
	  			obj6.free();
	  			obj5.free();
	  			obj4.free();
				}
				i += 2;
      } 
      else 
      {
				xwApp->error("Bad widths (W2) array in Type 0 font");
				++i;
      }
      obj3.free();
      obj2.free();
    }
    qsort(widths.excepsV, widths.nExcepsV, sizeof(CoreFontCIDWidthExcepV),
	  &cmpWidthExcepV);
  }
  obj1.free();

  desFontDictObj.free();
  ok = true;
  return;

 err3:
  obj3.free();
  obj2.free();
 err2:
  obj1.free();
  desFontDictObj.free();
 err1:;
}

XWCoreCIDFont::~XWCoreCIDFont()
{
	if (collection) 
    delete collection;
  
    if (cMap) 
        cMap->decRefCnt();
    
    if (ctu) 
        ctu->decRefCnt();
    
    if (widths.exceps)
        free(widths.exceps);
        
    if (widths.excepsV)
        free(widths.excepsV);
        
    if (cidToGID) 
        free(cidToGID);
}

XWString * XWCoreCIDFont::getCollection()
{
    return cMap ? cMap->getCollection() : (XWString *)0;
}

int XWCoreCIDFont::getNextChar(char *s, 
                               int len, 
                               uint *code,
			                   uint *u, 
			                   int uSize, 
			                   int *uLen,
			                   double *dx, 
			                   double *dy, 
			                   double *ox, 
			                   double *oy)
{
  uint cid;
  int c;
  double w, h, vx, vy;
  int n, a, b, m;
  XWDocSetting docsetting;

  if (!cMap) {
    *code = 0;
    *uLen = 0;
    *dx = *dy = 0;
    return 1;
  }

  *code = (uint)(cid = cMap->getCID(s, len, &c, &n));
  if (ctu) {
    *uLen = ctu->mapToUnicode(ctuUsesCharCode ? c : cid, u, uSize);
  } else {
    *uLen = 0;
  }
  if (!*uLen && uSize >= 1 && docsetting.getMapUnknownCharNames()) {
    u[0] = *code;
    *uLen = 1;
  }

  // horizontal
  if (cMap->getWMode() == 0) {
    w = widths.defWidth;
    h = vx = vy = 0;
    if (widths.nExceps > 0 && cid >= widths.exceps[0].first) {
      a = 0;
      b = widths.nExceps;
      // invariant: widths.exceps[a].first <= cid < widths.exceps[b].first
      while (b - a > 1) {
	m = (a + b) / 2;
	if (widths.exceps[m].first <= cid) {
	  a = m;
	} else {
	  b = m;
	}
      }
      if (cid <= widths.exceps[a].last) {
	w = widths.exceps[a].width;
      }
    }

  // vertical
  } else {
    w = 0;
    h = widths.defHeight;
    vx = widths.defWidth / 2;
    vy = widths.defVY;
    if (widths.nExcepsV > 0 && cid >= widths.excepsV[0].first) {
      a = 0;
      b = widths.nExcepsV;
      // invariant: widths.excepsV[a].first <= cid < widths.excepsV[b].first
      while (b - a > 1) {
	m = (a + b) / 2;
	if (widths.excepsV[m].last <= cid) {
	  a = m;
	} else {
	  b = m;
	}
      }
      if (cid <= widths.excepsV[a].last) {
	h = widths.excepsV[a].height;
	vx = widths.excepsV[a].vx;
	vy = widths.excepsV[a].vy;
      }
    }
  }

  *dx = w;
  *dy = h;
  *ox = vx;
  *oy = vy;

  return n;
}

XWCharCodeToUnicode * XWCoreCIDFont::getToUnicode()
{
    if (ctu) 
        ctu->incRefCnt();
        
    return ctu;
}

int XWCoreCIDFont::getWMode()
{
    return cMap ? cMap->getWMode() : 0;
}

XWCoreFontDict::XWCoreFontDict(XWRef *xref, ObjRef *fontDictRef, XWDict *fontDict)
{
	int i;
  XWObject obj1, obj2;
  ObjRef r;

  numFonts = fontDict->getLength();
  fonts = (XWCoreFont **)malloc(numFonts * sizeof(XWCoreFont *));
  for (i = 0; i < numFonts; ++i) {
    fontDict->getValNF(i, &obj1);
    obj1.fetch(xref, &obj2);
    if (obj2.isDict()) {
      if (obj1.isRef()) {
	r = obj1.getRef();
      } else {
	// no indirect reference for this font, so invent a unique one
	// (legal generation numbers are five digits, so any 6-digit
	// number would be safe)
	r.num = i;
	if (fontDictRef) {
	  r.gen = 100000 + fontDictRef->num;
	} else {
	  r.gen = 999999;
	}
      }
      fonts[i] = XWCoreFont::makeFont(xref, fontDict->getKey(i),
				   r, obj2.getDict());
      if (fonts[i] && !fonts[i]->isOk()) {
	delete fonts[i];
	fonts[i] = NULL;
      }
    } else {
      xwApp->error("font resource is not a dictionary");
      fonts[i] = NULL;
    }
    obj1.free();
    obj2.free();
  }
}

XWCoreFontDict::~XWCoreFontDict()
{
    for (int i = 0; i < numFonts; ++i) 
    {
        if (fonts[i]) 
            delete fonts[i];
    }
    
    if (fonts)
        free(fonts);
}

XWCoreFont * XWCoreFontDict::lookup(const char *tag)
{
    for (int i = 0; i < numFonts; ++i) 
    {
        if (fonts[i] && fonts[i]->matches(tag)) 
            return fonts[i];
    }
    return 0;
}

