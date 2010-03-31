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
// RoseDiagramView.h: interface for the RoseDiagramView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RoseDiagramVIEW_H__91D17D14_A63C_11D3_B822_00A0C9D5342F__INCLUDED_)
#define AFX_RoseDiagramVIEW_H__91D17D14_A63C_11D3_B822_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GRAPHVIEW_H__A9C4CEB5_A34D_11D3_B821_00A0C9D5342F__INCLUDED_)
#include "Client\GraphWindow\GraphView.h"
#endif

class RoseDiagramDrawer;


class RoseDiagramView: public GraphView
{
public:
	RoseDiagramView();
	virtual ~RoseDiagramView();
  afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	bool fConfigInit(CWnd* wPar);
  bool NewGraphLayer(bool fColX, const String& sColX, const String& sColY, bool fConfigLayer, bool fXAxisDefined);
  void SaveSettings(const FileName& fn, int iGraphWindow);
  void LoadSettings(const FileName& fn, int iGraphWindow);
  void OnGraphAddColumn();
	TableDoc* GetDocument();
	const TableDoc* GetDocument() const;
	RoseDiagramDrawer* rdd;
protected:
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	Column colX;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_RoseDiagramVIEW_H__91D17D14_A63C_11D3_B822_00A0C9D5342F__INCLUDED_)
