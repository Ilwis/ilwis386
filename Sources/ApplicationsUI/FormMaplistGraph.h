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
// FormMaplistGraph.h: interface for the FormMaplistGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FORMMAPLISTGRAPH_H__4ABBF042_6CE9_44FD_BAB9_B6BD634D93FD__INCLUDED_)
#define AFX_FORMMAPLISTGRAPH_H__4ABBF042_6CE9_44FD_BAB9_B6BD634D93FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Client\FormElements\formbase.h"
#include "Engine\Function\SpectralAngle.h"

LRESULT Cmdmaplistgraph(CWnd *wnd, const String& s);


class FieldGraph;
class FieldMapList;
class MaplistGraphFunction;

class _export FormMaplistGraph : public FormBaseDialog  
{
public:
	FormMaplistGraph(CWnd* parent, const String& sMpl);
	virtual ~FormMaplistGraph();
	virtual void create();
	virtual int exec();
	virtual FormEntry * CheckData();
protected:
	afx_msg LRESULT OnUpdate(WPARAM, LPARAM);
	virtual void shutdown(int iReturn=IDCANCEL);
private:
	int HandleMaplistChange(Event *);
	int HandleMaplistRefChange(Event *);
	int HandleStretchChange(Event *);
	int HandleOnClickContinuous(Event *);
	int HandleOnClickOnTop(Event *);
	int HandleDragOnGraph(Event *);
	int ClipboardCopy(Event *);
	void SetCoord(CoordWithCoordSystem & cwcs);
	void RefreshInfoText();
	FormEntry* feDefaultFocus();
	FieldGraph* m_fgMaplistGraph;
	FieldMapList * m_fml;
	FieldMapList * m_fmlRef;
	CheckBox * m_cbFixedStretch;
	FieldReal * m_frStretchMin;
	FieldReal * m_frStretchMax;
	StaticText * m_sInfoText1;
	StaticText * m_sInfoText2;
	PushButton * m_pbCopy;
	CheckBox* m_cbCont;
	CheckBox* m_cbOnTop;
	MaplistGraphFunction * m_mgf;
	MaplistGraphFunction * m_mgfRef;
	String m_sMplName;
	String m_sMplRefName;
	bool m_fEnableCheckData;
  bool m_fMouse;
	bool m_fDigitizer;
	bool m_fContinuous;
	bool m_fOnTop;
	bool m_fFixedStretch;
	double m_rStretchMin;
	double m_rStretchMax;
	ValueRange vrPrecision;
	CheckBox* m_cbRefSpectrum;
	bool m_fShowSpectrum;
	CheckBox* m_cbSpectralAngle;
	bool m_fSpectralAngle;
	StaticText * m_sInfoText3;
	ClassifierSpectralAngle* clfSpAngle;
	String sInfoText3;
	int ShowSpectAngle(Event *);

	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_FORMMAPLISTGRAPH_H__4ABBF042_6CE9_44FD_BAB9_B6BD634D93FD__INCLUDED_)
