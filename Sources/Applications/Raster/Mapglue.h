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

#ifndef ILWMAPGLUE_H
#define ILWMAPGLUE_H
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void*> parms=vector<void *>());
ApplicationMetadata metadataMapGlue(ApplicationQueryData *query);

class DATEXPORT MapGlue: public MapFromMap
{
  enum DomainCombinations
    { dcILLEGAL, dcEQUAL, dcSORTSORT, dcIMAGEIMAGE,
      dcVALUEVALUE, dcPICTUREPICTURE, dcCOLORIMAGE,
      dcCOLORPICTURE, dcCLASSVALUE, dcCOLORCLASS, dcCOLORVALUE};
public:
  static const char* sSyntax();  
  virtual bool fFreezing();
  virtual String sExpression() const;
  static MapGlue* create(const FileName& fn, MapPtr& p, const String& sExpr);
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual void Store();
  MapGlue(const FileName& fn, MapPtr& p);
  MapGlue(const FileName& fn, MapPtr& p, const Array<Map>& _maps, bool fRepAlw, const String& sNewDomain);
  MapGlue(const FileName& fn, MapPtr& p, const GeoRef& georef, const Array<Map>& _maps, bool fRepAlw, const String& sNewDomain);
  ~MapGlue();
private:
  bool fMergeBits(Map& mpResample);
  bool fMergeValues(Map& mpResample);
  bool fMergeByteValues(Map& mpResample);
  bool fMergeColorPicture(Map& mpResample);
  bool fMergeColorImage(Map& mpResample);
  bool fMergeSortSortOrPicturePicture(Map& mpResample, int & iEndLastID);
  bool fMergeValueColor(Map& mpResample);
  bool fMergeClassColor(Map& mpResample);
  bool fMergeEqualDomains(Map& mpResample, bool fFirstIsPicture);
  void MakeNewRowColLimitsAndCrdBounds(const GeoRef& gr1, const GeoRef& gr2,MinMax &mmMapLimits, CoordBounds& cb);
  RangeReal rrDetermineValueRange(Map& ValMap);
  bool fGlueRasterMaps(const Map& map2, bool fNewGeoref, MinMax& mm, int & iEndLastID);
  void MakeRecodeTable(Map& map1, ArrayLarge<long>& aiRecode, int & iEndLastID);
  void DetermineOutputDomain(const Map& map, DomainValueRangeStruct& ndvr, bool &fAllUniqueID, bool fCalc=true);
  void LegalMapCollection();
  static DomainCombinations dcDomainCombination(const Domain& dm1, const Domain& dm2);
  void GlueAttributeTables();
	void MergeDomainSorts(const Map&);
  Array<Map> maps;
  bool fReplaceAlways;
	bool fAllUniqueID;
  String sNewDom;
  bool fCreateGeoRef;
};
#endif






