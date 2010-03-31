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

#ifndef FIELDMULTIOBJECTSELECT_H
#define FIELDMULTIOBJECTSELECT_H

#include "Client\FormElements\selector.h"

class ObjectSelectorBase;
class SelectedObjectsSelector;
class InputObjectSelector;
class ObjectSelectorBase;

class _export AttributeFileName : public FileName
{
public:
	AttributeFileName(FileName& PFile) ;
	AttributeFileName( const AttributeFileName& PFile) : FileName(PFile.sFullPath())
	{
		sCol = PFile.sCol;		
	}		
	AttributeFileName() : FileName()
	{}	

	AttributeFileName(const String& s) : FileName(s)
	{}

	AttributeFileName(char *s) : FileName(s)
	{}		
};

class _export FMOSItem
{
public:
	FMOSItem(const FMOSItem& it);
	FMOSItem(const AttributeFileName& fn);
	FMOSItem();
	AttributeFileName fnItem() const;
	virtual String sItem() const;
	virtual FMOSItem *Clone();
	virtual bool fEqual(FMOSItem *item);
private:
	AttributeFileName fnObject;
};

class _export FMOSItems : public vector<FMOSItem *>
{
public:
	void CleanUp();
};

class _export ObjectSelectorBase: public BaseSelector
{
public:
  ~ObjectSelectorBase();
  virtual FMOSItem *fiGetItem(int id);
  String sName(int id);
  void SetSelectedItem(const FMOSItem& item);
  bool fIsItemSelected(int id);
  virtual void GetSelectedItems(vector<FMOSItem *>& vec );
  void SetSelectionFunction(CallBackHandler *cbh, NotifyProc func); 
  virtual void AddItem(FMOSItem* );
  void ChangeItem(int iIndex, FMOSItem *item);
  void DeleteItem(int iIndex);
  int iGetCount();
  void ResetContent();
  void create();
protected:
  ObjectSelectorBase(FormEntry* par);
  void StoreData(); // empty implementation!
  NotifyProc npSelection;
  CallBackHandler *cbh;
  FMOSItems vfiItems;
};

class _export FieldMultiObjectSelectBase: public FieldGroup
{
public:
  FieldMultiObjectSelectBase(FormEntry* par, const String& sQuestion, ObjectLister* ol, vector<AttributeFileName>& selected);
  void create();
  void SetObjectLister(ObjectLister *);
  String sGetText();

protected:
  SelectedObjectsSelector* ossSelected;
  InputObjectSelector* mosInput;  
  FieldGroup *fgLeft;
  FieldGroup *fgRight;
  FieldGroup *fgMiddle;
  FieldGroup *fgAll;
  FieldBlank* fb;
  StaticTextSimple* sts;
  PushButton* pbAdd;
  PushButton* pbDel;
private:
  int Add(Event*);
  int Del(Event*);

};

class _export SelectedObjectsSelector: public ObjectSelectorBase
{
public:
	SelectedObjectsSelector(FormEntry* par, vector<AttributeFileName>& selected);
	~SelectedObjectsSelector();
	void create();
	void StoreData();
//	int ChangeItem(int iIndex, FMOSItem item);
protected:
	void SetFont(CFont *font);	

private:
	CFont *fnt;
	vector<AttributeFileName>& arrSelected;
};

class _export InputObjectSelector: public ObjectSelectorBase
{
public:
  InputObjectSelector(FormEntry* par,ObjectLister *lister);
  ~InputObjectSelector();
  void SetObjectLister(ObjectLister *lister);
  void create();
private:
  bool fOK(const AttributeFileName& fn);
  void FillListBox();
  ObjectLister *olLister;
};

class _export FieldMultiObjectSelect:  public FieldMultiObjectSelectBase
{
public:
	FieldMultiObjectSelect(FormEntry* par, const String& sQuestion, ObjectLister *lister, vector<AttributeFileName>& arrSelected);

};
#endif
