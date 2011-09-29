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
/* Interface for FieldOneSelect
   by Wim Koolhoven, june 1994
   (c) Ilwis System Development ITC
	Last change:  WK    2 Mar 98    6:27 pm
*/
#ifndef FIELDONESEL_H
#define FIELDONESEL_H

#include "Client\FormElements\odlb&cb.h"

class _export OneSelectEdit: public OwnerDrawCB
{
public:
	OneSelectEdit(FormEntry* fe, FormBase* parent, zPoint pos, int id,
					long *v, long sort, int iWidth, bool edit =false);
	~OneSelectEdit();              
	int storeData();
	void MeasureItem(LPMEASUREITEMSTRUCT );
	int DrawItem(Event* dis);
	void SetFont(CFont* pFont, BOOL bRedraw = TRUE);

private:
	int WindowPosChanging(Event *);
	void OnSetFocus(CWnd *);
	void OnKillFocus(CWnd *);
	long *val;
	IlwisAppContext *ilwapp;

	DECLARE_MESSAGE_MAP();
};

class FieldOneSelect: public FormEntry
{
public:
	FieldOneSelect(FormEntry* parent, Parm* parm,
				   long* value, bool fSort = false);
	_export FieldOneSelect(FormEntry* parent,
				   long* value, bool fSort = false, bool edit=false);
	_export                ~FieldOneSelect();

	OneSelectEdit          *ose;
	void _export           DrawItem(Event* dis);
	void _export           SetFocus();
	virtual void _export   SetCallBack(NotifyProc np);
	virtual void _export   SetCallBack(NotifyProc np, CallBackHandler* cb);

//private:
	void _export show(int sw);
//protected:
	virtual void _export   StoreData();
	virtual void _export   create();  // override should call this and then should fill ose
	String _export         sGetText();
	virtual bool _export   DrawObject(long value, DRAWITEMSTRUCT* dis); // 16 x 16, override!
	virtual bool _export   fDrawIcon();
	ItemDrawer idw;
	long *val;
	bool fSort;
	bool edit;
};

class FieldOneSelectStringSimple: public FieldOneSelect
{
public:  
	_export FieldOneSelectStringSimple(FormEntry* parent, long* value, const vector<String>& vs, bool edit=false);
	_export ~FieldOneSelectStringSimple();
  void create();
  void SetFocus();
private:
  const vector<String>& vs;
};

class FieldOneSelectString: public FieldGroup
{
public:
  _export FieldOneSelectString(FormEntry* parent, const String& sQuestion, long* value, const vector<String>& vs, bool edit=false);
	_export ~FieldOneSelectString();
	_export void SetComboWidth(short iWidth);
	_export void SetVal(int iVal);
	_export void SelectVal(const String& sVal);
	_export int iVal();
	_export void resetContent(const vector<String>& vs);
  virtual void _export SetCallBack(NotifyProc np); 
  virtual void _export SetCallBack(NotifyProc np, CallBackHandler* cb);
  virtual void SetFocus() { if ( foss) foss->SetFocus(); }
private:
  FieldOneSelectStringSimple* foss;
};



#endif FIELDONESEL_H




