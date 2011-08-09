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
/*Interface for Data object form entries
// by Jelle Wind, dec 1993
// (c) Computer Department ITC
	Last change:  MS    1 Jul 97    2:57 pm
*/
#ifndef ILW_FENTDATA
#define ILW_FENTDATA
#include "Engine\Map\Raster\MapList\maplist.h"

// base data type for name edit, a form entry for data type input

class NameEdit;
class ObjectLister;

class _export FieldDataTypeSimple: public FormEntry
{
public:
	FieldDataTypeSimple(FormEntry*, Parm *prm, const String& sExt, bool fExt);
	FieldDataTypeSimple(FormEntry*, String *psName, const String& sExt, bool fExt, ObjectLister *obl, const FileName& dir=FileName(), bool fulltree=true);
	FieldDataTypeSimple(FormEntry*, Parm *prm, ObjectLister*, bool fExt);
	FieldDataTypeSimple(FormEntry*, String *psName, ObjectLister*, bool fExt);
                // psName : destination for result
                // sExt : extension for search of file names
                // fExt : return name with extension ?
	~FieldDataTypeSimple();
	void SetAllowEmpty(bool f = true) { fAllowEmpty = f; }
	void show(int sw);            // show entry
	void Enable();
	void Disable();
	FormEntry* CheckData();       // validate entry value
	void SetFocus();              // overriden
	void create();                // create entry
	void StoreData();		// store result in destination
	void SetVal(const String& sVal);
	virtual void DrawItem(DRAWITEMSTRUCT* dis); // draw in list
	String sName();
	void FillDir();
	NameEdit* ne;                         // name edit class
	ObjectLister* objLister() const { return ol; }
	void SetObjLister(ObjectLister* objl);  // deletes old one!
	String sGetText();
	String sGetExtentsions();
protected:
	void Init();

	String _sName, *_psName;              // local storage and destination
	String _sExt;                         // used extension for files
	bool _fExt;                           // return name with extension ?
	FileName baseDir;
	bool showFullTree;

	int iDialogWidth;                     // width of name edit
	bool fAllowEmpty;
private:
	ObjectLister* ol;
};

class FieldDataType: public FormEntry
{
public:
  _export FieldDataType(FormEntry*, const String& sQuestion,Parm *prm, const String& sExt, bool fExt);
  _export FieldDataType(FormEntry*, const String& sQuestion,String *psName, const String& sExt, bool fExt, ObjectLister *obl=NULL, const FileName& baseDir=FileName(), bool showFullTree=true);
  _export FieldDataType(FormEntry*, const String& sQuestion,Parm *prm, ObjectLister*, bool fExt);
  _export FieldDataType(FormEntry*, const String& sQuestion, String *psName, ObjectLister*, bool fExt);
                // psName : destination for result
                // sExt : extension for search of file names
                // fExt : return name with extension ?
  _export ~FieldDataType();
  void SetObjLister(ObjectLister* objl) 
    { fld->SetObjLister(objl); }
  virtual void setHelpItem(const HelpTopic& htp)
    { fld->setHelpItem(htp); FormEntry::setHelpItem(htp); }
  void SetAllowEmpty(bool f = true) { fld->SetAllowEmpty(f); }
  void SetVal(const String& sVal)
    { fld->SetVal(sVal); }
  void SetFocus()
    { fld->SetFocus(); }  // overriden
  FormEntry* CheckData()       // validate entry value
    { return fld->CheckData(); }
  void _export enable();
  void disable();
  void _export DisableUnset();
  void create() { CreateChildren(); }               // create entry
  String sName() { return fld->sName(); } // return result
  void StoreData()             // store result in destination
    { fld->StoreData(); }
  void SetCallBack(NotifyProc np)
	{ FormEntry::SetCallBack(np); 
		fld->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
    { FormEntry::SetCallBack(np,cb); 
			fld->SetCallBack(np,cb); }
  void _export SetWidth(short iWidth);
  void FillDir() { fld->FillDir(); }
  String _export sGetText();
	static void _export GetAttribParts(String& sIn, String& sM, String& sC=String(""), String& sT=String(""));
	String _export sGetExtentsions();

protected:
  StaticTextSimple* st;
  FieldDataTypeSimple *fld;
};

class FieldDataTypeC: public FieldDataType
{
public:
  FieldDataTypeC(FormEntry* fe, const String& sQuestion,Parm *prm, const String& sExt, bool fExt, NotifyProc npCreate)
  : FieldDataType(fe,sQuestion,prm,sExt,fExt)
  { init(npCreate); }
  _export FieldDataTypeC(FormEntry* fe, const String& sQuestion, String *psName, const String& sExt, bool fExt, NotifyProc npCreate)
  : FieldDataType(fe,sQuestion,psName,sExt,fExt)
  { init(npCreate); }
  FieldDataTypeC(FormEntry* fe, const String& sQuestion,Parm *prm, ObjectLister* ol, bool fExt, NotifyProc npCreate)
  : FieldDataType(fe,sQuestion,prm,ol,fExt)
  { init(npCreate); }
  FieldDataTypeC(FormEntry* fe, const String& sQuestion, String *psName, ObjectLister* ol, bool fExt, NotifyProc npCreate)
  : FieldDataType(fe,sQuestion,psName,ol,fExt)
  { init(npCreate); }
  void _export create();
  void _export SetVal(const String& sVal);
private:
  void _export init(NotifyProc npCreate);
  OwnButtonSimple *pbCreate;
};


class FieldDataTypeCreate: public FormEntry
{
public:
  _export FieldDataTypeCreate(FormEntry*, const String& sQuestion, Parm *prm,
                      const String& sExt, bool fExt);
  _export FieldDataTypeCreate(FormEntry*, const String& sQuestion,
                      String *psName, const String& sExt, bool fExt);
  _export ~FieldDataTypeCreate();
  virtual void setHelpItem(const HelpTopic& htp)
    { fld->setHelpItem(htp); FormEntry::setHelpItem(htp); }
  void _export SetVal(const String& sVal);
  void SetFocus()
    { fld->SetFocus(); }  // overriden
  FormEntry _export *CheckData();      // validate entry value
  void enable() 
  { fld->enable(); 
    if (st) st->Enable(); }
  void disable() 
  { fld->disable(); 
    if (st) st->Disable(); }
  void create() { CreateChildren(); }
  void SelectAll() { fld->SelectAll(); }
  const String _export sName();
  void _export StoreData();             // store result in destination
  void SetCallBack(NotifyProc np)
    { FormEntry::SetCallBack(np); fld->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler *cb)
    { FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
  void _export SetWidth(short iWidth);
  String _export sGetText();
protected:
  StaticTextSimple* st;
  FieldStringSimple* fld;
  String _sName, *_psName;              // local storage and destination
  String _sExt;                         // used extension for files
  bool _fExt;                           // return name with extension ?
};


class _export FieldCoordSystem: public FieldDataType
{
public:
  FieldCoordSystem(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".CSY",
                    true)
    {}
};

class _export FieldDomain: public FieldDataType
{
public:
  FieldDomain(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".DOM",
                    true)
    {}
};

class _export FieldMap: public FieldDataType
{
public:
  FieldMap(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".MPR",
                    true)
    { setHelpItem(htpUiMap); }
  FieldMap(FormEntry* parent, const String& sQuestion, String *sName, ObjectLister *obl=NULL)
    : FieldDataType(parent, sQuestion, sName, ".MPR", true, obl)
    { setHelpItem(htpUiMap); }
};

class _export FieldMapCreate: public FieldDataTypeCreate
{
public:
  FieldMapCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".MPR", false)
    { setHelpItem(htpUiMap); }

  FieldMapCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MPR", false)
    { setHelpItem(htpUiMap); }
};


#define sBaseMapExtensions ".MPR.MPS.MPA.MPP"

class _export FieldBaseMap: public FieldDataType
{
public:
  FieldBaseMap(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, sBaseMapExtensions,
                    true) // return name with extension
    { setHelpItem(htpUiBaseMap); }

  FieldBaseMap(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, sBaseMapExtensions,
                    true) // return name with extension
    { setHelpItem(htpUiBaseMap); }
};


class _export FieldClassifier: public FieldDataType
{
public:
  FieldClassifier(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".CLF",
                    true)
    { }

  FieldClassifier(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".CLF",
                    true)
    { }
};

class _export FieldSegmentMap: public FieldDataType
{
public:
  FieldSegmentMap(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".MPS",
                    true)
    { setHelpItem(htpUiSegMap); }

  FieldSegmentMap(FormEntry* parent, const String& sQuestion, String *sName, ObjectLister *obl=NULL)
    : FieldDataType(parent, sQuestion, sName, ".MPS",
                    true, obl)
    { setHelpItem(htpUiSegMap); }

};

class _export FieldSegmentMapCreate: public FieldDataTypeCreate
{
public:
  FieldSegmentMapCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".MPS",
                    false)
    { setHelpItem(htpUiSegMap); }

  FieldSegmentMapCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MPS",
                    false)
    { setHelpItem(htpUiSegMap); }
};


class _export FieldPolygonMap: public FieldDataType
{
public:
  FieldPolygonMap(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".MPA",
                    true)
    { setHelpItem(htpUiPolMap); }
  FieldPolygonMap(FormEntry* parent, const String& sQuestion, String *sName, ObjectLister *obl=NULL)
    : FieldDataType(parent, sQuestion, sName, ".MPA", true, obl)
    { setHelpItem(htpUiPolMap); }
};

class _export FieldPolygonMapCreate: public FieldDataTypeCreate
{
public:
  FieldPolygonMapCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".MPA",
                    false)
    { setHelpItem(htpUiPolMap); }
  FieldPolygonMapCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MPA",
                    false)
    { setHelpItem(htpUiPolMap); }
};

class _export FieldPointMap: public FieldDataType
{
public:
  FieldPointMap(FormEntry* parent, const String& sQuestion,
           Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".MPP", true)
    { setHelpItem(htpUiPntMap); }
  FieldPointMap(FormEntry* parent, const String& sQuestion, String *sName, ObjectLister *obl=NULL)
    : FieldDataType(parent, sQuestion, sName, ".MPP", true, obl)
    { setHelpItem(htpUiPntMap); }
};

class _export FieldPointMapCreate: public FieldDataTypeCreate
{
public:
  FieldPointMapCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".MPP",
                    false)
    { setHelpItem(htpUiPntMap); }
  FieldPointMapCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MPP",
                    false)
    { setHelpItem(htpUiPntMap); }
};

class _export FieldMatrix: public FieldDataType
{
public:
  FieldMatrix(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".MAT",
                    true)
    {}
};

class _export FieldMatrixCreate: public FieldDataTypeCreate
{
public:
  FieldMatrixCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MAT",
                    false)
    {}
};

class _export FieldStereoPair: public FieldDataType
{
public:
  FieldStereoPair(FormEntry* parent, const String& sQuestion,
           String *sName)
    : FieldDataType(parent, sQuestion, sName, ".STP",
                    true)
    {}
};

class _export FieldStereoPairCreate: public FieldDataTypeCreate
{
public:
  FieldStereoPairCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MAT",
                    false)
    {}
};

class _export FieldView: public FieldDataType
{
public:
  FieldView(FormEntry* parent, const String& sQuestion,
            Parm *prm)
    : FieldDataType(parent, sQuestion, prm, ".MPV",
                    true)
    { setHelpItem(htpUiView); }
  FieldView(FormEntry* parent, const String& sQuestion,
            String *sName)
    : FieldDataType(parent, sQuestion, sName, ".MPV",
                    true)
    { setHelpItem(htpUiView); }
};

class _export FieldViewCreate: public FieldDataTypeCreate
{
public:
  FieldViewCreate(FormEntry* parent, const String& sQuestion,
                       Parm *prm)
    : FieldDataTypeCreate(parent, sQuestion, prm, ".MPV",
                    true)
    { setHelpItem(htpUiTbl); }

  FieldViewCreate(FormEntry* parent, const String& sQuestion,
                       String *sName)
    : FieldDataTypeCreate(parent, sQuestion, sName, ".MPV",
                    true)
    { setHelpItem(htpUiTbl); }
};

class _export FieldMapFromMapList: public FormEntry
  // a variation on FieldColumn
{
public:
  FieldMapFromMapList(FormEntry*, const String& sQuestion, const MapList&, int* iMapNr);
  void SetVal(const String& sVal)
    { fld->ose->SelectString(-1, sVal.c_str()); }
  virtual FormEntry* CheckData();       // validate entry value
  void FillWithMaps(const MapList&);
  //long DrawItem(DRAWITEMSTRUCT *dis) { return fld->DrawItem(dis); }
  virtual void create();                // create entry
  virtual void StoreData();             // store result in destination
//  String sName() { CheckData(); return _sName; }
  void SetFocus();                     // overriden
  void SetCallBack(NotifyProc np) 
		{ FormEntry::SetCallBack(np); fld->SetCallBack(np); }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
    { FormEntry::SetCallBack(np,cb); fld->SetCallBack(np,cb); }
protected:
  MapList mpl;
  int* iNr;
  StaticTextSimple *st;
  FieldOneSelect *fld;
};

#endif




