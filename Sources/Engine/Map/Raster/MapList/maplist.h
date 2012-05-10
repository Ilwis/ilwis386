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


#ifndef ILWMAPLIST_H
#define ILWMAPLIST_H

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"

class MapList;
class MapListVirtual;

class MapListPtr: public IlwisObjectPtr
{
  friend MapList;
  friend class MapListVirtual;
  friend class MapListEditorField;
  friend class _export MapListForm;

public:
	virtual String              sType() const;
	long                        iSize() const 
	                                { return ma.iSize()-iOffset; }
	long                        iLower() const 
	                                { return iOffset; }
	long                        iUpper() const 
	                                { return ma.iSize()-1; }
	const GeoRef&               gr() const 
	                                { return _gr; }
	RowCol                      rcSize() const 
	                                { return _rcSize; }
	void _export SetGeoRef(const GeoRef&); // set georef of all maps, dangerous function!
	const Map&                  map(long iBandNr) const
	                                { return ma[iBandNr]; }
	virtual void                Store();
	virtual String              sName(bool fExt = false, const String& sDirRelative = "") const;
	RealMatrix _export         *mtVarCov(bool fForce=false);
	RealMatrix _export         *mtCorr(bool fForce=false); 
	CVector _export            *vecMean(bool fForce=false);
	CVector _export            *vecStd(bool fForce=false); 
	virtual void                Updated();
	void _export                CheckGeoRefs() const;
	void                        CheckDomains() const;
	virtual void                GetObjectDependencies(Array<FileName>& afnObjDep);
	virtual void                Rename(const FileName& fnNew);
	void _export                SetBandPreFix(const String& sBandPreFix);
	const String _export &      sBandPreFix() const;
	FileName _export            fnRealName(const FileName& fn) const;
	String _export              sDisplayName(long iBandNr) const; 
	String _export              sDisplayName(const FileName& fn) const; 
	void _export                AddMap(const Map& mp);
	void _export                RemoveMap(const FileName& fn);
	bool _export                fOK(const Map& mp);
	void _export                MoveMap(int iNewIndex, const Map& mp);
	void _export                GetObjectStructure(ObjectStructure& os);
	void _export                CreatePyramidLayers();
	void _export                DeletePyramidFiles();
	bool _export                fHasPyramidFiles();
  virtual bool _export fDependent() const; 
  virtual bool _export fCalculated() const; 
  virtual void _export Calc(bool fMakeUpToDate=false); 
  virtual void _export DeleteCalc(); 
  virtual void _export BreakDependency(); 
  virtual _export String sExpression() const;
	virtual IlwisObjectVirtual *pGetVirtualObject() const;
  void _export SetDomainValueRangeStruct(const DomainValueRangeStruct&); // for virtual maplist
   bool _export         		fTblAtt() const;
   Table _export        		tblAtt() const;
   void _export         		SetAttributeTable(const Table& tbl);
   RangeReal _export			getRange();
   double _export getStep() const;
   Domain _export					dm() const;

protected:
  _export MapListPtr(const FileName& fn);
  MapListPtr(const FileName& fn, const String& sExpr);
  _export MapListPtr(const FileName& fn, const Array<FileName>& fnaMaps);
  _export MapListPtr(const FileName& fn, const Array<Map>& aMaps);
  _export MapListPtr(const FileName& fn, const DomainValueRangeStruct& dvsMap, 
                     const GeoRef& gr, const RowCol& rc, long iNrBands, const String& sBandPrefix);

  ~MapListPtr();

  static MapListPtr*          create(const FileName&);
  static MapListPtr*          create(const FileName&, const String& sExpression);
  void                        SetDomain(const Domain&); // dangerous function!
  void                        GetOIFList(Array<String>& asBands, IntArray& aiBnd1, IntArray& aiBnd2, IntArray& aiBnd3);
  String                      sOIF() ; // oif index info
	bool _export                fUsesDependentObjects() const;

private:                                                                                 
  void                        CalculateStats();
  bool                        fCalcLongVarCov();
  bool                        fCalcRealVarCov();
  void                        CalcCorr();
  double                      rOIF(int b1, int b2, int b3) const;
  void OpenMapListVirtual(); // load MapListVirtual and set member pmlv
  void _export SetSize(int iSize);

  GeoRef                      _gr;
  RowCol                      _rcSize;
  Array<Map>                  ma;
  RealMatrix                  _mtVarCov;
  RealMatrix                  _mtCorr;
  CVector                     _vecMean;
  CVector                     _vecStd;
  ObjectTime                        tmCalcStat;
  ObjectDependency            objdep;
  String                      _sBandPreFix;
  long                        iOffset;
  Table						  attTable;
  RangeReal					  range;

  MapListVirtual* pmlv;
};  

class MapList: public IlwisObject
{
    public:
        _export MapList();
        _export MapList(const FileName& fn);
        _export MapList(const FileName& fn, const String& sExpression);
        _export MapList(const String& sExpression);
        _export MapList(const String& sExpression, const String& sPath);
        _export MapList(const MapList& ml);
        _export MapList(const FileName& fn, const Array<FileName>& fnaMaps);
        _export MapList(const FileName& fn, const DomainValueRangeStruct& dvsMap, 
                        const GeoRef& gr, const RowCol& rc, long iNrBands, const String& sBandPrefix);
        void                        operator = (const MapList& ml) { SetPointer(ml.pointer()); }
        MapListPtr*                 ptr() const { return static_cast<MapListPtr*>(pointer()); }
        MapListPtr*                 operator -> () const { return ptr(); }
        const Map&                  operator [] (int i) const { return ptr()->map(i); }
        static MapListPtr*          pGet(const FileName& fn);

   protected:
        MapList(IlwisObjectPtr* ptr);
   
   private:
        static IlwisObjectPtrList   listMapList;
};

#endif





