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
/* $Log: /ILWIS 3.0/Calculator/Parser.cpp $
 * 
 * 20    3-06-05 12:49 Retsios
 * [bug=6562] Swapped domain and type to what is expected in AddInst
 * (otherwise the bug mentioned occurs as the wrong path of AddInst is
 * used and the result gets the type CalcVarInt, which is the default).
 * 
 * 19    9/24/03 3:36p Martin
 * Merged from AggregateValue
 * 
 * 20    7/23/03 8:33a Martin
 * Added parsing of the structure of an AggregateValue expression.
 * 
 * 18    11/29/02 3:24p Martin
 * neighb functions were in some case case sensitive
 * 
 * 17    12-09-00 8:43a Martin
 * added a correct var instruction
 * 
 * 16    28-08-00 4:24p Martin
 * added command tblvalue
 * 
 * 15    17-07-00 11:08a Martin
 * correct error message when using nested nb expressions
 * 
 * 14    3/29/00 3:03p Wind
 * added true, false and "true", "false" in all calculators
 * 
 * 13    4-01-00 10:28 Wind
 * partial redesign and simplification to solve some bugs related with
 * user defined functions
 * 
 * 12    15-11-99 11:12 Wind
 * cleanup unused member functions and function parameters
 * 
 * 11    11-11-99 12:43 Wind
 * error in previous change
 * 
 * 10    10-11-99 10:41 Wind
 * renamed some functons, removed iNrExpr
 * 
 * 9     9/16/99 11:52a Wind
 * added [..] functioonality to neighbour functions
 * 
 * 8     9/15/99 4:23p Wind
 * removed check on too many nested if's
 * 
 * 7     9/15/99 11:22a Wind
 * added nbstd
 * 
 * 6     9/08/99 10:12a Wind
 * adpated for quoted file names
 * 
 * 5     8/10/99 12:41p Wind
 * Added nbavg
 * 
 * 4     4/09/99 10:37a Martin
 * Some case problems solved
 * 
 * 3     15-03-99 10:08 Koolhoven
 * Header comments
 * 
 * 2     3/12/99 3:05p Martin
 * Added support for case insensitive
// Revision 1.5  1998/09/16 17:30:36  Wim
// 22beta2
//
// Revision 1.4  1997/09/29 16:46:13  Wim
// Changed max nr expr nested to 5.
//
// Revision 1.3  1997-09-27 16:08:40+02  Wim
// If too many nested statements you get now an error message
//
/* parser.c
	Last change:  WK    3 Apr 98    3:43 pm
*/
#include "Headers\toolspch.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Scripting\Parser.h"
#include "Engine\Scripting\Parscnst.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\mapcalc.hs"

#define ttOPERATOR 7
#define ttKEYWORD 8
#define ttTYPE 9
#define ttPARM 10

static void TooDeepError()
{
  throw ErrorObject(TR("Too many nested statements"), errCalculate);
}

Parser::Parser(TokenizerBase* tokenizer, CodeGeneratorBase *gen)
 { tokinp = tokenizer;
   codegen = gen;
 }

Parser::~Parser()
{
}

void Parser::ErrExp(const Token& tok, const String& s)
{
//  if (codegen->fErr)
//    return; // 17/1/97 - Wim - to prevent double error messages
  String sErr;
  if (tok.tt() != ttNONE)
    sErr = String(TR("'%S' expected").c_str(), s);
  else
    sErr = TR("Unexpected end of text");
  codegen->Error(sErr, tok.iLine(), tok.iPos());
}

void Parser::ErrUnexp(const Token& tok)
{
  String sErr;
  if (tok.tt() != ttNONE)
    sErr = String(TR("Unexpected '%S'").c_str(), tok.sVal());
  else
    sErr = TR("Unexpected end of text");
  codegen->Error(sErr, tok.iLine(), tok.iPos());
}

void Parser::GetNextToken()
{
  tok = tokinp->tokGet();
  codegen->iCursorLine = tok.iLine();
  codegen->iCursorCol = tok.iPos();
}

void Parser::GetNextToken(char cTo)
{
  tok = tokinp->tokGet(cTo);
  codegen->iCursorLine = tok.iLine();
  codegen->iCursorCol = tok.iPos();
}

////////////////////////////

static char* sComp[]       = { "<>", "<=", ">=", ":=", /*"\$<", "\$>", "\$<=", "\$>=",*/ 0};
static char *sAliasOper1[] = { "le", "ge", "lt", "gt", "eq", "ne", 0 };
static char *sAliasOper2[] = { "<=", ">=", "<", ">", "=", "<>", 0 };
static char *sOperList[]   =
  { operPLUS, operMIN, operTIMES, operDIVIDE, operPOWER,
    operEQ,   operLT,  operGT,    operNE,     operLE,   operGE,  operASSIGN,
//    "\$<"  "\$>", "\$<=", "\$>=",
    operDIV,  operMOD, operNOT,   operAND,    operOR,   operXOR, 0 };

ExpressionParser::ExpressionParser(TokenizerBase* tokenizer, CodeGenerator *gen)
 : Parser(tokenizer, gen),
	fInNBFunction(false)
{
  tokinp->CreateListOfType(ttOPERATOR, sOperList);
  tokinp->SetSpaceChars(" \n\t\r");
  tokinp->SetComposed(sComp);
  tokinp->CreateAlias(sAliasOper1, sAliasOper2);
  //fLeftSide = true;
}

bool ExpressionParser::fPriority3Oper(const Token& tok) const
{
   return (tok == operEQ) || (tok == operNE) || (tok == operLT) ||
        (tok == operLE) || (tok == operGT) || (tok == operGE);
}

void ExpressionParser::Expression(bool fTextAfter)
{
  codegen->AddInst("parmstart");
  codegen->AddInst("parmnext");
  SimpleExpr();
  while (fPriority3Oper(tok)) {
      String s = tok.sVal();
      GetNextToken(); // skip operator
      codegen->AddInst("parmnext");
      SimpleExpr();
      codegen->AddInst("operator", s);
  }
  if (!fTextAfter) {
    TokenType tt = tok.tt();
    if ((tt > ttNONE) && (tt <= ttIDENTIFIER)) {
      char c = tok.sVal()[0];
      if (0 ==strchr(";,)]}", c))
        ErrExp(tok, "operator");
    }
  }
  codegen->AddInst("parmend");
	fInNBFunction = false;
}

bool ExpressionParser::fPriority2Oper(const Token& tok) const
{
   return (tok == operMIN) || (tok == operPLUS) ||// (tok == operMOD) ||
        (tok == operOR)  || (tok == operXOR)  || (tok == operASSIGN);
}


void ExpressionParser::SimpleExpr()
{
  codegen->AddInst("parmstart");
  codegen->AddInst("parmnext");
  Term();
  while (fPriority2Oper(tok)) {
    String s = tok.sVal();
    bool fAssign = tok == operASSIGN;
    GetNextToken(); // skip operator
    codegen->AddInst("parmnext");
    Term();
    if (!fAssign)
      codegen->AddInst("operator", s);
  }
  codegen->AddInst("parmend");
}

void ExpressionParser::Term()
{
  codegen->AddInst("parmstart");
  codegen->AddInst("parmnext");
  SimpleTerm();
  while (fPriority1Oper(tok)) {
    String s = tok.sVal();
    GetNextToken(); // skip operator
    codegen->AddInst("parmnext");
    SimpleTerm();
    codegen->AddInst("operator", s);
  }
  codegen->AddInst("parmend");
}

bool ExpressionParser::fPriority1Oper(const Token& tok) const
{
   return (tok == operTIMES) || (tok == operDIVIDE)||
         (tok == operDIV)   || (tok == operMOD) || (tok == operAND);
}

void ExpressionParser::SimpleTerm()
{
  Factor();
  if (tok == operPOWER) {
    GetNextToken(); // skip operator
    SimpleTerm();
    codegen->AddInst("operator", operPOWER);
  }
}

void ExpressionParser::Factor()
{
  if (tok == cLEFTBRACKET) {
    GetNextToken(); // skip left bracket
    Expression();
    if (tok != cRIGHTBRACKET) {
      ErrExp(tok, cRIGHTBRACKET);
      SkipTo(cSEMICOLON);
      return;
    }
    GetNextToken(); // skip right bracket
  }
  else if ((tok.tt() > ttIDENTIFIER) && (tok == operMIN)) {
    GetNextToken(); // skip unary operator
    Factor();
    codegen->AddInst("operator", "neg");
  }
  else if ((tok.tt() > ttIDENTIFIER) && (tok == operNOT)) {
    GetNextToken(); // skip unary operator
    Factor();
    codegen->AddInst("operator", "not");
  }
  else {
    String s = tok.sVal();
    bool fLeftAllowed;
    Operand(&fLeftAllowed);
//    if (codegen->fErr) return;
    if (tok == operASSIGN) {
//      fLeftSide = false;
      if (!fLeftAllowed) {
        ErrExp(tok, "operator");
        return;
      }
      GetNextToken();  // skip assign
      codegen->AddInst("assignexprstart");
      codegen->AddInst("exprstart");
      Expression();
      codegen->AddInst("exprend");
      codegen->AddInst("store", s);
      codegen->AddInst("assignexprend");
    }
  }
}

void ExpressionParser::FuncCall(const String& s)
{
    int iParms = 0;
    GetNextToken(); // skip left bracket
    if (fCIStrEqual(s , "coord")) { // coord(Xexpr,Yexpr [,coordsys])
      codegen->AddInst("parmstart");
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      else
        GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      String sCoordSys = "unknown";
      if (tok != cCOMMA) {
        if (tok != cRIGHTBRACKET) {
          ErrExp(tok, cRIGHTBRACKET);
          SkipTo(cSEMICOLON);
          return;
        }
      }
      else {
        GetNextToken(); // skip ,
        codegen->AddInst("parmnext");
        sCoordSys = tok.sVal();
        GetNextToken(); // skip sCoordSys
      }
      GetNextToken(); // skip right bracket
      codegen->AddInst("coord", sCoordSys);
      codegen->AddInst("parmend");
      return;
    }
    if ((fCIStrEqual(s , "color")) || (fCIStrEqual(s , "colorhsi"))) { // color(redexpr,greenexpr,blueexp)
      codegen->AddInst("parmstart");
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // red or hue
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      else
        GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // green or saturation
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      else
        GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();  // blue or intensity
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst(s.c_str()); // color or colorhsi
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    if (fCIStrEqual(s , "transform")) { // transform(Coord,coordsys)
      codegen->AddInst("parmstart");
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("parmnext");
      String sCoordSysNew = tok.sVal();
      GetNextToken(); // skip coordsys
      String sCoordSysOld;
      if (tok == cCOMMA) {
        sCoordSysOld = sCoordSysNew;
        GetNextToken(); // skip ,
        codegen->AddInst("parmnext");
        sCoordSysNew = tok.sVal();
        GetNextToken(); // skip right bracket,
      }
      else if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst("transform", sCoordSysNew, sCoordSysOld);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if ((fCIStrEqual(s , "mapvalue")) || 
						 (fCIStrEqual(s , "maprow")) || 
						 (fCIStrEqual(s , "mapcol")))
    { // mapvalue(map, coord) etc.
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sMap = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst(s.c_str(), sMap);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if (fCIStrEqual(s , "rasvalue")) { // rasvalue(map, row, col)
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sMap = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // line
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // column
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst("rasvalue", sMap);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
		else if (fCIStrEqual(s , "tblvalue")) 
	{ // rasvalue(map, row, col)
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sTable = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // line
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // column
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst("tblvalue", sTable);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
	else if (fCIStrEqual(s , "aggregatevalue")) 		
	{
		codegen->AddInst("parmstart");
		codegen->AddInst("parmnext");
		String sInputMap = tok.sVal();
		GetNextToken();
		codegen->AddInst("parmnext");
		if (tok == cCOMMA )
		{
			GetNextToken();	
			codegen->AddInst("exprstart");
			codegen->AddInst("parmnext");
			Expression(); // operation
			codegen->AddInst("exprend");			
			if (tok == cCOMMA)
			{
				GetNextToken();
				codegen->AddInst("exprstart");
				codegen->AddInst("parmnext");
				Expression();	
				codegen->AddInst("exprend");
			}
		}			
		if (tok != cRIGHTBRACKET) 
		{
				ErrExp(tok, cRIGHTBRACKET);
				SkipTo(cSEMICOLON);
				return;
		}				
		codegen->AddInst("aggregatevalue", sInputMap);
		codegen->AddInst("parmend");
		GetNextToken();
		return;
	}		
    else if ((fCIStrEqual(s , "mapmin"))  || 
							(fCIStrEqual(s , "mapmax")) || //(s == "mapavg")|| (s == "mapstd") ||
             (fCIStrEqual(s , "minCrdX")) || 
						 (fCIStrEqual(s , "minCrdY")) || 
						 (fCIStrEqual(s , "maxCrdX")) || 
						 (fCIStrEqual(s , "maxCrdY")) ||
             (fCIStrEqual(s , "maprows")) || 
						 (fCIStrEqual(s , "mapcols")) || 
						 (fCIStrEqual(s , "pixsize")) || 
						 (fCIStrEqual(s , "pixarea")) ||
             (fCIStrEqual(s , "tblrecs")) || 
						 (fCIStrEqual(s , "tblcols")))
    { // mapmin(mapname.ext) etc.
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sMap = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst(s.c_str(), sMap);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if ((fCIStrEqual(s , "mapcrd")) || (fCIStrEqual(s , "mapcolor")))
    { // mapcrd(rasmap) or mapcrd(rasmap,line,col) etc.
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sMap = tok.sVal();
      GetNextToken(); // skip map name
      if (tok == cRIGHTBRACKET) {
        codegen->AddInst(s.c_str(), sMap);
        GetNextToken(); // skip right bracket
        return;
      }
      else if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // line
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // column
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      const_cast<String&>(s) &= String("rc");
      codegen->AddInst(s.c_str(), sMap);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if (fCIStrEqual(s , "rprcolor")) { // rprcolor(rpr,value)
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sRpr = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // value
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst("rprcolor", sRpr);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if ((fCIStrEqual(s , "pntcrd")) || 
						(fCIStrEqual(s , "pntval")) || 
						(fCIStrEqual(s , "pntnr"))|| 
						(fCIStrEqual(s , "pntx"))|| 
						(fCIStrEqual(s , "pnty"))) {
      // pntcrd(pntmap,index) or pntval(pntmap,index)
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      String sPnt = tok.sVal();
      GetNextToken(); // skip map name
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression(); // index
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst(s.c_str(), sPnt);
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if (fCIStrEqual(s , "clfy")) { // clfy(expression,groupdomain)
      codegen->AddInst("parmstart");
      codegen->AddInst("parmnext");
      codegen->AddInst("exprstart");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      else
        GetNextToken(); // skip ,
      codegen->AddInst("parmnext");
      String sDom = tok.sVal();
      GetNextToken();
      if (tok == ":") {  // identifier may contain :
        sDom &= ':';
        GetNextToken(); // skip :
        sDom &= tok.sVal(); // next part of identifier
        GetNextToken();
      }
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip right bracket
      codegen->AddInst("clfy", sDom);
      codegen->AddInst("parmend");
      return;
    }
    else if (fCIStrEqual(s , "inmask")) { // clfy(str_expr,mask)
      codegen->AddInst("parmstart");
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cCOMMA) {
        ErrExp(tok, cCOMMA);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        return;
      }
      GetNextToken(); // skip ,
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if (tok != cRIGHTBRACKET) {
        ErrExp(tok, cRIGHTBRACKET);
        SkipTo(cSEMICOLON);
        return;
      }
      codegen->AddInst(s.c_str());
      codegen->AddInst("parmend");
      GetNextToken(); // skip right bracket
      return;
    }
    else if (fCIStrEqual(s.sLeft(2) , "nb")) {
      if (fNeighbFunc(s))
        return;
    }
    codegen->AddInst("callcheck", s); // check func name exists
    codegen->AddInst("parmstart");
    while (tok != cRIGHTBRACKET) {
      iParms++;
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
      if ((tok != cCOMMA) && (tok != cRIGHTBRACKET)) {
        ErrExp(tok, cRIGHTBRACKET);
        if (tok == cSEMICOLON) // skip to next SEMICOLON
          GetNextToken(); // skip semi colon
        SkipTo(cSEMICOLON);
        //break;
        return;  // 17/1/97 - Wim - looks more consistent
      }
      if (tok == cCOMMA)
        GetNextToken(); // skip ,
    }
    if (tok == cRIGHTBRACKET)
      GetNextToken(); // skip right bracket
    codegen->AddInst("call", s, String("%i", iParms));
    codegen->AddInst("parmend");
}

bool ExpressionParser::fNeighbFunc(const String& s2)
{
	String s = String(s2).toLower();	
  String s1 = s.sLeft(5);
  if ((fCIStrEqual(s1 , "nbmin")) || 
     (fCIStrEqual(s1 , "nbmax")) || 
     (fCIStrEqual(s1 , "nbsum")) || 
     (fCIStrEqual(s1 , "nbcnt")) || 
     (fCIStrEqual(s1 , "nbprd")) || 
     (fCIStrEqual(s1 , "nbavg")) || 
     (fCIStrEqual(s1 , "nbstd")) || 
     (fCIStrEqual(s.sLeft(6) , "nbcndp"))) 
	{
		if ( fInNBFunction )
		{
			fInNBFunction = false;
			throw ErrorObject(String(TR("Nested use of %S not possible here").c_str(), s));
		}
		fInNBFunction = true;
    char c = s[5];
    if (0 == strchr("p4589", c))
      return false;
    bool fPos = c == 'p';
    if (fPos) {
      c = s[6];
      if (0 == strchr("4589", c))
        return false;
    }
    long iNb = 511;
    if (c == '4')
      iNb = 170;
    else if (c == '5')
      iNb = 186;
    else if (c == '8')
      iNb = 495;
    else if (tok == cLEFTSQBRACKET) {
      iNb = 0;
      while (tok != cRIGHTSQBRACKET) {
        GetNextToken();
        long i = tok.sVal().iVal();
        if ((i > 0) && (i <= 9))
          iNb |= (1 << (i-1));
        else {
          ErrExp(tok, cCOMMA);
          SkipTo(cSEMICOLON);
          return true;  
        }
        GetNextToken();
        if ((tok != cCOMMA) && (tok != cRIGHTSQBRACKET)) {
          ErrExp(tok, cCOMMA);
          SkipTo(cSEMICOLON);
          return true;
        }
      }
      GetNextToken(); // skip right sq bracket
      if (tok != cLEFTBRACKET) {
        ErrExp(tok, cLEFTBRACKET);
        SkipTo(cSEMICOLON);
        return true;
      }
      GetNextToken(); // skip left bracket
    }
    codegen->AddInst("nbsta", iNb);
    codegen->AddInst("parmstart");
    codegen->AddInst("exprstart");
    codegen->AddInst("parmnext");
    Expression();
    codegen->AddInst("exprend");
    bool fCond = false;
    if (tok == cCOMMA && (fCIStrEqual(s1 , "nbmin") || fCIStrEqual(s1 ,"nbmax") || 
                          fCIStrEqual(s1 , "nbprd") || fCIStrEqual(s1 ,"nbavg")|| fCIStrEqual(s1 , "nbstd")))
    { // condition follows
      fCond = true;
      GetNextToken(); // skip comma
      codegen->AddInst("exprstart");
      codegen->AddInst("parmnext");
      Expression();
      codegen->AddInst("exprend");
    }
    if (tok != cRIGHTBRACKET) {
      ErrExp(tok, cRIGHTBRACKET);
      SkipTo(cSEMICOLON);
      return true;
    }
    codegen->AddInst("nbend");
    codegen->AddInst(s.c_str(), long(fPos)+2*long(fCond));
    codegen->AddInst("parmend");
    GetNextToken(); // skip right bracket
    return true;
  }
  else if (fCIStrEqual(s , "nbflt")) {
    codegen->AddInst("parmstart");
    codegen->AddInst("parmnext");
    String sFlt = tok.sVal();
    GetNextToken(); // skip filter name
    if (tok != cRIGHTBRACKET) {
      ErrExp(tok, cRIGHTBRACKET);
      SkipTo(cSEMICOLON);
      return false;
    }
    codegen->AddInst("nbflt", sFlt);
    codegen->AddInst("parmend");
    GetNextToken(); // skip right bracket
    return true;
  }
  return false;
}

void ExpressionParser::Operand(bool *fLeftAllowed)
{
  *fLeftAllowed = false;
  if (tok.tt() == ttNONE) {
    ErrUnexp(tok);
    return;
  }
  if (tok.tt() == ttINT) {
    codegen->AddInst("consti", tok.sVal());
    GetNextToken(); // skip constant
    return;
  }
  else if (tok.tt() == ttREAL) {
    codegen->AddInst("constr", tok.sVal());
    GetNextToken(); // skip constant
    return;
  }
  else if (tok.tt() == ttSTRING) {
		if (fCIStrEqual(tok.sVal(), "true"))
	    codegen->AddInst("consti", "1");
		else if(fCIStrEqual(tok.sVal(), "false"))
	    codegen->AddInst("consti", "0");
		else
      codegen->AddInst("consts", tok.sVal());
    GetNextToken(); // skip constant
    return;
  }
  else if (tok.tt() == ttCHAR) {
    codegen->AddInst("constc", tok.sVal());
    GetNextToken(); // skip constant
    return;
  }
  else if ((tok.tt() == ttUNDEF) && (tok.sVal() == "?")) {
    codegen->AddInst("undef");
    GetNextToken(); // skip constant
    return;
  }
  else if (tok.tt() == ttOPERATOR) {
    ErrExp(tok, "operand");
    GetNextToken();
    return;
  }
  String s = tok.sVal();
  GetNextToken(); // skip %
  if (s == "%") {
    s &= tok.sVal();
    GetNextToken();
  }
  else if (tok == ".") {  // identifier may contain .
    s &= '.';
    GetNextToken(); // skip .
    s &= tok.sVal(); // next part of identifier
    GetNextToken();
  }
  else if (tok == ":") {  // identifier may contain :
    s &= ':';
    GetNextToken(); // skip :
    s &= tok.sVal(); // next part of identifier
    GetNextToken();
  }
  if (tok == cLEFTBRACKET) {  // function or proc
    FuncCall(s);
  }
  else if (tok == cLEFTSQBRACKET) {  // index
    if (fNeighbFunc(s)) 
      return;
    *fLeftAllowed = true;
    short iDim = 0;
    while (tok != cRIGHTSQBRACKET) {
      GetNextToken(); // start expression
      iDim++;
      codegen->AddInst("exprstart");
      Expression();
      codegen->AddInst("exprend");
      if ((tok != cRIGHTSQBRACKET) && (tok != cCOMMA)) {
        ErrExp(tok, cRIGHTSQBRACKET);
        SkipTo(cSEMICOLON);
        GetNextToken();
        return;
      }
    }
    GetNextToken();
    String sDim("%i", iDim);
    if (tok != operASSIGN)
      codegen->AddInst("indexload", s, sDim);
    else {
      GetNextToken();
      codegen->AddInst("assignexprstart");
      Expression();
      codegen->AddInst("indexstore", s, sDim);
      codegen->AddInst("assignexprend");
    }
  }
  else { // normal identifier
    if ((s[0] != '\'') && (s[0] != '.') && (s[0] !=  '\\') && (s[0] != '%') && !isalpha(s[0])) {
      ErrExp(tok, "operand or function");
      GetNextToken();
      return;
    }
    *fLeftAllowed = true;
    if (tok != operASSIGN) {
      if (tok.sVal()=="#") {
        GetNextToken();
        if (tok.sVal() == cLEFTSQBRACKET) {
          GetNextToken(); // skip [
          codegen->AddInst("exprstart");
          Expression();
          codegen->AddInst("exprend");
          if (tok.sVal() != cRIGHTSQBRACKET) {
            ErrExp(tok, cRIGHTSQBRACKET);
            SkipTo(cSEMICOLON);
            GetNextToken();
            return;
          }
          codegen->AddInst("nbnum", s);
          GetNextToken(); // skip ]
        }
        else
          codegen->AddInst("loadnb", s);
      }
      else
        codegen->AddInst("load", s);
    }
  }
}

void ExpressionParser::SkipTo(const Token& tokFind, bool fGetNextToken)
{
  if (tok.sVal() != tokFind.sVal()) {
//    printf("Skipping to %s\n", (char*)tokFind.sVal());
    while ((tok.sVal() != tokFind.sVal()) && !fEnd())
      GetNextToken();
  }
  if (fGetNextToken)
    GetNextToken();
}

//////////////////////////

static char *sKeywList[] = { kwPROCEDURE, kwFUNCTION, kwBEGIN, kwEND,
                             kwRETURN, kwWHILE, kwDO, kwREPEAT, kwUNTIL,
                             kwFOR, kwTO, kwSTEP, kwIF, kwTHEN, kwELSE,
                             kwDELETE, kwRENAME, 0};

static char* sVarTypes[] = { sVAR, sMAP, sTABLE, sCOLUMN, 
                             sCOORD, sTABLE2, sCLFY,
                             sBYTE, sINT, sREAL, sBOOL, 
                             sSTRING, sVALUE, sCOLOR, 0 };

ProgramParser::ProgramParser(TokenizerBase* tokenizer, CodeGenerator *gen)
 : ExpressionParser(tokenizer, gen)
{
  tokinp->CreateListOfType(ttKEYWORD, sKeywList);
  tokinp->CreateListOfType(ttTYPE, sVarTypes);
  tokinp->SetComment("//");
}


void ProgramParser::SkipBlock()
{
  GetNextToken();
  while ((tok.sVal() != kwEND) && !fEnd())
    if (tok.sVal() != kwBEGIN)
      GetNextToken();
    else
      SkipBlock();
  GetNextToken();
}

void ProgramParser::StatementList(const Token& tokStop)
{
  while (!fCIStrEqual(tok.sVal() , tokStop.sVal()) && !fEnd())
    Statement();
  if (!fCIStrEqual(tok.sVal() , tokStop.sVal())) {
    ErrExp(tok, tokStop.sVal());
    return;
  }
  GetNextToken();
}

void ProgramParser::CompoundStatement()
{
  if (tok != kwBEGIN)
    Statement();
  else {
    GetNextToken();
    StatementList(Token(kwEND, ttKEYWORD));
  }
}


void ProgramParser::Statement()
{
  try {
    if (tok == cSEMICOLON) {
      GetNextToken();
      return;
    }
    if (tok.tt() == ttNONE) return;
    if (tok.tt() == ttKEYWORD) {
      if (tok == kwBEGIN)
        CompoundStatement();
      else if (tok == kwIF) {
        int iLabFalse;
        GetNextToken(); // skip IF
        codegen->AddInst("boolexprstart");
        Expression();
        codegen->AddInst("boolexprend");
        if (tok != kwTHEN)
          ErrExp(tok, kwTHEN);
        else {
          codegen->AddInst("gotofalse", iLabFalse=codegen->iNewLabel());
          GetNextToken();
        }
        CompoundStatement();
        if (tok == kwELSE) {
          int iLabAfter;
          codegen->AddInst("goto", iLabAfter=codegen->iNewLabel());
          codegen->AddInst("label", iLabFalse);
          GetNextToken();
          CompoundStatement();
          codegen->AddInst("label", iLabAfter);
        }
        else
          codegen->AddInst("label", iLabFalse);
      }
      else if (tok == kwFOR) {
        GetNextToken();
        String sCount = tok.sVal();
        GetNextToken();
        if (tok != operASSIGN)
           ErrExp(tok, operASSIGN);
        codegen->AddInst("intexprstart");
        GetNextToken(); // skip ':='
        Expression(true);
        codegen->AddInst("intexprend");
        codegen->AddInst("store", sCount);
        if (tok != kwTO)
          ErrExp(tok, kwTO);
        int iLab1 = codegen->iNewLabel(), iLab2;
        codegen->AddInst("label", iLab1);
        GetNextToken(); // skip 'to'
        codegen->AddInst("intexprstart");
        Expression(true);
        codegen->AddInst("load", sCount);
        codegen->AddInst("operator", operGE);
        codegen->AddInst("gotofalse", iLab2 = codegen->iNewLabel());
  //??      codegen->AddInst("exprstart");
        int iStep = 1;
        if (tok == kwSTEP) {
          GetNextToken();
          if (tok.tt() != ttINT)
            ErrExp(tok, "integer constant");
          else
            iStep = atoi(tok.sVal().c_str());
          GetNextToken();
        }
        if (tok != kwDO)
          ErrExp(tok, kwDO);
        else
          GetNextToken();
        CompoundStatement();
        codegen->AddInst("intexprstart");
        codegen->AddInst("load", sCount);
        codegen->AddInst("consti", String("%i",iStep));
        codegen->AddInst("operator", operPLUS);
        codegen->AddInst("store", sCount);
        codegen->AddInst("intexprend");
        codegen->AddInst("goto", iLab1);
        codegen->AddInst("label", iLab2);
      }
      else if (tok == kwWHILE) {
        GetNextToken();
        int iLabCheck = codegen->iNewLabel();
        codegen->AddInst("label", iLabCheck);
        codegen->AddInst("boolexprstart");
        Expression(true);
        codegen->AddInst("boolexprend");
        int iLab = codegen->iNewLabel();
        codegen->AddInst("gotofalse", iLab);
        if (tok != kwDO)
          ErrExp(tok, kwDO);
        else
          GetNextToken();
        CompoundStatement();
        codegen->AddInst("goto", iLabCheck);
        codegen->AddInst("label", iLab);
      }
      else if (tok == kwREPEAT) {
        int iLab = codegen->iNewLabel();
        codegen->AddInst("label", iLab);
        GetNextToken();
        StatementList(Token(kwUNTIL));
        codegen->AddInst("boolexprstart");
        Expression();
        codegen->AddInst("boolexprend");
        codegen->AddInst("gotofalse", iLab);
        if (tok != cSEMICOLON)
          ErrExp(tok, cSEMICOLON);
      }
      else if (tok == kwDELETE) {
        GetNextToken();
        codegen->AddInst("delete", tok.sVal());
        GetNextToken();
        if (tok != cSEMICOLON)
          ErrExp(tok, cSEMICOLON);
      }
      else {
        ErrUnexp(tok);
        GetNextToken();
      }
    }
    else {
      codegen->AddInst("statementstart");
      Expression();
      codegen->AddInst("statementend");
    }
  } // end try
  catch (const Token& t)
  {
    ErrUnexp(t);
    SkipTo(cSEMICOLON);
  }
}

void ProgramParser::Declaration()
{
  String sType = tok.sVal();
  String sDomain;
  GetNextToken();
  if (tok == cLEFTBRACKET) {
    FuncCall(sType);
    return;
  }
  if (tok == cLEFTBRACE) {
    GetNextToken('}'); // skip left brace
    sDomain = tok.sVal();
    GetNextToken();
    if (tok != cRIGHTBRACE) {
      ErrExp(tok, cRIGHTBRACE);
      SkipTo(cSEMICOLON);
      return;
    }
    GetNextToken(); // skip right brace
  }
  while (tok != cSEMICOLON) {
    codegen->AddInst("var", tok.sVal(), sType, sDomain);
    GetNextToken();
    if (tok != cSEMICOLON)
      if (tok == cCOMMA)
        GetNextToken();
      else {
        ErrExp(tok, cCOMMA);
        SkipTo(cSEMICOLON);
        return;
      }
  }
  GetNextToken();
}

void ProgramParser::Program()
{  do {
    if (tok == kwPROCEDURE)
      ProcDef();
    else if (tok == kwFUNCTION)
      FuncDef();
    else if (tok.tt() == ttTYPE)
      Declaration();
    else
      Statement();
//  } while (!fEnd());
  } while (tok.tt() != ttNONE);
  codegen->AddInst("stop");
}

int ProgramParser::ParameterList()
{
  if (tok != cLEFTBRACKET) {
    ErrExp(tok, cLEFTBRACKET);
    SkipTo(kwBEGIN);
    return 0;
  }
  int iParms = 0;
  String sType;
  GetNextToken(); // skip left bracket
  while (tok != cRIGHTBRACKET) {
    bool fType = false;
    String sDomain, sType;
    if (tok.tt() == ttTYPE) {
      fType = true;
      sType = tok.sVal();
      GetNextToken();
      if (tok.sVal() == cLEFTBRACE) {
        GetNextToken('}');
        sDomain = tok.sVal();
        GetNextToken();
        if (tok != cRIGHTBRACE) {
          ErrExp(tok, cCOMMA);
          SkipTo(cRIGHTBRACKET);
        }
        GetNextToken(); // skip right brace
      }
    }
    // 'tok' contains parameter name
    codegen->AddInst("parm", tok.sVal(), sType, sDomain);
    iParms++;
    GetNextToken(); // skip parameter name
    if ((tok != cCOMMA) && (tok != cRIGHTBRACKET)) {
      ErrExp(tok, cCOMMA);
      SkipTo(cRIGHTBRACKET);
      return 0;
    }
    if (tok == cCOMMA)
      GetNextToken(); // skip comma
  }
  return iParms;
}


void ProgramParser::ProcDef()
{
  GetNextToken(); // skip procedure keyword
  codegen->AddInst("procdef", tok.sVal());
  GetNextToken(); // skip proc name
  ParameterList();
  GetNextToken(); // skip right bracket
  if (tok != kwBEGIN) {
    ErrExp(tok, kwBEGIN);
    SkipBlock();
  }
  GetNextToken(); // skip begin keyword
  while (tok.tt() == ttTYPE)
    Declaration();
  StatementList(Token(kwEND));
  codegen->AddInst("endprocdef");
}

void ProgramParser::FuncDef()
{
  GetNextToken(); // skip function keyword
  codegen->AddInst("funcdef", tok.sVal());
  GetNextToken(); // skip func name
  ParameterList();
  GetNextToken(); // skip right bracket
  if (tok == cCOLON) {
    GetNextToken(); // skip colon
      codegen->AddInst("returntype", tok.sVal());
    GetNextToken(); // skip type identifier
  }
  if (tok != kwBEGIN) {
    ErrExp(tok, kwBEGIN);
    SkipBlock();
  }
  GetNextToken(); // skip begin keyword
  while (tok.tt() == ttTYPE)
    Declaration();
  StatementList(Token(kwRETURN));

  codegen->AddInst("exprstart");
  Expression();
  codegen->AddInst("exprend");
  if (tok == cSEMICOLON)
    GetNextToken(); // skip semicolon
  if (tok != kwEND) {
    ErrExp(tok, kwEND);
    SkipTo(cSEMICOLON, true);
    return;
  }
  GetNextToken(); // skip end keyword
  codegen->AddInst("endfuncdef");
}




