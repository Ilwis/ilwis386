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
// AcceleratorManager.cpp: implementation of the AcceleratorManager class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning( disable : 4786 )

#include "Client\Headers\AppFormsPCH.h"
#include "Client\MainWindow\AcceleratorManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcceleratorManager::AcceleratorManager()
	: fHandleAccelerator(true)
{
}

AcceleratorManager::~AcceleratorManager()
{
}

void AcceleratorManager::HandleAccelerators(bool fYes)
{
	fHandleAccelerator = fYes;
}

#define sMen(ID) ILWSF("men",ID).c_str()

void AcceleratorManager::AddAccel(CMenu* men, vector<ACCEL>& va)
{
	map<String, int> keycodes;
	keycodes["F1"] = VK_F1;
	keycodes["F2"] = VK_F2;
	keycodes["F3"] = VK_F3;
	keycodes["F4"] = VK_F4;
	keycodes["F5"] = VK_F5;
	keycodes["F6"] = VK_F6;
	keycodes["F7"] = VK_F7;
	keycodes["F8"] = VK_F8;
	keycodes["F9"] = VK_F9;
	keycodes["F10"]= VK_F10;					
	keycodes["F11"]= VK_F11;
	keycodes["F12"]= VK_F12;
	keycodes["Ins"]= VK_INSERT;
	keycodes["Del"]= VK_DELETE;
	keycodes["Enter"]= VK_RETURN;
	keycodes["Home"]= VK_HOME;
	keycodes["End"]= VK_END;

	int iCnt = men->GetMenuItemCount();
	for (int i = 0; i < iCnt; ++i) {
		int id = men->GetMenuItemID(i);
		if (-1 == id) {
			CMenu* menPopup = men->GetSubMenu(i);
			if (menPopup)
				AddAccel(menPopup, va);
		}
		// id == 0: seperator
		else if (id > 0) {
			String s = sMen(id);
			int iPos = s.iPos(8); // ^H
			if (iPos > 0) {
				String sSub = s.substr(iPos+1);
				if (sSub != "") {
					String sLeft = sSub.sLeft(3);
					int iPlus = sSub.iPos('+');
					ACCEL acc;
					acc.cmd = id;
					acc.fVirt = FVIRTKEY;
					acc.key = 0;   // indicate not found

					String sKey = iPlus < 0 ? sSub : sSub.substr(iPlus + 1);
					map<String, int>::iterator result = keycodes.find(sKey);
					if ( result != keycodes.end() )
						acc.key = (*result).second;
					else
						acc.key = sSub[iPlus+1];

					if ("Alt" == sLeft)
						acc.fVirt |= FALT;
					else if ("Ctr" == sLeft)
						acc.fVirt |= FCONTROL;
					else if ("Shf" == sLeft)
						acc.fVirt |= FSHIFT;

					if (acc.key != 0)
						va.push_back(acc);    // only add accelerator if one is found
				}
			}
		}
	}
}

