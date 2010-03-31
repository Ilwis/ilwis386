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
/*$Log: /ILWIS 3.0/Function/MXLIKELI.cpp $
 * 
 * 3     8/01/01 9:52 Willem
 * Comments...
 * 
 * 2     20-07-01 11:30 Koolhoven
 * work with netto size of domain class, items could be deleted
 */
//Revision 1.3  1998/09/16 17:30:34  Wim
//22beta2
//
//Revision 1.2  1997/08/20 17:09:13  janh
//In ::SetSampleSet I inserted abs. value of determinant to avoid log of neg arguments
//mostly very close to 0 but nevertheless not allowed
//
/* ClassifierMaxLikelihood
   Copyright Ilwis System Development ITC
   June 1995, by Jelle Wind
	Last change:  JHE  20 Aug 97    6:02 pm
*/

#include "Engine\Function\MXLIKELI.H"
#include "Engine\Base\DataObjects\ERR.H"

const char* ClassifierMaxLikelihood::sSyntax()
{
  return "ClassifierMaxLikelihood()\nClassifierMaxLikelihood(threshold)";
}

ClassifierMaxLikelihood::~ClassifierMaxLikelihood()
{
}

ClassifierMaxLikelihood* ClassifierMaxLikelihood::create(const FileName& fn, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms > 1)
    ExpressionError(sExpr, sSyntax());
  double rThres = rUNDEF;
  if (iParms == 1)
    rThres = as[0].rVal();
  return new ClassifierMaxLikelihood(fn, rThres);
}

ClassifierMaxLikelihood::ClassifierMaxLikelihood(const FileName& fn)
: ClassifierMinMahaDist(fn)
{
}

ClassifierMaxLikelihood::ClassifierMaxLikelihood(const FileName& fn, double rThresh)
: ClassifierMinMahaDist(fn, rThresh)
{
}

void ClassifierMaxLikelihood::Store()
{
  ClassifierMinMahaDist::Store();
  WriteElement("Classifier", "Type", "ClassifierMaxLikelihood");
}    

String ClassifierMaxLikelihood::sName(bool fExt, const String& sDirRelative) const
{
  String s = IlwisObjectPtr::sName(fExt, sDirRelative);
  if (s.length() != 0)
    return s;
  if (rThreshold != rUNDEF)
    return String("ClassifierMaxLikelihood(%f)", rThreshold);
  else 
    return String("ClassifierMaxLikelihood()");
}

double ClassifierMaxLikelihood::rAdd(int iClass)
{
  return lnDet[iClass];
}

void ClassifierMaxLikelihood::SetSampleSet(const SampleSet& sms) 
{
  ClassifierMinMahaDist::SetSampleSet(sms);
  for (int iOrd = 1; iOrd <= iClasses; iOrd++) {
    int cl = sms->dc()->iKey(iOrd);
    double vcvInvDet = abs(varcovinv[cl].rDet());
    // because the determinant of the inverse matrix equals 
    // the inverse of the determinant of the original matrix:
    lnDet[cl] = log(1/vcvInvDet);
  }
}


