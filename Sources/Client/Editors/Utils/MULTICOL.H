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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/MULTICOL.H $
 * 
 * 3     25-05-00 12:25p Martin
 * complex pointmap display works again
 * 
 * 2     7-06-99 17:21 Koolhoven
 * No .cpp file yet
 * 
 * 1     4-06-99 12:17 Koolhoven
// Revision 1.1  1998/09/16 17:32:28  Wim
// 22beta2
//
/* MultiColumnSelector, FieldMultiColumn
   by Wim Koolhoven, feb. 1998
  (c) Ilwis System Development ITC
	Last change:  WK   29 Apr 98   11:10 am
*/

#ifndef ILWMULTICOL_H
#define ILWMULTICOL_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

struct IMPEXP ColumnAndColor
{
  Column col;
  Color clr;
};

typedef Array<ColumnAndColor> ColumnAndColorArray;

class IMPEXP GeneralColumnSelector;
class IMPEXP MultiColumnSelector;
class IMPEXP ColumnAndColorSelector;
class IMPEXP FieldMultiColumn;

class IMPEXP GeneralColumnSelector: public BaseSelector
{
public:
  String sName(int id);
  void SetSel(int id, bool fSel);
  void SetSel(const String&, bool fSel);
  bool fSel(int id);
  int iGetSelected(IntBuf& buf);
protected:
  GeneralColumnSelector(FormEntry* par, TablePtr* tbl);
  void DrawItem(Event* dis);
  TablePtr* tbl;
  void StoreData(); // empty implementation!
};

class IMPEXP MultiColumnSelector: public GeneralColumnSelector
{
public:
  MultiColumnSelector(FormEntry* par, TablePtr* tbl, long types);
  void create();
private:
  long dmTypes;
};

class _export ColumnAndColorSelector: public GeneralColumnSelector
{
public:
  ColumnAndColorSelector(FormEntry* par, TablePtr* tbl, ColumnAndColorArray& arr, bool fColor=true);
  ~ColumnAndColorSelector();
  void create();
  FormEntry* CheckData();
  void StoreData();
  void ShowColor(bool fColor);
  String sName(int id);
  int idActive();
  int Add(const Column&);
  void Del(int id);
  int Del(const Column&);
  int iCols() const;
  Column col(int id) const;
  Color clr(int id) const;
  void SetColor(int id, Color clr);
  int ChangeColor(Event*);
	void CleanUp();
protected:
  void DrawItem(Event* dis);
private:
  int mouseButtonDown(Event *);
  int mouseButtonUp(Event *);
  int mouseMove(Event *);
  bool fDrag;
  int idDrag;
  zCursor *cur, curArrow;
  int iDfltCol;
  bool fColor;
  ColumnAndColorArray& arr;
};

class IMPEXP FieldMultiColumn: public FieldGroup
{
public:
  FieldMultiColumn(FormEntry* par, const String& sQuestion, TablePtr* tbl, ColumnAndColorArray& arr, long types, bool fColor=true);
  void ShowColor(bool fColor);
  void create();
	void CleanUp();
private:
  int Add(Event*);
  int Del(Event*);
  int ChangeColor(Event*);
  bool fColor;
  TablePtr* tbl;
  MultiColumnSelector* mcs;
  ColumnAndColorSelector* cacs;
  PushButton* pbColor;
};

#endif // ILWMULTICOL_H

