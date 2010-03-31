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
/*-----------------------------------------------
   $Log: /ILWIS 3.0/FormElements/FilenameLister.cpp $
 * 
 * 4     8/03/01 16:42 Willem
 * - Added function to retrieve all selected items
 * - Added style parameter to constructor
 * 
 * 3     27/02/01 14:34 Willem
 * Added functionality to be able to also handle internal bands in
 * maplists properly
 * 
 * 2     1/02/01 11:24 Willem
 * - The OwnerDrawListbox now also stores the strings (added
 * LBS_HASSTRINGS)
 * - The FileNameLister enables RMB in OwnerDrawListBox
 * 
 * 1     11/01/01 17:05 Willem
 * Added new class FilenameLister derived from selector to display
 * filenames with icons

   FilenameLister.cpp
   by Willem Nieuwenhuis, 1/11/01
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\FilenameLister.h"

FilenameLister::FilenameLister(FormEntry* fe, Array<String>& as, DWORD styles)
	: BaseSelector(fe), m_as(as)
{
	  style = styles | WS_VSCROLL | LBS_HASSTRINGS;
}

void FilenameLister::StoreData()
{
}

void FilenameLister::create()
{
	// The StringArray contains filenames in two forms:
	// 1. Filename
	// 2. Display_name|Filename  (used for Maplist bands)
	//    The actual Filename is used by the contextmenu (started from OwnerDrawListBox lb)
	// If no display name is available the Filename is the display name
	BaseSelector::create();
	lb->SetShowPopup(true);
	lb->ResetContent();
	// Add all filename strings; these however are not the ones being displayed
	// The OwnerDrawn listbox calls sName() for this purpose.
	for (int i = 0; i < m_as.iSize(); ++i)
		lb->AddString(m_as[i].scVal());
}

String FilenameLister::sName(int id)
{
	return m_as[id].sHead("|");
}

String FilenameLister::sGetText()
{
	String sText;
	for (int i = 0; i < m_as.size() ; ++i) 
	{
		sText &= m_as[i].sHead("|");
		sText &= "\r\n";
	}
	return sText;
}

void FilenameLister::GetSelectedNames(Array<String>& asSelected)
{
	asSelected.clear();
	int iCount = lb->GetCount();
	if (iCount == 0)
		return;

	int* piSelIDList = new int[iCount];
	lb->GetSelItems(iCount, piSelIDList); // get ID's of all selected names

	for (int i = 0; i < iCount; i++)
	{
		int iSelID = piSelIDList[i];
		String sName = m_as[iSelID].sTail("|");
		asSelected &= sName.length() > 0 ? sName : m_as[iSelID];
	}

	delete [] piSelIDList;
}
