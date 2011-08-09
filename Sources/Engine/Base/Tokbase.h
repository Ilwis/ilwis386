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
// tokbase.h
// Interface for basic tokenisation
// june 1994, Jelle Wind
	Last change:  WK   28 Aug 98    5:30 pm
*/
#ifndef ILWTOKBASE_H
#define ILWTOKBASE_H

#define TokenType short

#define ttNONE  0
#define ttUNDEF 1
#define ttINT 2
#define ttREAL 3
#define ttSTRING 4
#define ttCHAR 5
#define ttIDENTIFIER 6

/*
A token instance from class Token is the basic item that is used by
the parser. It is supplied by the tokenizer. It contains the characters
of which it consists and the token type assigned to it by the tokenizer.
Furthermore it contains information about its position in the input. This
information is only used in case an error occured in parsing and the position
of the error token should be displayed.
	Last change:  WK   28 Jan 97   11:53 am
*/

class Token {
public:
  _export Token(const String& s = "", TokenType tt = ttNONE, int iLine = 0, int iPos = 0);
  int iLine()    const { return _iLine; }
  int iPos()     const { return _iPos; }
  const String& sVal()  const { return _sVal; }
  TokenType tt() const { return _tt; }
  bool operator==(const char*s) const
                      { return 0 == _strcmpi(sVal().c_str(), s); }
  bool operator!=(const char*s) const
                      { return 0 != _strcmpi(sVal().c_str(), s); }
private:
  String _sVal;      // character representation of token
  int _iLine, _iPos; // position of token
  TokenType _tt;     // token type
};


/*
TextInputBase is the base class for the supply of the text to be tokenized
by a tokenizer. It contains functions to get the next character, to look one
character ahead and to give the position of a character.
*/
class TextInputBase// : public ErrorHandling
{
public:
  TextInputBase() {}// : ErrorHandling() {}
  virtual ~TextInputBase();
  virtual char cGet() = 0;               // get next character
  virtual char _export cLookForward() = 0;       // look ahead character
  virtual char _export cLookNextForward() = 0;       // look ahead character
  virtual bool fEnd() = 0;               // end of input ?
  short iLine() { return iCurrLine; }    // current line (counting from 1)
  short iPos()  { return iCurrPos; }     // current position in line (from 0 !)
  virtual String sLine(short iLine) = 0;
    // return characters of line iLine of the input, this can be used f.e.
    // to display error line
  String APPEXPORT sCurrentLine();
protected:
  short iCurrLine, iCurrPos;             // iOffset: offset in input list
};


/*
Class Tokenizerbase is the base class for tokenisation. It has as a member
an instance of TextInputBase from which it gets its characters. It returns
value constants (integer, reals also scientific notation), string constants
(enclosed in "), character constants (enclosed in '), identifiers (starting
with a..z or A..Z, followed by alphanumerical characters or .).
It can also be supplied with a list of strings and a type indicator. In this
case the strings are recognized as different tokens and given the type
corresponding to the list string.
Further it can be supplied with a list of aliases to be recognized.
*/

struct WordsOfType { TokenType tt; char** sWordList; };
inline WordsOfType undef(const WordsOfType&) { return WordsOfType(); }
struct Alias { char *sWord1, *sWord2; };
inline Alias undef(const Alias&) { return Alias(); }

/*template class IMPEXP Array<Alias>;
template class IMPEXP Array<WordsOfType>;*/

typedef Array<Alias> AliasArray;
typedef Array<WordsOfType> WordsOfTypeArray;

class TextInput;

class TokenizerBase// : public ErrorHandling
{
public:
  TokenizerBase(TextInputBase* inpText)// : ErrorHandling()
    { inp = inpText; sComp = 0; sSpaces = 0; sComment = 0; }
  virtual _export ~TokenizerBase();
  bool _export fEnd();
  void _export SetSpaceChars(char* sSpaceChars);
  void SetComposed(char** sComposed) // list should be terminated by 0 string
    { sComp = sComposed; }
  void _export SetComment(char* sComm);
  void _export CreateListOfType(TokenType t, char** sList);
     // list should be terminated by 0 string
  void _export CreateAlias(char** sList1, char** sList2);
     // lists should be terminated by 0 string
     // strings in sList1 are aliases of the strings in sList2
     // and are replaced by them
//  Token _export tokGet(bool fColonInIdentifier = true);
  Token _export tokGet();
  Token _export tokGet(char cTo);
  Token _export tokToCloseBracket();
  String _export sCurrentLine();
protected:
  char APPEXPORT cSkipSpaces();
  void APPEXPORT GetNext(String& s, TokenType& tt, short& iLine, short& iPos);
  void APPEXPORT GetNext(String& s, char cTo, short& iLine, short& iPos);
public:  
  TextInputBase* inp;
protected:  
  char** sComp;
  char* sSpaces;
  char* sComment;
  WordsOfTypeArray WordsOfTypeList;
  AliasArray AliasList;
};

// This is the actual class used as character input for the tokenizer
// It contains a character array with the complete text to be tokenized.
class TextInput : public TextInputBase
{
public:
  _export TextInput(const String& s);
  virtual _export ~TextInput();
  char _export cGet();
  virtual char _export cLookForward();
  char _export cLookNextForward();
  String _export sLine(short iLine);
  bool _export fEnd();
protected:
  CharArray cSource;
  short iOffset;
};

#endif // ILWTOKBASE_H







