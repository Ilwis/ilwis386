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
#ifndef IMPORTGARTRIP_H
#define IMPORTGARTRIP_H

#include "Engine\DataExchange\IMPVEC.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\RECSTRUC.H"

class GartripText
{
public:
	GartripText(const FileName& fnFile, const FileName& fnObject, const String& sOptions, Tranquilizer&, CWnd*);
	~GartripText();
	void Import();
	static bool _export fIsGartripFormat(String sFileName, string &sCoordSysInfo);
	
private:
	enum iColumnType {iSTRING, iVALUE, iTIME};
	static bool fStripNextToken(CString &csInOut, String &sTokenOut);
	static bool fToUTMZone(String sUTMString, int &iZone, bool &fNorth);
	bool fGetCoordsFromStrings(String sFirstCoord, String sSecondCoord, double & rFirst, double & rSecond);
	void SetUTMZone(CoordSystemProjection* csyPrj, String sUTM);
	void ParseHeaderLines(CStdioFile &fileIn);
	void AddNewColumn(Table & tbl, String sName, iColumnType iType, int iColNr);
	void SetTranquilizer();
	bool fUpdateTranquilizer(CFile &);
	void AddSegment(long iRec, ArrayLarge<Coord> &cPoints, SegmentMap& segMap);
	bool fColHasValues(int iCol);
	bool fParseFirstDouble(String sString, double & rValue);
	void MakeUTMCoordSystem(FileName fnCoord, CoordSystem& cs);
	void CreateCoordinateSystems();
	iColumnType iColTypes(int iCol);
	FileName fnIn;
	FileName fnOut;
	long iFnInSize;
	CoordSystem csyDest;
	CoordSystem csySrc;
	CoordSystemProjection* csyPrjSrc;
	CoordSystemProjection* csyPrjDest;
	bool fUsesUTM;
	bool fUsesLatLon;
	bool fWaypoints;
	int iAltitudeCol;
	int iFlagCol;
	int iDescriptionColWithValues;
	double rHeightOffset;
	vector<String> arColNames;
	vector<Column> arColumns;
	map <String, Coord> mpPointCollection;
	Tranquilizer&     trq;
	CWnd              *m_wnd;
};

#endif IMPORTGARTRIP_H
