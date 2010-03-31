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
// BaseMapDrawer.h: interface for the BaseMapDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BASEMAPDRAWER_H__EEDE7CB4_E843_11D2_B744_00A0C9D5342F__INCLUDED_)
#define AFX_BASEMAPDRAWER_H__EEDE7CB4_E843_11D2_B744_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_DRAWER_H__96068AF6_E5AE_11D2_B73F_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\Drawers\Drawer.h"
#endif

class FieldColumn;
class FieldRepresentationC;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP BaseMapDrawer : public Drawer  
{
	friend class BaseMapDrawerForm;
protected:
	BaseMapDrawer(MapCompositionDoc*);
  BaseMapDrawer(MapCompositionDoc*, const MapView& view, 
                const char* sSection, const char* sType);
public:
	virtual ~BaseMapDrawer();
	//{{AFX_VIRTUAL(BaseMapDrawer)
  virtual Domain dm() const;
  virtual Representation rpr() const;
  virtual DomainValueRangeStruct dvrs() const;
  virtual RangeReal rrStretchRange() const;
  virtual RangeReal rrLegendRange() const;
  virtual Color clrRaw(long iRaw) const;
  virtual Color clrVal(double rVal) const;

  virtual String sName();
  virtual Ilwis::Record rec(const Coord&);
  virtual bool fAttributes();
  virtual bool fRepresentation();
  virtual void EditRepresentation();
  virtual bool fEditable();
  virtual bool fProperty();
  virtual void Prop();
  virtual bool fEditDomain();
  virtual void EditDomain();
  virtual void ExecuteAction(const Coord&);
  virtual void WriteLayer(MapView&, const char* sSection);
	virtual bool fLegendUseful() const;
	//}}AFX_VIRTUAL
	//{{AFX_MSG(BaseMapDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
  BaseMapPtr* basemap() const { return bmap; }
  DrawMethod drmMethod() const { return drm; }
	bool fShowDisplayOptions();
  void SetCustomLegendValueRange(ValueRange vr);
  bool fAttTable;
  Column colAtt;
protected:
  Color clrDraw(Color) const;
  Color clrRandom(int iRaw) const;
  void InitClrRandom();
  BaseMapPtr* bmap;  
  Domain _dm;
	Table tblAtt;
  DrawMethod drm;  
  Representation _rpr;
  bool fValue, fStretch;
  RangeInt riStretch;
  RangeReal rrStretch;
  Color clr, clr0;
  int iMultColors;
  DrawColors drc;
  enum StretchMethod { smLINEAR, smLOGARITHMIC } sm;
private:
  bool m_fCustomLegendValueRange;
  ValueRange m_vrLegend;
  vector<Color> m_clrRandom;
	DECLARE_MESSAGE_MAP()
};

class BaseMapDrawerForm: public DrawerForm
{
public:
  BaseMapDrawerForm(BaseMapDrawer*, const String& sTitle, bool fShowForm=true);
protected:
  int exec();
  void InitAtt(bool fAllowString=false);
  int AttCallBack(Event*);
  void InitStretch(FormEntry*);
  void InitRprValue(RadioGroup*);
  void InitRprClass(RadioGroup*);
  void InitSingle(RadioGroup*);
  void InitMultiple(RadioGroup*);
  void InitColoring(RadioGroup*);
  void InitBool();
int StretchCallBack(Event*);
  int RprValueCallBack(Event*);
//  int RprClassCallBack(Event*);
  virtual void DomUseChanged()=0;
  BaseMapDrawer* bmd;
  BaseMapPtr* bm;
  Domain dmMap, dmUse;
  bool fAtt;
  String sAttCol, sRprName;
  DomainType dmtUse() const;
  CheckBox *cbAtt;
  FieldColumn* fcAtt;
  StaticText *stAttDom;
  FormEntry *fgValue, *fgClass, *fgID, *fgBool, *fgColor;
  FieldRepresentationC *fldRprValue, *fldRprClass;
  RadioButton* rbRpr;
  RadioButton* rbSingle;
  RadioButton* rbMultiple;
  FormEntry* stStretch;
  FieldRangeInt* fri;
  FieldRangeReal* frr;
  Table tblAtt;
};


#endif // !defined(AFX_BASEMAPDRAWER_H__EEDE7CB4_E843_11D2_B744_00A0C9D5342F__INCLUDED_)
