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
// MakeEpipolarFormView.h: interface for the MakeEpipolarFormView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAKEEPIPOLARFORMVIEW_H__7B988135_4BF7_4E5F_89AF_80FCCE906890__INCLUDED_)
#define AFX_MAKEEPIPOLARFORMVIEW_H__7B988135_4BF7_4E5F_89AF_80FCCE906890__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class FieldRowColFiducial;
class FieldRowColPrincPnt;
class MapPaneView;
class MakeEpipolarDocument;

class MakeEpipolarFormView: public FormBaseView  
{
public:
	MakeEpipolarFormView();
	virtual ~MakeEpipolarFormView();
	MakeEpipolarDocument * GetDocument();
	BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	static const int iFieldSize;
	void SetActiveColor(Color &);
	Color GetActiveColor(); // to read/set the respective protected member
	void SetInputImageName(const String&);
protected:
	virtual int DataChanged(Event *);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	afx_msg HBRUSH OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor);
private:
  // int calc(Event*);
  StaticText *stDummy, *stFiducials, *stOffFlight, *stPP, *stTPP, *stRot, *stScf, *stInputTxt;
  Color clrTextNormal, clrCurrentlyActive, clrBackNormal, clrBackCurrentlyActive;
  CBrush *brNormal, *brCurrentlyActive;
  FieldRowColFiducial *fFm[4];
	FieldRowColFiducial *sFm[2];
	FieldRowCol *frcTPP, *frcPP;
	StaticText *stfRot, *stfScf, *stInputName;
	RowCol rcPP, rcTPP;
	bool fInCalc;
	bool fInUpdate;
	bool fPPDefined;	// we can't do crdPP.fUndef() because we don't know for sure what
										// iFindPrincPointLocation does with crdPP
	FieldRowCol * frcFieldRowColFromHint(int);
	FieldRowCol * frcFieldRowColFromCurrentState();
	CheckBox * cbCheckBoxFromHint(int);
	RowCol Decreased(const RowCol&); // upper-left = (0,0), but for user = (1,1)
	RowCol Increased(const RowCol&); // these also take care of rcUNDEFs
	String sInputImageName;

	DECLARE_DYNCREATE(MakeEpipolarFormView)
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAKEEPIPOLARFORMVIEW_H__7B988135_4BF7_4E5F_89AF_80FCCE906890__INCLUDED_)
