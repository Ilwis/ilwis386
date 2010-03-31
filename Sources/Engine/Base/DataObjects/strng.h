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
/* Intreface for String for ILWIS 2.0
// sept 1993, by Jelle Wind
// (c) Computer Department ITC
	Last change:  WK    9 Sep 98    5:50 pm
*/
#ifndef ILWSTRNG_H
#define ILWSTRNG_H

typedef string STLstring;
typedef STLstring::iterator strIter;

class String : public STLstring 
{
public:
    _export String();
    _export String(const String& s);
    _export String(const STLstring& s); 
    _export String(const CString& s);
    _export String(char*);
    _export String(const char *sFormat, ...);
    _export String(char c, unsigned int l);
   
    char*                sVal() 
                           { return const_cast<char *>(c_str()); }
    const char *         scVal() const 
                           { return c_str(); }
    short                shVal() const 
                           { return shortConv(iVal()); }
    bool _export         fVal() const;
    long _export         iVal() const;
    float                flVal() const 
                           { return floatConv(rVal()); }
    double _export       rVal() const;
    bool                 operator<(const String& s)  const 
                           { return _stricmp(c_str(),s.c_str())<0;  }
    bool                 operator>(const String& s)  const 
                           { return _stricmp(c_str(),s.c_str())>0;  }
    bool                 operator<=(const String& s) const 
                           { return _stricmp(c_str(),s.c_str())<=0; }
    bool                 operator>=(const String& s) const 
                           { return _stricmp(c_str(),s.c_str())>=0; }
    bool                 operator<(const char* str)  const 
                           { return _stricmp(c_str(),str)<0;  }
    bool                 operator>(const char* str)  const 
                           { return _stricmp(c_str(),str)>0;  }
    bool                 operator<=(const char* str) const 
                           { return _stricmp(c_str(),str)<=0; }
    bool                 operator>=(const char* str) const 
                           { return _stricmp(c_str(),str)>=0; }
    friend bool          operator<(const char* str,const String& s)  
                           { return _stricmp(str,s.c_str())<0; }
    friend bool          operator>(const char* str,const String& s)  
                           { return _stricmp(str,s.c_str())>0; }
    friend bool          operator<=(const char* str,const String& s) 
                           { return _stricmp(str,s.c_str())<=0; }
    friend bool          operator>=(const char* str,const String& s) 
                           { return _stricmp(str,s.c_str())>=0; }
    friend int           strcmp(const String& s1, const String& s2)
                           { return strcmp(s1.c_str(),s2.c_str()); }
    friend int           strcmp(const String& s, const char * p)
                           { return strcmp(s.c_str(),p); }
    friend int           strcmp(const char *p, const String& s)
                           { return strcmp(p, s.c_str()); }
    String _export       operator&(String& s);
    String _export       operator&(const char* str);
    const String _export &operator&=(const String&);
    const String _export &operator&=(const char* str);
    const String _export &operator&=(char c);
    short _export        iPos(char c) const;
    short _export        iPos(String& s) const;
    unsigned int         length() const 
                            { return (unsigned int)size(); }
    String _export       sSub(int iFirst, int iLength) const;
    String               sLeft(int iLength) const 
                            { return sSub(0, iLength); }
    String               sRight(int iLength) const 
                            { return sSub(length()-iLength, iLength); }
    String _export       sTrimSpaces() const; // at begin and end of string!
    String _export       &toLower();
    String _export       &toUpper();
  	String _export       sTail(const String& sDelim) const;
  	String _export       sHead(const String& sDelim) const;

    String _export       sQuote(bool fAlways = false, int iLoc = iUNDEF) const; // if false than it puts only quotes if needed, iLoc = from where
    String _export       sUnQuote() const;
    char _export *       strchrQuoted(char c) const;
    char _export *       strrchrQuoted(char c, int iLoc = iUNDEF) const;
	wchar_t *			 toWChar(wchar_t r[]) const;
    static const char _export * pcCharsToBeQuoted();
    static const char _export * pcCharsNotAllowedOutsideQuotes();

protected:
private:
    void                 VarFormat(const char* sFormat, va_list arg_ptr);
};

typedef Buf<String>  StringBuf;

void _export Split(const String& s, Array<String>& as, char* delimiters=" ");
void _export SplitOn(const String& s, Array<String>& as, const String sDel=" ", const String& ignoreInBetween="'");
void _export Combine(String& s, const Array<String>& as, char cDelimiter=' ');
bool _export fCIStrEqual(const String & s1, const String& s2);

#endif
