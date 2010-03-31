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
/* $Log: /ILWIS 3.0/FormElements/nameedit.h $
 * 
 * 16    20-11-02 17:20 Retsios
 * Merge from SMCE - added minimum treectrl width
 * 
 * 19    20-11-02 17:16 Retsios
 * Added minimum width option for the treectrl - in SMCE the user can
 * determine the width of the combo box, which was previously directly
 * determining the width of the treectrl, which might become too narrow.
 * 
 * 18    19-11-02 15:06 Retsios
 * Merge from Main
 * 
 * 17    15-07-02 18:04 Retsios
 * Added function to expand an attribute table and select a column when
 * the otc is already expanded up to the map with the attribute table
 * (needed for SMCE)
 * 
 * 16    9-07-02 17:48 Retsios
 * Made OpenTreeCtrl and HideTreeCtrl so that other classes can call these
 * directly without having to send a OnLButtonDown message.
 * Made otc protected so that derived classes can access it.
 * Changes were needed for using InPlaceNameEdit in SMCE.
 * 
 * 15    14-11-02 18:37 Retsios
 * Merge from SMCE - Gave some more options for access, added option for
 * expansion up to an attribute column
 * 
 * 17    15-07-02 18:04 Retsios
 * Added function to expand an attribute table and select a column when
 * the otc is already expanded up to the map with the attribute table
 * (needed for SMCE)
 * 
 * 16    9-07-02 17:48 Retsios
 * Made OpenTreeCtrl and HideTreeCtrl so that other classes can call these
 * directly without having to send a OnLButtonDown message.
 * Made otc protected so that derived classes can access it.
 * Changes were needed for using InPlaceNameEdit in SMCE.
 * 
 * 15    14-11-02 18:37 Retsios
 * Merge from SMCE - Gave some more options for access, added option for
 * expansion up to an attribute column
 * 
 * 17    15-07-02 18:04 Retsios
 * Added function to expand an attribute table and select a column when
 * the otc is already expanded up to the map with the attribute table
 * (needed for SMCE)
 * 
 * 16    9-07-02 17:48 Retsios
 * Made OpenTreeCtrl and HideTreeCtrl so that other classes can call these
 * directly without having to send a OnLButtonDown message.
 * Made otc protected so that derived classes can access it.
 * Changes were needed for using InPlaceNameEdit in SMCE.
 * 
 * 14    14-11-00 18:34 Koolhoven
 * added function ResetObjectTreeCtrl()
 * 
 * 13    13/07/00 12:49 Willem
 * - The SetFont function now calculates the correct height for the
 * NameEdit
 * - NameEdit in closed form now also has the context menu
 * 
 * 12    12/07/00 9:47 Willem
 * Added SetFont() function: This will set the font for both the ComboBox
 * as well as for the ObjectTreeCtrl. It will also adjust the size of the
 * text ctrl of the combobox
 * 
 * 11    24/05/00 16:43 Willem
 * - Added protection against a parent FormEntry == 0
 * - Moved message function (mouseup, keydown etc) from private to
 * protected section (needed for InPlaceNameEdit)
 * 
 * 10    24/05/00 9:08 Willem
 * Replaced _export with IMPEXP macro
 * 
 * 9     9-05-00 10:53a Martin
 * attrib columns appear in the tree control. Namedit and objlist have
 * been extended to use this behaviour
 * 
 * 8     2-02-00 17:25 Koolhoven
 * ALT + ArrowDown now also opens the ObjectTreeCtrl
 * 
 * 7     19-10-99 14:30 Koolhoven
 * Allow MapLists as container of raster maps to show in tree list and
 * nameedit
 * 
 * 6     18-10-99 17:54 Koolhoven
 * NameEdit now no longer shows a ListBox when clicked but a TreeCtrl.
 * 
 * 5     12-10-99 18:36 Koolhoven
 * Removed funciton WindowPosChanging()
 * 
 * 4     12-10-99 18:30 Koolhoven
 * BaseNameEdit is no longer derived from BaseZapp
 * 
 * 3     26-08-99 12:47 Koolhoven
 * Header comment
 * 
 * 2     25-08-99 18:59 Koolhoven
 * Drag and drop to NameEdit now works 
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/22 13:56:46  Wim
// BaseNameEdit::SetVal() is not longer inline
// NameEdit::SetVal() added
//
// Revision 1.2  1997-09-22 15:20:44+02  Wim
// added sPath()
//
/* ui/nameedit.h
// Interface for NameEdit
// by Wim Koolhoven, aug. 1993
// (c) Computer Department ITC

// moved to ui by Jelle, replaced zStrings and char * (nov. 1993)
// being changed to ListBox with own Button by Wim (may 1994)
	Last change:  WK   22 Sep 97    3:27 pm
*/
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
			int iWidth);
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




