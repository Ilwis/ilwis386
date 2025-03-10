/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* MapFloat
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  16 Apr 97   10:11 am
*/

#ifndef ILWMAPFloat_H
#define ILWMAPFloat_H
#include "Engine\Map\Raster\MAPLINE.H"

class DATEXPORT MapFloat: public MapLine
{
	friend class MapStore;
	friend class MapStoreBase;
	MapFloat(const FileName& fnDat, MapStore& p)
		: MapLine(fnDat, p) { }
	MapFloat(const FileName& fnDat, MapStore& p, const RowCol& rcSize, const DomainValueRangeStruct& dvs, long iStartOff, long iRowLen, bool fPixIntL, bool fSwapBytes)
		: MapLine(fnDat, p , rcSize, dvs, iStartOff, iRowLen, fPixIntL, fSwapBytes) { }

public:
	// Get/Put one pixel value
	double rValue(RowCol) const;

	//  String sValue(RowCol, short iWidth=0) const;
	void PutRaw(RowCol, long);
	void   PutVal(RowCol, double);
	void   PutVal(RowCol, const String&);

	// Get/Put a line
	void GetLineRaw(long iLine, ByteBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineRaw(long iLine, IntBuf&,  long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineRaw(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineVal(long iLine, LongBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void GetLineVal(long iLine, RealBuf&, long iFrom, long iNum, int iPyrLayer = 0) const;
	void PutLineRaw(long iLine, const ByteBuf&, long iFrom, long iNum);
	void PutLineRaw(long iLine, const IntBuf&,  long iFrom, long iNum);
	void PutLineRaw(long iLine, const LongBuf&, long iFrom, long iNum);
	void PutLineVal(long iLine, const LongBuf&, long iFrom, long iNum);
	void PutLineVal(long iLine, const RealBuf&, long iFrom, long iNum);

	virtual String sType() const;
	virtual StoreType st() const;
protected:
	void IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos, Tranquilizer *trq);
};

#endif // ILWMAPFloat_H




