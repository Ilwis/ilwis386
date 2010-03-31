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
// GraphTreeField.h: interface for the GraphTreeField class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHTREEFIELD_H__B7F11525_6CBB_4173_9E79_9B0E4BCE4B03__INCLUDED_)
#define AFX_GRAPHTREEFIELD_H__B7F11525_6CBB_4173_9E79_9B0E4BCE4B03__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_GRAPHLAYERTREEITEM_H__E0A6D3A0_9BC9_4C43_B79C_B35E63F0A7D9__INCLUDED_)
#include "Client\TableWindow\GraphLayerTreeItem.h"
#endif

typedef void (GraphTreeItem::*NotifyFieldFinished)();

class GraphTreeField : public CEdit
{
public:
	GraphTreeField(CWnd* wnd, CRect, String, GraphTreeItem*, NotifyFieldFinished);
	virtual ~GraphTreeField();
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
  void OnKillFocus(CWnd *);
	BOOL PreTranslateMessage(MSG* pMsg);
protected:
  GraphTreeItem* gti;
  NotifyFieldFinished nff;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_GRAPHTREEFIELD_H__B7F11525_6CBB_4173_9E79_9B0E4BCE4B03__INCLUDED_)
