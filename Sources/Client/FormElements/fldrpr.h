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
/* FieldRepresentation
   by Wim Koolhoven, september 1995
   Copyright Ilwis System Development ITC
	Last change:  WK   14 Mar 97    6:31 pm
*/
   
#ifndef FIELDRPR_H
#define FIELDRPR_H
#include "Engine\Domain\dm.h"

#ifndef OBJLIST_H
#include "Client\FormElements\objlist.h"
#endif

class FieldRepresentation: public FieldDataType
{
public:
  FieldRepresentation(FormEntry* parent, const String& sQuestion, Parm *prm) :
    FieldDataType(parent, sQuestion, prm, ".RPR",true)
  { SetHelpTopic(htpUiRepresentation); }
  FieldRepresentation(FormEntry* parent, const String& sQuestion, String *sName) :
    FieldDataType(parent, sQuestion, sName, ".RPR", true)
  { SetHelpTopic(htpUiRepresentation); }
  FieldRepresentation(FormEntry* parent, const String& sQuestion, String *sName, const Domain& dm) :
    FieldDataType(parent, sQuestion, sName, new RepresentationLister(dm), true)          
  { SetHelpTopic(htpUiRepresentation); }
};

class FieldRepresentationC: public FieldDataTypeC
{
public:
  _export FieldRepresentationC(FormEntry* fe, const String& sQuestion, 
               String* sRpr, const Domain& dom);
  _export void SetDomain(const Domain& dom);
  _export void AllowRprAttr(bool fAllow);
private:  
  Domain dm;
  String sNewName;
  int CreateRepresentation(void*);
};

class FormCreateRepresentation: public FormWithDest
{
public:
	_export FormCreateRepresentation(CWnd* wPar, String* sRpr, const Domain& dom);
private:    
  int exec();
  int CallBackName(Event *);
	int CallBackDomain(Event *);
  FormEntry* feDefaultFocus();
	CheckBox *fcCreateGradual;
  FieldDataTypeCreate* fdr;  
	FieldDataType *fdom;

  String* sRpr;
  String sNewName;
  String sDescr;
  String sDom;
  StaticText* stRemark;
	bool fCreateGradual;
};  

#endif // FIELDRPR_H




