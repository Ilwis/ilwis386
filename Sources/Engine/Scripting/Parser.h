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
#ifndef ILW_PARSERH
#define ILW_PARSERH
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\Stack.h"
#include "Engine\Scripting\CODEGEN.H"

//!
class Parser //: public ErrorHandling
{
  friend CALCEXPORT Calculator;
public:
  Parser(TokenizerBase* tokenizer, CodeGeneratorBase *gen);
  _export ~Parser();
  void GetNextToken();
  const Token& tokCur() const { return tok; }
  bool fEnd() { return tokinp->fEnd(); }
protected:
  void GetNextToken(char cTo);
  void ErrExp(const Token& tok, const String& s);
  void ErrExp(const Token& tok, char c)
    { String s =String(c,1); ErrExp(tok, s); }
  void ErrUnexp(const Token& tok);
  Token tok;
  TokenizerBase *tokinp;
  CodeGeneratorBase *codegen;
};


//!
class ExpressionParser : public Parser
{
public:
  _export ExpressionParser(TokenizerBase* tokenizer, CodeGenerator *gen);
  void Expression(bool fTextAfter = false);
protected:
  void SkipTo(const Token& tokFind, bool fGetNextToken = false);
  void SkipTo(const char* sFind, bool fGetNextToken = false)
    { SkipTo(Token(sFind), fGetNextToken); }
  void FuncCall(const String& sFunc);
private:
  bool fNeighbFunc(const String& sFunc);
	bool fInNBFunction; // boolean to be set when in a NB function, to detect syntax errors with illegal use of other NB functions
  void Operand(bool *fLeftAllowed);
  bool fPriority3Oper(const Token& tok) const;
  bool fPriority2Oper(const Token& tok) const;
  bool fPriority1Oper(const Token& tok) const;
  void SimpleExpr();
  void Term();
  void SimpleTerm();
  void Factor();
};

//!
class CALCEXPORT ProgramParser : public ExpressionParser
{
public:
  ProgramParser(TokenizerBase* tokenizer, CodeGenerator *gen);
  void Program();
  void Statement();
  void Declaration();
  void ProcDef();
  void FuncDef();
private:
  void CompoundStatement();
  void StatementList(const Token& tokStop);
  int ParameterList();
  void SkipBlock();
};

#endif // ILW_PARSERH
