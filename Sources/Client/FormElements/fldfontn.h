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
/* Interface for FieldFontName
   by Wim Koolhoven, may 1998
   (c) Ilwis System Develpment ITC
	Last change:  WK   14 May 98   10:20 am
*/
#ifndef FIELDFONTNAME_H
#define FIELDFONTNAME_H

class FieldFontNameSimple: public FieldOneSelect
{
	friend int FAR PASCAL AddNewFont(const LOGFONT* lplf, const TEXTMETRIC* lptm, unsigned long nFontType, LPARAM lp);

public:
  _export FieldFontNameSimple(FormEntry* parent, String* sFontName, unsigned long iAttr);
  _export ~FieldFontNameSimple();
  void SetVal(const String&);
protected:
  void create();
  void StoreData();
private:
  long iFont;
  String* sFont;
	unsigned long iAttributes;
};

class FieldFontName: public FieldGroup
{
public:
  _export FieldFontName(FormEntry* parent, const String& sQuestion, String* sFontName, unsigned long iAttr=0);
  void SetVal(const String& sFontName)
    { ffns->SetVal(sFontName); }
private:
  FieldFontNameSimple* ffns;
};

class FieldLogFont: public FieldGroup
{
public:
	enum FontAttributes{faTRUETYPE=1, faVECTOR=2, faFIXED=4, faROTATION=8};

  _export FieldLogFont(FormEntry* parent, LOGFONT* lf, 	unsigned long iAttributes=0);
  void StoreData();
private:
	String sFont;
	bool fBold, fItalic;
	long iHeight;
	long iRotation;
	LOGFONT* lf;
};


#endif // FIELDFONTNAME_H





