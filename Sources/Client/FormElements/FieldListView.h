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
#ifndef ILW_FieldListView_H
#define ILW_FieldListView_H

#include "Engine\DataExchange\TableExternalFormat.h"
#include "Client\FormElements\ColumnListCtrl.h"

class FieldListView;

struct _export FLVColumnInfo {
	FLVColumnInfo(String name, int w) : columnName(name), width(w) {}
	String columnName;
	int width;
};

class _export FLVColumnListCtrl: public CListCtrl {
public:
	FLVColumnListCtrl();
	void SetParent(FieldListView *view);

private:
	FieldListView *parentFormEntry;
	afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
	void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP();
};

class _export FieldListView : public FormEntry
{
public:
	FieldListView(FormEntry* feParent, const vector<FLVColumnInfo>& m_colInfo, long _extraStyles = 0);
	virtual ~FieldListView();

	virtual void create();

	int iRowCount();
	void SetRowCount(int iNrItems);

	void SetColWidth(int iCol, int iWidth);
	int iNrCols();
	 void show(int);
	 char *item(int row, int col);
	 void AddData(const vector<String>& v);
	 void setData(int row, const vector<String>& v);
	 void update();
	 void getSelectedRowNumbers(vector<int>& rowNumbers) const;
	 void setSelectedRows(vector<int>& rowNumbers);
	 void clear();

	//void ToggleAsKey(int iItem);
	//void ToggleSelectedAsKey();

	void CallChangeCallback();

protected:
	void Fill();

private:

	void BuildColumns();

	String m_sName;
	String *m_psName;
	vector<FLVColumnInfo> m_colInfo;
	vector<vector<String> > data;
	FLVColumnListCtrl m_clctrl;
	long extraStyles;
};

#endif
