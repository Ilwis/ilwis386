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
/* RepresentationItems
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK   10 Mar 98    4:08 pm
*/

#ifndef ILWRPRITEMS_H
#define ILWRPRITEMS_H
#include "Engine\Domain\dm.h"
#include "Engine\Table\Col.h"

class _export RepresentationItems: public RepresentationPtr
{
protected:  
	RepresentationItems(const FileName&);
	RepresentationItems(const FileName&, const Domain&);
public:
	virtual ~RepresentationItems();
	virtual String sType() const;
	void GetColors(ColorBuf&) const;
	void GetColorLut(ColorBuf&) const;
	Color clrRaw(long iRaw) const;
	virtual Color clrRawDefault(long iRaw) const;
	Color clrSecondRaw(long iRaw) const;
	byte iColor(double rValue) const;  // 0..1 
	short iPattern(long iRaw) const;
	void GetPattern(long iRaw, short aPat[8]) const;
	String sSymbolType(long iRaw) const;
	short iSymbolSize(long iRaw) const;
	short iSymbolWidth(long iRaw) const;
	Color clrSymbol(long iRaw) const;
	Color clrSymbolFill(long iRaw) const;
	String sSymbolFont(long iRaw) const;
	double rSymbolRotation(long iRaw) const;
	double rTransparencyItem(long iRaw) const;
	short iLine(long iRaw) const;
	Color clrLineFill(long iRaw) const;
	double rLineWidth(long iRaw) const;
	double rLineDist(long iRaw) const;
	static const double rDefaultLineWidth;
	void getProperties(long iRaw, RepresentationProperties *props);
protected:
	Column colColor, colPattern,
		colSmbType, colSmbSize, colSmbWidth, colSmbClr, colSmbFC,
		colPatternData, colSmbFont, colSmbRot, colSecondClr,
		colLineType, colLineClrFill, colLineWidth, colLineDist, colTransparency, colSmbType2;
	static Color clrDefault[16];       
};

#endif // ILWRPRITEMS_H




