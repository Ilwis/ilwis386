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

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\DataExchange\hdfincludes\hdf.h"
#include "Engine\DataExchange\hdfincludes\vg.h"
#include "Engine\DataExchange\hdfincludes\mfhdf.h"
#include <stdio.h>
#include <String>
#include <vector>
#include <conio.h>
#include "Headers\Hs\IMPEXP.hs"

using namespace std;

#include "Engine\DataExchange\HDFReader.h"

#define  FILE_NAME   "test2.hdf"

CCriticalSection HDFData::m_CriticalSection ;//;

class HDF4DataSet;

template<class T> void TransferToBuffer(T& buf, void *bufOther, int iNumber, int32 iDataType)
{
  int i=0;
	if ( buf.iSize() < iNumber )
		buf.Size(iNumber);

	switch(iDataType )
	{
		case DFNT_CHAR8:
		case DFNT_INT8:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((char *)bufOther)[i];
			break;
		case DFNT_UCHAR8:
		case DFNT_UINT8:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((unsigned char *)bufOther)[i];
			break;
		case DFNT_INT16:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((short *)bufOther)[i];
			break;
		case DFNT_UINT16:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((unsigned short *)bufOther)[i];
			break;
		case DFNT_INT32:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((long *)bufOther)[i];
			break;
		case DFNT_UINT32:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((unsigned long *)bufOther)[i];
			break;
		case DFNT_FLOAT32:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((float *)bufOther)[i];
			break;
		case DFNT_FLOAT64:
			for(i=0; i<iNumber; ++i )
				buf[i] = ((double *)bufOther)[i];
			break;
	};


}

template<class T> void TransferToValue(T& Val, void *bufOther, int32 iDataType)
{
  int i=0;
	switch(iDataType )
	{
		case DFNT_CHAR8:
		case DFNT_INT8:
				Val = ((char *)bufOther)[0];
			break;
		case DFNT_UCHAR8:
		case DFNT_UINT8:
				Val = ((unsigned char *)bufOther)[0];
			break;
		case DFNT_INT16:
				Val = ((short *)bufOther)[0];
			break;
		case DFNT_UINT16:
				Val = ((unsigned short *)bufOther)[0];
			break;
		case DFNT_INT32:
				Val = ((long *)bufOther)[0];
			break;
		case DFNT_UINT32:
				Val = ((unsigned long *)bufOther)[0];
			break;
		case DFNT_FLOAT32:
				Val = ((float *)bufOther)[0];
			break;
		case DFNT_FLOAT64:
				Val = ((double *)bufOther)[0];
			break;
	};

}

//---------------------------------------------------------------------------------------
HDFData::Cell::Cell(int iX, int iY, int iZ)
{ 
	m_vLocation.push_back(iX);
	if ( iY != iUNDEF )
		m_vLocation.push_back(iY);
	if ( iZ !=iUNDEF )
		m_vLocation.push_back(iZ);
}

int HDFData::Cell::iMaxDims() const
{
	return m_vLocation.size();
}

int HDFData::Cell::iCoord(Axis a) const
{
	if ((int) a < m_vLocation.size() )
		return m_vLocation[a];
	return iUNDEF;
}

//--------------------------------------------------------------------------------------

HDFData *HDFData::Create(const FileName& fnFile, HDFData::DataObject daType, const String& sValuePath)
{
	if ( !Hishdf(const_cast<char *>(fnFile.sFullPath().c_str() )))
		return NULL;
	
	HDFData *pObject = NULL;
	switch ( daType )
	{
		case HDFData::daDataSet:
			pObject =  new HDF4DataSet(fnFile, sValuePath);
			break;
		case HDFData::daVData:
			pObject = new HDF4VData(fnFile, sValuePath);
			break;
	}		
	return pObject;
}

HDFData::HDFData() :
	m_iFileID(iUNDEF)
{
}

HDFData::HDFData(const FileName& fnFile, const String& sPath) :
  m_fnFile(fnFile),
	m_iFileID(iUNDEF)
{
	ParsePath(sPath);
}

HDFData::~HDFData()
{
	if ( m_iFileID > 0 )
	{
		Vend(m_iFileID);
		Hclose (m_iFileID);
	}
//	delete [] buffer;	
}

void HDFData::SetNewPath(const String& sPath)
{
	if ( m_iFileID > 0 )
	{
		Vend(m_iFileID);
		Hclose (m_iFileID);
		m_iFileID = iUNDEF;
	}
	m_vPath.resize(0);
	ParsePath(sPath);
}

void HDFData::ParsePath(const String& sPath)
{
	if ( sPath == "") return;
	int iWhere = 0, iLast = 0;
	while ( (iWhere = sPath.find("\\", iLast)) != String::npos)
	{
		String sPart = sPath.substr(iLast, iWhere - iLast );
		m_vPath.push_back(sPart);
		iLast = iWhere + 1;
	}
	String sLast = sPath.substr(iLast, sPath.size() - iLast);
	m_vPath.push_back(sLast);
}

int32 HDFData::iFollowPath(int32 iRefID, int iLevel)
{
	int32 iGroupID = Vattach(m_iFileID, iRefID, "r");
	if ( iGroupID < 0 ) return iUNDEF;
	char   sGroupNm[VGNAMELENMAX];
	int32 iStatus = Vgetname(iGroupID, sGroupNm); 
	String sGroupName(sGroupNm);
	// is it the end of the branch? (not leaf)
	if ( sGroupName == m_vPath[m_vPath.size() - 2] && iLevel == m_vPath.size() - 2)
		return iGroupID;

	// is this branch at this level correct?, if not so backup
	if ( sGroupName != m_vPath[iLevel] )
		return iUNDEF;

	// branch seems to be correct, we need to go deeper though
	int32 iNumPairs = Vntagrefs (iGroupID);
	for(int iIndex = 0; iIndex < iNumPairs; ++iIndex)
	{
		int32 iTag, iRef;
		// find all the objects in this group
		intn inStatus = Vgettagref (iGroupID, iIndex, &iTag, &iRef);
		if ( Visvg( iGroupID, iRef)) // if it is a group we can go deeper
		{
			int32 iNewGroupID =  iFollowPath(iRef, iLevel + 1);
			if ( iNewGroupID > 0 ) // found an end of a branch
				return iNewGroupID;
		}
	}
	Vdetach (iGroupID);
	return iUNDEF; //  no group found
}

void HDFData::GoToData()
{
	 if ( m_fnFile == FileName() ) return;

	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);	 
	
   m_iFileID = Hopen (m_fnFile.sFullPath().c_str(), DFACC_READ, 0);
	 if ( m_iFileID < 0 ) return;

	 Vstart (m_iFileID);

	 if ( m_vPath.size() == 0 )
		 return;

	 int iNumLones = 0;
	 iNumLones = Vlone (m_iFileID, NULL, iNumLones );
	 if ( iNumLones > 0 )
	 {
		 int32 *arRefs = new int32[iNumLones];
		 iNumLones = Vlone (m_iFileID, arRefs, iNumLones);
		 for(int iLone = 0; iLone < iNumLones; ++iLone )
		 {
				int32 iGroupID = iFollowPath( arRefs[iLone]);
				if ( iGroupID > 0 )
				{
					int32 iNumPairs = Vntagrefs (iGroupID);
					for(int iIndex = 0; iIndex < iNumPairs; ++iIndex)
					{
						int32 iTag, iRef;
						intn inStatus = Vgettagref (iGroupID, iIndex, &iTag, &iRef);
						if ( fFindData(iRef) )
						{
							Vdetach (iGroupID);
							return;
						}
					}
				}
				Vdetach (iGroupID);
		 }
		 delete [] arRefs;
	 }
}
//---------------------------------------------------------------------------------------

HDF4VData::HDF4VData() :
	m_iVDataID(iUNDEF),
	m_iNRecords(iUNDEF)
{
}

HDF4VData::HDF4VData(const FileName& fnFile, const String& sPath) :
	HDFData(fnFile, sPath),
	m_iVDataID(iUNDEF),
	m_iNRecords(iUNDEF)	
{
}

HDF4VData::~HDF4VData()
{
	for(int i =0; i < m_vFieldInfo.size(); ++i)
		delete m_vFieldInfo[i];

	VSdetach(m_iVDataID);
}

bool HDF4VData::fInit()
{
  if ( m_iFileID > 0 ) return true; // already done	
	GoToData();

	return true;
}

bool HDF4VData::fValid()
{
	if ( !fInit() ) return false;	
	return m_iVDataID >= 0;
}

bool HDF4VData::fGetData(const String& sField,  unsigned char **data, FieldInfo **fld)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);
	
	for(vector<FieldInfo *>::iterator cur = m_vFieldInfo.begin(); cur != m_vFieldInfo.end(); ++cur)
	{
		if ( (*cur)->m_sName == sField)
			*fld = (*cur);
		
	}	
	if ( fld == NULL ) return false;
	
	*data = new unsigned char[(*fld)->m_iSize * m_iNRecords ];
	int32 iStatus = VSsetfields(m_iVDataID, const_cast<char *>(sField.c_str()));
	if ( iStatus < 0 ) return FALSE;
	
	iStatus = VSread(m_iVDataID, *data, m_iNRecords, FULL_INTERLACE);
	
	return true;
}

void HDF4VData::GetField(const String& sField, LongBuf& buffer)
{
	if ( !fInit() ) return;
	unsigned char *data = NULL;
	FieldInfo *fldInfo = NULL;
	if ( buffer.iSize() < m_iNRecords )
		buffer.Size(m_iNRecords);
	
	if ( fGetData(sField, &data, &fldInfo) )
	{
		TransferToBuffer(buffer, (void *)data, m_iNRecords, fldInfo->m_iDataType);
	}		

}

bool HDF4VData::fFindData(int32 iRef)
{
	int32 iVDataID = VSattach(m_iFileID, iRef, "r");
	if ( iVDataID >= 0 )
	{
		int32 iNRecords;
		int32 iInterlace;
		int32 iDataSize;
		char sName[MAX_NC_NAME];
		char fields[500];

    for (int i=0; i<500; i++)
         fields[i] = '\0';
     
		int32 iStatus = VSinquire(iVDataID, &iNRecords, &iInterlace, fields, &iDataSize, sName);
		if ( sName == m_vPath[m_vPath.size() - 1])
		{
			m_iVDataID = iVDataID;
			int iWhere = 0, iLast = 0;
			m_iNRecords = iNRecords;			
			String sFields(fields);
			for(int iFld = 0; iFld < VFnfields(m_iVDataID); ++iFld )
			{
				FieldInfo *fldInfo = new FieldInfo;
				iWhere = sFields.find(',', iLast);
				String sField = sFields.substr(iLast, iWhere - iLast);
				iLast = iWhere + 1;
				fldInfo->m_sName = sField;
				fldInfo->m_iSize = VSsizeof(m_iVDataID, const_cast<char *>(sField.c_str()));
				VSfindex(m_iVDataID, const_cast<char *>(sField.c_str()), &(fldInfo->m_iIndex));
				fldInfo->m_iDataType = VFfieldtype(m_iVDataID, fldInfo->m_iIndex);
				m_vFieldInfo.push_back(fldInfo);
			}
			return true;
		}

	}

	return false;
    
}



//----------------------------------------------------------------------------------------

HDF4DataSet::HDF4DataSet() :
	m_iSDID(iUNDEF),
	buffer(NULL)
{
}
HDF4DataSet::HDF4DataSet(const FileName& fnFile, const String& sPath) :
	HDFData(fnFile, sPath), 
  m_iSDID(iUNDEF),
	buffer(NULL)
{
}


HDF4DataSet::~HDF4DataSet()
{
	if ( m_iSDID > 0 )
	{
		SDend(m_iSDID);
	}
}

bool HDF4DataSet::fValid()
{
	if ( !fInit() ) return false;		
	return m_DSInfo.m_iDataSet >= 0;
}


void HDF4DataSet::SetNewPath(const String& sPath)
{
	HDFData::SetNewPath(sPath);
	if ( m_iSDID > 0 )
	{
		SDend(m_iSDID);
		m_iSDID = iUNDEF;
	}
}

bool HDF4DataSet::fInit()
{
  if ( m_iFileID > 0 ) return true; // already done	
	GoToData();
	if ( m_DSInfo.m_iDataSet < 0 && m_vPath.size() != 0) return false; // no data found and were not in the root
	return true;
}

void HDF4DataSet::GetRow(int iRow, LongBuf& buf, int iColStart, int iNumber)
{
	if ( !fInit() ) return;
	if ( 	fGetData(true, iRow, iColStart, iNumber))
	{
		TransferToBuffer(buf, buffer, iNumber, m_DSInfo.m_iDataType);
	}
}

long HDF4DataSet::iCellValue(Cell c)
{
	if ( !fInit() ) return iUNDEF;
	if ( fGetData(c) )
	{
		long iValue = iUNDEF;
		TransferToValue(iValue, buffer, m_DSInfo.m_iDataType);
		return iValue;			
	}
	return iUNDEF;
}

double HDF4DataSet::rCellValue(Cell c)
{
	if ( !fInit() ) return iUNDEF;
	if ( fGetData(c) )
	{
		double rValue = rUNDEF;
		TransferToValue(rValue, buffer, m_DSInfo.m_iDataType);
		return rValue;			
	}
	return iUNDEF;
}

void HDF4DataSet::GetRow(int iRow, IntBuf& buf, int iColStart, int iNumber)
{
	if ( !fInit() ) return;
	if ( 	fGetData(true, iRow, iColStart, iNumber))
	{
		TransferToBuffer(buf, buffer, iNumber, m_DSInfo.m_iDataType);
	}
}

void HDF4DataSet::GetRow(int iRow, ByteBuf& buf, int iColStart, int iNumber)
{
	if ( !fInit() ) return;
	if ( 	fGetData(true, iRow, iColStart, iNumber))
	{
		TransferToBuffer(buf, buffer, iNumber, m_DSInfo.m_iDataType);
	}
}

void HDF4DataSet::GetRow(int iRow, RealBuf& buf, int iColStart, int iNumber)
{
	if ( !fInit() ) return;
	if ( 	fGetData(true, iRow, iColStart, iNumber))
	{
		TransferToBuffer(buf, buffer, iNumber, m_DSInfo.m_iDataType);
	}
}

bool HDF4DataSet::fGetData(const Cell& c)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);	
	if ( buffer == NULL )
		CreateLineBuffer();
	int32 start[32], edge[32];
	memset(start, iUNDEF, sizeof(int32) * 32);
	memset(edge, iUNDEF, sizeof(int32) * 32);
	for(int i = 0; i < m_DSInfo.m_iDims; ++i )
	{
		start[i] = c.iCoord((Cell::Axis)i);
		edge[i] = 1;
		if ( i >= c.iMaxDims() )
			start[i] = 0;
	}
	return SDreaddata(m_DSInfo.m_iDataSet, start, NULL, edge , (VOIDP)buffer) == 0;
}

bool HDF4DataSet::fGetData(bool fRow, int iRowStart, int iColStart, int& iNumber)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);
	if ( iNumber < 0 ) iNumber = m_DSInfo.m_dim[ fRow ? 1 : 0];

	if ( buffer == NULL )
		CreateLineBuffer();
	int32 start[3], edge[3];
	start[0] = iRowStart;
	start[1] = iColStart >= 0 ? iColStart : 0;
	edge[0] = fRow ? 1 : iNumber  ;
	edge[1] = fRow ? iNumber  : 1;
	bool b =  SDreaddata(m_DSInfo.m_iDataSet, start, NULL, edge , (VOIDP)buffer) == 0;
	return b;
}


void HDF4DataSet::CreateLineBuffer()
{
	int iSize = max(m_DSInfo.m_dim[1] , m_DSInfo.m_dim[0]);
	switch(m_DSInfo.m_iDataType )
	{
		case DFNT_CHAR8:
		case DFNT_INT8:
			buffer = new char[iSize]; break;
		case DFNT_UCHAR8:
		case DFNT_UINT8:
			buffer = new unsigned char[iSize]; break;
		case DFNT_INT16:
			buffer = new short[iSize]; break;
		case DFNT_UINT16:
			buffer = new unsigned short[iSize]; break;
		case DFNT_INT32:
			buffer = new long[iSize]; break;
		case DFNT_UINT32:
			buffer = new unsigned long[iSize]; break;
		case DFNT_FLOAT32:
			buffer = new float[iSize]; break;
		case DFNT_FLOAT64:
			buffer = new unsigned long[iSize]; break;
	};
}

bool HDF4DataSet::fFindData(int32 iRef)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);	
	if ( m_iSDID < 0 )
	{
		m_iSDID = SDstart(m_fnFile.sFullPath().c_str(), DFACC_READ);
		if ( m_iSDID < 0 ) 
			throw ErrorObject(String(SIEErrFileNotFound_S.scVal(), m_fnFile.sFullPath()));
	}

	int32 iIndex2 = SDreftoindex(m_iSDID, iRef);
	if (iIndex2 >= 0 )
	{
		int32 iSDId = SDselect(m_iSDID, iIndex2);
		if ( iSDId >= 0 )
		{
			char sNameTemp[100];
			int32 iDataType, iNumAttr;
			int32 rank, dim[3];
			SDgetinfo(iSDId, sNameTemp, &rank, dim, &iDataType, &iNumAttr);
			String sName(sNameTemp);
			if ( sName == m_vPath[ m_vPath.size() - 1 ] )
			{
				if (iSDId > 0 )
				{
					char sNameTemp[100];
					m_DSInfo.m_iDataSet = iSDId;
					SDgetinfo(m_DSInfo.m_iDataSet, sNameTemp, &rank, m_DSInfo.m_dim, &(m_DSInfo.m_iDataType), &(m_DSInfo.m_iNumAttr));
					m_DSInfo.m_sName = sNameTemp;
					int iSize = 0;
					while(m_DSInfo.m_dim[iSize++] > 0 );
					m_DSInfo.m_iDims = iSize - 1;
					return true;
				}
			}
			SDendaccess(iSDId);
		}
	}
	return false;
}

String HDF4DataSet::sGetAttribute(const String& sAttr)
{
	ILWISSingleLock lock(&m_CriticalSection, TRUE,SOURCE_LOCATION);	
	fInit();
	if ( m_iSDID < 0 )
	{
		m_iSDID = SDstart(m_fnFile.sFullPath().c_str(), DFACC_READ);
		if ( m_iSDID < 0 ) return "";
	}

	int32 iDataSet = m_DSInfo.m_iDataSet != iUNDEF ? m_DSInfo.m_iDataSet : m_iSDID;
			
	int32 iNT, iC;
	int32 iInd = SDfindattr(iDataSet, const_cast<char *>(sAttr.c_str()));
	if ( iInd < 0 ) return "";

	int32 iSt = SDattrinfo(iDataSet, iInd, const_cast<char *>(sAttr.c_str()), &iNT, &iC);
	if ( iSt < 0 ) return "";

	char *buf1 = new char[iC];
	iSt = SDreadattr(iDataSet, iInd, buf1);
	if ( iSt < 0 ) return "";

	String sBuf(buf1);
	delete [] buf1;
	return sBuf;

	return "";
}

HDFData::Cell HDF4DataSet::clGetSize()
{
	if ( ! fInit()) return Cell();
	return Cell(m_DSInfo.m_dim[0], m_DSInfo.m_dim[1], m_DSInfo.m_dim[2]);
}
