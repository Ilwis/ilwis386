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
#ifndef RPRLISTVIEW_H
#define RPRLISTVIEW_H

class InPlaceNameEdit;

class RprListView : public CListCtrl, public BaseZapp
{
public:
	RprListView(FormEntry*, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, map<String, String>& defRpr);

private:
	void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);

	InPlaceNameEdit *ne;
	FormEntry *par;

	map<String, String> &defRpr;

  DECLARE_MESSAGE_MAP();

};


class FieldRprListView: public FormEntry
{
public:
  _export ~FieldRprListView();
  _export FieldRprListView(FormEntry* par, map<String, String> &_rprmap);
  void _export show(int);
  virtual void StoreData() ;
  void setDirty() { rprList->Invalidate(); }
	bool _export fValid();

protected:
  void _export create();
  void SetFocus() { rprList->SetFocus(); } // overriden
  virtual String sName(int id) { return String();}
  RprListView *rprList;
	map<String, String> &defRpr;
	map<String, String> tempRpr;
};

#endif;
