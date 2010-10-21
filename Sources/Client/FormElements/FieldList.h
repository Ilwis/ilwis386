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
   $Log: /ILWIS 3.0/FormElements/FieldList.h $
 * 
 * 4     30/06/00 15:47 Willem
 * The listctrl can now handle dynamic resizing
 * Added callback mechanism
 * 
 * 3     28/06/00 12:50 Willem
 * Completed the components
 * 
 * 2     26/06/00 15:51 Willem
 * New component to edit a list
 * 
 * 1     26/06/00 15:40 Willem

   FieldList.h
   by Willem Nieuwenhuis, 26/6/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_FIELDLIST_H
#define ILW_FIELDLIST_H

#include "Client\FormElements\ListCtrlList.h"

// single column list
class _export FieldLister : public FormEntry
{
public:
	FieldLister(FormEntry* feParent, vector<String> &vsList, vector<String>& _columns);
	virtual ~FieldLister();

	virtual void create();

	// access the number of items in the listctrl
	int iRowCount();
	void SetRowCount(int iNrItems);

	// get details of the item container
	String sListItem(int iItem, int iSubItem=0);
	void SetListItem(int iItem, const String& sField);
	int iNrItems();

	void CallChangeCallback();
	void show(int sw);
	void getSelectedIndexes(vector<int>& indexes);
	void setReadOnly(bool yesno) { m_clctrl.setReadOnly(yesno); }

protected:
	void Fill();

private:
	void BuildColumns();

	String m_sName;
	String *m_psName;
	String part;
	vector<String>& m_vsList;
	vector<String>& columns;
	ListCtrlList m_clctrl;
};

#endif
