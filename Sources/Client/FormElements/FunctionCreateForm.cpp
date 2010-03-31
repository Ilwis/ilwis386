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
#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Function\FUNCUSER.H"
#include "Client\FormElements\FunctionCreateForm.h"
#include "Headers\Hs\OTHEROBJ.hs"
#include "Client\ilwis.h"

// defined in funcs.c void CreateInternFuncs(Array<InternFuncDesc*>& fdl)
static char* sInternFunc[] =
	{
		"rnd", "neg", "sgn", "abs", "floor",
		"ceil", "round", "sqrt", "log", "ln",
		"log10", "exp", "sq", "degrad", "raddeg",
		"sin", "cos", "tan", "asin", "acos",
		"atan", "atan2", "sinh", "cosh", "tanh",
		"isundef", "strlt", "strle", "strgt", "strge",
		"ndvi", "pow", "hyp", "iff", "ifundef",
		"ifnotundef", "length", "sub", "left", "right",
		"in", "strpos", "inrange", "min", "max",
		"avg", "stdev", "corr", "string", "value",
		"dist", "dist2", "crdx", "crdy", "clrred",
		"clrgreen", "clrblue", "clryellow", "clrmagenta", "clrcyan",
		"clrgrey", "clrhue", "clrsat", "clrintens", (char*)0
	};

// defined in parser.c void ExpressionParser::FuncCall(const String& s)
static char* sInternParserFunc[] =
	{
		"coord", "color", "colorhsi", "transform",
		"mapvalue", "maprow", "mapcol", "rasvalue",
		"mapmin", "mapmax", "mincrdx", "mincrdy", "maxcrdx", "maxcrdy",
		"maprows", "mapcols", "pixsize", "pixarea", "tblrecs", "tblcols",
		"mapcrd", "mapcolor", "rprcolor",
		"pntcrd", "pntval", "pntnr",
		"pntx", "pnty", "clfy", "inmask", (char*)0
	};

// defined in parser.c bool ExpressionParser::fNeighbFunc(const String& s)
static char* sInternNbFunc[] =
	{
		"nbmin", "nbmin4", "nbmin5", "nbmin8", "nbmin9", "nbminp4", "nbminp5", "nbminp8", "nbminp9",
		"nbmax", "nbmax4", "nbmax5", "nbmax8", "nbmax9", "nbmaxp4", "nbmaxp5", "nbmaxp8", "nbmaxp9",
		"nbsum", "nbsum4", "nbsum5", "nbsum8", "nbsum9", "nbsump4", "nbsump5", "nbsump8", "nbsump9",
		"nbcnt", "nbcnt4", "nbcnt5", "nbcnt8", "nbcnt9", "nbcntp4", "nbcntp5", "nbcntp8", "nbcntp9",
		"nbprd", "nbprd4", "nbprd5", "nbprd8", "nbprd9", "nbprdp4", "nbprdp5", "nbprdp8", "nbprdp9",
		"nbcndp","nbcndp4", "nbcndp5", "nbcndp8", "nbcndp9",
		"nbflt", (char*)0
	};

static char* sInternWords[] =
	{
		"eq", "ne", "lt", "le", "gt", "ge",
		"not", "div", "mod", "and", "or", "xor",
		"bool", "byte", "int", "real", "string", "coord", "value", "color",
		"var", "map", "table", "column", "table2", "tableclfy",
		"procedure", "function", "begin", "end", "return",
		"while", "do", "repeat", "until", "for", "to", "step",
		"if", "then", "else", "include", "delete", "rename",
		"nbpos", "nbdis", "pi", "pidiv2", "pidiv4", "pi2", (char*)0
	};

FormCreateFunction::FormCreateFunction(CWnd* wPar, String* str)
: FormWithDest(wPar, SOOTitleCreateFunction),
  sFun(str)
{
	iImg = IlwWinApp()->iImage(".fun");

	sNewName = *sFun;
	ffn = new FieldDataTypeCreate(root, SOOUiFunctionName, &sNewName, ".FUN", true);
	ffn->SetCallBack((NotifyProc)&FormCreateFunction::CallBackName);
	StaticText* st = new StaticText(root, SOOUiExpression);
	st->psn->SetBound(0,0,0,0);
	FieldString* fs = new FieldString(root, "", &sExpr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	st = new StaticText(root, SOOUiDescription);
	st->psn->SetBound(0,0,0,0);
	fs = new FieldString(root, "", &sDescr);
	fs->SetWidth(120);
	fs->SetIndependentPos();
	String sFill('x', 50);
	stRemark = new StaticText(root, sFill);
	stRemark->SetIndependentPos();

	SetMenHelpTopic(htpFunCreate);
	create();
}

FormCreateFunction::~FormCreateFunction()
{
}

int FormCreateFunction::exec()
{
	FormWithDest::exec();
	*sFun = sNewName;
	FileName fn = FileName(*sFun, ".fun");
	FuncUser fun(fn, sExpr);
	if (sDescr != "")
		fun->sDescription = sDescr;
	fun->Store();
	return 0;
}

int FormCreateFunction::CallBackName(Event*)
{
	ffn->StoreData();
	bool fOk = false;
	if (sNewName.length() > 0)
	{
		bool fIntern = false;
		int i;
		FileName fn(sNewName, ".fun");
		if (!fn.fValid())
			stRemark->SetVal(SOORemNotValidFunName);
		else if(File::fExist(fn))
			stRemark->SetVal(SOORemFunExists);
		else
		{
			for (i = 0; sInternFunc[i]; ++i)
				if (fn.sFile == sInternFunc[i])
				{
					fIntern = true;
					stRemark->SetVal(SOORemInternalFunction);
        }
			if (!fIntern)
			{
				for (i = 0; sInternParserFunc[i]; ++i)
					if (fn.sFile == sInternParserFunc[i])
					{
						fIntern = true;
						stRemark->SetVal(SOORemParserFunction);
          }
      }
			if (!fIntern)
			{
				for (i = 0; sInternWords[i]; ++i)
					if (fn.sFile == sInternWords[i])
					{
						fIntern = true;
						stRemark->SetVal(SOORemReservedWord);
          }
      }
			if (!fIntern)
			{
				for (i = 0; sInternNbFunc[i]; ++i)
					if (fn.sFile == sInternNbFunc[i])
					{
						fIntern = true;
						stRemark->SetVal(SOORemNeighbourhoodFunction);
          }
      }
			if (!fIntern)
			{
				fOk = true;
				stRemark->SetVal("");
      }
    }
  }
	else
		stRemark->SetVal("");
	if (fOk)
		EnableOK();
	else    
		DisableOK();
	return 0;
}
