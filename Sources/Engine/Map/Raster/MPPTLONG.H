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
/* MapPatchLong
   Interface for ILWIS 2.0
   24 Apr 95, by Willem Nieuwenhuis
   Copyright ILWIS Department, ITC
	Last change:  JEL  16 Apr 97   10:12 am
*/

#ifndef ILWMAPPATCH_LONG_H
#define ILWMAPPATCH_LONG_H
#include "Engine\Map\Raster\MAPPATCH.H"

class DATEXPORT MapPatchLong : public MapPatch
{
    friend class MapStore;
	MapPatchLong(const FileName& fnDat, MapStore& p)
		: MapPatch(fnDat, p) { }
	MapPatchLong(const FileName& fnDat, MapStore& p, const RowCol& rcSize, const DomainValueRangeStruct& dvs)
		: MapPatch(fnDat, p, rcSize, dvs) { }

public:
	// Get/Put one pixel value
	long iRaw(RowCol) const;
	void PutRaw(RowCol, long);

	// Get/Put a patch
	void GetPatchRaw(RowCol, BytePatch&) const;
	void GetPatchRaw(RowCol, IntPatch& ) const;
	void GetPatchRaw(RowCol, LongPatch&) const;
	void GetPatchVal(RowCol, LongPatch&) const;
	void GetPatchVal(RowCol, RealPatch&) const;
	void PutPatchRaw(RowCol, const BytePatch&);
	void PutPatchRaw(RowCol, const IntPatch& );
	void PutPatchRaw(RowCol, const LongPatch&);
	void PutPatchVal(RowCol, const LongPatch&);
	void PutPatchVal(RowCol, const RealPatch&);

	virtual String sType() const;
	virtual StoreType st() const;
};

#endif   




