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
#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Client\ilwis.h"
#include "Client\Editors\Utils\GradientBand.h"
#include "Engine\Representation\Rprgrad.h"

//--[ GradientBand ]---------------------------------------------------
const static int iBANDDIM = 45;
const static int iTICKSIZE = 8;

GradientBand::GradientBand() 
: rLowerVal(rUNDEF)
,	rUpperVal(rUNDEF)
,	wndPar(0)
{
	rectOuter.SetRectEmpty();
}

GradientBand::~GradientBand()
{
}

bool GradientBand::Create(CWnd* par, const Representation& _rpr, CRect& rct) 
{
	rectOuter = rct;
	rpr = _rpr;
	wndPar = par; 
	SetOuterRect(rct);
	FillNumbersArray();
	return true;
}

void GradientBand::Draw(CDC *dc)
{
	FillNumbersArray();

	int iOldMode = dc->SetBkMode(TRANSPARENT);
	CFont *oldFnt = dc->SelectObject(&font);

	// border
	{
		CPen pen(PS_SOLID, 1, GetSysColor(COLOR_WINDOWTEXT));
		CBrush brush(GetSysColor(COLOR_3DLIGHT));
		CBrush *oldBrush = dc->SelectObject(&brush);
		CPen *oldPen = dc->SelectObject(&pen);
		CRect rct(rectInner);
		rct.InflateRect(2,2);
		dc->Rectangle(&rct);
		dc->SelectObject(oldPen);
		dc->SelectObject(oldBrush);
	}

	// draw bar itself
	if (fHorizontal()) 
	{
		int iTop = rectInner.top;
		int iBottom = rectInner.bottom;
		int iLeft = rectInner.left;
		int iWidth = rectInner.Width();
		for (int iX = 0; iX < iWidth; ++iX) 
		{
			Color color = clr(double(iX)/iWidth);
			CPen pen(PS_SOLID, 1, color);
			CPen* penOld = dc->SelectObject(&pen);
			dc->MoveTo(iLeft + iX, iTop);
			dc->LineTo(iLeft + iX, iBottom);
			dc->SelectObject(penOld);
		}
	}
	else 
	{
		int iLeft = rectInner.left;
		int iRight = rectInner.right;
		int iBottom = rectInner.bottom - 1;
		int iHeight = rectInner.Height();
		for (int iY = 0; iY < iHeight; ++iY)
		{
			Color color = clr(double(iY)/iHeight);
			CPen pen(PS_SOLID, 1, color);
			CPen* penOld = dc->SelectObject(&pen);
			dc->MoveTo(iLeft, iBottom - iY);
			dc->LineTo(iRight, iBottom - iY);
			dc->SelectObject(penOld);
		}
	}

	// tickmarks and text
	for (vector<ShowStruct>::reverse_iterator iter = vssShow.rbegin(); iter != vssShow.rend(); ++iter)
	{
		ShowStruct ss = *iter;
		double rVal = ss.rVal;
		String str = sVal(rVal);
		double rPerc = (rVal - rLowerVal) / (rUpperVal - rLowerVal);
		SetText(dc, rPerc, str, ss.fShow);
	}

	dc->SetBkMode(iOldMode);
	dc->SelectObject(oldFnt);
}


Color GradientBand::clr(double rPerc) const
{
	if (0 != rpr->prv()) {
		rPerc *= rUpperVal - rLowerVal;
		rPerc += rLowerVal;
	}
	return rpr->clr(rPerc);
}

void GradientBand::CalcStepValues(RangeReal r, double& rLowerLim, double& rStep)
{
	double rMult[3]= {1.0,2.0,5.0};
	double rRange = abs(r.rHi() - r.rLo());
	short iPow = ((short)(log10(rRange))) - 1;
	double rGuess = rRange / 5.0; // about 5 numbers along the edges.
  for(int i=0; i< 3; ++i)
  {
    if(rMult[i] * pow(10.0, iPow) > rGuess)
    {
			if(i == 0)
			{
				i = 3;
				iPow--;
			}
      rGuess = rMult[i-1] * pow(10.0, iPow);
      break;
    }
    if (i == 2)
		{
			i=0;
			++iPow;
		}
  }
	rLowerLim = ((int)(r.rLo() / rGuess)) * rGuess;
	if (rLowerLim < r.rLo())
		rLowerLim = r.rLo();
  rStep = rGuess;
}

//-----------------------------------------------------------------------------------------------
// purpose : Sets a value text along the side determined by the style (right, left, up down)
// parameters : CDC *cd         Device context used
//              double rPerc    determines the place where the text will be place (perc of rect side)
//              double rLV      The value to be placed
// returns : -
//-------------------------------------------------------------------------------------------------
void GradientBand::SetText(CDC* cdc, double rPerc, const String& sLV, bool fShowText)
{
	int iX, iY;
	CSize textSize = cdc->GetTextExtent(sLV.c_str(), sLV.size());

	// set ticks and determine text position
	if (!fHorizontal())
	{
		iX = rectInner.left - textSize.cx - iTICKSIZE;
		iY = rectInner.bottom - rectInner.Height() * rPerc - textSize.cy / 2 - 1;
		cdc->MoveTo(rectInner.left, iY + textSize.cy / 2);
		cdc->LineTo(rectInner.left - iTICKSIZE, iY + textSize.cy / 2);			
	}		
	else
	{
		iX = rectInner.left + rectInner.Width() * rPerc - textSize.cx / 2;
		iY = rectInner.bottom + iTICKSIZE;
		cdc->MoveTo(iX + textSize.cx / 2, rectInner.bottom);
		cdc->LineTo(iX + textSize.cx / 2, rectInner.bottom + iTICKSIZE - 2);
	}
	if (fShowText)
		cdc->TextOut(iX, iY, sLV.c_str(), sLV.size());
}

void GradientBand::SetHorizontal(bool fHorz)
{
	m_fHorizontal = fHorz;
}

CRect GradientBand::SetOuterRect(CRect rct)
{
	rectOuter = rct;
	rectInner = rectOuter;
	CSize maxSize = szMaxTextSize();
	if (!fHorizontal())
	{
		rectInner.left = maxSize.cx + iTICKSIZE + 3;
		rectInner.right -= 4;
		rectInner.top += maxSize.cy / 2;
		rectInner.bottom -= maxSize.cy / 2; 
	}
	else
	{
		rectInner.top += 4;
		rectInner.bottom -= maxSize.cy + iTICKSIZE;
		rectInner.left += maxSize.cx / 2;
		rectInner.right -= maxSize.cx / 2;
	}
	return rectOuter;	
}	

//-----------------------------------------------------------------------------------------------
// purpose : Calculates the maximum size needed by the text alognside the band
// parameters : -
// returns : CSize the calculated size
//------------------------------------------------------------------------------------------------- 
CSize GradientBand::szMaxTextSize()
{
	FillNumbersArray();
	CDC *dc = wndPar->GetDC();
	CFont *oldFnt = dc->SelectObject(&font);
	int iMaxX = -1, iMaxY = -1;

	for (vector<ShowStruct>::const_iterator iter = vssShow.begin(); iter != vssShow.end(); ++iter)
	{
		const ShowStruct ss = *iter;
		if (!ss.fShow)
			continue;
		String str = sVal(ss.rVal);
		CSize sz = dc->GetTextExtent(CString(str.c_str()));
		iMaxX = max(iMaxX, sz.cx);
		iMaxY = max(iMaxY, sz.cy);
	}
	dc->SelectObject(oldFnt);
	return CSize(iMaxX, iMaxY);
}

void GradientBand::FillNumbersArray()
{
	RepresentationGradual* prg = rpr->prg();
	rLowerVal = prg->rValue(0);
	rUpperVal = prg->rValue(prg->iLimits()-1);
	double rLower, rValStep;
	CalcStepValues(RangeReal(rLowerVal,rUpperVal), rLower, rValStep);

	vssShow.clear();
	CDC *dc = wndPar->GetDC();
	CFont *oldFnt = dc->SelectObject(&font);

	ShowStruct ss;
	ss.rVal = rLower;
	ss.fShow = true;
	vssShow.push_back(ss);
	int iXLeft = rectInner.left;
	int iYTop = rectInner.top;
	String sV = sVal(rLowerVal);
	CSize sz = dc->GetTextExtent(sV.c_str());
	CSize szEnd = dc->GetTextExtent(sVal(rUpperVal).c_str());
	iXLeft += sz.cx/2;
	iYTop += sz.cy/2;
	for(rLower += rValStep; rLower <= rUpperVal; rLower += rValStep)
	{
		ss.rVal = rLower;
		ss.fShow = false;
		sV = sVal(rLower);
		sz = dc->GetTextExtent(sV.c_str());
		if (fHorizontal())
		{
			int iCurrentX = rectInner.left + (rLower - rLowerVal) / (rUpperVal - rLowerVal) * rectInner.Width();
			if (iCurrentX - sz.cx/2 >= iXLeft)
			{
				ss.fShow = true;
				iXLeft = iCurrentX + sz.cx/2;
			}
		}
		else
		{
			int iCurrentY = rectInner.top + (rLower - rLowerVal) / (rUpperVal - rLowerVal) * rectInner.Height();
			if (iCurrentY - sz.cy/2 >= iYTop)
			{
				ss.fShow = true;
				iYTop = iCurrentY + sz.cy/2;
			}
		}
		vssShow.push_back(ss);
	}
	dc->SelectObject(oldFnt);
}



String GradientBand::sVal(double rVal) const
{
	String sVal;
	DomainValue* pdv = rpr->dm()->pdv();
	if (rpr->prv())
		sVal = pdv->sValue(rVal, 0);
	else        
		sVal = String("%5.1f %%", 100 * rVal);

	return sVal;
}
	
//-----------------------------------------------------------------------------------------------
// purpose : Calculates the total size needed by the band (text + rectInner)
// parameters : -
// returns : -
//------------------------------------------------------------------------------------------------- 
CSize GradientBand::GetSizeNeeded()
{
	CSize bandSize, sz = szMaxTextSize();
	if (fHorizontal())
	{
		bandSize.cx = rectOuter.Width();
		bandSize.cy = iBANDDIM + sz.cy + rectOuter.top + 6; // +6 for tick mark
	}
	else 
	{
		bandSize.cy = rectOuter.Height();
		bandSize.cx = iBANDDIM + sz.cx + rectOuter.left + 16; // +6 for tick mark
	}
	return bandSize;
}

bool GradientBand::fHorizontal() const
{
	return m_fHorizontal; 
}

void GradientBand::SetFont(CFont *fnt)
{
	LOGFONT lf;
	fnt->GetLogFont(&lf);
	font.CreateFontIndirect(&lf);
}
