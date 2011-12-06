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
/* FieldSampleSetC
   by Wim Koolhoven, august 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   26 May 97    4:50 pm
*/
   
#ifndef FIELDMAPSAMPLE_H
#define FIELDMAPSAMPLE_H

class FieldMapList;

class FieldSampleSet: public FieldDataType
{
public:
  _export FieldSampleSet(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".SMS",
                    true)
    {  }
  _export FieldSampleSet(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".SMS",
                    true)
    { }
};

class FieldSampleSetCreate: public FieldDataTypeCreate
{
public:
  _export FieldSampleSetCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".SMS", false)
    {  }

  _export FieldSampleSetCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".SMS", false)
    {  }
};

class FieldSampleSetC: public FieldDataTypeC
{
public:
  _export FieldSampleSetC(FormEntry* fe, const String& sQuestion, 
                 String* sMap)
  : FieldDataTypeC(fe, sQuestion, sMap, ".SMS", true, 
     (NotifyProc)&FieldSampleSetC::CreateSampleSet)              
  { sNewName = *sMap; }
private:  
  String sNewName;
  int _export CreateSampleSet(void*);
};

class FormCreateSampleSet: public FormWithDest
{
public:
	_export FormCreateSampleSet(CWnd* wPar, String* sMap, const String& sMpr = "", const String& sMpl = "", bool createOnly=true);
private:    
	int exec();
	int CallBackName(Event *);
	int ChangeMapList(Event*);
	int showFields(Event *);
	void SetOKButton();
	int checkSMS(Event *);
	
	FieldDataTypeCreate* fdss;
	FieldMapList *m_fml;
	FieldSampleSet *fss;
	StaticText* stRemark;
	CheckBox *cb;
	FormEntry *entry;
	String* sMap;
	String sDescr, sDom, sMapList, sBgMap;
	String m_sDomErr, m_sNameErr;
	bool   m_fOkDomain, m_fOkName;
	bool useExisting;
};

#endif // FIELDMAPSAMPLE_H




