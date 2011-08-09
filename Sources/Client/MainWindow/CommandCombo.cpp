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
#include "Headers\messages.h"
#include "Engine\Base\AssertD.h"
#include "Client\ilwis.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Client\MainWindow\Catalog\CatalogFrameWindow.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\mainwind.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Hs\Mainwind.hs"


BEGIN_MESSAGE_MAP( CommandBar, BaseBar )
	//{{AFX_MSG_MAP( CommandBar )
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


//--[ CommandBar ]-------------------------------------------------------------------
CommandBar::CommandBar() 
{
}

CommandBar::~CommandBar()
{
}


BOOL CommandBar::Create(CFrameWnd* pParent, UINT nID, CommandCombo::cbType t ) 
{
	ASSERT_VALID(pParent);	// must have a parent

	if (!BaseBar::Create(pParent, nID))
		return FALSE;

	commLine.Create(this, t);
	iHeight = commLine.GetItemHeight(-1) + 10;
	ASSERT_VALID(this);

	// caption during floating
	SetWindowText("Command Line");

	return TRUE;
}

void CommandBar::OnSize(UINT nType, int cx, int cy) 
{
	BaseBar::OnSize(nType, cx, cy);
	if (!IsWindow(commLine.m_hWnd))
		return;
	CDC *pdc = commLine.GetDC();
	CSize sz = pdc->GetTextExtent("gk", 2);
	commLine.ReleaseDC(pdc);
	commLine.MoveWindow(0, 0, cx, sz.cy * 10);
}

void CommandBar::Activate()
{
	if ( IsWindow(commLine.m_hWnd))
		commLine.SetFocus();
}

void CommandBar::AddToHistory(const CString& str)
{
	if (str.GetLength() > 0)
		if (str[0] != '*')
			commLine.AddToHistory(str);
}


//--[ CommandCombo ]---------------------------------------------------------------------
BEGIN_MESSAGE_MAP( CommandCombo, CComboBox )
	//{{AFX_MSG_MAP( CommandCombo )
	ON_MESSAGE(ILWM_SETCOMMANDLINE, OnSetCommandLine)
	ON_CONTROL_REFLECT(CBN_KILLFOCUS, OnKillFocus)
	ON_CONTROL_REFLECT(CBN_SETFOCUS, OnSetFocus)
	ON_WM_ACTIVATE()
	ON_WM_MOUSEMOVE()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CommandCombo::CommandCombo() 
{
}

CommandCombo::~CommandCombo()
{}

BOOL CommandCombo::Create(CWnd* pParentWnd, cbType t, UINT nID, DWORD dwStyle)
{
	CRect rct, rect;
	BOOL fRet=CComboBox::Create(dwStyle, CRect(0,0,150,25), pParentWnd, nID);
	ISTRUE(fINotEqual, fRet, FALSE);
	type = t;

	String sEntry = type == cbMain ? "MainComLine" : "TableComLine";

	IlwisSettings settings(sEntry);
	IlwisSettings settings2("DefaultSettings");
	iMaxStored = settings2.iValue("HistoryEntries", 8);
	for (int i=0; i< iMaxStored; ++i)
	{
		String s = settings.sValue(String("String%d", i));
		if ( s != "?")
			AddString(s.c_str());
	}

	return fRet;
};

void CommandCombo::OnMouseMove(UINT nFlags, CPoint point) 
{
	CComboBox::OnMouseMove(nFlags, point);
	MainWindow *mw = dynamic_cast<MainWindow *>( AfxGetMainWnd()) ;
	if (mw)
		mw->SetStatusLine(TR("Type an expression on the command line"));
}

void CommandCombo::AddToHistory(const CString& str)
{
	String sEntry = type == cbMain ? "MainComLine" : "TableComLine";
	IlwisSettings settings(sEntry);
	InsertString(0,str);
	int iCount = min(GetCount(), iMaxStored);
	for ( int i =0; i<iCount; ++i)
	{
		CString s;
		GetLBText(i, s);
		settings.SetValue(String("String%d", i), String(s));
	}

	if (GetCount() > 30) // userdefine in future ??
		DeleteString(30);

}

void CommandCombo::OnKillFocus()
{
	CommandBar *bar = dynamic_cast<CommandBar *>(GetOwner());
	if (bar) 
	{
		AcceleratorManager* am = dynamic_cast<AcceleratorManager*>(bar->wndParent);
		if (am)
			am->HandleAccelerators(true);
	}
}

void CommandCombo::OnSetFocus()
{
	CommandBar *bar = dynamic_cast<CommandBar *>(GetOwner());
	if (bar) 
	{
		AcceleratorManager* am = dynamic_cast<AcceleratorManager*>(bar->wndParent);
		if (am)
			am->HandleAccelerators(false);
	}
}

BOOL CommandCombo::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam) 
		{
			case VK_RETURN:
			{
				CString str;
				GetWindowText(str);
				if (str.IsEmpty())
					return TRUE;

				GetTopLevelParent()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)(LPCSTR)str);
				// fall through
			}
			case VK_ESCAPE:
				if ( ::IsWindow(m_hWnd)) {
					ShowDropDown(FALSE);
					SetWindowText("");
				}
				return TRUE;
			case VK_UP:
				if (!GetDroppedState())
					pMsg->wParam = VK_DOWN;
				return FALSE;
			case VK_DOWN:
				if (!GetDroppedState())
					pMsg->wParam = VK_UP;
				return FALSE;
		}
	}
	return CComboBox::PreTranslateMessage(pMsg);
}

LRESULT CommandCombo::OnSetCommandLine(WPARAM wPar, LPARAM)
{
	String *s = (String *)wPar;
	for(string::iterator cur = s->begin(); cur != s->end();)
	{
		if (*cur == '\n')
			s->erase(cur);
		else
			++cur;

	}
	if (s != NULL)
		SetWindowText(s->c_str());

	return 1;
}
	 
void CommandBar::SetCommandLine(const char* str, bool fAddToHistory)
{
	String sCmd(const_cast<char *>(str)); // else the wrong constructor is chosen and it will go wrong on strings like %e
	// erase \n and \r from the line
	for(string::iterator cur = sCmd.begin(); cur != sCmd.end();)
	{
		if (*cur == '\n' || *cur == '\r')
			sCmd.erase(cur);
		else
			++cur;

	}
	commLine.SetWindowText(sCmd.c_str());
	if ( fAddToHistory)
	{
		commLine.AddToHistory(CString(sCmd.c_str()));
		commLine.logger.LogLine(String(sCmd.c_str()));
	}
}

void CommandCombo::OnActivate( UINT nState, CWnd* pWndOther, BOOL bMinimized )
{
}
