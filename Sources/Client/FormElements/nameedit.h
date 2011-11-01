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

#ifndef NAMEEDIT_H
#define NAMEEDIT_H

#include "Engine\Table\tbl.h"

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class BaseNameEdit;
class NameEdit;
class ColumnNameEdit;
class FormEntry;
class Table;
//class DATEXPORT TableView;
class DragEvt;
class ObjectLister;
class ObjectTreeCtrl;

class IMPEXP BaseNameEdit: public CComboBox //, public BaseZapp
{
public:
  BaseNameEdit(FormEntry* fe, FormBase* parent, CPoint pos, int id, bool fExist, int iWidth, unsigned int iStyle=0);
  ~BaseNameEdit();

  void          SetVal(const String& sVal);
  virtual int   CheckData()=0;
  void          DrawItem(DRAWITEMSTRUCT* dis);
  void          OnKillFocus( CWnd* pNewWnd );
  long          OnGetDlgCode(WPARAM wpar, LPARAM lpar);
  void          OnSelChange();
  void          OnDblClick();
  void          OnClick();
  void          OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
  void          OnSelEndOk();
  void          OnCloseUp();
//  int           CloseUp(Event *);
//  int           EndOk(Event *);
//  int           WindowPosChanging(Event *);
  void          MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
  String        sName() ;
                  
protected:
  virtual void  OnSetFocus(CWnd *pOld);
  bool          fExist;
  bool          fEndOk;
  String        _sName;
	String        sMap;
	String        sAttrib;
  ItemDrawer    idw;
  IlwisAppContext *ilwapp;
  FormEntry     *_fe;
  FormBase      *parent;

  DECLARE_MESSAGE_MAP()
};

class NameEditDropTarget;
class COleDataObject;

class IMPEXP NameEdit: public BaseNameEdit
{
	friend class ObjectTreeCtrl;
public:
	NameEdit(FormEntry* fe, FormBase* parent, CPoint pos, int id,
			const String& s, bool fExist, ObjectLister*, bool fExt,
			int iWidth, bool showFull=true);
	~NameEdit();
	void DrawItem(DRAWITEMSTRUCT* dis);
	virtual int CheckData();
	void SetVal(const String& sVal);
	void SetVal(const FileName& fn);
	void SetObjLister(ObjectLister* objl) ; // deletes old one!
	void SetFont( CFont* pFont, BOOL bRedraw = TRUE );

	int GetChar(void *);
	void FillDir();
	const char* sPath() const { return sDir; }
	void addDir(int f,char* s);
	BOOL OnDrag(const FileName&);
	BOOL OnDrop(const FileName&);
	void OnDropDown();
	void SetAttribColumnDomainTypes(long dmTypes);
	void ResetObjectTreeCtrl();
	void OpenTreeCtrl();
	void HideTreeCtrl();
	void SelectAttribColumn(const FileName& fnMapWithCol);
	void SetMinTreeWidth(int iWidth);
protected:
	LRESULT OnShowDropDown(WPARAM, LPARAM);
	afx_msg void OnLButtonDown(UINT, CPoint);
	afx_msg void OnLButtonDblClk(UINT, CPoint);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu( CWnd* pWnd, CPoint point );
	ObjectTreeCtrl* otc;
private:

	bool fExt;
	char sDir[MAXPATH];
	ObjectLister* ol;
	NameEditDropTarget* nedt;
	long dmTypes;
	int iMinTreeWidth;
	bool fullTree;

	DECLARE_MESSAGE_MAP();
};


class IMPEXP ColumnNameEdit: public BaseNameEdit
{
public:
  ColumnNameEdit(FormEntry* fe, FormBase* parent, zPoint pos, int id,
                 const String& s, Table* tbl, int iWidth);
//  ColumnNameEdit(FormEntry* fe, zWindow* parent, zPoint pos, int id,
//                 const String& s, TableView* tvw, int iWidth);
  virtual int CheckData();
}; 

#endif // NAMEEDIT_H




