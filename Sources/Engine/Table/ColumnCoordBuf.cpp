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
#include "Headers\toolspch.h"
#include <Geos.h>
#include "Engine\Table\ColumnCoordBuf.h"
#include "Engine\Domain\dmcoordbuf.h"

ColumnCoordBuf::ColumnCoordBuf(long iRecs, long iOffset, ColumnStore& cs, bool fCreate) 
: ColumnBinary(iRecs, iOffset, cs, fCreate)
{}

ColumnCoordBuf::~ColumnCoordBuf()
{
}

String ColumnCoordBuf::sType() const
{
	return "CoordBuf";
}

void ColumnCoordBuf::PutVal(long iRec, const CoordinateSequence *seq, long iSize)
{
	DomainCoordBuf *pcrd = this->dm()->pdmcb();
	double *b;
	if ( pcrd && pcrd->f3D()) {
		b = new double[iSize * 3];
		for(int i = 0; i < iSize; ++i) {
			double x = seq->getAt(i).x;
			double y = seq->getAt(i).y;
			double z = seq->getAt(i).z;
			b[2*i] = x;
			b[2*i+1] = y;
			b[2*i+2] = z;
		}
		BinMemBlock bmb(iSize * 24, (void*)b);
	    ColumnBinary::PutVal(iRec, bmb);
	}else {
		b = new double[iSize * 2];
		for(int i = 0; i < iSize; ++i) {
			double x = seq->getAt(i).x;
			double y = seq->getAt(i).y;
			b[2*i] = x;
			b[2*i+1] = y;
		}
		BinMemBlock bmb(iSize * 16, (void*)b);
	    ColumnBinary::PutVal(iRec, bmb);
	}
}

CoordinateArraySequence *ColumnCoordBuf::iGetValue(long iRec, bool fResize) const
{
	const BinMemBlock& bmb = bmbGetVal(iRec);
	DomainCoordBuf *pcrd = this->dm()->pdmcb();
	int iSz = bmb.iSize() / sizeof(double);
	vector<Coordinate> *coords = new vector<Coordinate>();
	if ( pcrd && pcrd->f3D()) {
		for(int i = 0; i < iSz; i+=3) {
			Coordinate c;
			c.x = *(((double *)bmb.ptr()) + i);
			c.y = *(((double *)bmb.ptr()) + i + 1);
			c.z = *(((double *)bmb.ptr()) + i + 2);
			//MessageBox(0,"Huh?","",MB_OK);
			coords->push_back(c);
		}
	} else {
		for(int i = 0; i < iSz; i+=2) {
			Coordinate c;
			c.x = *(((double *)bmb.ptr()) + i);
			c.y = *(((double *)bmb.ptr()) + i + 1);
			coords->push_back(c);
		}
	}
	return new CoordinateArraySequence(coords);
	
}

Coord ColumnCoordBuf::crdBegin(long iRec) const
{
    const BinMemBlock& bmb = bmbGetVal(iRec);
	DomainCoordBuf *pcrd = this->dm()->pdmcb();
	if ( pcrd && pcrd->f3D())
		return Coord(*(double*)bmb.ptr(),*((double*)(bmb.ptr())+1),*((double*)(bmb.ptr())+2));
	else
		return Coord(*(double*)bmb.ptr(),*((double*)(bmb.ptr())+1));
}

Coord ColumnCoordBuf::crdEnd(long iRec) const
{
  const BinMemBlock& bmb = bmbGetVal(iRec);
	double *buf = (double*)bmb.ptr();
	DomainCoordBuf *pcrd = this->dm()->pdmcb();
	int iSz = bmb.iSize() / sizeof(8);
	if ( pcrd && pcrd->f3D())
		return Coord(*((double*)bmb.ptr() + (iSz - 3)), *((double*)bmb.ptr() + (iSz - 2)),*((double*)bmb.ptr() + (iSz - 1)));
	else
	    return Coord (*((double*)bmb.ptr() + (iSz - 2)), *((double*)bmb.ptr() + (iSz - 1)));
}

int ColumnCoordBuf::iNumberOfCrds(long iRec) const
{
	long iSz = ColumnBinary::iBufSize(iRec);
	DomainCoordBuf *pcrd = this->dm()->pdmcb();
	iSz /= pcrd && pcrd->f3D() ? 24 : 16;
	return iSz;
}

String ColumnCoordBuf::sValue(long iRec, short iWidth, short iDec) const
{
	int iSz = iNumberOfCrds(iRec);
	return String("%d points", iSz);
}
