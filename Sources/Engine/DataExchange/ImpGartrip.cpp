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
#define IMPGARTRIP_C
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\IMPGartrip.H"
#include "Engine\SpatialReference\Csproj.h"
#include "Engine\SpatialReference\UTM.H"
#include "Engine\Domain\DomainUniqueID.h"

const char cGartripDelimiters[] = ";,	"; // semicolon, comma, tab (null terminated, therefore 4 positions)

// special string constants:
// for determining .txt file type
const char sTRACK[] = "Track";
const char sWAYPOINTS[] = "Waypoints";
// delimiter for routes
const char sROUTE[] = "Route";
const char sEND[] = "<end>";
// "hint" from first headerline on "csy == utm?"
const char sUTM[] = "UTM";
// column names
const char sNAME[] = "Name";
const char sDESCRIPTION[] = "Description";
const char sDEPTH[] = "Depth";
const char sZONE[] = "Zone";
const char sLATITUDE[] = "Latitude";
const char sLONGITUDE[] = "Longitude";

const int iMAX_COLUMNS = 50; // a typical gartrip file has less than 10 columns ...

GartripText::GartripText(const FileName& fnFile, const FileName& fnObject, const String& sOptions, Tranquilizer& _trq, CWnd* w)
  : trq(_trq)
	, m_wnd(w)
	, fnIn(fnFile)
	, fnOut(fnObject)
	, rHeightOffset(0.0)
	, csyPrjSrc(0)
	, csyPrjDest(0)
	, fUsesUTM(false)
	, fUsesLatLon(false)
	, fWaypoints(false)
	, iAltitudeCol(-1)
	, iFlagCol(-1)
	, iDescriptionColWithValues(-1)
{
	mpPointCollection.clear();
	Array<String> as;
	Split(sOptions, as);
	// The command-line arguments are (besides the in-filename and the out-filename):
	// - an offset for height correction (optional)

	if (as.size() > 0)
	{
		double rTemp;
		if (0 < sscanf(as[0].c_str(), "%lf", &rTemp))
			rHeightOffset = rTemp;
	}
}

GartripText::~GartripText()
{
}

void GartripText::Import()
{
	CStdioFile fileIn (fnIn.sFullPath().c_str(), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);
	CFileStatus rStatus;
	fileIn.GetStatus(rStatus);
	iFnInSize = rStatus.m_size;

	CString csLineBuffer;

	ParseHeaderLines(fileIn);

	CreateCoordinateSystems();

	SetTranquilizer();

	// Create the Point Map
  FileName fnPointMap (fnOut, ".mpp", true);
	Domain dmPointMap(fnPointMap, 0, dmtUNIQUEID, "Pnt");
	CoordBounds cbMap (csyDest->cb); //temporary .. needs to be set according to the real coords
  PointMap pntMap(fnPointMap, csyDest, cbMap, dmPointMap);
	pntMap->fErase = true; // auto-delete if something goes wrong

	vector <Coord> vCoord; // for temporarily inserting the point map coordinates (performance)

	// Create the Attribute Table
	FileName fnTable (fnOut, ".tbt", true);
	Table tbl = Table(fnTable, dmPointMap);
  String sD ("Attribute table for point map %S%S", fnTable.sFile, fnTable.sExt);
  tbl->sDescription = sD;
	pntMap->SetAttributeTable(tbl);
	tbl->fErase = true; //auto-delete if something goes wrong

	typedef vector<String> ColData;
	vector<ColData> arTableData; // for temporarily inserting the table data (performance)
	arTableData.resize(iMAX_COLUMNS);

	// Create the Segment Map
	Domain dmSegmentMap (FileName(fnOut, ".dom", true), 0, dmtID);
	DomainIdentifier* dmIdentifierPtr = dmSegmentMap->pdid();
	dmSegmentMap->fErase = true; // auto-delete if something goes wrong
	
  DomainValueRangeStruct dvsSeg(dmSegmentMap);
	FileName fnSegmentMap (fnOut, ".mps", true);
  SegmentMap segMap(fnSegmentMap, csyDest, cbMap, dvsSeg);
	segMap->fErase = true; // auto-delete if something goes wrong
	ArrayLarge<Coord> cPoints; // array to hold points for the segment map
	cPoints.Reset();

	cbMap = CoordBounds(); // reset to <empty>

	long iRec = 1; // record counter, first record == 1

	long iSegRec = 0; // record counter for segments, first record == 1, iSegRec will be increased before adding the first segment
	long iSegID = 0; // ID counter for segment id's
	long iTrackID = 0; // ID counter for track id's

	bool fSegmentsFound = !fWaypoints; // Prevent unnecessarily creating a segment map

	bool fAborted = false; // To delete the objects when cancelling

	// Track files should always result in a segment map
	// Waypoint files may result in a segment map, depending on the "Route" statements

	// columns Waypoints file:
	// Name
	// Description
	// UTM zone (only if format = UTM or British Grid or Irish Grid)
	// Latitude OR Easting
	// Longitude OR Northing
	// Creation time - optional
	// Symbol code - optional
	// Altitude (meters) - optional
	// columns Track file:
	// UTM zone (only if format = UTM or British Grid or Irish Grid)
	// Latitude OR Easting
	// Longitude OR Northing
	// Creation time - optional
	// Flag - optional
	// Description - optional
	// Altitude - optional

	// Main import loop (a second one follows for the optional Routes in waypoints)
	while (fileIn.ReadString(csLineBuffer) &&
		(0 != csLineBuffer.Left(5).CompareNoCase(sROUTE)) &&
		(!(fAborted = fUpdateTranquilizer(fileIn)))) // side effect: trq result is stored
	{
		if (csLineBuffer.GetLength() == 0)
			continue; // ignore empty lines
		
		int iCurrentCol = 0;
		String sName;
		if (fWaypoints)
		{
			// extra compulsory columns: Name, Description
			fStripNextToken(csLineBuffer, sName);
			String sDescription;
			fStripNextToken(csLineBuffer, sDescription);
			if (1 == iRec) // first record
			{
				AddNewColumn(tbl, sNAME, iSTRING, iCurrentCol); // Name

				if (iColTypes(iCurrentCol + 1) == iVALUE)
					AddNewColumn(tbl, sDEPTH, iVALUE, iCurrentCol + 1); // Depth
				else
					AddNewColumn(tbl, sDESCRIPTION, iSTRING, iCurrentCol + 1); // Description
			}
			arTableData[iCurrentCol++].push_back(sName);
			if (iCurrentCol == iDescriptionColWithValues)
			{
				double rValue;
				if (fParseFirstDouble(sDescription, rValue))
					sDescription = String("%lf", rValue);
			}
			arTableData[iCurrentCol++].push_back(sDescription);
		}

		if (fUsesUTM)
		{
			String sUTMZone;
			fStripNextToken(csLineBuffer, sUTMZone);
			if (1 == iRec) // first record
				SetUTMZone(csyPrjDest, sUTMZone); // for dest the zone is changed
			SetUTMZone(csyPrjSrc, sUTMZone);
			++iCurrentCol; // we don't want to store this
		}
		String sFirstCoord;
		fStripNextToken(csLineBuffer, sFirstCoord);
		String sSecondCoord;
		fStripNextToken(csLineBuffer, sSecondCoord);
		double rFirst, rSecond;
		Coord cPoint;
		if (fGetCoordsFromStrings(sFirstCoord, sSecondCoord, rFirst, rSecond))
		{
			// append the current point to the vector
			cPoint = Coord(rFirst, rSecond);
		}
		vCoord.push_back(cPoint);
		
		cbMap += Coord(rFirst, rSecond);

		if (fWaypoints)
			mpPointCollection[sName] = Coord(rFirst, rSecond);

		// done with the compulsory columns .. now the optional ones

		iCurrentCol += 2; // we don't want to store the coords in the table
		// at this point iCurrentCol should be: fWaypoints ? (fUsesUTM ? 5 : 4) : (fUsesUTM ? 3 : 2);
		String sTokenBuffer;
		while (fStripNextToken(csLineBuffer, sTokenBuffer))
		{
			if (1 == iRec) // first record
				AddNewColumn(tbl, arColNames[iCurrentCol], iColTypes(iCurrentCol), iCurrentCol);
			if ((iCurrentCol == iAltitudeCol) && (rHeightOffset != 0.0))
			{
				double rAltitude;
				if (1 == sscanf(sTokenBuffer.c_str(), "%lf", &rAltitude))
					sTokenBuffer = String("%lf", rAltitude + rHeightOffset);
			}
			else if (iCurrentCol == iDescriptionColWithValues)
			{
				double rDepth;
				if (fParseFirstDouble(sTokenBuffer, rDepth))
					sTokenBuffer = String("%lf", rDepth);
			}
			else if (iCurrentCol == iFlagCol && !fWaypoints)
			{
				// Trackfile .. check if we need to close the segment
				int iFlag;
				if (1 == sscanf(sTokenBuffer.c_str(), "%d", &iFlag))
					if (iFlag != 0)
					{
						if (cPoints.iSize() > 0)
						{
							// note that the current point with Flag == iFlag isn't added yet
							AddSegment(iSegRec, cPoints, segMap);
							cPoints.Reset();
						}
						switch (iFlag)
						{
						case 1: // new segment
							++iSegID;
							break;
						case 2: // new track
							++iTrackID;
							break;
						case 3: // new segment and track
							++iSegID;
							++iTrackID;
							break;
						}
						dmIdentifierPtr->iAdd(String("Track %d, Segment %d", iTrackID, iSegID));
						++iSegRec; // points to the rec just added in dmIdentifierPtr
					}
			}
			arTableData[iCurrentCol++].push_back(sTokenBuffer);
		}

		if (!fWaypoints) // Trackfile .. append point to cPoints
			cPoints &= Coord(rFirst, rSecond);

		++iRec;
	} // EOF or encountered "Route" for Waypoints file

	// still append any remaining segment
	if (cPoints.iSize() > 0)
		AddSegment(iSegRec, cPoints, segMap);

	// Now we can set the right coordbounds to the pnt and seg map
	pntMap->SetCoordBounds(cbMap);
	segMap->SetCoordBounds(cbMap);

	long iTotalRecords = iRec - 1;

	// Now we have the final record count - we can resize the domain and the attribute table
	DomainSort *pds = dmPointMap->pdsrt();
	pds->Resize(iTotalRecords);
	tbl->CheckNrRecs(); // grow the attribute table to the size of its domain

	// fill the attribute table with the data stored temporarily
	for (int iCurrentCol = 0; iCurrentCol < iMAX_COLUMNS; ++iCurrentCol)
	{
		if (arColumns[iCurrentCol].fValid()) // arColumns has "gaps" (administration was easier)
		{
			for (int iCurrentRec = 1; iCurrentRec <= iTotalRecords; ++iCurrentRec)
				arColumns[iCurrentCol]->PutVal(iCurrentRec, arTableData[iCurrentCol][iCurrentRec - 1]);
		}
	}

	if (!fAborted)
	{
		// copy vector to Buf here, then insert into pointmap
		LongBuf lBuf; // buffer for iRaw's that are inserted into the pointmap at once
		CoordBuf cBuf; // buffer for Coords that are inserted into the pointmap at once
		lBuf.Size(iTotalRecords);
		cBuf.Size(iTotalRecords);
		for (int i=0; i<iTotalRecords; ++i)
		{
			cBuf[i]=vCoord[i];
			lBuf[i]=i+1; // the iRaw values that start at 1
		}

		pntMap->PutBufRaw(cBuf, lBuf);
	}

	if (!fAborted && fWaypoints && (0 == csLineBuffer.Left(5).CompareNoCase(sROUTE)))
	{
		// extra handling for "Routes"
		fSegmentsFound = true;
		trq.SetText(TR("Importing Routes"));
		cPoints.Reset();
		bool fRouteNameFollows = true;
		// secondary import loop for the routes
		do
		{
			if (0 == csLineBuffer.Left(5).CompareNoCase(sROUTE))
			{
				// new route found .. add the previously stored points to the segment map and start afresh
				if (cPoints.iSize() > 0)
				{
					AddSegment(iSegRec, cPoints, segMap);
					cPoints.Reset();
				}
				String sRouteName;
				fStripNextToken(csLineBuffer, sRouteName); // the word "Route"
				fStripNextToken(csLineBuffer, sRouteName); // the name / identifier
				dmIdentifierPtr->iAdd(sRouteName);
				++iSegRec; // points to the rec just added in dmIdentifierPtr
			}
			else
			{
				String sWaypointName;
				fStripNextToken(csLineBuffer, sWaypointName);
				if (mpPointCollection.find(sWaypointName) != mpPointCollection.end())
					cPoints &= mpPointCollection[sWaypointName];
			}
		}
		while (fileIn.ReadString(csLineBuffer) && (!(fAborted = fUpdateTranquilizer(fileIn))));

		// still add the last segment
		if (cPoints.iSize() > 0)
			AddSegment(iSegRec, cPoints, segMap);
	}

	if (!fAborted)
	{
		if (fUsesUTM)
			if (csyDest.fValid())
				csyDest->fErase = false;
		if (pntMap.fValid()) pntMap->fErase = false;
		if (tbl.fValid()) tbl->fErase = false;
		if (fSegmentsFound)
		{
			if (dmSegmentMap.fValid()) dmSegmentMap->fErase = false;
			if (segMap.fValid()) segMap->fErase = false;
		}
		trq.SetText(TR("Storing imported data..."));
	}
}

bool GartripText::fStripNextToken(CString &csInOut, String &sTokenOut)
{
	// expected format of csIn: <a token><a delimiter><a token><a delimiter> ...
	// return: true = success, false = eol (no token)
	// csInOut becomes <a token><a delimiter> ... (first token AND delimiter stripped off)
	// sTokenOut becomes <a token> ... (the first token, without delimiter)
	if (csInOut.GetLength() > 0)
	{
		int iNextDelimiterPos = csInOut.FindOneOf(cGartripDelimiters);
		// iNextDelimiterPos can be -1 (last token) or >0 (next delimiter found)
		if (iNextDelimiterPos >= 0)
		{
			sTokenOut = String(csInOut.Left(iNextDelimiterPos));
			csInOut = csInOut.Right(csInOut.GetLength() - iNextDelimiterPos - 1);
		}
		else
		{
			sTokenOut = String(csInOut);
			csInOut = "";
		}
		sTokenOut = sTokenOut.sTrimSpaces(); // extra service of this function
		return true;
	}
	else
	{
		// extra service to return an empty token:
		// most likely the caller would forget to check the bool result, so that the previous
		// token is repeated
		sTokenOut = "";
		return false; // eol
	}
}

bool GartripText::fGetCoordsFromStrings(String sFirstCoord, String sSecondCoord, double & rFirst, double & rSecond)
{
	if ((1 == sscanf(sFirstCoord.c_str(), "%lf", &rFirst)) && (1 == sscanf(sSecondCoord.c_str(), "%lf", &rSecond)))
	{
		// basically rFirst and rSecond have their corresponding values here
		// depending on conditions, several adjustments may be needed
		if (!fUsesLatLon)
		{
			// projected cs .. gartrip unit is km
			rFirst *= 1000.0; // km to m conversion
			rSecond *= 1000.0; // km to m conversion
			if (fUsesUTM &&
				((csyPrjSrc->prj->fGetHemisphereN() != csyPrjDest->prj->fGetHemisphereN()) ||
				(csyPrjSrc->prj->iGetZoneNr() != csyPrjDest->prj->iGetZoneNr())))
			{
				Coord c (rFirst, rSecond);
				Coord cConverted (csyDest->cConv(csySrc, c)); // from src to dest
				rFirst = cConverted.x;
				rSecond = cConverted.y;
			}
		}
		else // LatLon - swap them .. in the gartrip txt file they're in the wrong order
		{
			double rTemp = rFirst;
			rFirst = rSecond;
			rSecond = rTemp;
		}
		return true;
	}
	else
	{
		// try Lat Lon .. prepare the strings
		// Note that the LatLon::rDegree(String) in dat2.h does not work as we need it
		sFirstCoord = sFirstCoord.sTrimSpaces();
		sSecondCoord = sSecondCoord.sTrimSpaces();
		char cHeadFirst (sFirstCoord[0]);
		char cHeadSecond (sSecondCoord[0]);
		String sNSEW ("NnSsEeWw");
		if ((sNSEW.iPos(cHeadFirst) != shUNDEF) && (sNSEW.iPos(cHeadSecond) != shUNDEF))
		{
			sFirstCoord = sFirstCoord.sSub(1, sFirstCoord.length() - 1);
			sSecondCoord = sSecondCoord.sSub(1, sSecondCoord.length() - 1);
		}
		String sNegatives ("SsWw");
		int iSignFirst = (sNegatives.iPos(cHeadFirst) != shUNDEF) ? -1 : 1;
		int iSignSecond = (sNegatives.iPos(cHeadSecond) != shUNDEF) ? -1 : 1;
		String sThreeDoubles ("%lf%*[^-0-9]%lf%*[^-0-9]%lf");
		double rDeg1, rMin1, rSec1;
		double rDeg2, rMin2, rSec2;
		int iScannedFirst = sscanf(sFirstCoord.c_str(), sThreeDoubles.c_str(), &rDeg1, &rMin1, &rSec1);
		int iScannedSecond = sscanf(sSecondCoord.c_str(), sThreeDoubles.c_str(), &rDeg2, &rMin2, &rSec2);
		if ((iScannedFirst == 3) && (iScannedSecond == 3))
		{
			// LatLon case: swap them .. in the gartrip txt file they're in the wrong order
			rSecond = iSignFirst * (rDeg1 + rMin1 / 60.0 + rSec1 / 3600.0);
			rFirst = iSignSecond * (rDeg2 + rMin2 / 60.0 + rSec2 / 3600.0);
			return true;
		}
		else if ((iScannedFirst == 2) && (iScannedSecond == 2))
		{
			// LatLon case: swap them .. in the gartrip txt file they're in the wrong order
			rSecond = iSignFirst * (rDeg1 + rMin1 / 60.0);
			rFirst = iSignSecond * (rDeg2 + rMin2 / 60.0);
			return true;
		}
		else if ((iScannedFirst == 1) && (iScannedSecond == 1))
		{
			// LatLon case: swap them .. in the gartrip txt file they're in the wrong order
			rSecond = iSignFirst * rDeg1;
			rFirst = iSignSecond * rDeg2;
			return true;
		}
		else
			return false;
	}
}

void GartripText::SetUTMZone(CoordSystemProjection* csyPrj, String sUTM)
{
	int iZone;
	bool fNorth;
	if ((0 != csyPrj) && fToUTMZone(sUTM, iZone, fNorth))
	{
		csyPrj->prj->Param(pvZONE, (long)iZone);
		csyPrj->prj->Param(pvNORTH, (long)(fNorth ? 1 : 0));
	}
}

void GartripText::ParseHeaderLines(CStdioFile &fileIn)
{
	// decide if we're dealing with a "Waypoints" or a "Track" file
	CString csLineBuffer;
	fileIn.ReadString(csLineBuffer); // Headerline 1 of 2 (type of file)
	if (csLineBuffer.Find(sTRACK) >= 0)
		fWaypoints = false;
	else if (csLineBuffer.Find(sWAYPOINTS) >= 0)
		fWaypoints = true;
	else
	{
		String sErr (TR("Not a valid GARtrip file"));
		throw ErrorObject(WhatError(sErr, errFORMERROR));
	}

	// find out if we're dealing with UTM
	if ((csLineBuffer.Find(sUTM) >= 0))
		fUsesUTM = true; // there's one more chance with the 2nd header if detection here fails

	fileIn.ReadString(csLineBuffer); // Headerline 2 of 2 (column names) - optional???

	arColNames.resize(iMAX_COLUMNS);
	arColumns.resize(iMAX_COLUMNS);

	String sToken;
	int iCurrentCol = 0; // first col = 0
	while (fStripNextToken(csLineBuffer, sToken))
	{
		arColNames[iCurrentCol++] = sToken;
		if (fCIStrEqual(sToken, sZONE))
			fUsesUTM = true;
		else if (fCIStrEqual(sToken, sLATITUDE) || fCIStrEqual(sToken, sLONGITUDE))
			fUsesLatLon = true;
	}

	iAltitudeCol = 5 + (fWaypoints?1:0) + (fUsesUTM?1:0);

	if (!fWaypoints)
		iFlagCol = 3 + (fUsesUTM?1:0);
}

GartripText::iColumnType GartripText::iColTypes(int iCol)
{
	// call only once for the description column as this call is "expensive"
	// call for description column before using the var iDescriptionColWithValues that is set here
	if (fWaypoints)
	{
		switch (iCol - (((iCol > 2) && fUsesUTM)?1:0))
		{
		case 1:
			if (fColHasValues(iCol))
			{
				iDescriptionColWithValues = iCol;
				return iVALUE;
			}
			else
				return iSTRING;
			break;
		case 4:
			return iTIME;
			break;
		case 5:
		case 6:
			return iVALUE;
			break;
		default:
			return iSTRING;
		}
	}
	else // Track
	{
		switch (iCol - (((iCol > 0) && fUsesUTM)?1:0))
		{
		case 2:
			return iTIME;
			break;
		case 3:
			return iVALUE;
			break;
		case 4:
			if (fColHasValues(iCol))
			{
				iDescriptionColWithValues = iCol;
				return iVALUE;
			}
			else
				return iSTRING;
			break;
		case 5:
			return iVALUE;
			break;
		default:
			return iSTRING;
		}
	}
}

void GartripText::CreateCoordinateSystems()
{
	csyDest = CoordSystem("unknown");
	csySrc = csyDest; // Default: use the same csy for src and dest
	if (fUsesUTM)
	{
		FileName fnCoordDest(fnOut, ".csy", true);
		MakeUTMCoordSystem(fnCoordDest, csyDest);
		csyPrjDest = csyDest->pcsProjection();
		csyDest->fErase = true; // auto-delete if something goes wrong (don't forget to set it to false if import went ok)
		FileName fnCoordSrc (FileName::fnUnique(String("%S__xx1.csy", fnOut.sFile))); // note that we don't use the same name as csyDest - as the file does not exist, it'll get the same name, and auto-delete csyDest after completion
		MakeUTMCoordSystem(fnCoordSrc, csySrc);
		csyPrjSrc = csySrc->pcsProjection();
		csySrc->fErase = true; // we won't need it anymore when the import is done
		// Zone and Hemisphere will be set later ...
	}
	if (!(csySrc.fValid() && csyDest.fValid()))
	{
		String sErr (TR("Error in coordinate system"));
		throw ErrorObject(WhatError(sErr, errFORMERROR));
	}
}

void GartripText::SetTranquilizer()
{
	if (fWaypoints)
	{
    trq.SetTitle(TR("Import GARtrip text file"));
    trq.SetText(TR("Importing Waypoints"));
	}
	else // Tracks
	{
    trq.SetTitle(TR("Import GARtrip text file"));
    trq.SetText(TR("Importing Tracks"));
	}
	trq.SetOnlyGauge(true); // otherwise the current file pos is displayed, misleading us to think that we have so many tracks
}

void GartripText::AddNewColumn(Table & tbl, String sName, iColumnType iType, int iColNr)
{
	ValueRange vr;
	String dom;
	switch (iType)
	{
		case iVALUE:
			dom = "value";
			vr = ValueRange(Domain("value"));
			break;
		case iTIME:
			dom = "value";
			vr = ValueRange(0, 9999999999L); // should be able to hold the # of secs since 13-dec-1989 00:00:00
			// 9999999999 is enough for 317 years - til 2306
			break;
		default:
			dom = "string";
			break;
	}
	FileName fnDom(dom, ".dom", true);
	Domain dm(fnDom);
	DomainValueRangeStruct dvs(dm, vr);
	Column col(tbl, sName.sQuote(), dvs);
	arColumns[iColNr] = col;
}

bool GartripText::fUpdateTranquilizer(CFile & file)
{
	long iPos = file.GetPosition();
	return trq.fUpdate(iPos, iFnInSize);
}

void GartripText::AddSegment(long iRec, ArrayLarge<Coord> &cPoints, SegmentMap& segMap)
{
	// adds a new segment to segMap with value=iRec
	long iSize = cPoints.iSize();
	CoordBuf cBuf(iSize);
	ILWIS::Segment *segCur = CSEGMENT(segMap->newFeature());
	
	// this code is needed to convert the Array to a Buf
	long iCount=0;
	for(; iCount<iSize; ++iCount)
		cBuf[iCount]=cPoints[iCount];

	segCur->PutCoords(iCount, cBuf);
	segCur->PutVal(iRec);
}

bool GartripText::fColHasValues(int iCol)
{
	CStdioFile fileIn (fnIn.sFullPath().c_str(), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);
	CString csLineBuffer;
	int iLinesRead = -2; // to skip the headers
	bool fTryMoreLines = true;
	bool fValues = false;

	while (fileIn.ReadString(csLineBuffer) && fTryMoreLines)
	{
		if (iLinesRead >= 0) // now we're at the data section
		{
			String sColVal;
			for (int i=0; i<=iCol; ++i)
				fStripNextToken(csLineBuffer, sColVal);
			// col nr. iCol from csLineBuffer is in sColVal

			if (sColVal.length() > 0)
			{
				fTryMoreLines = false; // one line is enough
				double rValue;
				if (fParseFirstDouble(sColVal, rValue))
					fValues = true;
			}
		}
		++iLinesRead;
	}
	return ((iLinesRead > 0) && fValues);
}

void GartripText::MakeUTMCoordSystem(FileName fnCoord, CoordSystem& cs)
{
	CoordSystemProjection *	cspr = new CoordSystemProjection(fnCoord, 1);
	cs.SetPointer(cspr);
	cspr->datum = new MolodenskyDatum("WGS 1984","");
	cspr->ell = cspr->datum->ell;
		
	Projection prj = cspr->prj;
	String sPrj ("UTM");
	prj = Projection(sPrj, cspr->ell);
	cspr->prj = prj;
}

bool GartripText::fParseFirstDouble(String sString, double & rValue)
{
	return (1 == sscanf(sString.c_str(), "%*[^-0-9]%lf", &rValue));
}

bool GartripText::fIsGartripFormat(String sFileName, string &sCoordSysInfo)
{
	sCoordSysInfo = "";
	FileName fnFile (sFileName);
	if (fnFile.fExist())
	{
		CStdioFile fileIn (fnFile.sFullPath().c_str(), CFile::modeRead|CFile::shareDenyNone|CFile::typeText);
		CString csLineBuffer;
		if (fileIn.ReadString(csLineBuffer))
		{
			bool fWaypoint = false;
			if ((csLineBuffer.Find(sTRACK) >= 0) || (fWaypoint = (csLineBuffer.Find(sWAYPOINTS) >= 0)))
			{
				// Right! we're dealing with a GARtrip file .. return true at end of this block
				int iFirstSpace = csLineBuffer.Find(' ');
				csLineBuffer = csLineBuffer.Right(csLineBuffer.GetLength() - iFirstSpace - 1);
				// now csLineBuffer containts the csy info
				sCoordSysInfo = String(csLineBuffer).sTrimSpaces();
				// if we're dealing with UTM, find the zone
				if ((csLineBuffer.Find(sUTM) >= 0))
				{
					int iZoneCol = fWaypoint ? 2 : 0;
					fileIn.ReadString(csLineBuffer);
					if (fileIn.ReadString(csLineBuffer)) // first data line
					{
						String sUTM;
						for (int i=0; i<=iZoneCol; ++i)
							fStripNextToken(csLineBuffer, sUTM);
						// col nr. iZoneCol from csLineBuffer is in sUTM
						int iZone;
						bool fNorth;
						if (fToUTMZone(sUTM, iZone, fNorth))
						{
							String sAppend ("; Zone %d %s", iZone, fNorth ? "North" : "South");
							sCoordSysInfo += sAppend;
						}
					}
				}
				return true; // because the format was correct
			}
		}
	}
	return false; // it was not a gartrip file
}

bool GartripText::fToUTMZone(String sUTMString, int &iZone, bool &fNorth)
{
	// expected format for sUTM: nnC or nC where n is a digit and C is a character
	char cBand;
	if (2 == sscanf(sUTMString.c_str(), "%d%c", &iZone, &cBand))
	{
		// cBand in A..M: south; cBand in N..Z: north
		fNorth = (('n' <= cBand) && (cBand <= 'z'))||(('N' <= cBand) && (cBand <= 'Z'));
		return true;
	}
	else
		return false; // could have side effects on iZone of fNorth if false (due to sscanf)!
}

void ImpExp::ImportGartrip(const FileName& fnFile, const FileName& fnObject, const String& sOptions)
{
	try 
	{
		trq.SetTitle(TR("Import GARtrip text file"));
		GartripText gt(fnFile, fnObject, sOptions, trq, win);
		
		gt.Import();
	}
	catch (AbortedByUser& ) {  // just stop, no message
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}
