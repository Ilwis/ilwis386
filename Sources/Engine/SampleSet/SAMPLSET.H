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
/* SampleSet, SampleSetPtr
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  JEL  11 Jun 96    4:43 pm
*/

#ifndef ILWSMPLSET_H
#define ILWSMPLSET_H
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\objdepen.h"
#include "Engine\SampleSet\SMPLHS.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Map\Raster\Map.h"
//#include "Engine\SampleSet\FSPACE.H"
class DATEXPORT FeatureSpace;

class SampleSetPtr: public IlwisObjectPtr
{
  friend class SampleSet;
  static SampleSetPtr* create(const FileName&, const String& sExpr);
public:
  virtual ~SampleSetPtr();
  virtual void Store();
  virtual String sName(bool fExt=false) const;
  virtual String sType() const;
  void Init();
  bool _export fInitStat();
  void _export RegisterFS(FeatureSpace* fs);
  void _export UnregisterFS(FeatureSpace* fs);
  byte _export AppClass(const String& sClassName);    // Appends new class,
                                              // and returns the raw
                                              // value in the map
  void _export DelClass(byte bKey);                   // Deletes class[bKey],
                                              // updates the map
  byte _export MrgClass(byte dKey1, byte dKey2);
                                              // Merges classes,
                                              // updates the map
                                              // and returns the new
                                              // raw value in the map
                                              // of the merged class
  const MapList&   mpl()   const { return _mpl; }
  const Map& map()   const { return smplmap; }
  const Map& mapBack()   const { return backmap; }
  void _export SetMapList(const MapList& mpl);
  void _export SetBackground(const Map& backmp);
  void _export SetSampleMap(const Map& smplmp);
  const SmplStats& smplS() const { return _smplS; }
  const SmplHist&  smplH() const { return _smplH; }
  const SmplSum&   smplSum() const  { return _sSum; }
  const SmplSumXY& smplSumXY() const { return _sSumXY; }
  double rPixInClass(byte bKey)  { return _sSum.rPixInClass(bKey); }
  void _export SetSlct(const Array<RowCol>& rca);
  void _export UpdateStat(const Array<RowCol>& rca);
  void _export StatInfo(byte   bKey,                  // Returns the mean,
                short  iBand,                 // standard deviation,
                double &rMean,                // predominant value,
                double &rStd,                 // the number of pix with
                byte   &bPred,                // bPred and the total number
                long   &iPredNr,              // of pix in this class in
                long   &iTotNr);              // this band
  void _export SaveOldSampleMapValues(const Array<RowCol>& rca);
  short iBands() const { return iNrBands; }
  short iClasses() const { return bNrClass; }
  Domain& dm() { return _dm; }
  DomainClass* dc() { return _dc; }
  void GetObjectDependencies(Array<FileName>& afnObjDep);
	void GetObjectStructure(ObjectStructure& os)	;
private:
  SampleSetPtr(const FileName&);
  _export SampleSetPtr(const FileName&, const MapList& mpl, const Domain& dm);
  _export SampleSetPtr(const FileName&, const MapList& mpl, const Map& smplmap);
  Map           smplmap, backmap;
  MapList       _mpl;
  Domain        _dm;
  DomainClass*  _dc;
  short         iNrBands;
  byte          bNrClass;
  SmplStats     _smplS;
  SmplHist      _smplH;
  SmplSum       _sSum;
  SmplSumXY     _sSumXY;
  bool          fSelection;
  Array<FeatureSpace*> fsl;
  ObjectDependency objdep;
  void RemSlct();
  void UpdateStat(RowCol rc, long iR, long iIndex=iUNDEF);
  void IncrementSmplS(byte bNew);
  void IncrementSmplHSums(RowCol rc, byte bNew, bool Add);
  bool fInitStatistics;
  ByteArray bOldSampleMapValues;
};

class DATEXPORT SampleSet: public IlwisObject
{
  static IlwisObjectPtrList listSmpl;
public:
  _export SampleSet();
  _export SampleSet(const FileName& fn);
  _export SampleSet(const FileName& fn, const MapList& ml, const Domain& dm);
  SampleSet(const FileName& fn, const MapList& ml, const Map& smplmap);
  _export SampleSet(const String& sExpr, const String& sPath);
  SampleSet(const FileName& fn, const String& sExpr);
  _export SampleSet(const SampleSet& pcp);
  SampleSet(IlwisObjectPtr* ptr);
  void operator = (const SampleSet& pcp) { SetPointer(pcp.pointer()); }
  SampleSetPtr* ptr() const { return static_cast<SampleSetPtr*>(pointer()); }
  SampleSetPtr* operator -> () const { return ptr(); }
  static SampleSetPtr* pGet(const FileName& fn);
};

#endif
