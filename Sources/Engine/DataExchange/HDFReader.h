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
class _export HDFData
{
public:
	HDFData();
	HDFData(const FileName& fnFile, const String& sPath);
	virtual ~HDFData();

	enum DataObject{daAttribute, daDataSet, daVData};
	class _export Cell
	{
	public:
		enum Axis{aX, aY, aZ};

		Cell(int iX = iUNDEF, int iY = iUNDEF, int iZ = iUNDEF);
	  int iMaxDims() const;
		int iCoord(Axis a) const;
	private:		
		vector<int>  m_vLocation;
	};

	static HDFData *Create(const FileName& fnFile, DataObject, const String& sValuePath);

	virtual void SetNewPath(const String& sPath);
	virtual void GetField(const String& sField, LongBuf& data) {}; // for VData
	virtual void GetRow(int iRow, LongBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF) {}
	virtual void GetRow(int iRow, ByteBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF) {}
	virtual void GetRow(int iRow, RealBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF) {}
	virtual void GetRow(int iRow, IntBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF) {}	
	virtual long iCellValue(Cell c) { return iUNDEF; }
	virtual double rCellValue(Cell c) {return rUNDEF; }
	virtual String sGetAttribute(const String& sAttr) { return ""; }
	virtual HDFData::Cell clGetSize() { return Cell(iUNDEF, iUNDEF, iUNDEF);}
	virtual bool fValid() { return false; }

protected:
	vector<String> m_vPath;
	FileName m_fnFile;
	int32 m_iFileID;
	static CCriticalSection m_CriticalSection;

	void GoToData();
	void ParsePath(const String& sPath);
	int32 iFollowPath(int32 iRefID, int iLevel = 0);
	virtual bool fFindData(int32 ID) { return false;}
};

//---------------------------------------------------------------------------------------

class HDF4VData : public HDFData
{
public:
	friend HDFData *HDFData::Create(const FileName& fnFile , DataObject, const String& );

	HDF4VData();
	~HDF4VData();

	void GetField(const String& sName, LongBuf& data);
	virtual bool fValid();	

private:
	HDF4VData(const FileName& fnFile, const String& sPath);

	struct FieldInfo
	{
		String m_sName;
		int32 m_iDataType;
		int32 m_iSize;
		int32 m_iIndex;

	};

	vector<FieldInfo *> m_vFieldInfo;
	int32 m_iVDataID;
	int32 m_iNRecords;	

	virtual bool fFindData(int32 iRef);
	bool fInit();
	bool fGetData(const String& sField, unsigned char **data, FieldInfo **fld);

};
class HDF4DataSet : public HDFData
{
public:
	friend HDFData *HDFData::Create(const FileName& fnFile , DataObject, const String& );
	
	HDF4DataSet();
	~HDF4DataSet();
	void GetRow(int iRow, LongBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF);
	void GetRow(int iRow, ByteBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF);
	void GetRow(int iRow, RealBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF);
	void GetRow(int iRow, IntBuf& buf, int iColStart = iUNDEF, int iNumber = iUNDEF);	
	long iCellValue(Cell c);
	double rCellValue(Cell c);
	void SetNewPath(const String& sPath);
	String sGetAttribute(const String& sAttr);
	virtual HDFData::Cell clGetSize();
	virtual bool fValid();

private:
	HDF4DataSet(const FileName& fnFile, const String& sPath);
	
	struct DataSetInfo
	{
		DataSetInfo() : m_iDims(iUNDEF), m_iDataType(iUNDEF), m_iNumAttr(iUNDEF), m_iDataSet(iUNDEF) 
		{
			memset(m_dim, iUNDEF, sizeof(int32) * 32);
		}

		int32 m_dim[32];
		int m_iDims;
		int32 m_iDataType;
		int32 m_iNumAttr;
		String m_sName;
		int32 m_iDataSet;

	};

	DataSetInfo m_DSInfo;
	int32 m_iSDID;
	void  *buffer;

	bool fFindData(int32 iRef);
	void CreateLineBuffer();
	bool fGetData(bool fRow, int iRowStart, int iColStart, int& iNumber);
	bool fGetData(const Cell& c);
	bool fInit();
};
