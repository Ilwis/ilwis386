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
   Semivariogram
   march 1998, by Wim Koolhoven
   (c) Ilwis System Development, ITC
	Last change:  WK   20 Apr 98    4:39 pm
*/

#ifndef SEMIVARIOGRAM_H
#define SEMIVARIOGRAM_H
#include "Engine\Base\DataObjects\strng.h"
#include "Engine\Base\DataObjects\ilwisobj.h"

enum SemiVariogramModel
  { svmSPHERICAL, svmEXPONENTIAL, svmGAUSSIAN,
    svmWAVE, svmCIRCULAR, svmPOWER,
    svmRATQUAD };

class SemiVariogram
{
  friend class FieldSemiVariogram;
	friend class FieldSemiVariogramList;
public:
  _export SemiVariogram();
  _export SemiVariogram(const String& sExpr);
  _export SemiVariogram(const FileName& fn, const char* sSection);
  _export String sExpression() const;
  void _export Store(const FileName& fn, const char* sSection) const;
  double _export rCalc(double rDist) const;
  String _export sModel() const;
private:
  SemiVariogramModel svm;
  double rNugget, rSill, rRange;
  double rPower, rSlope;
};

#endif // SEMIVARIOGRAM_H




