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
#ifndef BASE_VIEW_H
#define BASE_VIEW_H

#include "Client\MainWindow\DescriptionBar.h"

class IlwisSettings;

//! The BaseView class adds functionality to add and remove menu items to Windows from the Views
class BaseView
{
public:
			/*! SetMenuState() will give a new appearance to the menu
				\param iCommand the ID of the menu item to set the state of
				\param iFlags   determines the appearance of the menu item. This
		                        can be normal, selected, disabled or checked
			*/
	void _export   SetMenuState(unsigned int iCommand, unsigned int iFlags);
			/*! GetMenuState() will return the current appearance of the menu
				\param iCommand the ID of the menu item to get the state of
				\return The current appearance of the menu item. This
		                can be normal, selected, disabled or checked
			*/
	UINT           GetMenuState(unsigned int iCommand);

			/*! LoadState() will retrieve the specific View settings
				\param settings the registry subkey from where to recover the settings
			*/
	virtual void _export  LoadState(IlwisSettings& settings);
			/*! SaveState() will store specific View settings
				\param settings the registry subkey to where the settings are saved
			*/
	virtual void _export  SaveState(IlwisSettings& settings);

protected:
			/*! BaseView() constructor, does set the pView member to the current View
			*/
	_export BaseView();
			/*! ~BaseView() desctructor will do cleanup
			*/
	_export ~BaseView();

			/*! CreateDescriptionBar() will initialize the descriptionbar and set
				the description field with the object description
			*/
	void _export    CreateDescriptionBar();
			/*! SetDescription() Set the object description to a new value
			    \param s the new description
			*/
	void _export    SetDescription(string s);
			/*! sGetDescription() Retrieve the current object description
				\return the object description
			*/
	string _export  sGetDescription();
			/*! DeleteMenuItem() will remove a menuitem from the menu. The item is
				passed as "menu,submenu1,submenu2,menuitem"
				\param m The (top level) menu from which the menu item is to be removed
				\param sMenus The menu item to be removed
				\return true if the item has been removed succesfully
			*/
	bool            DeleteMenuItem(CMenu *m, const String sMenus);
			/*! DeleteMenuItem() will remove a menuitem from the menu
				\param iItem The ID of the menu item to be removed
			*/
	void            DeleteMenuItem(unsigned int iItem);
			/*! InsertMenuItem() will add a new menuitem
				\param iCommand The command identifier of the menu
				\param iBefore The menu item before which the new item is added
			*/
	void  _export      InsertMenuItem(unsigned iCommand, unsigned int iBefore);
			/*! AddToFileMenu() appends a new menu item to the "File" menu
				\param iID The command ID of the menu item
				\param iType The type of the menu item; currently supported are MF_STRING
				             for regular items, and MF_SEPARATOR for separators
			*/
	void _export    AddToFileMenu(unsigned int iID, unsigned int iType=MF_STRING);
			/*! AddToEditMenu() appends a new menu item to the "Edit" menu
				\param iID The command ID of the menu item
				\param iType The type of the menu item; currently supported are MF_STRING
				             for regular items, and MF_SEPARATOR for separators
			*/
	void _export    AddToEditMenu(unsigned int iID, unsigned int iType=MF_STRING);
			/*! AddToOptionsMenu() appends a new menu item to the "Options" menu
				\param iID The command ID of the menu item
				\param iType The type of the menu item; currently supported are MF_STRING
				             for regular items, and MF_SEPARATOR for separators
			*/
	void _export    AddToOptionsMenu(unsigned int iID, unsigned int iType=MF_STRING);
			/*! AddToHelpMenu() appends a new menu item to the "Help" menu
				\param iID The command ID of the menu item
				\param iType The type of the menu item; currently supported are MF_STRING
				             for regular items, and MF_SEPARATOR for separators
			*/
	void _export    AddToHelpMenu(unsigned int iID, unsigned int iType=MF_STRING);
			/*! AddToViewMenu() appends a new menu item to the "View" menu
				\param iID The command ID of the menu item
				\param iType The type of the menu item; currently supported are MF_STRING
				             for regular items, and MF_SEPARATOR for separators
			*/
	void _export    AddToViewMenu(unsigned int iID, unsigned int iType=MF_STRING);
			/*! OnDescriptionChanged() message function called when the description in 
			                           the DescriptionBar changes
				\param wP Contains a pointer to the new description
				\param lP Unused
			*/
	LRESULT _export OnDescriptionChanged(WPARAM wP, LPARAM lP);
	LRESULT _export OnViewSettings(WPARAM wP, LPARAM lP);
                    
			/*! pView member containing a pointer to the View
			*/
	CView*          pView;
			/*! descBar member containing the DescriptionBar
			*/
	DescriptionBar  descBar;

private:
	void            AddMenuItem(CMenu *menu, CMenu *subMenu, const String sItem, 
								unsigned int iType, unsigned int id);
	bool            AppendMenuItem(const String sMenus="", unsigned int iID=-1,
								unsigned int iType=MF_STRING, CMenu *menu=NULL);
	bool            AppendSeparatorAfter(const String sMenus);
	int             iFindMenuString(const CMenu *menu, const String& sStr);
	void            AddToMenu(const String sSubMenu, unsigned int iID, unsigned int iType);
	CMenu           *FindMenu(CMenu *subMenu, const string& sPopUp);
};

#endif
