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
// FormMaplistGraph.cpp: implementation of the FormMaplistGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "ApplicationsUI\FormMaplistGraph.h"
#include "Engine\Base\Algorithm\MaplistGraphFunction.h"
#include "Client\FormElements\FieldGraph.h"
#include "Client\FormElements\fldlist.h"
#include "Client\ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\constant.h"

//////////////////////////////////////////////////////////////////////
// FormMaplistGraph
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(FormMaplistGraph, FormBaseDialog)
	//{{AFX_MSG_MAP(FormMaplistGraph)
	ON_MESSAGE(ILW_UPDATE, OnUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

LRESULT Cmdmaplistgraph(CWnd *wnd, const String& s)
{
	new FormMaplistGraph(CWnd::GetDesktopWindow(), s);
	return -1;
}

FormMaplistGraph::FormMaplistGraph(CWnd* parent, const String& sMpl)
: FormBaseDialog(parent, "Maplist Graph", true, false, true)
, m_mgf(0)
,	m_fEnableCheckData(false)
, m_fMouse(true)
, m_fDigitizer(true)
, m_fContinuous(false)
, m_fOnTop(false)
, m_fFixedStretch(false)
, m_rStretchMin(0)
, m_rStretchMax(350)
, m_sMplName(sMpl)
, vrPrecision(ValueRange(-LONG_MAX+1, LONG_MAX, 0.001))
{
	IlwisSettings settings("pixelinfo");

	m_fMouse = settings.fValue("UseMouse", true);		
	m_fDigitizer = settings.fValue("UseDigitizer", false);				

	create();

	m_mgf = new MaplistGraphFunction();
	m_fgMaplistGraph->SetBorderThickness(50, 20, 20, 20);
	if (m_sMplName.length() > 0)
		HandleMaplistChange(0);
}

FormMaplistGraph::~FormMaplistGraph()
{
	m_fgMaplistGraph->SetFunction(0);
	if (m_mgf)
		delete m_mgf;
}

void FormMaplistGraph::create()
{
	int iImg = IlwWinApp()->iImage(".grh");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);
		
	fbs |= fbsNOOKBUTTON | fbsNOCANCELBUTTON;

	m_fml = new FieldMapList(root, "MapList", &m_sMplName, true);
	m_fml->SetCallBack((NotifyProc)&FormMaplistGraph::HandleMaplistChange);
	m_fml->SetIndependentPos();

	m_pbCopy = new PushButton(root, "Clipboard &Copy", (NotifyProc) (&FormMaplistGraph::ClipboardCopy));
	m_pbCopy->Align(m_fml, AL_AFTER);
	
	m_cbCont = new CheckBox(root, "C&ontinuous", &m_fContinuous);
	m_cbCont->SetCallBack((NotifyProc)&FormMaplistGraph::HandleOnClickContinuous);
	m_cbCont->Align(m_pbCopy, AL_AFTER);

	m_cbOnTop = new CheckBox(root, "&Always On Top", &m_fOnTop);
	m_cbOnTop->SetCallBack((NotifyProc)&FormMaplistGraph::HandleOnClickOnTop);
	m_cbOnTop->Align(m_cbCont, AL_AFTER);

	m_cbFixedStretch = new CheckBox(root, "Fix Stretch", &m_fFixedStretch);
	m_cbFixedStretch->SetCallBack((NotifyProc)&FormMaplistGraph::HandleStretchChange);
	m_cbFixedStretch->Align(m_fml, AL_UNDER);
	m_cbFixedStretch->SetIndependentPos();
	m_frStretchMin = new FieldReal(m_cbFixedStretch, "", &m_rStretchMin, vrPrecision);
	m_frStretchMin->SetCallBack((NotifyProc)&FormMaplistGraph::HandleStretchChange);
	m_frStretchMin->SetIndependentPos();
	m_frStretchMax = new FieldReal(m_cbFixedStretch, "", &m_rStretchMax, vrPrecision);
	m_frStretchMax->SetCallBack((NotifyProc)&FormMaplistGraph::HandleStretchChange);
	m_frStretchMax->SetIndependentPos();
	m_frStretchMax->Align(m_frStretchMin, AL_AFTER);

	m_fgMaplistGraph = new FieldGraph(root);
	m_fgMaplistGraph->SetCallBack((NotifyProc)&FormMaplistGraph::HandleDragOnGraph);
	m_fgMaplistGraph->SetWidth(300);
	m_fgMaplistGraph->SetHeight(300);
	m_fgMaplistGraph->Align(m_cbFixedStretch, AL_UNDER);
	m_fgMaplistGraph->SetIndependentPos();

	m_sInfoText1 = new StaticText(root, "                                                                                                                                                                         ");
	m_sInfoText1->Align(m_fgMaplistGraph, AL_UNDER);

	m_sInfoText2 = new StaticText(root, "                     ");
	m_sInfoText2->Align(m_sInfoText1, AL_AFTER);

	FormBaseDialog::create();

	AfxGetApp()->PostThreadMessage(ILW_ADDDATAWINDOW, (WPARAM)m_hWnd, 0);
}

void FormMaplistGraph::shutdown(int iReturn) 
{
	AfxGetApp()->PostThreadMessage(ILW_REMOVEDATAWINDOW, (WPARAM)m_hWnd, 0);
	FormBaseDialog::shutdown(iReturn);
}

int FormMaplistGraph::exec()
{
	return 1;
}

FormEntry* FormMaplistGraph::feDefaultFocus()
{
  return m_pbCopy;
}

int FormMaplistGraph::HandleMaplistChange(Event *)
{
	m_fml->StoreData();
	if (m_mgf)
	{
		m_mgf->SetMaplist(m_sMplName);
		m_fgMaplistGraph->SetFunction(m_mgf); // arranges that the XY-axes are re-set and plots the graph
	}
	else
		m_fgMaplistGraph->Replot();

	RefreshInfoText();

	return 1;
}

int FormMaplistGraph::HandleOnClickContinuous(Event *)
{
	m_cbCont->StoreData();

	return 1;
}

int FormMaplistGraph::HandleOnClickOnTop(Event *)
{
	m_cbOnTop->StoreData();

  if (m_fOnTop)
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
  else
    SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);


	return 1;
}

int FormMaplistGraph::HandleStretchChange(Event *)
{
	m_cbFixedStretch->StoreData();
	m_frStretchMin->StoreData();
	m_frStretchMax->StoreData();
	if (m_mgf)
	{
		m_mgf->SetStretch(m_fFixedStretch, m_rStretchMin, m_rStretchMax);
		m_fgMaplistGraph->SetFunction(m_mgf); // arranges that the XY-axes are re-set and plots the graph
	}
	return 1;
}

int FormMaplistGraph::HandleDragOnGraph(Event *)
{
	if (m_mgf)
	{
		DoublePoint dp = m_mgf->dpDragPoint();
		if (dp.X != rUNDEF && dp.Y != rUNDEF)
		{
			int iDec = m_mgf->iDec();
			long iBand = floor(dp.X);
			String sPoint ("[%li, %.*f]", iBand, iDec, dp.Y);
			m_sInfoText2->SetVal(sPoint);
		}
		else
			m_sInfoText2->SetVal("");
	}
	else
		m_sInfoText2->SetVal("");
	
	return 1;
}

int FormMaplistGraph::ClipboardCopy(Event *)
{
	if (m_mgf)
	{
		String sClipboardText = m_mgf->sClipboardText();
		BOOL fSuccess = OpenClipboard();
		if (fSuccess)
		{
			EmptyClipboard();
			int iLen = sClipboardText.length();
			HGLOBAL hnd = GlobalAlloc(GMEM_FIXED, iLen+2);
			char* pc = (char*)GlobalLock(hnd);
			strcpy(pc, sClipboardText.c_str());
			GlobalUnlock(hnd);
			SetClipboardData(CF_TEXT,hnd);
			CloseClipboard();
		}
	}

	return 1;
}

FormEntry *FormMaplistGraph::CheckData()
{
	if (m_fEnableCheckData)
		return FormBaseDialog::CheckData();
	else
		return 0;
}

LRESULT FormMaplistGraph::OnUpdate(WPARAM wParam, LPARAM lParam)
{
	CoordMessage cm = (CoordMessage) wParam;
  CoordWithCoordSystem* cwcs = (CoordWithCoordSystem*)(void*) lParam;
  bool fUpdate = false;
	switch (cm) {
		case cmMOUSEMOVE:
	    fUpdate = m_fMouse && m_fContinuous;
			break;
		case cmMOUSECLICK:
	    fUpdate = m_fMouse;
			break;
		case cmDIGIMOVE:
	    fUpdate = m_fDigitizer && m_fContinuous;
			break;
		case cmDIGICLICK1:
		case cmDIGICLICK2:
		case cmDIGICLICK3:
		case cmDIGICLICK4:
	    fUpdate = m_fDigitizer;
			break;
	}
  if (fUpdate)
		SetCoord(*cwcs);
	return 0;
}

void FormMaplistGraph::SetCoord(CoordWithCoordSystem & cwcs)
{
	if (m_mgf)
	{
		m_mgf->SetCoordAndCsy(cwcs);
		m_mgf->ReadData();
		m_fgMaplistGraph->SetFunction(m_mgf); // arranges that the XY-axes are re-set and plots the graph
		
		RefreshInfoText();
	}
}

void FormMaplistGraph::RefreshInfoText()
{
	if (m_mgf)
	{
		GeoRef grf = m_mgf->grfMpl();
		CoordSystem csy = m_mgf->csyMpl();
		Coord crd = m_mgf->crdMpl();
		String sRowCol ("[?,?]");
		if (grf.fValid())
		{
			RowCol rc = grf->rcConv(crd);
			rc.Row += 1;
			rc.Col += 1; // user sees RowCols from 1 to max (instead of from 0 to max-1)
			sRowCol = String("[%li,%li]", rc.Row, rc.Col);
		}
		String sLatLon ("?, ?");
		if (csy.fValid())
		{
			LatLon ll = csy->llConv(crd);
			sLatLon = ll.sValue();
		}
		String sCoord ("(?,?)");
		if (!crd.fUndef())
			sCoord = String("(%.1f,%.1f)", crd.x, crd.y);
		String sInfoText ("%S    %S    %S", sRowCol, sCoord, sLatLon);
		m_sInfoText1->SetVal(sInfoText);
	}
}
