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
/*
// Revision 1.3  1998/09/16 17:32:28  Wim
// 22beta2
//
// Revision 1.2  1997/08/18 17:52:14  Wim
// Added Clear() and Paste()
//
/* SampleSetEditor
by Wim Koolhoven, sep. 1994
(c) Ilwis System Development ITC
Last change:  WK   18 Aug 97    6:36 pm
*/

#ifndef SMPLEDIT_H
#define SMPLEDIT_H
#include "Client\Editors\Map\PixelEditor.h"
#include "Engine\SampleSet\SAMPLSET.H"

class _export SampleStatWindow;
class FeatureSpaceWindow;

class _export SampleSetEditor: public PixelEditor
{
public:
	SampleSetEditor(MapPaneView*, const SampleSet&);
	~SampleSetEditor();
	virtual int Edit(const Coord&);
	virtual void Paste();
	virtual void Clear();
	virtual void EditFieldOK(Coord, const String&);
	virtual void drawSelect(RowCol rc = rcUNDEF);
	virtual bool OnContextMenu(CWnd* pWnd, CPoint point);
	virtual zIcon icon() const;
	virtual String sTitle() const;
	virtual void SelectionChanged();
	virtual void PreSaveState();
	void MakeFSWindow(FeatureSpaceWindow* fsw, CPoint pos, int iID);
	void OnUpdateEdit(CCmdUI* pCmdUI);
	//{{AFX_MSG(SampleSetEditor)
	afx_msg void OnDelClass();
	afx_msg void OnMergeClass();
	afx_msg void OnShowRpr();
	afx_msg void OnFeatureSpace();
	afx_msg void OnStatisticsPane();
	afx_msg void OnUpdateStatisticsPane(CCmdUI* pCmdUI);
	//}}AFX_MSG
private:  
	SampleSet sms;
	SampleStatWindow* wSmplStat;
	vector<GeneralBar*>	vgb;
	DECLARE_MESSAGE_MAP()
};

#endif // SMPLEDIT_H