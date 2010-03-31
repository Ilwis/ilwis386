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
/* VarCovCorrMatrix
   Copyright Ilwis System Development ITC
   april 1996, by Jelle Wind
	Last change:  WK   12 Aug 98   12:23 pm
*/

#ifndef ILWVARCOVMT_H
#define ILWVARCOVMT_H
#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\objdepen.h"

class DATEXPORT VarCovCorrMatrix: public MatrixObjectPtr
{
  friend class DATEXPORT MatrixObjectPtr;
  VarCovCorrMatrix(const MapList& maplist, bool fCorr);
  static VarCovCorrMatrix* create(const String& sExpr);
public:
  virtual ~VarCovCorrMatrix();
  virtual String sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual String sType() const;
  virtual void GetRowText(Array<String>& as) const;
  virtual void GetColumnText(Array<String>& as) const;
  String sSummary() const;
//virtual String sAdditionalInfo() const;
//virtual bool fAdditionalInfo() const;
  virtual void MakeUsable();
  virtual bool fUsable() const;
  virtual int iWidth() const;
  virtual String sValue(int iRow, int iCol, int iWidth) const;
private:  
  MapList mpl;
  bool fCorrMat;
  Time tmCalc;
  ObjectDependency objdep;
};

#endif




