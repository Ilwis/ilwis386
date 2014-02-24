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
#include "Engine\Map\Raster\MAPLINE.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\MapStoreForeignFormat.h"

MapStoreForeignFormat::MapStoreForeignFormat( MapStore& ptrMap, const LayerInfo& inf) :
	MapLine(FileName(), ptrMap, inf.grf->rcSize(), inf.dvrsMap, 0, 0, true, true),
	fnObj(ptrMap.fnMap()),
	mpStore(ptrMap),
	ff(NULL)
{
	FileName fnPyr;
	ObjectInfo::ReadElement("MapStore", "PyramidFile", ptrMap.fnMap(), fnPyr);
	if ( fnPyr.fExist() )
		InitializePyramids(fnPyr);
	iRowLength = inf.grf->rcSize().Col;
}

MapStoreForeignFormat::MapStoreForeignFormat(MapStore& ptrMap) :
	MapLine(FileName(), ptrMap),
	fnObj(ptrMap.fnMap()),
	mpStore(ptrMap),
	ff(NULL)
{
	FileName fnPyr;
	ObjectInfo::ReadElement("MapStore", "PyramidFile", ptrMap.fnMap(), fnPyr);
	if ( fnPyr.fExist() )
		InitializePyramids(fnPyr);	
}

String MapStoreForeignFormat::sType() const
{
	return "ForeignFormat";
}

StoreType MapStoreForeignFormat::st() const
{
	return stFOREIGNFORMAT;
}

MapStoreForeignFormat::~MapStoreForeignFormat()
{
	delete ff;
	ff = NULL;
}

long MapStoreForeignFormat::iRaw(RowCol rc) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		return ff->iRaw(rc);
	
	return iUNDEF;
}

long MapStoreForeignFormat::iValue(RowCol rc) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		return ff->iValue(rc);
	
	return iUNDEF;
}

double MapStoreForeignFormat::rValue(RowCol rc) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		return ff->rValue(rc);
	
	return rUNDEF;
}


template<class T> void GetLineRaw(long l, Buf<T>& b, const MapStoreForeignFormat *mpf, long iColFrom, long iColNum, int iPyrLayer)
{
	if (0 == mpf->filePyramid)
		return;
	if (l < 0 || l >= mpf->iLines()) {
		for (long i=0; i < b.iSize(); ++i )
			b[i] = 0;
		return;
	}
	
	if ( iPyrLayer > (int)mpf->iPyramidLayerOffset.size())
		iPyrLayer = 0;		
	int iDiv = (int)pow(2, (double)iPyrLayer);
	
	long iNum = iColNum == 0 ? mpf->iCols() / iDiv : iColNum;
	long iRightCols = max(0, iNum+iColFrom-mpf->iCols() / iDiv); //columns to right of map
	long iLeftCols = -min(iColFrom, 0); // columns to left of map
	
	int iFOffSet;
	iFOffSet = mpf->iPyramidLayerOffset[iPyrLayer - 1] ;	// 0 based index	
	for (long i=0; i < iLeftCols ; ++i)
		b[i] = 0;
	int iDataSize = 1;

	StoreType st = mpf->dvs.st();
	if ( mpf->dm()->dmt() == dmtIMAGE )
		st = stBYTE;

	switch (st)
	{
		case stBYTE:
			{
				ByteBuf buf(b.iSize());
				byte HUGEBUFPTR* p = buf.buf()+iLeftCols;
				ULONGLONG iOff = iFOffSet + ((ULONGLONG)l * int(mpf->iCols()/ iDiv) + (iColFrom+iLeftCols));
				mpf->filePyramid->Seek(iOff);
				mpf->filePyramid->Read((iNum - iLeftCols-iRightCols), p);
				for(int i=0; i<b.iSize(); ++i)
					b[i] = buf[i];
			}					
			break;
		case stINT:
			{
				IntBuf buf(b.iSize());
				short HUGEBUFPTR* p = buf.buf()+iLeftCols;
				ULONGLONG iOff = iFOffSet + 2 * ((ULONGLONG)l * int(mpf->iCols()/ iDiv) + (iColFrom+iLeftCols));
				mpf->filePyramid->Seek(iOff);
				mpf->filePyramid->Read(2 * (iNum - iLeftCols-iRightCols), p);
				for(int i=0; i<b.iSize(); ++i)
					b[i] = buf[i];
			}					
			break;
		case stLONG:
			{
				LongBuf buf(b.iSize());
				long HUGEBUFPTR* p = buf.buf()+iLeftCols;
				ULONGLONG iOff = iFOffSet + 4 * ((ULONGLONG)l * int(mpf->iCols()/ iDiv) + (iColFrom+iLeftCols));
				mpf->filePyramid->Seek(iOff);
				mpf->filePyramid->Read(4 * (iNum - iLeftCols-iRightCols), p);
				for(int i=0; i<b.iSize(); ++i)
					b[i] = buf[i];
			}
			break;
		case stFLOAT:
			{
				FloatBuf buf(b.iSize());
				float HUGEBUFPTR* p = buf.buf()+iLeftCols;
				ULONGLONG iOff = iFOffSet + 4 * ((ULONGLONG)l * int(mpf->iCols()/ iDiv) + (iColFrom+iLeftCols));
				mpf->filePyramid->Seek(iOff);
				mpf->filePyramid->Read(4 * (iNum - iLeftCols-iRightCols), p);
				for(int i=0; i<b.iSize(); ++i)
					b[i] = buf[i]; 
			}
			break;
		case stREAL:
			{
				RealBuf buf(b.iSize());
				double HUGEBUFPTR* p = buf.buf()+iLeftCols;
				ULONGLONG iOff = iFOffSet + 8 * ((ULONGLONG)l * int(mpf->iCols()/ iDiv) + (iColFrom+iLeftCols));
				mpf->filePyramid->Seek(iOff);
				mpf->filePyramid->Read(8 * (iNum - iLeftCols-iRightCols), p);
				for(int i=0; i<b.iSize(); ++i)
					b[i] = buf[i]; 
			}
			break;
	}
	
	for (long i=iNum-iRightCols; i < iNum ; ++i)
		b[i] = 0;
}

void MapStoreForeignFormat::GetLineRaw(long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		if ( iPyrLayer == 0 )
			ff->GetLineRaw(iLine, buf, iFrom, iNum);
		else
			::GetLineRaw(iLine, buf, this, iFrom, iNum, iPyrLayer);
}

void MapStoreForeignFormat::GetLineRaw(long iLine, IntBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		if ( iPyrLayer == 0 )
			ff->GetLineRaw(iLine, buf, iFrom, iNum);
		else 
			::GetLineRaw(iLine, buf, this, iFrom, iNum, iPyrLayer);
}

void MapStoreForeignFormat::GetLineRaw(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		if ( iPyrLayer == 0 )
			ff->GetLineRaw(iLine, buf, iFrom, iNum);
		else 
			::GetLineRaw(iLine, buf, this, iFrom, iNum, iPyrLayer);
}

void MapStoreForeignFormat::GetLineVal(long iLine, RealBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		if ( iPyrLayer == 0 )		
			ff->GetLineVal(iLine, buf, iFrom, iNum);
		else 
			::GetLineRaw(iLine, buf, this, iFrom, iNum, iPyrLayer);
}

void MapStoreForeignFormat::GetLineVal(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer) const
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
		if ( iPyrLayer == 0 )		
			ff->GetLineVal(iLine, buf, iFrom, iNum);
		else 
			::GetLineRaw(iLine, buf, this, iFrom, iNum, iPyrLayer);	
}

void MapStoreForeignFormat::PutLineRaw(long iLine, const ByteBuf& buf, long iFrom, long iNum) 
{
	if ((const_cast<MapStoreForeignFormat *>(this))->createForeignFormat() && dvs.fRawAvailable())
	{
		ff->PutLineRaw(fnObj, iLine, buf, iFrom, iNum);
	}			
	
}

void MapStoreForeignFormat::PutLineRaw(long iLine, const IntBuf& buf, long iFrom, long iNum) 
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat() && dvs.fRawAvailable() )
	{
		ff->PutLineRaw(fnObj, iLine, buf, iFrom, iNum);
	}		
}

void MapStoreForeignFormat::PutLineRaw(long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat() && dvs.fRawAvailable())
	{
		ff->PutLineRaw(fnObj, iLine, buf, iFrom, iNum);
	}		
	
}

void MapStoreForeignFormat::PutLineVal(long iLine, const RealBuf& buf, long iFrom, long iNum) 
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
	{
		ff->PutLineVal(fnObj, iLine, buf, iFrom, iNum);
	}		
}

void MapStoreForeignFormat::PutLineVal(long iLine, const LongBuf& buf, long iFrom, long iNum) 
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
	{
		ff->PutLineVal(fnObj, iLine, buf, iFrom, iNum);
	}		
}

void MapStoreForeignFormat::PutRaw(RowCol rc, long iVal)
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
	{
		LongBuf buf(1);
		buf[0] = iVal;
		PutLineRaw(rc.Row, buf, rc.Col, 1);
	}			
}

void MapStoreForeignFormat::PutVal(RowCol rc, double rVal)
{
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
	{
		RealBuf buf(1);
		buf[0] = rVal;
		PutLineVal(rc.Row, buf, rc.Col, 1);
	}			
}

void MapStoreForeignFormat::PutVal(RowCol rc, const String& s)
{
	if (!dvs.fRawAvailable())	
		return;
	
	if ( (const_cast<MapStoreForeignFormat *>(this))->createForeignFormat())
	{
		LongBuf buf(1);
		buf[0] = dvs.iRaw(s);
		PutLineVal(rc.Row, buf, rc.Col, 1);
	}		
}

void MapStoreForeignFormat::Store()
{
	if ( fChanged && ff )
	{
		ff->SetForeignGeoTransformations(mpStore.gr()->cs(), mpStore.gr());
	}		
}



void MapStoreForeignFormat::IterateCreatePyramidLayer(int iPyrLayer, ULONGLONG &iLastFilePos, Tranquilizer *trq)
{
	int iDiv = (int)pow(2, (double)iPyrLayer);	
	int iColsPyrLayer = iCols() / iDiv;
	int iLinePrevPyrLayer = iLines() / iDiv;
	StoreType stType	;
	Map mp(fnObj);
	if ( mp->dm()->dmt() == dmtIMAGE )
		stType = stBYTE;
	else
		stType = mp->dvrs().st();

	switch( stType )
	{
	case stBYTE:
		{
			ByteBuf buf(iColsPyrLayer);
			ByteBuf bufPyr(iColsPyrLayer / 2);
			
			for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
			{
				if ( trq )
				{
					if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
					{
						filePyramid->SetErase()	;
						delete filePyramid;
						filePyramid = 0;
						return;
					}
				}
				int iPyrCol = 0;		
				GetLineRaw(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
				for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
				{
					byte iPred = buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
					bufPyr[iCol / 2] = iPred;
				}	
				filePyramid->Seek(iLastFilePos);
				iLastFilePos += filePyramid->Write(bufPyr.iSize() * sizeof(byte), (void *)bufPyr.buf());
			}						
		}
		break;
	case stREAL:
		{
			RealBuf buf(iColsPyrLayer);
			RealBuf bufPyr(iColsPyrLayer / 2);
			
			for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
			{
				if ( trq )
				{
					if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
					{
						filePyramid->SetErase()	;
						delete filePyramid;
						filePyramid = 0;
						return;
					}
				}				
				int iPyrCol = 0;		
				GetLineVal(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
				for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
				{
					double rPred = buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
					bufPyr[iCol / 2] = rPred;
				}	
				filePyramid->Seek(iLastFilePos);
				iLastFilePos += filePyramid->Write(bufPyr.iSize() * 8, (void *)bufPyr.buf());
			}
		}
		break;
	case stLONG:
		{
			LongBuf buf(iColsPyrLayer);
			LongBuf bufPyr(iColsPyrLayer / 2);
			
			for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
			{
				if ( trq )
				{
					if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
					{
						filePyramid->SetErase()	;
						delete filePyramid;
						filePyramid = 0;
						return;
					}
				}
				int iPyrCol = 0;		
				GetLineRaw(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
				for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
				{
					long iPred = buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
					bufPyr[iCol / 2] = iPred;
				}	
				filePyramid->Seek(iLastFilePos);
				iLastFilePos += filePyramid->Write(bufPyr.iSize() * 4, (void *)bufPyr.buf());
			}				
		}
		break;
	case stINT:
		{
			IntBuf buf(iColsPyrLayer);
			IntBuf bufPyr(iColsPyrLayer / 2);
			
			for(int iLine = 0, iPyrLine = 0; iPyrLine < iLinePrevPyrLayer / 2; iLine += 2, ++iPyrLine)
			{
				if ( trq )
				{
					if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
					{
						filePyramid->SetErase()	;
						delete filePyramid;
						filePyramid = 0;
						return;
					}
				}
				int iPyrCol = 0;		
				GetLineRaw(iLine, buf, 0, iColsPyrLayer, iPyrLayer);			
				for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
				{
					short iPred = buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
					bufPyr[iCol / 2] = iPred;
				}	
				filePyramid->Seek(iLastFilePos);
				iLastFilePos += filePyramid->Write(bufPyr.iSize() * sizeof(short), (void *)bufPyr.buf());
			}						
		}
		break;
	case stFLOAT:
		{
			RealBuf buf(iColsPyrLayer);
			FloatBuf bufPyr(iColsPyrLayer / 2);
			
			for(int iLinePrevPyrLayer = 0, iPyrLine = 0; iLinePrevPyrLayer < iLinePrevPyrLayer; iLinePrevPyrLayer += 2, ++iPyrLine)
			{
				if ( trq )
				{
					if ( trq->fUpdate(iPyrLine, iLinePrevPyrLayer) )
					{
						filePyramid->SetErase()	;
						delete filePyramid;
						filePyramid = 0;
						return;
					}
				}
				int iPyrCol = 0;		
				GetLineVal(iLinePrevPyrLayer, buf, 0, iColsPyrLayer, iPyrLayer);			
				for(int iCol = 0; iCol < iColsPyrLayer - 2; iCol += 2)
				{
					float iPred = (float)buf[iCol]; //rPredominant(bufPrev[iCol], bufPrev[iCol+1], buf[iCol], buf[iCol + 1]);
					bufPyr[iCol / 2] = iPred;
				}	
				filePyramid->Seek(iLastFilePos);
				iLastFilePos += filePyramid->Write(bufPyr.iSize() * sizeof(float), (void *)bufPyr.buf());
			}						
		}	
		break;
	};			
}

ForeignFormat *MapStoreForeignFormat::createForeignFormat(){
	if ( ff == NULL) {
		ParmList pm;
		ff = ForeignFormat::Create(mpStore.fnMap(), pm);
		ff->Init();
	}
	return ff;

}
