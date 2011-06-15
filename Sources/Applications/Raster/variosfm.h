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
/* VariogramSurfaceRas
   Copyright Ilwis System Development ITC
   november 1998, by Jan Hendrikse
   Last change:  WK   18 May 99    6:46 pm
*/

#ifndef VARIOSFM_H
#define VARIOSFM_H
#include "Engine\Map\Raster\Map.h"
#include "Engine\Applications\MAPFMAP.H"

IlwisObjectPtr * createMapVariogramSurfaceRas(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapVariogramSurfaceRas(ApplicationQueryData *query);

class DATEXPORT MapVariogramSurfaceRas: public MapFromMap
{
  friend DATEXPORT MapVirtual;
  friend DATEXPORT MapFromMap;
public:
  MapVariogramSurfaceRas(const FileName& fn, MapPtr&);
  MapVariogramSurfaceRas(const FileName& fn, MapPtr&, const Map& map, 
                         const long iLags);
  ~MapVariogramSurfaceRas();
  static MapVariogramSurfaceRas* create(const FileName& fn, MapPtr&,
                                        const String& sExpression);
  static const char* sSyntax();
  static ValueRange _export vrDefault(const Map&);
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  virtual bool fValueRangeChangeable() const;
  virtual bool fGeoRefChangeable() const;
protected:

	String sAddInfo() const;
  double rDataMean;		// Mean of input pix vals
  double rDataVariance; 
	long iNrValidInPixels;
private:
  void Init();
  long iNrLags;
  double rLagSpacing;
  void MapVariogramSurfaceRas::ShiftArrayBufIn(Array<RealBuf>& arBuf, long iNextRow,
                                               long iFr, long iNr);
};

#endif
