/****************************************************************************
 **
 ** 版权所有 (C) 2006-2007 秀文公司.
 **
 ****************************************************************************/
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <QtDebug>
#include "XWApplication.h"
#include "XWPDFLexer.h"

static char specialChars[256] = {
  1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0,   // 0x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 1x
  1, 0, 0, 0, 0, 2, 0, 0, 2, 2, 0, 0, 0, 0, 0, 2,   // 2x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0,   // 3x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 4x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0,   // 5x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 6x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 2, 0, 0,   // 7x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 8x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // 9x
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // ax
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // bx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // cx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // dx
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   // ex
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0    // fx
};

XWPDFLexer::XWPDFLexer(XWRef *xref, XWStream *str, QObject * parent)
    :QObject(parent)
{
  XWObject obj;

  curStr.initStream(str);
  streams = new XWArray(xref);
  streams->add(curStr.copy(&obj));
  strPtr = 0;
  freeArray = true;
  curStr.streamReset();
}

XWPDFLexer::XWPDFLexer(XWRef *xref, XWObject *obj, QObject * parent)
    :QObject(parent)
{
  XWObject obj2;

  if (obj->isStream()) {
    streams = new XWArray(xref);
    freeArray = true;
    streams->add(obj->copy(&obj2));
  } else {
    streams = obj->getArray();
    freeArray = false;
  }
  strPtr = 0;
  if (streams->getLength() > 0) {
    streams->get(strPtr, &curStr);
    curStr.streamReset();
  }
}

XWPDFLexer::~XWPDFLexer()
{
  if (!curStr.isNone()) {
    curStr.streamClose();
    curStr.free();
  }
  if (freeArray) {
    delete streams;
  }
}

XWObject * XWPDFLexer::getObj(XWObject *obj)
{
  char *p;
  int c, c2;
  bool comment, neg, done;
  int numParen;
  int xi;
  double xf, scale;
  XWString *s;
  int n, m;

  // skip whitespace and comments
  comment = false;
  while (1) {
    if ((c = getChar()) == EOF) {
      return obj->initEOF();
    }
    if (comment) {
      if (c == '\r' || c == '\n')
	comment = false;
    } else if (c == '%') {
      comment = true;
    } else if (specialChars[c] != 1) {
      break;
    }
  }

  // start reading token
  switch (c) {

  // number
  case '0': case '1': case '2': case '3': case '4':
  case '5': case '6': case '7': case '8': case '9':
  case '-': case '.':
    neg = false;
    xf = xi = 0;
    if (c == '-') {
      neg = true;
    } else if (c == '.') {
      goto doReal;
    } else {
      xf = xi = c - '0';
    }
    while (1) {
      c = lookChar();
      if (isdigit(c)) {
	getChar();
	xi = xi * 10 + (c - '0');
	xf = xf * 10 + (c - '0');
      } else if (c == '.') {
	getChar();
	goto doReal;
      } else {
	break;
      }
    }
    if (neg) {
      xi = -xi;
    }
    obj->initInt(xi);
    break;
  doReal:
    scale = 0.1;
    while (1) {
      c = lookChar();
      if (c == '-') {
	// ignore minus signs in the middle of numbers to match
	// Adobe's behavior
	xwApp->error(tr("Badly formatted number"));
	getChar();
	continue;
      }
      if (!isdigit(c)) {
	break;
      }
      getChar();
      xf = xf + scale * (c - '0');
      scale *= 0.1;
    }
    if (neg) {
      xf = -xf;
    }
    obj->initReal(xf);
    break;

  // string
  case '(':
    p = tokBuf;
    n = 0;
    numParen = 1;
    done = false;
    s = NULL;
    do {
      c2 = EOF;
      switch (c = getChar()) {

      case EOF:
#if 0
      // This breaks some PDF files, e.g., ones from Photoshop.
      case '\r':
      case '\n':
#endif
	xwApp->error(tr("Unterminated string"));
	done = true;
	break;

      case '(':
	++numParen;
	c2 = c;
	break;

      case ')':
	if (--numParen == 0) {
	  done = true;
	} else {
	  c2 = c;
	}
	break;

      case '\\':
	switch (c = getChar()) {
	case 'n':
	  c2 = '\n';
	  break;
	case 'r':
	  c2 = '\r';
	  break;
	case 't':
	  c2 = '\t';
	  break;
	case 'b':
	  c2 = '\b';
	  break;
	case 'f':
	  c2 = '\f';
	  break;
	case '\\':
	case '(':
	case ')':
	  c2 = c;
	  break;
	case '0': case '1': case '2': case '3':
	case '4': case '5': case '6': case '7':
	  c2 = c - '0';
	  c = lookChar();
	  if (c >= '0' && c <= '7') {
	    getChar();
	    c2 = (c2 << 3) + (c - '0');
	    c = lookChar();
	    if (c >= '0' && c <= '7') {
	      getChar();
	      c2 = (c2 << 3) + (c - '0');
	    }
	  }
	  break;
	case '\r':
	  c = lookChar();
	  if (c == '\n') {
	    getChar();
	  }
	  break;
	case '\n':
	  break;
	case EOF:
	  xwApp->error(tr("Unterminated string"));
	  done = true;
	  break;
	default:
	  c2 = c;
	  break;
	}
	break;

      default:
	c2 = c;
	break;
      }

      if (c2 != EOF) {
	if (n == PDF_TOKBUFSIZE) {
	  if (!s)
	    s = new XWString(tokBuf, PDF_TOKBUFSIZE);
	  else
	    s->append(tokBuf, PDF_TOKBUFSIZE);
	  p = tokBuf;
	  n = 0;
	}
	*p++ = (char)c2;
	++n;
      }
    } while (!done);
    if (!s)
      s = new XWString(tokBuf, n);
    else
      s->append(tokBuf, n);
    obj->initString(s);
    break;

  // name
  case '/':
    p = tokBuf;
    n = 0;
    s = NULL;
    while ((c = lookChar()) != EOF && !specialChars[c]) {
      getChar();
      if (c == '#') {
	c2 = lookChar();
	if (c2 >= '0' && c2 <= '9') {
	  c = c2 - '0';
	} else if (c2 >= 'A' && c2 <= 'F') {
	  c = c2 - 'A' + 10;
	} else if (c2 >= 'a' && c2 <= 'f') {
	  c = c2 - 'a' + 10;
	} else {
	  goto notEscChar;
	}
	getChar();
	c <<= 4;
	c2 = getChar();
	if (c2 >= '0' && c2 <= '9') {
	  c += c2 - '0';
	} else if (c2 >= 'A' && c2 <= 'F') {
	  c += c2 - 'A' + 10;
	} else if (c2 >= 'a' && c2 <= 'f') {
	  c += c2 - 'a' + 10;
	} else {
	  xwApp->error(tr("Illegal digit in hex char in name"));
	}
      }
     notEscChar:
      // the PDF spec claims that names are limited to 127 chars, but
      // Distiller 8 will produce longer names, and Acrobat 8 will
      // accept longer names
      ++n;
      if (n < PDF_TOKBUFSIZE) {
	*p++ = c;
      } else if (n == PDF_TOKBUFSIZE) {
	*p = c;
	s = new XWString(tokBuf, n);
      } else {
	s->append((char)c);
      }
    }
    if (n < PDF_TOKBUFSIZE) {
      *p = '\0';
      obj->initName(tokBuf);
    } else {
      obj->initName(s->getCString());
      delete s;
    }
    break;

  // array punctuation
  case '[':
  case ']':
    tokBuf[0] = c;
    tokBuf[1] = '\0';
    obj->initCmd(tokBuf);
    break;

  // hex string or dict punctuation
  case '<':
    c = lookChar();

    // dict punctuation
    if (c == '<') {
      getChar();
      tokBuf[0] = tokBuf[1] = '<';
      tokBuf[2] = '\0';
      obj->initCmd(tokBuf);

    // hex string
    } else {
      p = tokBuf;
      m = n = 0;
      c2 = 0;
      s = NULL;
      while (1) {
	c = getChar();
	if (c == '>') {
	  break;
	} else if (c == EOF) {
	  xwApp->error(tr("Unterminated hex string"));
	  break;
	} else if (specialChars[c] != 1) {
	  c2 = c2 << 4;
	  if (c >= '0' && c <= '9')
	    c2 += c - '0';
	  else if (c >= 'A' && c <= 'F')
	    c2 += c - 'A' + 10;
	  else if (c >= 'a' && c <= 'f')
	    c2 += c - 'a' + 10;
	  else
	    xwApp->error(tr("Illegal character <{0:02x}> in hex string"));
	  if (++m == 2) {
	    if (n == PDF_TOKBUFSIZE) {
	      if (!s)
		s = new XWString(tokBuf, PDF_TOKBUFSIZE);
	      else
		s->append(tokBuf, PDF_TOKBUFSIZE);
	      p = tokBuf;
	      n = 0;
	    }
	    *p++ = (char)c2;
	    ++n;
	    c2 = 0;
	    m = 0;
	  }
	}
      }
      if (!s)
	s = new XWString(tokBuf, n);
      else
	s->append(tokBuf, n);
      if (m == 1)
	s->append((char)(c2 << 4));
      obj->initString(s);
    }
    break;

  // dict punctuation
  case '>':
    c = lookChar();
    if (c == '>') {
      getChar();
      tokBuf[0] = tokBuf[1] = '>';
      tokBuf[2] = '\0';
      obj->initCmd(tokBuf);
    } else {
      xwApp->error(tr("Illegal character '>'"));
      obj->initError();
    }
    break;

  // error
  case ')':
  case '{':
  case '}':
    xwApp->error(tr("Illegal character '{0:c}'"));
    obj->initError();
    break;

  // command
  default:
    p = tokBuf;
    *p++ = c;
    n = 1;
    while ((c = lookChar()) != EOF && !specialChars[c]) {
      getChar();
      if (++n == PDF_TOKBUFSIZE) {
	xwApp->error(tr("Command token too long"));
	break;
      }
      *p++ = c;
    }
    *p = '\0';
    if (tokBuf[0] == 't' && !strcmp(tokBuf, "true")) {
      obj->initBool(true);
    } else if (tokBuf[0] == 'f' && !strcmp(tokBuf, "false")) {
      obj->initBool(false);
    } else if (tokBuf[0] == 'n' && !strcmp(tokBuf, "null")) {
      obj->initNull();
    } else {
      obj->initCmd(tokBuf);
    }
    break;
  }

  return obj;
}

bool XWPDFLexer::isSpace(int c)
{
  return c >= 0 && c <= 0xff && specialChars[c] == 1;
}

void XWPDFLexer::skipToNextLine()
{
  int c;

  while (1) {
    c = getChar();
    if (c == EOF || c == '\n') {
      return;
    }
    if (c == '\r') {
      if ((c = lookChar()) == '\n') {
	getChar();
      }
      return;
    }
  }
}

int XWPDFLexer::getChar()
{
  int c;

  c = EOF;
  while (!curStr.isNone() && (c = curStr.streamGetChar()) == EOF) {
    curStr.streamClose();
    curStr.free();
    ++strPtr;
    if (strPtr < streams->getLength()) {
      streams->get(strPtr, &curStr);
      curStr.streamReset();
    }
  }
  return c;
}

int XWPDFLexer::lookChar()
{
  if (curStr.isNone()) {
    return EOF;
  }
  return curStr.streamLookChar();
}

