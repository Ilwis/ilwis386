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
// colorIntensity.h: interface for the colorIntensity class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COLORINTENSITY_H__317CCAEF_4327_11D3_9CE7_00A0C9D5320B__INCLUDED_)
#define AFX_COLORINTENSITY_H__317CCAEF_4327_11D3_9CE7_00A0C9D5320B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

//--[ ColorIntensity ]--------------------------------------------------------------
static const unsigned int ID_COLORINTENSITY=5940;
static const unsigned int ID_COLORINTENSITY_REDV=5941;
static const unsigned int ID_COLORINTENSITY_GREENV=5942;
static const unsigned int ID_COLORINTENSITY_BLUEV=5943;

static const unsigned int ID_COLORINTENSITY_REDH=5944;
static const unsigned int ID_COLORINTENSITY_GREENH=5945;
static const unsigned int ID_COLORINTENSITY_BLUEH=5946;


class IMPEXP ColorIntensity : public CWnd
{
public:
	enum cIntensity{ciRED, ciGREEN, ciBLUE};
	ColorIntensity();
	~ColorIntensity();

	BOOL Create(CRect& rct, CWnd *parent, COLORREF iC, UINT nID);
	void        SetColor(COLORREF col, bool fSendMessage=true);
//	void        OnSetColor(WPARAM wParm, LPARAM lPar);
	void        Resize(UINT nType, int cx, int cy);
	void        ChangeCtrlStyle( long lStyle, BOOL bSetBit);
	void        SetOrientation(bool fVert);
	void				SetPostOffice(CWnd *wnd);

private:
	void            OnVScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void            OnHScroll( UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void            ColorIntensity::SetText(const CSliderCtrl& bar, const String& s, CStatic& text);
	void            OnMouseMove( UINT nFlags, CPoint point );

	ColorScrollBar  cscbRedV, cscbRedH;
	ColorScrollBar  cscbGreenV, cscbGreenH;
	ColorScrollBar  cscbBlueV, cscbBlueH;
	CStatic         stRed;
	CStatic         stGreen;
	CStatic         stBlue;

	CRect           outerRect;
	Color           color;
	bool            fVertical;
	CWnd						*wndPostOffice;

protected:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	DECLARE_MESSAGE_MAP();

};

#endif // !defined(AFX_COLORINTENSITY_H__317CCAEF_4327_11D3_9CE7_00A0C9D5320B__INCLUDED_)
