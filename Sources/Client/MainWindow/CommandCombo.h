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
#ifndef COMMANDCOMBO_H
#define COMMANDCOMBO_H

const static int ID_COMMAND_COMBO=200;
const static int ID_COMMANDBAR=201;

#ifndef FRAMEWINDOW_H
#include "Client\Base\Framewin.h"
#endif

#ifndef LOGGER_H
#include "Engine\Base\System\LOGGER.H"
#endif

#if !defined(AFX_BASEBAR_H__BE6B2775_400D_11D3_B7AC_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Utils\BaseBar.h"
#endif


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


//visual representation of the command line. String entered here will be handled by the command
//handler. The listbox-part is the history of previous commands
class CommandCombo : public CComboBox
{
	friend class CommandBar;

public:
	CommandCombo();
	~CommandCombo();
	enum cbType{cbMain, cbTable};

	_export BOOL Create(CWnd* pParentWnd,
		         cbType t,
		         UINT nID=ID_COMMAND_COMBO,
		         DWORD dwStyle=WS_CHILD | WS_VISIBLE | WS_GROUP | WS_TABSTOP |
				            WS_BORDER | WS_VSCROLL | CBS_AUTOHSCROLL | CBS_DROPDOWN);
protected:
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL PreTranslateMessage(MSG* pMsg);
	void AddToHistory(const CString& s);
	LRESULT OnSetCommandLine(WPARAM wPar, LPARAM);
	void OnKillFocus();
	void OnSetFocus();
	void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	void OnMouseMove(UINT nFlags, CPoint point) ;
	Logger logger;
	cbType type;
	int iMaxStored;

	DECLARE_MESSAGE_MAP();
};

// container for the commandline (see above).
class IMPEXP CommandBar : public BaseBar
{
	friend class CommandCombo;
public:
	CommandBar();
	~CommandBar();

	BOOL Create(CFrameWnd* pParent, UINT nID, CommandCombo::cbType t);
	void SetCommandLine(const char* str, bool fAddToHistory=false);
	void AddToHistory(const CString& s);
	void Activate();

protected:
	CommandCombo commLine;
	//{{AFX_MSG(CommandBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};


#endif
