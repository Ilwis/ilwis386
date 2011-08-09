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
/* ClassifierPriorProb
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JH   19 May 2001    5:00 pm
*/

#include "Engine\Function\PRIORPRB.H"
#include "Engine\Base\DataObjects\ERR.H"
#include "Headers\Hs\map.hs"
#include "Headers\Hs\Table.hs"
#include "Headers\Hs\tbl.hs"
#include "Headers\Err\ILWISDAT.ERR"

const char* ClassifierPriorProb::sSyntax()
{
  return "ClassifierPriorProb(table,column)\nClassifierPriorProb(threshold,table,column)";
}

static void ColumnNotFound(const String sCol) {
  throw ErrorObject(WhatError(String(TR("Invalid Probabilty Column %S").c_str(), sCol), errClassifier +8));
}

ClassifierPriorProb::~ClassifierPriorProb()
{
}

ClassifierPriorProb* ClassifierPriorProb::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if ((iParms < 2) || (iParms > 3))
    ExpressionError(sExpr, sSyntax());
  Table tbl;
  Column col;
  double rThres = rUNDEF;
	String sTbl, sCol;
	int iFirstParm = 0;
  if (iParms == 3)
	{
    rThres = as[0].rVal();
		iFirstParm++;
  }
  tbl = Table(as[iFirstParm]);
	if (!tbl.fValid())
		ErrorObject(WhatError(String(TR("Not a valid table name")), errClassifier+7), fn);		
	col = Column(tbl->col(as[iFirstParm + 1]));
	if (!col.fValid())
		ColumnNotFound(as[iFirstParm + 1]);
	sTbl = as[iFirstParm];
	sCol = as[iFirstParm + 1];
		
	//check and error message for wrong table dom, col dom and col values<0
	//moet nog geimplementeerd
	// bv throw  errMapClassify
	// Domain dmTbl = tbl->dm();
	//throw ErrorObject(WhatError(String(SMAPErrInvalidTableDom_S.c_str(), as[1]), errClassifier+3), fn);

	Domain dmCol = col->dm();
	if (0 == dmCol->pdv())
		throw ErrorObject(WhatError(String(TR("Column %S should have value domain").c_str(), sCol), errClassifier+4), fn);
	long iRec = tbl->iRecs();
	double rTmp;
	for (long i = 0; i < iRec; i++) {
		rTmp = col->rValue(i+1);
		if (rTmp == rUNDEF) 
			throw ErrorObject(WhatError(String(TR("Undefined probability is not allowed")), errClassifier+5), fn);
		else if (rTmp < 0) 
			throw ErrorObject(WhatError(String(TR("Negative probability %.f is not allowed").c_str(), rTmp), errClassifier+6), fn);
		else
			continue;
	}
	return new ClassifierPriorProb(fn, rThres, tbl, col);
}

ClassifierPriorProb::ClassifierPriorProb(const FileName& fn)
: ClassifierMaxLikelihood(fn)
{
  ReadElement("ClassifierPriorProb", "Table", tblPrior);
  String sColName;
  ReadElement("ClassifierPriorProb", "Column", sColName);
  colPrior = Column(tblPrior, sColName);
}

ClassifierPriorProb::ClassifierPriorProb(const FileName& fn, double rThresh,
                                         const Table& tbl, const Column& col)
: ClassifierMaxLikelihood(fn, rThresh), tblPrior(tbl), colPrior(col)
{
}

void ClassifierPriorProb::Store()
{
  ClassifierMaxLikelihood::Store();
  WriteElement("Classifier", "Type", "ClassifierPriorProb");
  WriteElement("ClassifierPriorProb", "Table", tblPrior);
  WriteElement("ClassifierPriorProb", "Column", colPrior);
}    

String ClassifierPriorProb::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("ClassifierPriorProb(%f,%S,%S)", rThreshold, tblPrior->sNameQuoted(true, sDirRelative), colPrior->sNameQuoted());
  else 
    return String("ClassifierPriorProb(%S,%S)", tblPrior->sNameQuoted(true, sDirRelative), colPrior->sNameQuoted());
}

double ClassifierPriorProb::rAdd(int iClass)
{
  return ClassifierMaxLikelihood::rAdd(iClass) + rPriorProb[iClass];
}

void ClassifierPriorProb::SetSampleSet(const SampleSet& sms) 
{
  ClassifierMaxLikelihood::SetSampleSet(sms);
  assert(sms->dm() == tblPrior->dm());
  // fill array with prior probabilties
  for (int iOrd = 1; iOrd <= iClasses; iOrd++) {
    int iClass = sms->dc()->iKey(iOrd);
    rPriorProb[iClass] =  - 2 * log(colPrior->rValue(iClass));
  }  
}


