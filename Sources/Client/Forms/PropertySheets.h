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
#if !defined(AFX_PROPERTYSHEETS_H__326B98BD_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
#define AFX_PROPERTYSHEETS_H__326B98BD_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class BasicPropertyFormPage: public FormBasePropertyPage
{
public:
	BasicPropertyFormPage(const IlwisObject& object, const String& sPageTitle);
	virtual void create();

	String sObjectDesc(const IlwisObject& obj);

protected:
	virtual void BuildPage();
	void         DisplayDefinition();  // put the definition field on the current tab

	StaticText         *m_stRemark;

	FieldGroup         *m_fgPageRoot;
	FlatIconButton     *m_fib;
	int PressButton(Event*);

	const IlwisObject  &m_obj;
	bool               m_fReadOnly;
};

// Definitions for all possible Property pages
class GeneralPropPage: public BasicPropertyFormPage
{
public:
	GeneralPropPage(const IlwisObject& object);
	virtual void BuildPage();

private:
	int exec();

	FieldString *m_fsDesc;
};

class BaseMapPropPage: public BasicPropertyFormPage
{
public:
	BaseMapPropPage(const IlwisObject& object, const String& sPageTitle);

	virtual void BuildPage();

	virtual void SetCoordSystemField();
	virtual void SetDomainValueRangeField();
	virtual void SetAttribTableField();
	virtual void SetMapDetails();

protected:
	FieldTableC       *ftAttTable;
	StaticText        *stAttTable;
	FieldCoordSystemC *m_fcs;
	FieldDomainC      *m_fdm;
	FieldGroup        *m_fgValues;
	FieldValueRange   *m_fvr;
	FormEntry         *m_feAlignUnder; // placeholder for correct alignment

	StaticText *m_stCS;
	StaticText *m_stCSDsc;
	StaticText *m_stDM;
	StaticText *m_stValues;
	StaticText *m_stPrecision;
	StaticText *m_stUnit;

	int CoordSysChange(Event*);
	int DomainChange(Event*);
	int CallBackAttTableChange(Event*);
	int EditDefinition(Event*);

	int exec();

	bool        m_fFirst;
	BaseMap     m_bm;
	ValueRange  m_vr;

	bool   fAttrTable;
	String sAttrTable;
	String sExpr;
	String m_sNewCS;
	String m_sNewDM;
};

class MapPropPage: public BaseMapPropPage
{
public:
	MapPropPage(const IlwisObject& ptr);

	virtual void SetCoordSystemField();
	virtual void SetDomainValueRangeField();
	virtual void SetMapDetails();

protected:
	FieldGeoRefC      *m_fgr;
	StaticText        *m_stGRDsc;
	StaticText        *m_stPyramids;
	PushButton        *pbPyramids;

	int exec();
	
	void StoreTypeInfo(StoreType st) const;
	void UpdateValueRangeFields();

	int ValueRangeCallBack(Event*);
	int DoChangeValueRange(Event*);
	int GeoRefChange(Event*);
	int EditDefinition(Event*);
	int CallBackPyramids(Event*);	

	int    m_iInterPolMethod;
	bool   m_fInterpol;
	bool   m_fInGRCallBack;
	String m_sNewGR;
};

class PointMapPropPage: public BaseMapPropPage
{
public:
	PointMapPropPage(const IlwisObject& ptr);

	virtual void SetMapDetails();
};

class PolygonMapPropPage: public BaseMapPropPage
{
public:
	PolygonMapPropPage(const IlwisObject& ptr);

	virtual void SetMapDetails();
};

class SegmentMapPropPage: public BaseMapPropPage
{
public:
	SegmentMapPropPage(const IlwisObject& ptr);

	virtual void SetMapDetails();
};

class TablePropPage: public BasicPropertyFormPage
{
public:
	TablePropPage(const IlwisObject& tbl);

	virtual void BuildPage();
	int exec();

private:
	String query;
};

class Table2DimPropPage: public BasicPropertyFormPage
{
public:
	Table2DimPropPage(const IlwisObject& tbl);

	virtual void BuildPage();
};

class HistogramPropPage: public BasicPropertyFormPage
{
public:
	HistogramPropPage(const IlwisObject& obj);


	virtual void BuildPage();
};

class AdditionalInfoPage: public BasicPropertyFormPage
{
public:
	AdditionalInfoPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	String sInfo;
};

class GeoRefPropPage: public BasicPropertyFormPage
{
public:
	GeoRefPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	int exec();

	FieldDataType	  *m_fm;
	FieldCoordSystemC *m_fcs;

	StaticText *m_stCS;
	StaticText *m_stCSDsc;

	int RefMapChange(Event*);
	int CoordSysChange(Event*);

	GeoRefCTP  *gctp;
	String     m_sNewCS;
	String     sRefMap;
};

class CoordSystemPropPage: public BasicPropertyFormPage
{
public:
	CoordSystemPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	String m_sRefMap;
};

class RepresentationPropPage: public BasicPropertyFormPage
{
public:
	RepresentationPropPage(const IlwisObject& obj);

	virtual void BuildPage();
};

class DomainPropPage: public BasicPropertyFormPage
{
public:
	DomainPropPage(const IlwisObject& obj);

	virtual void BuildPage();

	String sDomainConvertCommand();

private:
	int CallBackAttTableChange(Event*);
	int CallBackRprChange(Event*);
	int ToClasses(Event*);
	int PictureToClasses(Event*);
	int ToIds(Event*);

	int exec();

	FieldRepresentationC *m_frc;
	StaticText           *m_stRpr;
	FieldTableC          *m_ftAttTable;
	StaticText           *m_stAttTable;

	String m_sRprName;
	int    m_iWidth;

	bool   m_fAttrTable;
	String m_sAttrTable;
	String m_sCvtCommand;
};

class SampleSetPropPage: public BasicPropertyFormPage
{
public:
	SampleSetPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:  
	int MapListChange(Event*);
	int exec();

	FieldMapList  *m_fldMPL;
	FieldMap      *m_fldMap;
	SampleSetPtr  *sms;
	String         sBgMap, sMapList, sSmpMap;
	bool           fReadOnly, fPalette;
};

class MapListPropPage: public BasicPropertyFormPage
{
public:
	MapListPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	StaticText   *m_stRemark;
	FieldGeoRefC *m_fgr;
	StaticText   *m_stPyramids;
	PushButton   *pbPyramids;
	FieldTableC       *ftAttTable;
	StaticText        *stAttTable;
	
	bool       m_fInGRCallBack;
	String     m_sNewGR;
	MapList	   mpl;
	bool  fAttrTable;
	String sAttrTable;

	int GeoRefChange(Event*);
	int CallBackPyramids(Event *)	;	
	int CallBackAttTableChange(Event*);
	void SetAttribTableField();

	bool fCheckGeoRefConsistent(String& sRemark);
	
	int exec();
};

class ObjectCollectionPropPage: public BasicPropertyFormPage
{
public:
	ObjectCollectionPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	StaticText   *m_stRemark;
	FieldTableC       *ftAttTable;
	StaticText        *stAttTable;
	
	ObjectCollection	   oc;
	bool  fAttrTable;
	String sAttrTable;

	int CallBackAttTableChange(Event*);
	void SetAttribTableField();

	
	int exec();
}; 

class MapViewPropPage: public BasicPropertyFormPage
{
public:
	MapViewPropPage(const IlwisObject& obj);

	virtual void BuildPage();
};

class DependsOnPropPage: public BasicPropertyFormPage
{
public:
	DependsOnPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	int CheckUpToDate(Event*);
	int MakeSpace(Event*);
	int Calculate(Event*);
	int BreakLink(Event*);
};

class UsedByPropPage: public BasicPropertyFormPage
{
public:
	UsedByPropPage(const IlwisObject& obj);

	virtual void BuildPage();
};

class ContainsPropPage: public BasicPropertyFormPage
{
public:
	ContainsPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	Array<String> m_as;

	void          CollectContainedFiles();
};

class ContainedByPropPage: public BasicPropertyFormPage
{
public:
	ContainedByPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	Array<String> m_as;

	void          CollectOwners();
};

class HelpPropPage: public FormBasePropertyPage
{
public:
	HelpPropPage(const FileName& fnObj);
	~HelpPropPage();

	BOOL OnSetActive();

private:
	String      help;
	String   m_sExt;
	CHHCtrl* m_hhc;
};

class StereoPairPropPage: public BasicPropertyFormPage
{
public:
	StereoPairPropPage(const IlwisObject& obj);

	virtual void BuildPage();

private:
	StaticText   *m_stRemark;
	StaticText   *m_stPyramids;
	PushButton   *pbPyramids;
	int CallBackPyramids(Event *);
	int  iOption;
	RadioGroup *rg;
	bool fUseGrfLeft;
	int exec();
};

#endif // !defined(AFX_PROPERTYSHEETS_H__326B98BD_6CDC_11D3_B7E1_00A0C9D5342F__INCLUDED_)
