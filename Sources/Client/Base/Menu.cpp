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
#include "Client\Base\Menu.h"

static const int STRING_NOT_FOUND = iUNDEF;

Menu::Menu() :
  wndOwner(0),
	m_fContextMenu(false)		
{}

Menu::Menu(CWnd *wnd) :
	m_fContextMenu(false)
{
  SetOwner(wnd);
}

Menu::Menu(CWnd *par, CPoint pnt, UINT iFl) :
	m_fContextMenu(true),
	point(pnt),
	iFlags(iFl),
	wndOwner(par)
	
{
	m_Menu.CreatePopupMenu();	
}

Menu::~Menu()
{
	if (m_fContextMenu)
	{
		m_Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, wndOwner);	
		m_Menu.Detach();		
	}		
}

void Menu::SetOwner(CWnd *wnd)
{
  wndOwner = wnd;
}

int Menu::iFindMenuString(const CMenu *menu, const String& sStr)
{
	if (!menu) return STRING_NOT_FOUND;

	String sFindStr = sStr.sHead("&") + sStr.sTail("&");
	int iCount = menu->GetMenuItemCount();
	for ( int i = 0; i < iCount; ++i)
	{
		CString sIt;
		menu->GetMenuString(i, sIt, MF_BYPOSITION);
		String sItem(sIt);
		sItem = sItem.sHead("&") + sItem.sTail("&");
							
		if ( sFindStr == sItem )
			return i;
	}
	return STRING_NOT_FOUND;
}

// searches for a popup menu with the name sPopUp;
CMenu *Menu::FindMenu(CMenu *menu, const string& sPopUp)
{
	if ( !menu ) return NULL;

	int iIndex; 
	if ((iIndex=iFindMenuString(menu, sPopUp)) != STRING_NOT_FOUND)
		return menu->GetSubMenu(iIndex);
	return NULL;
}

// adds a menu item to the menu. If the main menu does not exist it also creates this
void Menu::AddMenuItem(CMenu *menu, CMenu *subMenu, const String sItem,
													 unsigned int iType, unsigned int iID)
{
	if (iType != MF_SEPARATOR)
	{
		if (iFindMenuString(menu,sItem.c_str()) != STRING_NOT_FOUND)
		{
			TRACE("Menu::AddMenuItem: Equal names are not allowed!");
			return;
		}
	}

	if ( menu == NULL) // only true in the case of the mainmenu (adding the first element)
	{
		CMenu mainMenu;
		mainMenu.CreateMenu();
		AddMenuItem(&mainMenu, subMenu,sItem, MF_POPUP, iID);
		if ( m_fContextMenu == false) // for non popup menus
			wndOwner->SetMenu(&mainMenu);
		mainMenu.Detach();
	}
	else 
	{
		if ( iType != MF_POPUP ) {
			menu->AppendMenu(iType, iID, sItem.c_str());
			
		}
		else {
			UINT style = iType & MF_OWNERDRAW ? MF_POPUP | MF_OWNERDRAW : MF_POPUP;
			menu->AppendMenu(MF_POPUP, (UINT)subMenu->GetSafeHmenu(), sItem.c_str());
		}
	}
}

// accepts string of the fromat "[<submenu>,]*[<menuitem>]" and constructs a menu (tree) from it
bool Menu::AppendMenuItem(const String sMenus, unsigned int iID, unsigned int iType, CMenu *menu)
{

	if (sMenus == "") 
		return false; // end of string reached, end of recursion

	if (menu == NULL && m_fContextMenu == false) 
		menu = wndOwner->GetMenu(); // mainmenu, parentFrame->GetMenu() will only return 0 in case of the first append
	else
		if (m_fContextMenu == true )
			menu = &m_Menu;

	CMenu *subMenu=FindMenu(menu, sMenus.sHead(",")); // can we find a submenu of the right name
	if ( subMenu) // if subMenu exists go further down this branch
	{
		if (!AppendMenuItem(sMenus.sTail(","), iID, iType, subMenu)) // recurse further down the tree, false means end of line
 			AddMenuItem(menu, subMenu, sMenus.sHead(","), iType, iID);
 	}
	else // no submenu of the right name found, one has to be created
	{
		if (iType != MF_SEPARATOR)
		{
			if (iFindMenuString(menu,sMenus.sHead(",").c_str()) != STRING_NOT_FOUND) // equal names in one submenu are not allowed
			{
				TRACE("Menu::AddMenuItem: Equal names are not allowed!");
				return false;
			}
		}

		CMenu subMenu;
		subMenu.CreateMenu();
		bool fEndOfString = sMenus.sTail(",") == "";
		if (!fEndOfString ) // menu created, but the menu string still contains items
			AppendMenuItem(sMenus.sTail(","), iID, iType, &subMenu); // recurse further down the chain

		AddMenuItem(menu, &subMenu, sMenus.sHead(","), fEndOfString ? iType : MF_POPUP, iID);

		subMenu.Detach();
	}

	return true;
}	

void Menu::InsertMenuItem(unsigned int iCommand, unsigned int iBefore)
{
	CMenu *menu= m_fContextMenu ? &m_Menu : wndOwner->GetMenu();

	String sText(ILWSF("men",iCommand));

	BOOL iRet=menu->InsertMenu(iBefore, MF_BYCOMMAND, iCommand, sText.c_str());

}

void Menu::Add(const String sSubMenu, unsigned int iID, unsigned int iType)
{
	if ( iType != MF_SEPARATOR)
	{
		String sSubItem(ILWSF("men",iID));
		String sItem;
		if ( sSubMenu != "")
			sItem = String("%S,%S", sSubMenu, sSubItem);
		else 
			sItem = sSubItem;
		try {
			AppendMenuItem(sItem, iID, iType);
		}
		catch (ErrorObject&) {
			// do nothing, sometimes equal items are not allowed message 
			// class Menu seems to be severely broken
		}
	}
	else
	{
		CMenu *menu= m_fContextMenu  ? &m_Menu : wndOwner->GetMenu();
		if (menu)
		{
			CMenu *submenu=FindMenu(menu, sSubMenu);
			if ( submenu )
			{
				int iCount = submenu->GetMenuItemCount();
				CString s;
				submenu->GetMenuString(iCount-1, s, MF_BYPOSITION);
				String sItem=sSubMenu + String(",%s", s);
				AppendSeparatorAfter(sItem);
			}
		}
	}
}
bool Menu::AppendSeparatorAfter(const String sMenus)
{
	return AppendMenuItem(sMenus, -1, MF_SEPARATOR, NULL);
}

void Menu::DeleteMenuItem(unsigned int iItem)
{
	CMenu *menu= m_fContextMenu == true ? &m_Menu : wndOwner->GetMenu();
	menu->DeleteMenu(iItem, MF_BYCOMMAND);
	if ( m_fContextMenu == false )
		wndOwner->DrawMenuBar();
}

bool Menu::DeleteMenuItem(const String& sMenus, CMenu *menu)
{
	if ( menu == 0 )
		menu= m_fContextMenu == true ? &m_Menu : wndOwner->GetMenu(); 
	CMenu *subMenu = FindMenu(menu, sMenus.sHead(","));
	if (!subMenu || !DeleteMenuItem(sMenus.sTail(","), subMenu))
	{
		int iIndex=iFindMenuString(menu, sMenus);
		if ( iIndex != STRING_NOT_FOUND )
			menu->DeleteMenu(iIndex, MF_BYPOSITION);
	}
	if ( !subMenu)
		return false;

	
	return true;
}
