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
/* MapImport
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  18 Feb 97    2:49 pm
*/
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\DataObjects\Tranq.h"

class DATEXPORT MapImport: public MapPtr
{
public:
  static void _export import(const FileName&, const FileName& fnOut = FileName());
  static void _export GetImportInfo(const FileName& fn, int& iMapType14, int& iScale, 
                                    bool& fINF, bool& fCOL, bool& fGeoRefNone, FileName& fnGrf, 
                                    const String& sSearchGrfPath = String());
  static void _export import(const FileName& fn, const FileName& fnNew, DomainType dmt, const FileName& fnDom, 
                             const FileName& fnGrf, const String& sDesc = "");
private:
  MapImport(const FileName& fn, const GeoRef& gr, const Domain& dm,
            const String& sD, const String& sT, const RangeReal& rrMinMax, const ValueRange& vr);
  void Store();
  String sData;
  String sType;
  virtual void PutRaw(RowCol, long) {}
  virtual void GetLineRaw(long, ByteBuf&, long, long) const {}
  virtual void GetLineRaw(long, IntBuf& , long, long) const {}
  virtual void GetLineRaw(long, LongBuf&, long, long) const {}
  virtual void GetLineVal(long, LongBuf&, long, long) const {}
  virtual void GetLineVal(long, RealBuf&, long, long) const {}
};


