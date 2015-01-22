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
/*
*/
/* $Log: /ILWIS 3.0/Tools/TOKBASE.CPP $
 * 
 * 13    8-07-02 15:48 Koolhoven
 * TextInput constructor: prevent double declaration of i
 * 
 * 12    5-07-02 18:59 Koolhoven
 * prevent warings with Visual Studio .Net
 * 
 * 11    29-11-99 11:16 Wind
 * added support for UNC
 * 
 * 10    9-11-99 10:13 Wind
 * removed flag form GetNextToken (was never used)
 * 
 * 9     5-11-99 13:02 Wind
 * allow underscore as first char of identifier
 * 
 * 8     20-10-99 15:52 Wind
 * debugged for path names
 * 
 * 7     10/18/99 12:55p Wind
 * debugged problem with dots in token
 * 
 * 6     9/29/99 4:56p Wind
 * allow characters with code > 128 in file name and tokens
 * 
 * 5     9/29/99 10:56a Wind
 * added support for the syntax: maplistname:bandnr
 * added check on illegal characters 0..31
 * 
 * 4     9/20/99 12:36p Wind
 * allow syntax maplist[^band_nr^]
 * 
 * 3     8-09-99 12:40p Martin
 * //->/*
 * 
 * 2     9/08/99 12:04p Wind
 * adpated to use if identifiers in quotes (for file names and column
 * names)
// Revision 1.2  1997/08/18 07:39:35  Wim
// ttSTRING does not need any further processing
//
/*
// tokbase.c
// Interface for basic tokenisations
// june 1994, Jelle Wind
	Last change:  WK   18 Aug 97    9:12 am
*/
#define TOKBASE_C
#include "Headers\toolspch.h"
#include <ctype.h>
#include "Engine\Base\Tokbase.h"

Token::Token(const String& s, TokenType tt, int iLine, int iPos)
{ 
  _sVal = s; 
  _tt = tt; 
  _iLine = iLine; 
  _iPos = iPos; 
}

TextInputBase::~TextInputBase()
{
}

TextInput::TextInput(const String& s) : TextInputBase()
{
  cSource.Resize(s.length()+1);
  unsigned int i;
  for (i = 0; i < s.length(); i++)
    cSource[i] = s[i];
  cSource[i] = 0;
  iCurrPos = -1; iOffset = 0;
  iCurrLine = 0;
  // if there is only one line the current line remains always 0
  // else it starts at 1 and will be increased after each '\n'
  // Check if input contains a '\n' character:
  for (i = 0; i < cSource.iSize(); i++)
    if (cSource[i] == '\n')
      { iCurrLine = 1; break; }
}

char TextInput::cGet()
// return current character and adjust pointer
{ if (fEnd()) return 0;
  char c = cSource[iOffset++];
  if (c == '\n') { // adjust also line indication
    iCurrLine++; iCurrPos = -1;
  }
  else
    iCurrPos++;
  return c;
}

TextInput::~TextInput()
{
}

bool TextInput::fEnd() 
{ 
  return iOffset + 1 >= cSource.iSize(); // think about ending zero! 16/1/97 - Wim
}

char TextInput::cLookForward()
// return next character
{ if (fEnd()) return 0;
  return cSource[iOffset];
}

char TextInput::cLookNextForward()
// return next next character
{ if (fEnd()) return 0;
  return cSource[iOffset+1];
}


String TextInput::sLine(short iLine)
// returns line iLine
{ short i = 0, iLin = iLine;
  String s;
  while (iLin > 0) {
    if (cSource[i] == '\n')
      iLin--;
    i++;
    if (i > iOffset)
      return "";
  }
  while (cSource[i] && (cSource[i] != '\n'))
    s &= cSource[i++];
  return s;
}

String TextInputBase::sCurrentLine()
{ String s;
  char c = cGet();
  while ((c != 0) && (c != '\n') && (c != '\r')) {
    s &= c;
    c = cGet();
  } 
  if (c != 0)
    cGet(); // skip '\n' or '\r'
  return s;
}

void TokenizerBase::SetSpaceChars(char* sSpaceChars)
{ 
  if (0 != sSpaces) {
    delete [] sSpaces;
    sSpaces = 0;
  }
  sSpaces = new char[strlen(sSpaceChars)+1];
  strcpy(sSpaces, sSpaceChars);
}

void TokenizerBase::SetComment(char* sComm)
{ 
  if (0 != sComment) {
    delete [] sComment;
    sComment = 0;
  }
  sComment = new char[strlen(sComm)+1];
  strcpy(sComment, sComm);
}


char TokenizerBase::cSkipSpaces()
// return first character after space characters
{
  char c;
  if (sSpaces) {
    while (strchr(sSpaces, c = inp->cLookForward()) != 0)
      if (c == 0)
        return 0;
      else
        inp->cGet();
  }
  else {

    while (isspace((unsigned char)(c = inp->cLookForward())))
      if (c == 0)
        return 0;
      else
        inp->cGet();
  }
  c = inp->cGet();
  if (sComment) {
    bool f = false;
    if (sComment[0] == c) { // could be comment
      if (strlen(sComment) == 2)
        f = sComment[1] == inp->cLookForward();
      else
        f = true;
    }
    if (f) { // skip to end of line
      do {
        c = inp->cGet();
      } while (c && (c != '\n'));
      return cSkipSpaces();
    }
  }
  return c;
}

bool TokenizerBase::fEnd() 
{ 
  return inp->fEnd(); 
}

void TokenizerBase::GetNext(String& s, char cTo, short& iLine, short& iPos)
{
  iLine = inp->iLine();
  iPos = inp->iPos();
  char c = inp->cLookForward();
  while (c && (c != cTo)) {
    s &= inp->cGet();
    c = inp->cLookForward();
  }
}


void TokenizerBase::GetNext(String& s, TokenType& tt, short& iLine, short& iPos)
// This routine returns:
// - int constants, for example: 9  998 897678   (type ttINT)
// - real constants, for example: 8.9 1.67e37    (type ttREAL)
// - character strings: enclosed in "            (type ttSTRING)
// - characters: enclosed in '                   (type ttCHAR)
// - identifiers: anything starting with a a..z, A..Z followed
//   by alphanumerical characters or .           (type ttIDENTIFIER)
// - undefined tokens: any non alphanumeric character or " and .
//   this should be checked on a higher level    (type ttUNDEF)
// - empty string (end of input)                 (type ttNONE)
{
  char c = cSkipSpaces();

  iLine = inp->iLine();
  iPos = inp->iPos();
  if (!c) { tt = ttNONE; s = ""; return; }

  s = String("%c",c);
  tt = ttUNDEF;

  if (isdigit((unsigned char)c)) { // find constant
    tt = ttINT;
    bool fDecimal = false, fExponent = false;
    bool fCont = true;
    do {
      c = inp->cLookForward();
      if (isdigit((unsigned char)c))
        s &= inp->cGet();
      else
       switch (c) {
        case 'e': case 'E': // exponent
          {
            if (fExponent) {
              fCont = false;
              break;
            }
            s &= inp->cGet();
            fExponent = true;
            fDecimal = true; // no decimal point allowed
            char c = inp->cLookForward();
            if (strchr("+-", c) != 0)
              s &= inp->cGet();
          }
          break;
        case '.':  // decimal point
          if (fDecimal) { // only one decimal point allowed
            fCont = false;
            break;
          }
          s &= inp->cGet();
          fDecimal = true;
          break;
        default:
          fCont = false;
      }
    } while (fCont);
    if (fExponent || fDecimal)
      tt = ttREAL;
  }
  /*else if (isalpha((unsigned char)c) || (c == '\\')) { // keyword, identifier, operator
    tt = ttIDENTIFIER;
    c = inp->cLookForward();
    while (isalnum((unsigned char)c) || (c == '_') || 
          (fColonInIdentifier && (c == ':') && (inp->cLookNextForward() != '=')) ||
          (c == '\\') || (c == '.')) {
      c = inp->cGet();
      if (c == 0)
        break;
      else
        s &= c;
      c = inp->cLookForward();
    }
  }*/
  else {
    if (c == '\"') {  // string constant
      s = "";
      tt = ttSTRING;
      while ((c = inp->cGet()) != '\"') {
        if (c == 0)
          break;
        s &= c;
      }
      return;
    }
    else if ((c == '\'') || isalpha((unsigned char)c) || (c == '_') || (c == '\\')) { // identifier
      tt = ttIDENTIFIER;
      bool fInQuotes = c == '\'';
//      bool fPath = (isalpha((unsigned char)c) && (inp->cLookForward() == ':')) || (c == '\\');
      bool fColon = false;

/*    if (isdigit((unsigned char)inp->cLookNextForward())) {
        fPath = false;
        fColon = true;
      }*/
      c = inp->cLookForward();
      if (c == '\\') {  // server name
        s &= inp->cGet();
        c = inp->cLookForward();
      }
      while ( c != 0 ) {
        if (fColon && !isdigit((unsigned char)c))
          break;
/*        if ((c == '\\') ) {
//          if (!(fPath || fInQuotes))
//          if (!(fPath || fInQuotes))
            break;
        } 
        else */
          if (!fInQuotes) {
/*          if (((c == '[') && (inp->cLookNextForward() == '^')) || 
              ((c == '^') && (inp->cLookNextForward() == ']'))) {   // check for: maplist[^ .. ^]
            c = inp->cGet(); s &= c;
            c = inp->cGet(); s &= c;
            c = inp->cLookForward();
            continue; */
         if ((0 != strchr(String::pcCharsNotAllowedOutsideQuotes(), c)) || ((c >= 0) && (c <= 31))) 
           if (c != ':') {
             if (c != '\\') 
               break;
           }
           else {
             char cNext = inp->cLookNextForward();
             if (cNext != '\\') {
               if (!isdigit((unsigned char)cNext) || fColon)
                 break;
               else
                 fColon = true;
             }
           }
        }
        c = inp->cGet(); 
        s &= c;
        if (c == '\'') {
          if (fInQuotes) {
            c = inp->cLookForward();
            if (c == '\'')
              s &= inp->cGet(); 
            else {
              fInQuotes = false;
//              fPath = false;
              if (0 == strchr(".:", c))
                break;
            }
          }
          else
            fInQuotes = true;
        }
        c = inp->cLookForward();
      }
      /*while ((c = inp->cGet()) != '\'') {
        if (c == 0)
          break;
        s &= c;
      }
      s &= '\'';*/
    }
    else if (sComp != 0) { // check composed tokens
      String sTmp = s;
      sTmp &= inp->cLookForward();
      for (int i = 0; sComp[i]; i++)
        if (!_strcmpi(sTmp.sVal(), sComp[i])) // its a composed token
          s &= inp->cGet();
    }
  }
  unsigned int i, j;
  // check for aliases
  for (i = 0; i < AliasList.iSize(); i++)
  {
    String sTemp=AliasList[i].sWord1;
    if (!_strcmpi(s.sVal(), sTemp.sVal()))
      s = AliasList[i].sWord2;
  // check for other token types
  }
  for (i = 0; i < WordsOfTypeList.iSize(); i++)
    for (j = 0; WordsOfTypeList[i].sWordList[j]; j++)
    {
	  String sTemp= WordsOfTypeList[i].sWordList[j];
      if (!_strcmpi(s.sVal(),sTemp.sVal())) {
        tt = WordsOfTypeList[i].tt;
        return;
      }
    }
}

void TokenizerBase::CreateAlias(char** sList1, char** sList2)
{ Alias al;
  for (short i = 0; sList1[i]; i++) {
    al.sWord1 = sList1[i];
    al.sWord2 = sList2[i];
    AliasList &= al;
  }
}

void TokenizerBase::CreateListOfType(TokenType t, char** sList)
{ WordsOfType sl;
  sl.tt = t;
  sl.sWordList = sList;
  WordsOfTypeList &= sl;
}

Token TokenizerBase::tokGet() {
    // this function converts the next token found to an instance of class Token
    String s; TokenType tt; short iLine, iPos;
    GetNext(s, tt, iLine, iPos);
    return Token(s, tt, iLine, iPos);
}

Token TokenizerBase::tokGet(char cTo) {
  // this function converts the next token found to an instance of class Token
  String s; 
  short iLine, iPos;
  GetNext(s, cTo, iLine, iPos);
  return Token(s, ttNONE, iLine, iPos);
}

Token TokenizerBase::tokToCloseBracket() {
  String s; 
  int iLine = inp->iLine(), iPos = inp->iPos();
  int iCloseBrackets = 1;
  while (iCloseBrackets > 0) {
    char c = inp->cGet(); 
    if (c == 0) {
      s = String();
      break;
    }  
    s &= c;
    if (c == '(')
      iCloseBrackets++;
    else if (c == ')')
      iCloseBrackets--;
  }
  return Token(s, ttNONE, iLine, iPos);
}

TokenizerBase::~TokenizerBase()
{
  if (sSpaces)
    delete [] sSpaces;
  if (sComment)
    delete [] sComment;
}

String TokenizerBase::sCurrentLine() {
  return inp->sCurrentLine();
}










