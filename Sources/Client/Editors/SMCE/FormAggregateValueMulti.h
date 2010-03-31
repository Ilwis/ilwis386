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
#ifndef FORMAGGREGATEVALUEMULTI_H
#define FORMAGGREGATEVALUEMULTI_H

#include "Client\FormElements\FieldMultiObjectSelect.h"
#include "Engine\Scripting\CalculateAggregateValue.h"

class FieldAggregateValueFunc;
class FieldMultiAggregateValueSelect;
class AggregateValueResultSelector;
class FormAggregateValueMulti;

struct AggregateValueStruct
{
	AttributeFileName fnMapUsed;
	String sOperation;
	double rAggregateValue;
	String sAttributeColumn;
};


class _export SelectedFMOItem : public FMOSItem
{
public:
	SelectedFMOItem();
	SelectedFMOItem(FMOSItem *item);
	SelectedFMOItem(const AttributeFileName& fn, const String& sCol="", const String& sRes="");
	virtual String sItem() const;
	virtual FMOSItem *Clone();
	virtual bool fEqual(FMOSItem *item);
	void SetAttributeColumn(const String& sCol);
	void SetAggregateResult(const String& sRes);
	String sGetAttributeColumn() const;
	double rGetResult() const;

private:
	String sAttributeColumn;
	String sAggregateResult;
};

struct ThreadStruct
{
	ThreadStruct() {};
	AggregateValueResultSelector	*selector;
	AggregateValueOperationStruct	avos;
	FormAggregateValueMulti *form;
	bool fRunning; // when true, the thread is (still) busy
	bool fAbort; // when this is set on true, thread should quit (return) as soon as possible
};

class _export FormAggregateValueMulti: public FormBaseDialog, public AggregateValueAdditional
{
	friend UINT CalcExpression(LPVOID pParms);
	
public:
	FormAggregateValueMulti(CWnd* parent);
	FormAggregateValueMulti(CWnd* parent, vector<AttributeFileName> *lmpInputMaps, 
		                    vector< AggregateValueStruct >* vrOutputValues, const dmType fd = (dmType)(dmVALUE|dmCLASS), const String& operation="");
	
	~FormAggregateValueMulti();
	void create();
  FormEntry *feDefaultFocus();  

private:
	int exec();
	FormEntry *CheckData();
	int CalculateAggregateValues(Event *);
	int HandleOperationChanges(Event *);
	int HandleChangesInMultiSelect(Event*);
	int HandleAttributeColumnChanges(Event *);
	int Selection(Event *);
	int Init(Event *);
	void OnCancel();

	vector<AttributeFileName>* arrInput;
	vector< AggregateValueStruct >* vrOutputValues;
	vector<AttributeFileName> vfnDummy;
	String sGeoref;
	dmType m_fd;

	FieldMultiAggregateValueSelect *fld;
	FieldString *fsGeoref;
	StaticIcon *ic;
	PushButton *pbCalc;
	ThreadStruct *ts;
	bool fEnableCheckData;
};

class AggregateValueInputSelector : public InputObjectSelector
{
public:
	AggregateValueInputSelector(FormEntry* par, FormAggregateValueMulti *a, ObjectLister *lister);

    virtual void GetSelectedItems(vector<FMOSItem *>& vec );
	String sGetText();
private:
	AggregateValueAdditional *ava;
};

class _export AggregateValueResultSelector: public SelectedObjectsSelector
{
public:
	AggregateValueResultSelector(FormEntry* par, vector<AttributeFileName>& selected);
	~AggregateValueResultSelector();
	void CheckContent(const String& sOperation);
	void InsertAttributeColum(int iIndex, const String& sCol);
	virtual void AddItem(FMOSItem* item);
	String sGetText();
private:
	CFont *fnt;
};

class FieldMultiAggregateValueSelect : public FieldMultiObjectSelectBase
{
public:
	FieldMultiAggregateValueSelect(FormEntry* par, const String& sQuestion, ObjectLister *lister, vector<AttributeFileName>& arrSelected, ThreadStruct** _pts);
	void CalculateAggregateValues(AggregateValueOperationStruct&);
	GeoRef grfGetSelectedGeoref();
	dmType dmtGetSelectedDomainType(const String& sAttribCol);
	Map mpGetSelectedMap();
	void CheckContent(const String& sOperation);
	void SetSelectionFunction(CallBackHandler *cbh, NotifyProc func);	
	void AddAttribColumnToSelectedFile(const String& sAttributeColumn);
	void GetResults(vector< AggregateValueStruct >* vrOutputValues);
	int GetRightCount();
private:
	ThreadStruct** pts;
};

#endif
