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
//$Log: /ILWIS 3.0/Matrix/Varcovmt.cpp $
 * 
 * 11    24-10-01 15:53 Koolhoven
 *  VarCovCorrMatrix::create() causes the maplist to calculate when needed
 * 
 * 10    8-11-00 10:22 Hendrikse
 * corrected as[i]in GetRowText
 * 
 * 9     24-05-00 16:36 Hendrikse
 * added 2 newline characters in sSummary() to improve the output lay-out
 * 
 * 8     9-02-00 10:54 Wind
 * replaced \n with \r\n in additional info
 * 
 * 7     4-02-00 18:41 Hendrikse
 * Modified in create() the MapList constr into  MapList
 * ml(FileName(as[0], ".mpl"));
 * 
 * 6     9/29/99 10:51a Wind
 * added case insensitive string comparison
 * 
 * 5     9/29/99 10:26a Wind
 * added caseinsensitive string comparison
 * 
 * 4     9/10/99 12:55p Wind
 * comments
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
//Revision 1.4  1998/09/16 17:30:35  Wim
//22beta2
//
//Revision 1.3  1997/09/18 17:29:18  Wim
//Do not change additional info of map list
//
//Revision 1.2  1997-08-26 13:43:45+02  janh
//newline \n added 3 times in sSummary
//
/* VarCovCorrMatrix
   Copyright Ilwis System Development ITC
   August 1995, by Jelle Wind
	Last change:  WK    2 Sep 98   11:25 am
*/

#include "Engine\Matrix\Varcovmt.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\DAT.hs"

static String sSyntax() {
 return "MatrixVarCov(maplist)\nMatrixCorr(maplist)";
}

VarCovCorrMatrix* VarCovCorrMatrix::create(const String& sExpr)
{
  FileName fn;
  String sFunc = MatrixObjectPtr::sParseFunc(sExpr);
  if (!fCIStrEqual(sFunc,"MatrixVarCov") && !fCIStrEqual(sFunc,"MatrixCorr"))
    ExpressionError(sExpr, sSyntax());
  Array<String> as(1);
  if (!IlwisObjectPtr::fParseParm(sExpr, as))
    ExpressionError(sExpr, sSyntax());
  MapList ml(FileName(as[0], ".mpl"));
  if (!ml->fCalculated())
    ml->Calc();
  if (!ml->fCalculated())
    return 0;
  for (int i = ml->iLower(); i <= ml->iUpper(); ++i)
    if (!ml->map(i)->fValues())
      ValueDomainError(ml->map(i)->dm()->sName(), ml->map(i)->fnObj, errMatrixVarCovCorr);
  bool fCorr = fCIStrEqual(sFunc,"MatrixCorr");
  return new VarCovCorrMatrix(ml, fCorr);
}

VarCovCorrMatrix::~VarCovCorrMatrix()
{
}

VarCovCorrMatrix::VarCovCorrMatrix(const MapList& maplist, bool fCorr)
: MatrixObjectPtr(FileName(), maplist->iSize(), maplist->iSize()), mpl(maplist), fCorrMat(fCorr)
{
  objdep.Add(mpl.ptr());
  if (fCorrMat)
    sDescription = String(SDATMsgCorrelationMatrix_S.scVal(), mpl->sName());
  else
    sDescription = String(SDATMsgVarCovarMatrix_S.scVal(), mpl->sName());
  RealMatrix* rmt;
  if (fCorrMat)
    rmt = mpl->mtCorr(false);
  else
    rmt = mpl->mtVarCov(false);
  if (0 != rmt) {
    mat = *rmt;
    tmCalc = Time::timCurr();
    SetAdditionalInfoFlag(true);
    SetAdditionalInfo(sSummary());
  }
  else
    tmCalc = 0;
}

String VarCovCorrMatrix::sType() const
{
  if (fCorrMat)
    return "Correlation Matrix";
  return "Variance-Covariance Matrix";
}

String VarCovCorrMatrix::sName(bool fExt, const String& sDirRelative) const
{
  String s = MatrixObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (fCorrMat)
    return String("MatrixCorr(%S)", mpl->sNameQuoted());
  return String("MatrixVarCov(%S)", mpl->sNameQuoted());
}

void VarCovCorrMatrix::GetRowText(Array<String>& as) const
{
  as.Resize(mpl->iSize());
  for (int i=0; i < mpl->iSize(); i++)
    as[i] = mpl->map(i+mpl->iLower())->fnObj.sFile;
}

void VarCovCorrMatrix::GetColumnText(Array<String>& as) const
{
  as.Resize(mpl->iSize());
  for (int i=0; i < mpl->iSize(); i++)
    as[i] = mpl->map(i+mpl->iLower())->fnObj.sFile;
}

String VarCovCorrMatrix::sSummary() const
{
  CVector vec = *(mpl->vecMean(true));
  String s(SDATOthMeanPerBand);
	s &= "\r\n";
  for (int i=0; i < vec.iRows(); i++)
    s &= String("%8.2f", vec(i));
  s &= "\r\n";
  vec = *(mpl->vecStd(true));
  s &= SDATOthStdPerBand;
	s &= "\r\n";
  for (int i=0; i < vec.iRows(); i++)
    s &= String("%8.2f", vec(i));
  return s;
}

void VarCovCorrMatrix::MakeUsable()
{
  RealMatrix* rmt;
  if (fCorrMat)
    rmt = mpl->mtCorr(true);
  else
    rmt = mpl->mtVarCov(true);
  if (0 != rmt) {
    mat = *rmt;
    SetAdditionalInfoFlag(true);
    SetAdditionalInfo(sSummary());
    tmCalc = Time::timCurr();
  }
  else
    tmCalc = 0;
}

bool VarCovCorrMatrix::fUsable() const
{
  if (!objdep.fAllOlder(tmCalc))
    return false;
  return true;
//  return objdep.fAllFrozenValid();
}

int VarCovCorrMatrix::iWidth() const
{
  if (fCorrMat)
    return 5;
  return 8;
}

String VarCovCorrMatrix::sValue(int i, int j, int iW) const
{
  if (iW <= 0)
    iW = iWidth();
//  if (fCorrMat)
//    return String("%*.2f", iW, mat(i, j));
  return String("%*.2f", iW, mat(i, j));
}




