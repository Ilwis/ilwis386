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
/* $Log: /ILWIS 3.0/FormElements/flddat.h $
 * 
 * 6     16/03/01 9:18 Willem
 * The fill() function now tries to remember and restore the selected
 * object
 * Added SelectExact() function for this purpose.
 * 
 * 5     20/11/00 13:06 Willem
 * Added fIcons flag to FieldDataTypeLarge constructor
 * 
 * 4     18-06-99 9:44a Martin
 * added _export
 * 
 * 3     3/31/99 12:18p Martin
 * Wrong DrawItem parameter transferred, now Event parm
 * 
 * 2     26-03-99 18:04 Koolhoven
 * DrawItem() changed
// Revision 1.2  1998/02/09 14:15:19  Wim
// Added member fAlsoSystemDir
// and function SetNoSystemDir()
//
/* FieldDataTypeLarge
   by Wim Koolhoven, october 1994
  (c) Ilwis System Development ITC
*/
#ifndef FORMDATATYPE_H
#define FORMDATATYPE_H

class FieldDataTypeLarge: public FormEntry
{
public:
  _export FieldDataTypeLarge(FormEntry*,
                     String *psName, const String& sExt, bool fIcons = true);
  _export FieldDataTypeLarge(FormEntry*,
                     String *psName, const String& sExt, ObjectLister*, bool fIcons = true);
  _export ~FieldDataTypeLarge();             
  void _export SetExt(const String& sExt);
  void SetOnlyEditable() { fOnlyEditable = true; }
  void SetNoSystemDir() { fAlsoSystemDir = false; }
  void create();                // create entry
  virtual FormEntry* CheckData();       // validate entry value
  void StoreData();		// store result in destination
  void show(int);
  void SetFocus() { lbObject->SetFocus(); } // overriden
  void CheckDrawItem(DRAWITEMSTRUCT* dis);
  void DrawItem(Event* ev);
  void SelectExact(const String& sSearch);
  void useBaseMaps(bool yesno);
  void Fill();

protected:
  String sDir;
  bool fromBaseMaps;
  String sName, *_psName;              // local storage and destination
  OwnerDrawListBox* lbObject;

private:
  Array<String*> asExt;
  void FillDir();
  void FillDrive();
  int DblClkObject(void *);
  int DblClkDir(void *);
  int CloseDrive(void *);
  ItemDrawer idw;
  CStatic* st;
  OwnerDrawListBox* lbDir;
  OwnerDrawCB* cbDrive;
  bool fIcons, fOnlyEditable, fAlsoSystemDir;
  ObjectLister* ol;
};    

class _export FieldDataObject: public FieldGroup
{
public:  
  FieldDataObject(FormEntry*, String *psName);
  void create();                // create entry
  void SetOnlyEditable() { fdtl->SetOnlyEditable(); }
  void SetNoSystemDir()  { fdtl->SetNoSystemDir(); }
private:
  int TypeChanged(void *);
  FieldOneSelect* fos;
  FieldDataTypeLarge* fdtl;
  long iType;
};

class _export DataObjectForm: public FormWithDest
{
public:
  DataObjectForm(CWnd* parent, const String& sTitle, String* sName, 
                 const String& htp);
  DataObjectForm(CWnd* parent, const String& sTitle, String* sName, 
                 const String& sExt, const String& htp);
};

  
#endif // FORMDATATYPE_H





