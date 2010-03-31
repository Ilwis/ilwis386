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
#ifndef BASE_VIEW2_H
#define BASE_VIEW2_H

#include "Client\MainWindow\DescriptionBar.h"

/*#undef ILWISCLIENT
#ifdef MAIN_EXPORTS
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif*/

class IlwisSettings;

//! The BaseView2 class adds functionality to add and remove menu items to Windows from the Views
class  BaseView2 : public CView
{
public:
	void           SetMenuState(unsigned int iCommand, unsigned int iFlags);
	UINT           GetMenuState(unsigned int iCommand);

	virtual void   LoadState(IlwisSettings& settings);
	virtual void   SaveState(IlwisSettings& settings);

protected:
	 BaseView2();
	 ~BaseView2();

	void     CreateDescriptionBar();
	void     SetDescription(string s);
	string   sGetDescription();

	bool     DeleteMenuItem(CMenu *m, const String sMenus);
	void     DeleteMenuItem(unsigned int iItem);
	void     InsertMenuItem(unsigned iCommand, unsigned int iBefore);

	void     AddToFileMenu(unsigned int iID, unsigned int iType=MF_STRING);
	void     AddToEditMenu(unsigned int iID, unsigned int iType=MF_STRING);
	void     AddToOptionsMenu(unsigned int iID, unsigned int iType=MF_STRING);
	void     AddToHelpMenu(unsigned int iID, unsigned int iType=MF_STRING);
	void     AddToViewMenu(unsigned int iID, unsigned int iType=MF_STRING);

	LRESULT  OnDescriptionChanged(WPARAM wP, LPARAM lP);
	LRESULT  OnViewSettings(WPARAM wP, LPARAM lP);
                    
	DescriptionBar  descBar;
protected:
	int             OnCreate(LPCREATESTRUCT lpcs);
	virtual void    OnInitialUpdate() ;
	virtual void    OnPaste();
	virtual void    OnCopy();
	virtual void    OnCut();
	void            OnUpdateClip(CCmdUI *cmd);

private:
	void            AddMenuItem(CMenu *menu, CMenu *subMenu, const String sItem, 
								unsigned int iType, unsigned int id);
	bool            AppendMenuItem(const String sMenus="", unsigned int iID=-1,
								unsigned int iType=MF_STRING, CMenu *menu=NULL);
	bool            AppendSeparatorAfter(const String sMenus);
	int             iFindMenuString(const CMenu *menu, const String& sStr);
	void            AddToMenu(const String sSubMenu, unsigned int iID, unsigned int iType);
	CMenu           *FindMenu(CMenu *subMenu, const string& sPopUp);
	virtual void    OnDraw(CDC *pdc);

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(BaseView2)
};

#endif
