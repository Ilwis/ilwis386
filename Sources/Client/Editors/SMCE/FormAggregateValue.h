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
#ifndef FORMAGGREGATEVALUE_H
#define FORMAGGREGATEVALUE_H


class FieldAggregateValueFunc;
class FieldMultiAggregateValueSelect;
class FieldColumn;

#include "Client\FormElements\objlist.h"
#include "Client\FormElements\fldcol.h"

class FieldColumnWithNone : public FieldColumn
{
public:
	FieldColumnWithNone(FormEntry*, const String& sQuestion, const Table&, String *psName, long types = 0);
	void create();
  virtual void FillWithColumns(const FileName& fnTbl=FileName());
  virtual void FillWithColumns(const FileName& fnTbl, const Domain& dmCol);
  virtual void FillWithColumns(const FileName& fnTbl, long types);  

  void SelectNone();
  void StoreData();
};

class _export AggregateValueAdditional
{
public:
	AggregateValueAdditional();
	String sGetAttributeColumn();
protected:
	void SetAdditionalBlock(FormEntry *above, FormEntry* feAlingUnder, bool fMulti);
	
	String sOperation;
	double rPower;
	String sBooleanExpression;
	int iBooleanChoice;
	bool fAdditional;
	String sMapName;
	String sAttributeColumn;

	FieldReal *frPower;
	RadioGroup* rgBoolean;
	FieldAggregateValueFunc *favf;
	FieldString *fsExpression;
	FieldMap *fmMapBoolean;
	FieldMap *fmMapWeight;
	CheckBox *cbAdditional;
	FieldColumnWithNone *fldCol;	

};

class FieldAggregateValueFuncSimple: public FieldOneSelect
{
public:
  FieldAggregateValueFuncSimple(FormEntry* par, String* sFunc, const dmType fd, const String& sDefault);
  void create(); 
  void StoreData();
protected:
private:
	long iSelected;
	String *sSelectedFunc;
	String sDefault;
	dmType m_FunctionDomain;
};

class _export FieldAggregateValueFunc: public FieldGroup
{
public:
  FieldAggregateValueFunc(FormEntry* parent, const String& sQuestion, String* sFunc, const dmType fd = (dmType)(dmVALUE|dmCLASS), const String& sDefault = "");
  virtual void SetCallBack(NotifyProc np);
  virtual void SetCallBack(NotifyProc np, CallBackHandler* cb); 
private:

 // int StoreData();

  FieldAggregateValueFuncSimple* fafs;
};
#endif
