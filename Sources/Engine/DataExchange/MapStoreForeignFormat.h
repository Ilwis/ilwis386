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
#include "Engine\DataExchange\ForeignStore.h"

class MapStoreForeignFormat : public MapLine
{
	template<class T> friend void GetLineRaw(long l, Buf<T>& b, const MapStoreForeignFormat *mpf, long iColFrom, long iColNum, int iPyrLayer);
public:
	MapStoreForeignFormat(MapStore&, const LayerInfo& inf);
	MapStoreForeignFormat(MapStore& ptrMap);
	~MapStoreForeignFormat();

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

	virtual long iRaw(RowCol) const;
	virtual long iValue(RowCol) const;
	virtual double rValue(RowCol) const;

  virtual void PutRaw(RowCol rc, long iVal);
	virtual void PutVal(RowCol, double);
	virtual void PutVal(RowCol, const String&);	 

	virtual String sType() const;
	virtual StoreType st() const;
	void Store();

 
protected:
	void IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos, Tranquilizer *trq);
 
private:
	ForeignFormat *createForeignFormat();

	const FileName &fnObj;
	MapStore& mpStore;
	ForeignFormat *ff;
};
