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
// BaseTblField.h: interface for the BaseTblField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASETBLFIELD_H__5E3C5B84_3056_11D3_B78F_00A0C9D5342F__INCLUDED_)
#define AFX_BASETBLFIELD_H__5E3C5B84_3056_11D3_B78F_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP BaseTblField  
{
public:
	virtual ~BaseTblField();

  void setFocus();
  void update();
  int iCol;
  long iRow;
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
protected:
  BaseTblField(BaseTablePaneView*, int col, long row);
  BaseTblField(BaseTablePaneView*, int col, long row, bool fReadOnly);
  BaseTblField(BaseTablePaneView*, int col, long row, const Domain&);
  BaseTblField(BaseTablePaneView*, int col, long row, const FileName&);
  void init(const String&);  // should be called in derived constructor
  String sText();
  BOOL fHasChanged();
  BaseTablePaneView* tbpn;
  Domain dm;
private:
  void moveTo(int col, long row);
	void FrameHandlesAccelerators(bool fYes);

  zPoint pCurr;
  CWnd* ctrl;
};

#endif // !defined(AFX_BASETBLFIELD_H__5E3C5B84_3056_11D3_B78F_00A0C9D5342F__INCLUDED_)
