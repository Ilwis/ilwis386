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
/* RepresentationGradual
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK    6 May 97    5:40 pm
*/
#define RPRGRAD_C
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Base\AssertD.h"

RepresentationGradual::RepresentationGradual(const FileName& fn)
: RepresentationPtr(fn), iClrSteps(1)
{
  int iLimits = iReadElement("RepresentationGradual", "Limits");
  iStretchSteps = iReadElement("RepresentationGradual", "StretchSteps");
  ac.Resize(iLimits);
  acr.Resize(iLimits-1);
  aiIndex.Resize(iLimits);
  arLimits.Resize(iLimits);
  for (int i = 0; i < iLimits; ++i) {
    if (i > 0) {
      String sBetween("Step%i", i);
      String s;
      ReadElement("RepresentationGradual", sBetween.c_str(), s);
      if (fCIStrEqual(s , "Upper"))
        acr[i-1] = crUPPER;
      else if (fCIStrEqual(s , "Lower"))
        acr[i-1] = crLOWER;
      else if (fCIStrEqual(s , "Stretch"))
        acr[i-1] = crSTRETCH;
      else
        acr[i-1] = crSTRETCH;    
    }  
    String sLimit("Limit%i", i);
    ReadElement("RepresentationGradual", sLimit.c_str(), arLimits[i]);
    String sColor("Color%i", i);
    ReadElement("RepresentationGradual", sColor.c_str(), ac[i]);
  }
  init();
}

RepresentationGradual::RepresentationGradual(const FileName& fn, const Domain& dom)
: RepresentationPtr(fn,dom),
ac(2), acr(1), aiIndex(2), arLimits(2), iClrSteps(1)
{
	reset();
}

RepresentationGradual::RepresentationGradual(const FileName& fn, const RepresentationGradual* rprg) : RepresentationPtr(fn) {
	ac.resize(rprg->ac.iSize());
	acr.resize(rprg->acr.iSize());
	aiIndex.resize(rprg->aiIndex.iSize());
	arLimits.resize(rprg->arLimits.iSize());
	for(int i =0; i < ac.size(); ++i)
		ac[i] = rprg->ac[i];
	for(int i=0; i < acr.size(); ++i)
		acr[i] = rprg->acr[i];
	for(int i=0; i < aiIndex.size(); ++i)
		aiIndex[i] = rprg->aiIndex[i];
	for(int i=0; i < arLimits.size(); ++i)
		arLimits[i] = rprg->arLimits[i];
	iStretchSteps = rprg->iStretchSteps;
	iClrSteps = rprg->iClrSteps;

}

void RepresentationGradual::reset() {
	ac.resize(2);
	acr.resize(1);
	aiIndex.resize(2);
	arLimits.resize(2);

	iStretchSteps = 5;
	ac[0] = Color(0,0,0);
	ac[1] = Color(255,255,255);
	acr[0] = crSTRETCH;
	arLimits[0] = 0;
	arLimits[1] = 1;
	init();
}
void RepresentationGradual::init()
{
	iClrSteps = 1;
	for (unsigned int i = 0; i < ac.iSize(); ++i)
	{
		if (i > 0)
		{
			if (acr[i-1] == crSTRETCH)
				iClrSteps += iStretchSteps;
			else
				iClrSteps += 1;
		}
		aiIndex[i] = iClrSteps;
	}
}

void RepresentationGradual::Store()
{
	RepresentationPtr::Store();
	WriteElement("Representation", "Type", "RepresentationGradual");
	WriteElement("RepresentationGradual", "Limits", (long)arLimits.iSize());
	WriteElement("RepresentationGradual", "StretchSteps", iStretchSteps);
	for (unsigned int i = 0; i < arLimits.iSize(); ++i)
	{
		if (i > 0)
		{
			String sBetween("Step%i", i);
			String s;
			switch (acr[i-1])
			{
				case crUPPER: s = "Upper"; break;
				case crLOWER: s = "Lower"; break;
				case crSTRETCH: s = "Stretch"; break;
			}
			WriteElement("RepresentationGradual", sBetween.c_str(), s);
		}  
		String sLimit("Limit%i", i);
		WriteElement("RepresentationGradual", sLimit.c_str(), arLimits[i]);
		String sColor("Color%i", i);
		WriteElement("RepresentationGradual", sColor.c_str(), ac[i]);
	}
}

RepresentationGradual::~RepresentationGradual()
{
}
  
String RepresentationGradual::sType() const
{
	return "Representation Gradual";
}

void RepresentationGradual::GetColors(ColorBuf& clr) const
{
	GetColorLut(clr);
}

void RepresentationGradual::GetColorLut(ColorBuf& cb) const
{
	cb.Size(iClrSteps+1);
	for (long i = 1; i <= iClrSteps; ++i)
		cb[i] = clrRaw(i);
}

Color RepresentationGradual::clrRaw(long iRaw) const
{
	if (iRaw == iUNDEF)
		return Color();
	if (iRaw < 1)
		iRaw = 1;
	else if (iRaw > iClrSteps)
		iRaw = iClrSteps;

	int i = 0;
	while (aiIndex[i] < iRaw) ++i;
	if (aiIndex[i] == iRaw)
		return ac[i];

	Color col, clrStart, clrEnd;  
	clrStart = ac[i-1];
	clrEnd   = ac[i];
	iRaw -= aiIndex[i-1];
	
	if (acr[i - 1] == crUPPER)
		return clrEnd;
	if (acr[i - 1] == crLOWER)
		return clrStart;

	// Handle stretch case
	int iNr = max(iRaw, 0);
	int iTotal = iStretchSteps;
	
	short iDiffRed   = clrEnd.red() - clrStart.red();
	iDiffRed *= iNr;
	iDiffRed /= iTotal;
	short iDiffGreen = clrEnd.green() - clrStart.green();
	iDiffGreen *= iNr;
	iDiffGreen /= iTotal;
	short iDiffBlue  = clrEnd.blue() - clrStart.blue();
	iDiffBlue *= iNr;
	iDiffBlue /= iTotal;
	col = clrStart;
	col.red()   += iDiffRed;
	col.green() += iDiffGreen;
	col.blue()  += iDiffBlue;
	return col;  
}

byte RepresentationGradual::iColor(double rValue) const
{
	if (rValue < arLimits[0])
		return 1;
	if (rValue > arLimits[arLimits.iSize()-1])    
		return iClrSteps;
	
	int i = 0;
	while (arLimits[i+1] < rValue) ++i;  
	if (arLimits[i] == rValue)
		return aiIndex[i];
	
	switch (acr[i])
	{
		case crUPPER:
			return aiIndex[i + 1];
		case crLOWER:
			return aiIndex[i];
		case crSTRETCH:
			// Compensate: the limits colors belong to both sides of the limit value
			//             not only in the upper direction, because then the upper most
			//             limit will never become available. This means that the lowest
			//             limit and highest limit colors only participate half the step size.
			double rHalf = (arLimits[i+1] - arLimits[i]) / (2 * iStretchSteps);
			double rFact = (rValue + rHalf - arLimits[i]) / (arLimits[i+1] - arLimits[i]);
			rFact *= iStretchSteps;
			return aiIndex[i] + (int)floor(rFact);
	}  
	return 0;
}

int RepresentationGradual::insert(double rVal, Color clr, ColorRange rng)
{
	if (rVal < arLimits[0])
		return iUNDEF;
	if (rVal > arLimits[arLimits.iSize()-1])    
		return iUNDEF;
	
	int iNew = 0;
	while (arLimits[iNew] < rVal) ++iNew;
	if (arLimits[iNew] == rVal)
	{
		ac[iNew] = clr;		
	}	
	else
	{
		arLimits.Insert(iNew,1);
		
		arLimits[iNew] = rVal;  
		ac.Insert(iNew,1);
		
		ac[iNew] = clr;
		acr.Insert(iNew,1);
		if ( rng == crUNDEF)
			acr[iNew] = acr[iNew-1];
		else
			acr[iNew] = rng;
		
		aiIndex.Append(1);
		init();
		Updated();
		return iNew;
	}
	return iUNDEF;
}

void RepresentationGradual::remove(unsigned int id)
{
	if (id <= 0 || id >= arLimits.iSize() - 1)
		return;
	ac.Remove(id,1);
	acr.Remove(id,1);
	aiIndex.Remove(id,1);
	arLimits.Remove(id,1);
	init();  
	Updated();
}

void RepresentationGradual::SetLimitValue(int iIndex, double rV)
{
	ISTRUE(fIGreaterEqual, iIndex, 0); // upper lower may not be changed
	ISTRUE(fILess, iIndex, (int)arLimits.iSize());
	arLimits[iIndex] = rV;
	init();
}

void RepresentationGradual::SetColorMethod(int iIndex, ColorRange cm)
{
	ISTRUE(fIGreaterEqual, iIndex, 0); // upper lower may not be changed
	ISTRUE(fILess, iIndex, (int)arLimits.iSize());
	acr[iIndex] = cm;
}

void RepresentationGradual::SetLimitColor(int iIndex, Color c)
{
	ISTRUE(fIGreaterEqual, iIndex, 0); 
	ISTRUE(fILess, iIndex, (int)arLimits.iSize());
	ac[iIndex] = c;
	init();
}

void RepresentationGradual::SetStretchSteps(int iSteps) 
{
	ISTRUE(fIGreater, iSteps, 0);
	iStretchSteps = iSteps;
	init();
}

double RepresentationGradual::rGetLimitValue(int iIndex)
{
	ISTRUE(fIGreaterEqual, iIndex, 0); // upper lower check
	ISTRUE(fILess, iIndex, (int)arLimits.iSize());
	return arLimits[iIndex];
}
