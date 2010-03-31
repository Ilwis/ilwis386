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
/* MapCluster
   Copyright Ilwis System Development ITC
   september 1996, by Jelle Wind
   Last change:  JEL  20 Jun 97    1:25 pm
*/

#ifndef ILWMAPCLUSTER_H
#define ILWMAPCLUSTER_H
#include "Engine\Applications\MAPFMPL.H"

#define MAXBANDS 4

struct ClusterRec { // defines cube in feature space
  double rBandsMin[MAXBANDS], rBandsMax[MAXBANDS]; // min/max of feature space
  long iFirst;                 // first record in HistRGB that lays in cube
  long iNumRec;                // nr. records in HistRGB that lay in cube
  long iCount;                  // total nr. of pixels that lay in cube
};  

struct HistBandsRec {
  union {
    struct bvals { unsigned int b1:4, b2:4, b3:4, b4:4; } bv;
    word iComb;   
  };
  long  iCount; // nr of occurrences
};

struct BoundRange {
    double bottom, top; // total range
};
  
inline ClusterRec undef(const ClusterRec&) 
{ ClusterRec x;  return x; }

inline HistBandsRec undef(const HistBandsRec&) 
{ HistBandsRec x; return x; }

IlwisObjectPtr * createMapCluster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapCluster: public MapFromMapList
{
public:
  virtual void Store();
  virtual bool fFreezing();
  virtual String sExpression() const;
  MapCluster(const FileName& fn, MapPtr&);
  MapCluster(const FileName& fn, MapPtr& p, const MapList& mpl, int iClass, const String& sStatTbl);
  ~MapCluster();
  static MapCluster* create(const FileName& fn, MapPtr&, const String& sExpr);
  static const char* sSyntax();
private:
  int iClusterCalc(ArrayLarge<ClusterRec>& pNewCluster, long iCLusterComb, long iTotal, int iNumCLusters);
  void initFeatureSpaceHistogram(int iBands);
  void initBandLineBuffers();
  void initStretchLookup();
  long getFSIndex(long col);

  void writeStatisticsTable(String& table);
public:  
  ArrayLarge<HistBandsRec> HistBands;
private:  
  int iClasses;  
	String sStatTable;
public:  
  int iBands;
  int iSortBands;
  byte bMax;
  int iShift;
private:  
  int indexShift;
  Array<RealArray> aarIndex2Value;
  RealBuf bLine[MAXBANDS];
  BoundRange abrLookup[MAXBANDS];
};

#endif




