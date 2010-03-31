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
/* RepresentationValue
by Wim Koolhoven
(c) Ilwis System Development ITC
Last change:  WK   25 Aug 98    3:50 pm
*/

#ifndef ILWRPRGRADUAL_H
#define ILWRPRGRADUAL_H
#include "Engine\Domain\dm.h"

class DATEXPORT RepresentationGradual: public RepresentationPtr
{
	friend class RepresentationPtr;
protected:  
	RepresentationGradual(const FileName&);
public:
	enum ColorRange { crUPPER, crLOWER, crSTRETCH };

	RepresentationGradual(const FileName&, const Domain&);
	virtual ~RepresentationGradual();
	virtual String sType() const;
	virtual void Store();
	virtual void GetColors(ColorBuf&) const;
	virtual void GetColorLut(ColorBuf&) const;

	virtual Color		clrRaw(long iRaw) const;        // 0 <  iRaw <= iClrSteps
	virtual					byte iColor(double rValue) const;  // 0..1
	int							iLimits() const { return arLimits.iSize(); }
	double					rValue(int iLim) const { return arLimits[iLim]; }
	int							iNoColors() 
	{ return ac.iSize(); }
	ColorRange			GetColorMethod(int iIndex) 
	{ return acr[iIndex]; }
	void _export		SetColorMethod(int iIndex, ColorRange rng);
	double _export 	rGetLimitValue(int iIndex) ;
	void _export		SetLimitValue(int iIndex, double rV);
	Color						GetColor(int iIndex) 
	{ return ac[iIndex] ; }
	void _export		SetLimitColor(int iIndex, Color c);
	int							iGetColorIndex(int iIndex) 
	{ return aiIndex[iIndex]; }
	int							iGetStretchSteps() 
	{ return iStretchSteps; }
	void _export		SetStretchSteps(int iSteps);
	void _export		insert(double rVal, Color clr);
	void _export		remove(unsigned int);

protected:
	void						init();

	Array<Color>		ac;
	Array<ColorRange> acr;
	Array<int>			aiIndex;
	Array<double>		arLimits;
	int							iStretchSteps;  
	int							iClrSteps;
};

#endif // ILWRPRGRADUAL_H





