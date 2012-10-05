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
/* MapIsoCluster
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
   Last change:  JEL   7 May 97    9:24 am
*/

#ifndef ILWMapIsoCluster_H
#define ILWMapIsoCluster_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMPL.H"

class DATEXPORT AggregateFunction;

IlwisObjectPtr * createMapIsoCluster(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapIsoCluster(ApplicationQueryData *query);

class DATEXPORT MapIsoCluster: public MapFromMapList
{
  friend DATEXPORT MapFromMapList;
public:  
  static const char* sSyntax();
  static MapIsoCluster* create(const FileName& fn, MapPtr& p, const String& sExpression);
  MapIsoCluster(const FileName& fn, MapPtr& p);
protected:
  virtual void Store();
  MapIsoCluster(const FileName& fn, MapPtr& p, const MapList& mpl, int _numDesiredCluster, int _minSampleSize, int _iter, double _std, int _minLDist, int maxLNumber) ;
  ~MapIsoCluster();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
private:
   void Init();
	int minimumLumpDistance;
	double stdDev;
	int maxLumpNumber;
	int numDesiredCluster;
	int minSampleSize;
	int maxSampleSize;
	int iterations;
	int def_n_sam;
	int data_std;
	int kmIdum;
	int sample_seed;
	bool is_rand;

};

#endif




