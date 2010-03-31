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
   $Log: /ILWIS 3.0/FormElements/FieldColumnLister.h $
 * 
 * 5     21/11/00 12:09 Willem
 * Added function ToggleAsKey(int)
 * 
 * 4     13/10/00 15:02 Willem
 * - The font of the ColumnLister is now set to form font
 * - Added extra columns: 'New Domain' (yesno type) and 'Width' (integer)
 * - Field will now call a callback function when a value is changed
 * 
 * 3     30/06/00 15:46 Willem
 * Added function to dynamically size the listctrl
 * 
 * 2     21/06/00 18:15 Willem
 * Improved version of the import table wizard
 * - better column selection handling
 * - scan is done each time the format selection is changed
 * 
 * 1     19/05/00 14:42 Willem
 * Add components for columnlist in import ascii table wizard

   FieldColumnLister.h
   by Willem Nieuwenhuis, 17/5/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_FIELDCOLUMNLISTER_H
#define ILW_FIELDCOLUMNLISTER_H

#include "Engine\DataExchange\TableExternalFormat.h"
#include "Client\FormElements\ColumnListCtrl.h"

class _export FieldColumnLister : public FormEntry
{
public:
	FieldColumnLister(FormEntry* feParent, vector<ClmInfo> &m_colInfo);
	virtual ~FieldColumnLister();

	virtual void create();

	int iRowCount();
	void SetRowCount(int iNrItems);

	ClmInfo& ciColumn(int iCol);
	void SetColWidth(int iCol, int iWidth);
	int iNrCols();
	void ToggleAsKey(int iItem);
	void ToggleSelectedAsKey();

	void CallChangeCallback();

protected:
	void Fill();

private:

	void BuildColumns();

	String m_sName;
	String *m_psName;
	vector<ClmInfo> &m_colInfo;
	ColumnListCtrl m_clctrl;
};

#endif
