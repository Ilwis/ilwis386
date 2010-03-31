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

#pragma warning( disable : 4715 )

#include <set>
#include "Headers\Hs\CONV.hs"
#include "Headers\Hs\segment.hs"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ApplicationMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\SegmentMapIlwis20.h"
#include "Engine\Map\Segment\SEG14.H"
#include "Engine\Base\System\commandhandler.h"

SegmentMapIlwis20::SegmentMapIlwis20()
{
}

SegmentMapIlwis20::SegmentMapIlwis20(const FileName& fnForgn, ForeignFormat::mtMapType _mtType)
: ForeignFormat(fnForgn.sFullPath(), _mtType)
{
	m_fIsPolygonMap = fCIStrEqual(fnForgn.sExt, ".mpa");
	m_sTitle = SSEGTitleExport20Segments;
}

SegmentMapIlwis20::~SegmentMapIlwis20()
{
}

void SegmentMapIlwis20::LoadTable(TablePtr* tbl)
{
}

void SegmentMapIlwis20::CopyAttributes(BaseMap bm)
{
	// Make a copy of the name of the attribute table in case of internal map domain
	if (bm->fTblAtt())
	{
		m_fnForeignAtt = bm->sTblAtt();
		if (m_fInternalDom)
			m_fnForeignAtt.Dir(fnGetForeignFile().sPath());
	}
}

bool SegmentMapIlwis20::fCheckCopyMap(BaseMap bm)
{
	String sVersion;
	ObjectInfo::ReadElement("Ilwis", "Version", bm->fnObj, sVersion);
	bool fCopyMade = sVersion.sHead(".").iVal() <= 2;

	// Copy the object and all used service objects
	// If version of object is 3, retain copied service objects but remove the copied main object
	// which will be recreated from scratch
	// For version 2 the copied main object will be adjusted
/*	ObjectCopierUI::CopyFiles(bm->fnObj, fnForeign, true);					
	if (!fCopyMade)
	{
		// Delete the copied main object
		Tranquilizer trq;
		CommandHandler::DeleteObjects(String("%S -force -quiet", fnForeign.sFullNameQuoted()), &trq);
	}*/

	return fCopyMade;
}

void SegmentMapIlwis20::AdjustMapAndTableDomain(BaseMap bm)
{
	// Add link to attributes when available
	if (bm->fTblAtt())
		ObjectInfo::WriteElement("BaseMap", "AttributeTable", fnGetForeignFile(), m_fnForeignAtt); // set attribute table

	if (m_dmMap->pdUniqueID() || m_fInternalDom)
	{
		int iRec = m_dmMap->pdsrt()->iNettoSize();
		String sPrefix;
		ObjectInfo::ReadElement("DomainSort", "Prefix", bm->fnObj, sPrefix);  // keep prefix
		m_dmMap = Domain(fnGetForeignFile(), iRec, dmtID, sPrefix);
		ObjectInfo::WriteElement("BaseMap", "Domain", fnGetForeignFile(), m_dmMap);    // rewrite domain

		// adjust the domain of the new attribute table
		if (bm->fTblAtt() && m_fInternalDom)
		{
			Table tblAtt(m_fnForeignAtt);
			tblAtt->SetDomain(m_dmMap);

			for (long j = 0; j < tblAtt->iCols(); j++)
			{
				// Check columns of attribute table for map domain
				Column colLoc = tblAtt->col(j);
				if (colLoc->dm()->fnObj == bm->fnObj)
					colLoc->SetDomainValueRangeStruct(m_dmMap);

				// Check columns for DomainUniqueID
				if (colLoc->dm()->dmt() == dmtUNIQUEID)
				{
					FileName fnColDom = FileName::fnUniqueShort(FileName(colLoc->sName(), ".dom"));
					Domain dmCol(fnColDom, tblAtt->iRecs(), dmtID);
					colLoc->SetDomainValueRangeStruct(dmCol);
				}

			}
			tblAtt->Updated();
		}
		ObjectInfo::WriteElement("TableView", (char*)0, m_fnForeignAtt, (char*)0); // remove clutter
	}
}

// Main entry: write segment map to version 2.x format
void SegmentMapIlwis20::Store(IlwisObject obj)
{
	if (File::fExist(fnGetForeignFile())) 
	{
		getEngine()->Message(SDATWarnFileAlreadyExists.scVal(),
                              m_sTitle.scVal(),
                              MB_OK | MB_ICONEXCLAMATION);
		return;
	}

	trq.SetTitle(m_sTitle);
	trq.Start();

	try {
		BaseMap& bm = static_cast<BaseMap&>(obj);

		m_dmMap = bm->dm();
		m_fInternalDom = m_dmMap->fnObj == bm->fnObj;

		CopyAttributes(bm);

		// Check the version of the map: if it is already version 2
		// only make a copy; in that case also adjust the attribute table reference if necessary
		bool fCopyMade = fCheckCopyMap(bm);

		if (!fCopyMade)
		{
			CreateFiles();

			WriteData(bm);

			WriteODF(bm);
			AdjustMapAndTableDomain(bm);

			CloseFiles();
		}
		else
			// Create a new domain if UniqueID, because this is specific to version 3 and up
			// Also change domain if it is internal
			// Adjust the domain of any attributes when internal domain is used
			AdjustMapAndTableDomain(bm);

		trq.Stop();
	}
	catch (...)
	{
		trq.Stop();  // make sure the Tranquilizer is stopped

		throw;
	}
}

void SegmentMapIlwis20::WriteODF(IlwisObject obj)
{
	BaseMapPtr *bm = dynamic_cast<BaseMapPtr*>(obj.pointer());

	String s;
	s = String("SegmentMap %S, generated by ILWIS 3",  fnGetForeignFile().sFile);
	ObjectInfo::WriteElement("Ilwis", "Description",   fnGetForeignFile(), s);
	ObjectInfo::WriteElement("Ilwis", "Time",          fnGetForeignFile(), Time::timCurr());
	ObjectInfo::WriteElement("Ilwis", "Version",       fnGetForeignFile(), "2.2");
	ObjectInfo::WriteElement("Ilwis", "Type",          fnGetForeignFile(), "BaseMap");

	FileName fnObject = bm->cs()->fnObj;
	fnObject.Dir(fnGetForeignFile().sPath());
	CoordSystem cs(bm->cs());
	if (File::fExist(fnObject))
		cs = CoordSystem(fnObject);
	ObjectInfo::WriteElement("BaseMap", "CoordSystem", fnGetForeignFile(), cs);
	ObjectInfo::WriteElement("BaseMap", "CoordBounds", fnGetForeignFile(), bm->cb());

	fnObject = m_dmMap->fnObj;
	fnObject.Dir(fnGetForeignFile().sPath());
	if (File::fExist(fnObject))
		m_dmMap = Domain(fnObject);
	ObjectInfo::WriteElement("BaseMap", "Domain",  fnGetForeignFile(), m_dmMap);

	if (bm->dm()->pdv() != 0)
		ObjectInfo::WriteElement("BaseMap", "Range",   fnGetForeignFile(), bm->dvrs().vr());

	ObjectInfo::WriteElement("BaseMap", "Type",        fnGetForeignFile(), "SegmentMap");

	ObjectInfo::WriteElement("SegmentMap", "Alfa",     fnGetForeignFile(), m_rAlfa);
	ObjectInfo::WriteElement("SegmentMap", "Beta1",    fnGetForeignFile(), m_rBeta1);
	ObjectInfo::WriteElement("SegmentMap", "Beta2",    fnGetForeignFile(), m_rBeta2);
	ObjectInfo::WriteElement("SegmentMap", "Type",     fnGetForeignFile(), "SegmentMapStore");

	double rTunnel, rSnap;
	ObjectInfo::ReadElement("SegmentMap", "TunnelWidth",  bm->fnObj, rTunnel);
	ObjectInfo::ReadElement("SegmentMap", "SnapDistance", bm->fnObj, rSnap);

	if (rTunnel != rUNDEF)
		ObjectInfo::WriteElement("SegmentMap", "TunnelWidth",  fnGetForeignFile(), rTunnel);
	if (rSnap != rUNDEF)
		ObjectInfo::WriteElement("SegmentMap", "SnapDistance", fnGetForeignFile(), rSnap);

	String sDataFile = FileName(m_filSegments->sName()).sRelative(true, fnGetForeignFile().sPath());
	ObjectInfo::WriteElement("SegmentMapStore", "DataSeg",     fnGetForeignFile(), sDataFile);

	// Write code file for segments only for segment maps, not for polygon maps
	if (!m_fIsPolygonMap)
	{
		sDataFile = FileName(m_filCodes->sName()).sRelative(true, fnGetForeignFile().sPath());
		ObjectInfo::WriteElement("SegmentMapStore", "DataSegCode", fnGetForeignFile(), sDataFile);
	}
	sDataFile = FileName(m_filCoords->sName()).sRelative(true, fnGetForeignFile().sPath());
	ObjectInfo::WriteElement("SegmentMapStore", "DataCrd",     fnGetForeignFile(), sDataFile);
	ObjectInfo::WriteElement("SegmentMapStore", "Status",      fnGetForeignFile(), 0);
	ObjectInfo::WriteElement("SegmentMapStore", "Format",      fnGetForeignFile(), 1);
	ObjectInfo::WriteElement("SegmentMapStore", "Segments",    fnGetForeignFile(), m_iNrSegs);
	ObjectInfo::WriteElement("SegmentMapStore", "Coordinates", fnGetForeignFile(), m_iNrCoords);
}

void SegmentMapIlwis20::CreateFiles()
{
	m_filSegments = new File(FileName(fnGetForeignFile(), ".sg#"), facCRT);
	m_filCodes = new File(FileName(fnGetForeignFile(), ".sc#"), facCRT);
	m_filCoords = new File(FileName(fnGetForeignFile(), ".cd#"), facCRT);

	m_filSegments->SetErase(true);
	m_filCodes->SetErase(true);
	m_filCoords->SetErase(true);
}

void SegmentMapIlwis20::CloseFiles()
{
	m_filSegments->SetErase(false);
	m_filCodes->SetErase(false);
	m_filCoords->SetErase(false);

	if (m_filSegments) delete m_filSegments;
	if (m_filCodes) delete m_filCodes;
	if (m_filCoords) delete m_filCoords;
}

void SegmentMapIlwis20::WriteData(IlwisObject obj)
{
	SegmentMap& bm = static_cast<SegmentMap&>(obj);
	
	WriteSegData(bm);
}

// Empty: segments don't have topology info
void SegmentMapIlwis20::WriteTopologyLinks(const Segment&)
{
}

void SegmentMapIlwis20::GetCoordinates(const Segment& seg, long& iCrd, CoordBuf& cbuf)
{
	seg.GetCoords(iCrd, cbuf);
}

void SegmentMapIlwis20::WriteSegData(SegmentMap segmap)
{
	m_rAlfa = max(segmap->cb().width(), segmap->cb().height()) / 30000;
	if (m_rAlfa < 1e-5)
		m_rAlfa = (float)1e-5;
	m_rBeta1 = segmap->cb().MinX() / 2 + segmap->cb().MaxX() / 2;
	m_rBeta2 = segmap->cb().MinY() / 2 + segmap->cb().MaxY() / 2;
	m_filCoords->Write(sizeof(float), &m_rAlfa);
	m_filCoords->Write(sizeof(float), &m_rBeta1);
	m_filCoords->Write(sizeof(float), &m_rBeta2);

	// clear control points area
	char *p = new char[20 * 8];
	ZeroMemory(p, 20 * 8);
	m_filCoords->Write(20 * 8, p);
	delete p;

	segtype st;
	String sSegVal;
	trq.SetText(SSEGTextStoringSegmentNames);

	DomainSort *pdsrt = m_dmMap->pdsrt();
	bool fCodes = (0 != pdsrt) && pdsrt->fCodesAvailable();

	Buf<crdtype> rcbuf;
	CoordBuf cbuf;

	// skip first special segment for now
	// will be filled in after last segment has been written
	m_filSegments->Seek(sizeof(segtype));
	// First segment has no code
	char pc[8];
	ZeroMemory(pc, 8);
	if (!m_fIsPolygonMap)
		m_filCodes->Write(m_dmMap->fRawAvailable() ? sizeof(long) : sizeof(double), pc);
	m_iNrSegs = 0;
	m_iNrCoords = 0;
	long iCrdIndex = 1;
	MinMax mmMap;

	long iNrTotSeg = segmap->iSeg();
	long iSegRaw;
	double rSegVal;
	Segment seg;
	for (seg = segmap->segFirst(); seg.fValid(); ++seg)
	{
		if (trq.fUpdate(m_iNrSegs, iNrTotSeg))
			return;

		ZeroMemory(&st, sizeof(segtype));
		if (!m_fIsPolygonMap)
		{
			if (m_dmMap->fRawAvailable())
			{
				rSegVal = rUNDEF;
				iSegRaw = seg.iRaw();

				if (0 == pdsrt)
					sSegVal = seg.sValue();
				else 
				{
					if (fCodes)
						sSegVal = pdsrt->sCodeByRaw(iSegRaw);
					else
						sSegVal = pdsrt->sNameByRaw(iSegRaw);

				}
			}
			else
			{
				iSegRaw = iUNDEF;
				rSegVal = seg.rValue();
			}

			sSegVal = sSegVal.sLeft(15);     // maximum length of segment names
			sSegVal = sSegVal.sTrimSpaces();
			for (short ii = 0; ii < sSegVal.length(); ii++)
				if (sSegVal[ii] == ' ')
					sSegVal[ii] = '_';
			st.code = sSegVal;
		}

		long iCrdCnt;
		GetCoordinates(seg, iCrdCnt, cbuf);
		if (iCrdCnt > rcbuf.iSize())
			rcbuf.Size(iCrdCnt);

		MinMax mmSeg;
		// convert to ILWIS 1.x internal coordinates
		for (int i = 0; i < iCrdCnt; i++)
		{
			double rCol, rRow;
			Coord c = cbuf[i];
			if (c.fUndef())
				rCol = rRow = rUNDEF;
			else 
			{
				rCol = (c.x - m_rBeta1) / m_rAlfa;
				rRow = (c.y - m_rBeta2) / m_rAlfa;
			}
			RowCol rc = RowCol(roundx(rRow), roundx(rCol));
			mmSeg += rc;
			rcbuf[i] = rc;
		}
		mmMap += mmSeg;

		long iToDo;
		long iOff = 0;
		while (iCrdCnt > 0) 
		{
			if (iCrdCnt < 990 + 10)     // '+ 10' to be certain last segment part has a least 10 points
				iToDo = iCrdCnt;
			else
				iToDo = 990;

			st.fstp = iCrdIndex;
			m_filCoords->Write((iToDo - 2) * sizeof(crdtype), &rcbuf[iOff + 1]);
			iCrdIndex += iToDo - 2 - 1;  // -2: don't count nodes
			st.lstp = iCrdIndex;
			iCrdIndex++;
			st.mm = mmSeg;
			st.fst = rcbuf[iOff];
			st.lst = rcbuf[iOff + iToDo - 1];

			m_filSegments->Write(sizeof(segtype), &st);
			if (!m_fIsPolygonMap)
			{
				if (m_dmMap->fRawAvailable())
					m_filCodes->Write(sizeof(long), &iSegRaw);
				else
					m_filCodes->Write(sizeof(double), &rSegVal);
			}
			else
				WriteTopologyLinks(seg);
			m_iNrSegs++;
			m_iNrCoords += iToDo;

			iOff += iToDo - 1;  // node needs to be done twice
			iCrdCnt -= iToDo;
			if (iCrdCnt > 0) iCrdCnt++;  // '++' because intermediate node needs to be done twice
		}

		if (m_iNrSegs > 32000)
		{
			// not all segments have been exported, so:
			//   extend the calculated boundary of the map to avoid
			//   that points are located exactly on the boundary
			mmSeg.rcMin.Col -= mmSeg.width() / 20;
			mmSeg.rcMin.Row -= mmSeg.height() / 20;
			mmSeg.rcMax.Col += mmSeg.width() / 20;
			mmSeg.rcMax.Row += mmSeg.height() / 20;
			String sWarn = String(SCVWarnTooManySegments_I.scVal(), m_iNrSegs);
			getEngine()->Message(sWarn.scVal(),
								SSEGTitleExport20Segments.scVal(),
								MB_OK | MB_ICONEXCLAMATION);
			break;
		}
		else
		{
			// all segments have been exported, therefore use the original
			// map boundary
			CoordBounds cb = segmap->cb();
			mmSeg.rcMin.Col = (cb.cMin.x - m_rBeta1) / m_rAlfa;
			mmSeg.rcMin.Row = (cb.cMin.y - m_rBeta2) / m_rAlfa;
			mmSeg.rcMax.Col = (cb.cMax.x - m_rBeta1) / m_rAlfa;
			mmSeg.rcMax.Row = (cb.cMax.y - m_rBeta2) / m_rAlfa;
		}
  }

	// write ILWIS 2.x segment map information record
	ZeroMemory(&st, sizeof(segtype));
	st.code = "";
	st.fst = crdtype(0, 1); // nr of control points, format version == 1
	st.lst = crdtype(0, m_iNrSegs);
	st.mm = mmMap;
	st.fstp = 0;  // not used
	st.lstp = m_iNrCoords; // nr of coords written;

	m_filSegments->Seek(0);
	m_filSegments->Write(sizeof(segtype), &st);
}

CoordSystem SegmentMapIlwis20::GetCoordSystem()
{
	return CoordSystem();
}
