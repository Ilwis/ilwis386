/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* Implementation for String for ILWIS 2.0
   sept 1993, by Jelle Wind
   (c) Computer Department ITC
	Last change:  WK   28 Sep 98    1:11 pm
*/
#define STRNG_C

#include "Headers\toolspch.h"
#include <stdarg.h>
#include <string.h>

#define StrSize 8
#undef isdigit

static char* _pcCharsToBeQuoted = "`~!@#$%^&()-+=[]{};, .'";
static char* _pcCharsNotAllowedOutsideQuotes = "`~!@#$%^&()-+=[]{};, \\/:*?\"<>"; 

//const String sUNDEF = String("?");

char cDecimalPoint()
#ifdef _Windows
{ char s[2];
  GetProfileString("intl", "sDecimal", ".", s, 2);
  return s[0];
}
#else
{ return '.'; }
#endif

bool _export fCIStrEqual(const String & s1, const String& s2)
{
  return _stricmp(s1.c_str(), s2.c_str())==0;
}
//------------------------------------------------------------------------------------------------
String::String() 
{}


String::String(const string& s) :
        string(s)
{}

String::String(const String& s) :
        string(s.scVal())
{}

String::String(const CString& s) :
        string((LPCTSTR)s)
{}

String::String(char *s) :
        string(s)
{
}

short String::iPos(char c) const
{
  int iRes=find_first_of(c);
  return iRes!=npos ? iRes : shUNDEF;
}

short String::iPos(String &s) const
{
  int iRes = find(s);
  return iRes!=npos ? iRes : shUNDEF;
}

String::String(const char* sFormat, ...) 
{
  if (sFormat) {
    va_list arg_ptr;
    va_start(arg_ptr, sFormat);
    VarFormat(sFormat, arg_ptr);
    va_end(arg_ptr);
  }
}

String::String(char c,unsigned int l) :
        string(l, c)
{
}

//String::operator bool() const
bool String::fVal() const
{
  const char* s = c_str();
  while (*s == ' ') s++;
  switch (*s) {
    case '0':
    case 'f':
    case 'F':
    case 'n':
    case 'N':
      return false;
    case '1':
    case 't':
    case 'T':
    case 'y':
    case 'Y':
      return true;
    default:
      return true;
  }
}

//String::operator long() const
long String::iVal() const
{
  return longConv(c_str());
}

//String::operator double() const
double String::rVal() const
{
  return doubleConv(c_str());
}

/*String& String::operator=(const String& s)
{
  if (0 == &s || 0 == s.p) {
    if (p) delete [] p;
    len = 6;
    block = 8;
    p = new char[block];
    strcpy(p, "(NULL)");
    return *this;
  }
  if (p == s.p) return *this;
//  len = s.length();
  len = strlen(s.scVal());
  if (len >= block) {
    if (p) delete [] p;
    p = new char[block = s.block];
  }
  strcpy(p, s.p);
  return *this;
}

String& String::operator=(const char* s) {
  if (!s) {
    *p = 0;
    len = 0;
  } else {
    len = strlen(s);
    if (len >= block) {
      block = adjust(len+1);
      if (p) delete [] p;
      p = new char[block];
    }
    strcpy(p, s);
  }
  return *this;
}

unsigned String::reSize(unsigned newCap) {
  if (newCap < length()) newCap = length();
  if (block != newCap+1) {
    block = adjust(newCap + 1);
    char *ptr = new char[block];
    strcpy(ptr, p);
    if (p) delete [] p;
    p = ptr;
  }
  return block-1;
}*/

String String::operator&(String& s)
{
  return String(*this + s);
  /*
  if (0 == &s || 0 == s.p)
    return *this;
  String t(' ', length() + s.length());
  strcpy(t.p, p);
  strcpy(&(t.p[length()]), s.p);
  t.len = length() + s.length();
  return t;*/
}

String String::operator&(const char* str)
{
  return operator&(String(str));
}

String operator&(const String& s1, const String& s2)
{
  String t = s1;
  t &= s2;
  return t;
}


const String& String::operator&=(const String& s) 
{
   *this += s;
  return *this;
}

String String::sSub(int iFirst, int iLength) const
{
	if (iFirst < 0) {
		iLength += iFirst;
		iFirst = 0;
	}
  return String(substr(iFirst, iLength));
}

String& String::toLower() 
{
  for (strIter cur=begin(); cur < end(); ++cur)
    if ((*cur >= 'A') && (*cur <= 'Z')) *cur+=32 ;
  return *this;
}

String& String::toUpper() 
{
  for (strIter cur=begin(); cur < end(); ++cur)
    if ((*cur >= 'a') && (*cur <= 'z')) *cur-=32 ;
  return *this;
}

String String::sTrimSpaces() const {
	// find first non space
	if ( size() <= 0 ) 
		return "";

	int iFirstNonSpace = find_first_not_of(' ');
	int iFindLastSpace = find_last_not_of(' ');
	if (iFirstNonSpace == npos || iFindLastSpace == npos)
		return "";

	return substr(iFirstNonSpace, iFindLastSpace - iFirstNonSpace + 1);
}

const String& String::operator&=(const char* s) 
{
  (*this)+=s;
  return *this;
}

const String& String::operator&=(char c) 
{
  (*this)+=c;
  return *this;
}


//#include    <stdarg.h>
//#include    <stdlib.h>
//#include    <ctype.h>


/* Flags for flag word    */
#define FLleft        1
#define FLplus        2
#define FLspc         4
#define FLhash        8
#define FLlong      0x10
#define FLlngdbl    0x20
#define FL0pad      0x40
#define FLprec      0x80
#define FLuc        0x100
#define FLuns       0x200
#define FLlptr      0x400        /*  pointer format (with :)    */

#define BUFMAX    30    /* temporary buffer size for _pformat()        */

static long  getlong(va_list *,int);
static void  doexponent(char * *ps,int exp);
static char * dosign(int,int);
static char * trim0(char *s);


void String::VarFormat(const char *format,va_list pargs)
{ int nout;        // # of chars sent to func()
  int i;
  long l;
  unsigned long ul;
  int pl=0;          // length of prefix string
  int width=0;       // field width
  int padding=0;     // # of chars to pad (on left or right)
  int precision;
  char buf[BUFMAX];
  int sign=0;        // sign for floating stuff
  double dval;
  char *prefix;
  char *p, c;
  const char *s;
  int sl=0;          // length of formatted string s
  int flags;       // FLxxx
  int base=0;        // number base (decimal, octal, etc.)
  int decpt;       // exponent (base 10) for floats
  String str;

  nout = 0;
  while ((c = *format++) != 0) {   // while not end of format string
    if (c != '%') {     // not a format control character
      (*this) &= c;
      nout++;
      continue;
    }
    prefix = (char *)""; // assume no prefix
    flags = 0;           // reset
    while ((c = *format++) != 0) {
      switch (c) {
        case '-':
          flags |= FLleft; /* left-justify   */
          break;
        case '+':
          flags |= FLplus; /* do + or - sign   */
          break;
        case ' ':
          flags |= FLspc;    /* space flag       */
          break;
        case '#':
          flags |= FLhash; /* alternate form   */
          break;
        default:
          goto getwidth;
      }
    }
getwidth:
    width = 0;
    if (c == '*') {
      width = va_arg(pargs,int);
      if (width < 0) {  /* negative field width   */
        flags |= FLleft;
        width = -width;
      }
      c = *format++;
    }
    else {
      if (c == '0')
        flags |= FL0pad;    /* pad with 0s   */
      while (isdigit(c)) {
        width = width * 10 + c - '0';
        c = *format++;
      }
    }
    precision = 0;
    if (c == '.') {       /* if precision follows       */
      flags |= FLprec;
      c = *format++;
      if (c == '*') {
        precision = va_arg(pargs,int);
        if (precision < 0) {
          flags &= ~FLprec;
          precision = 0;
        }
        c = *format++;
      }
      else {
        //if (c == '0')
        //  flags |= FL0pad;*/ /* pad with 0s   */
        while (isdigit(c)) {
          precision = precision * 10 + c - '0';
          c = *format++;
        }
      }
    }
    if (c == 'l') {
      flags |= FLlong;
      c = *format++;
    }
    else if (c == 'L') {
      flags |= FLlngdbl;
      c = *format++;
    }
    switch (c) {
      char gformat;
      case 's':
        s = va_arg(pargs,char *);
        if (s == NULL) s = "(null)";
        sl = strlen(s);        /* length of string   */
        if (flags & FLprec) {   /* if there is a precision */
          if (precision < sl)
            sl = precision;
          if (sl < 0) sl = 0;
        }
        break;
      case 'S':
        str = va_arg(pargs,String);
        s = str.sVal();
        if (s == NULL) s = "(null)";
        sl = strlen(s);        /* length of string   */
        if (flags & FLprec) {   /* if there is a precision */
          if (precision < sl)
            sl = precision;
          if (sl < 0) sl = 0;
        }
          break;
      case '%': case 'c':
        buf[0] = c == '%' ? '%' : va_arg(pargs, int);
        s = &buf[0];
        sl = 1;
        break;
      case 'd': case 'i':
        base = 10;
        ul = l = getlong(&pargs,flags);
        if (l < 0) {
          sign = 1;
          ul = -l;
        }
        else
          sign = 0;
        prefix = dosign(sign,flags);
        goto ulfmt;
      case 'b':
        base = 2;
        goto getuns;
      case 'o':
        base = 8;
        if (flags & FLhash)
          prefix = (char *) "0";
        goto getuns;
      case 'u':
        base = 10;
getuns:
        ul = getlong(&pargs,flags | FLuns);
        goto ulfmt;
      case 'p':
        flags |= FLlong | FLlptr | FL0pad;
        if (!(flags & FLprec))
          precision = 9;
      case 'X':
        flags |= FLuc;
      case 'x':
        base = 16;
        ul = getlong(&pargs,flags | FLuns);
        if ((flags & FLhash) && ul)
          prefix = (char *) ((flags & FLuc) ? "0X" : "0x");
        /* FALL-THROUGH */
ulfmt:
        { char *sbuf;
          sbuf = &buf[BUFMAX - 1];
          if (ul) {
            do {
              if (flags & FLlptr && sbuf == &buf[BUFMAX-1-4])
                *sbuf = ':';
              else {
                *sbuf = (char)((ul % base) + '0');
                if (*sbuf > '9')
                  *sbuf += (char)((flags & FLuc) ? 'A'-'0'-10 : 'a'-'0'-10);
                ul /= base;
              }
              sbuf--;
            }  while (ul);
            sbuf++;
          }
          else {   /* 0 and 0 precision yields 0 digits   */
            if (precision == 0 && flags & FLprec)
              sbuf++;
            else
              *sbuf = '0';
          }
          sl = &buf[BUFMAX] - sbuf;
          if (sl < precision) {
            if (precision > BUFMAX)
              precision = BUFMAX;
            for (i = precision - sl; i--;)
               *--sbuf = (flags & FLlptr && sbuf == &buf[BUFMAX-1-4]) ? ':' : '0';
            sl = precision;
          }
          s = (char *) sbuf;
        } // ulfmt
        break;
      case 'f':
        gformat = 0;
        if (!(flags & FLprec))    /* if no precision   */
          precision = 6;    /* default precision   */
        dval = va_arg(pargs,double);
fformat:
        p = fcvt(dval,precision,&decpt,&sign);
        prefix = dosign(sign,flags);
        { char *sbuf;
          sbuf = &buf[0];
          if (decpt <= 0)
            *sbuf++ = '0';    /* 1 digit before dec point */
          while (decpt > 0 && sbuf < &buf[BUFMAX - 1]) {
            *sbuf++ = *p++;
            decpt--;
          }
          if (precision > 0 || flags & FLhash) {
            *sbuf++ = '.'; //cDecimalPoint();
            while (decpt < 0 && precision > 0 && sbuf < &buf[BUFMAX]) {
              *sbuf++ = '0';
              decpt++;
              precision--;
            }
            while (precision-- > 0 && sbuf < &buf[BUFMAX])
              *sbuf++ = *p++;
            /* remove trailing 0s   */
            if (gformat && !(flags & FLhash))
              sbuf = trim0(sbuf);
          }
          sl = sbuf - &buf[0];    /* length of string   */
        }
        s = &buf[0];
        break;
      case 'e': case 'E':
        gformat = 0;
        if (!(flags & FLprec))    /* if no precision   */
	  precision = 6;    /* default precision   */
        dval = va_arg(pargs,double);
        p = ecvt(dval,precision + 1,&decpt,&sign);
eformat:
        prefix = dosign(sign,flags);
        { char *sbuf;
          sbuf = &buf[0];
          *sbuf++ = *p++;
          if (precision > 0 || flags & FLhash) {
            *sbuf++ = '.'; //cDecimalPoint();
            while (precision-- > 0 && sbuf < &buf[BUFMAX - 5])
              *sbuf++ = *p++;
            /* remove trailing 0s   */
            if (gformat && !(flags & FLhash))
              sbuf = trim0(sbuf);
          }
          *sbuf++ = c;
          if (dval)        /* avoid 0.00e-01   */
            decpt--;
          doexponent(&sbuf,decpt);
          sl = sbuf - &buf[0];    /* length of string   */
        }
        s = &buf[0];
        break;
      case 'g': case 'G':
        gformat = 1;
        if (!(flags & FLprec))    /* if no precision   */
	  precision = 12; //6;	  /* default precision	 */
        dval = va_arg(pargs,double);
        p = ecvt(dval,precision + 1,&decpt,&sign);
        if (decpt < -3 || decpt - 1 > precision) {  /* use e format       */
          c -= 'g' - 'e';
          goto eformat;
        }
        else
          goto fformat;
      case 'n':
        { int *pi;
          pi = va_arg(pargs,int *);
          *pi = nout;    /* set to # of chars so */
         }
         continue;
       default: ;
    }
    /* Send out the data. Consists of padding, prefix,   */
    /* more padding, the string, and trailing padding   */
    pl = strlen(prefix);        /* length of prefix string */
    nout += pl + sl;
    padding = width - (pl + sl);    /* # of chars to pad   */
    /* if 0 padding, send out prefix (if any)   */
    if (flags & FL0pad)
      for (; *prefix; prefix++)
        (*this) &= *prefix;
    /* if right-justified and chars to pad           */
    /*  send out prefix string               */
    if (padding > 0) {
      nout += padding;
      if (!(flags & FLleft))
        while (--padding >= 0)
          (*this) &= (flags & FL0pad) ? '0' : ' ';
    }
    /* send out prefix (if any)   */
    for (; *prefix; prefix++)
      (*this) &= *prefix;
    /* send out string   */
    for (i = 0; i < sl; i++)
      (*this) &= s[i];
    /* send out right padding   */
    if (flags & FLleft)
      while (--padding >= 0)
        (*this) &= ' ';
  } /* while */
}

// Get an int or a long out of the varargs, and return it.
static long getlong(va_list *ppargs,int flags)
{ long l;
  if (flags & FLlong)
    l = va_arg(*ppargs,long);
  else {
    l = va_arg(*ppargs,int);
    if (flags & FLuns) l &= 0xFFFFL;
  }
  return l;
}

// Add exponent to string s in form +-nn. At least 3 digits.
static void doexponent(char * *ps,int exp)
{ register char *s = *ps;
  *s++ = (exp < 0) ? ((exp = -exp),'-') : '+';
  *s++ = exp / 100 + '0';
  exp %= 100;
  *s++ = exp / 10 + '0';
  *s++ = exp % 10 + '0';
  *ps = s;
}

// Add sign to prefix.
static char * dosign(int sign,int flags)
{
  return (sign) ? (char *) "-" :
        (flags & FLplus) ? (char *) "+" :
        (flags & FLspc)  ? (char *) " " : (char *) "";
}

// Trim trailing 0s and decimal point from string.
static char * trim0(char *s)
{
  while (*(s-1) == '0') s--;
  if (*(s-1) == '.')  s--;
  return s;
}

//Array<String> StringxDummy;
// only needed to include Array<String> member functions in object file

void Split(const String& str, Array<String>& as, char* delimiters)
{
  String s = str;
  char * p = strtok( s.sVal(), delimiters);
  if (p)
    as &= String(p);
  while (1) {
    p = strtok(0, delimiters);
    if (p==0)
      break;
    as &= String(p);
  }
}

void _export SplitOn(const String& str, Array<String>& as, const String sDel, const String& ignoreInBetween)
{
	String part = "";
	boolean inBlock = false;

	for(int i = 0; i < str.size(); ++i) {
		
		char c = str[i];
		if ( ignoreInBetween.find(c) != string::npos) {
			inBlock = !inBlock;
		} else {
			if ( sDel.find(c) == string ::npos || inBlock) {
				part += c; 	
			} else {
				as &= part;
				part="";
			}
		}
	}
	as &= part;
}

void Combine(String& s, const Array<String>& as, char cDelimiter)
{
  s = "";
  for (short i=0; i < as.iSize(); i++) {
    s &= as[i];
    if (i != as.iSize()-1)
      s &= cDelimiter;
  }
}


String String::sHead(const String& sDelim) const
{
  int iPlace=find_first_of(sDelim);
  if (iPlace == -1) iPlace=size();
  return substr(0, iPlace);
}

String String::sTail(const String& sDelim) const
{
  int iPlace=find_first_of(sDelim);
  if (iPlace == -1) return "";
  if (iPlace + sDelim.size() > size()) return "";
  return substr(iPlace+sDelim.size());
}

String String::sQuote(bool fAlways /* = false*/, int iLoc /*=iUNDEF*/) const
 // if false than it puts only quotes if needed
{
  bool fQuote = fAlways;
  if (length() == 0)
    return *this;
  if (((*this)[0] == '\'') && ((*this)[length()-1] == '\''))
    return *this;

	if ( isdigit(((*this)[0])))
		fQuote=true;
  if (!fQuote) 
	{
    for (unsigned int i=0; i < length(); i++) 
		{
      if (0 != strchr(_pcCharsToBeQuoted, (*this)[i])) 
			{
        fQuote = true;
        break;
      }
		}
	}
  if (!fQuote)
    return *this;
  String s = "'";
  for (unsigned int i=0; i < length(); i++) 
	{
		if ( i == iLoc )
			s &= '\'';
		else
		{
			s &= (*this)[i];
			if ((*this)[i] == '\'')
				s &= '\'';
		}
  }
	if ( iLoc == iUNDEF )
		s &= '\'';
  return s;
}

String String::sUnQuote() const
{
  if ((*this)[0] != '\'')
    return *this;
  String s;
  for (unsigned int i=1; i < length(); i++) {
    char c = (*this)[i];
    if (c == '\'') {
      if (i == length()-1)
        break;
      if ((*this)[i+1] == '\'') {
        i++;
        s &= c;
      }
    } 
    else
      s &= c;
  }
  return s;
}

char * String::strchrQuoted(char c) const
{
  bool fInQuotes = false;
  for (unsigned int i=0; i < length(); i++) {
    if ((*this)[i] == '\'')
      fInQuotes = !fInQuotes;
    if (!fInQuotes)
      if ((*this)[i] == c)
        return const_cast<char*>(&(*this)[i]);
  }
  return 0;
}

char * String::strrchrQuoted(char c, int iLoc) const
{
  bool fInQuotes = false;
  char ch;
	int iStart = iLoc < 0 ? length() - 1 : iLoc;
  for (int i = iStart; i >= 0; i--) {
    ch = (*this)[i]; 
    if (ch == '\'')
      fInQuotes = !fInQuotes;
    if (!fInQuotes)
      if (ch == c)
        return const_cast<char*>(&(*this)[i]);
  }
  return 0;
}


const char * String::pcCharsToBeQuoted()
{
  return _pcCharsToBeQuoted;
}


const char * String::pcCharsNotAllowedOutsideQuotes()
{
  return _pcCharsNotAllowedOutsideQuotes;
}

wchar_t *String::toWChar(wchar_t result []) const{

	size_t origsize = size() + 1;
    size_t convertedChars = 0;
    mbstowcs_s(&convertedChars, result, origsize, c_str(), _TRUNCATE);
    return result;
}