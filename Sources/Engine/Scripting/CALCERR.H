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
/* calcerr.h
   Interface for calculator errors
   june 1997, Jelle Wind
	Last change:  JEL   6 Jun 97    3:12 pm
*/
#ifndef ILWCALCERR_H
#define ILWCALCERR_H
#include "Headers\base.h"
#include "Engine\Base\DataObjects\ERR.H"

//! Function that shows an error in an expression, using a caret.
void _export ShowExpressionError(const String& sTitle,
                     const String& sError,
                     const String& sExpr, const String& sCaret,
                     const String& sWhere);

//! Class for calculator Errors.
class CALCEXPORT CalcError : public ErrorObject
{
public:
  CalcError(const String& sExpr, const String& sErr, const String& sTitle, int err, int iLinePos, int iColPos);
  virtual void Show(const String& sTitle = "") const;
protected:
private:
  int iLinePos, iColPos;
  String sExpr;
};

#endif




