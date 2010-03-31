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
/* $Log: /ILWIS 3.0/Import_Export/IMPE00.H $
 * 
 * 7     8/17/01 14:05 Willem
 * The end-of-file detection mechanism skipped empty lines. However this
 * is not allowed in fixed size records (the entire IFO section). In the
 * IFO section empty lines are now properly interpreted.
 * 
 * 6     7/24/01 12:00 Willem
 * - E00 import now also handles compressed E00 files properly
 * - Added saveguard against files that end with a line only containing an
 * EOF character (important in case of split e00 files using .e01 .e02
 * files etc)
 * - It is possible in e00 attribute table that duplicate fiels are
 * included. The import will now make the column names unique before
 * adding them
 * 
 * 5     8/11/00 17:24 Willem
 * Extended e00 format command line with etxra option. This allows to
 * specify on the command line wether to create polygons or not.
 * 
 * 4     17/02/00 14:14 Willem
 * The messagebox function now use the impexp.win member as the window
 * instead of the desktop; this prevents the messagebox to disappear
 * behind other windows.
 * 
 * 3     9-09-99 4:36p Martin
 * correct some ported 2.22 linking problems
 * 
 * 2     9-09-99 2:40p Martin
 * ported 2.22 stuff
// Revision 1.9  1999/03/03 10:11:10  willem.ilwis.itc
// - Bugs 956, 957 and 958 experienced symptoms of the same problem. Part of
//   bug 1030 was also affected. The E00 attribute info has two fields to
//   indicate how many columns are in a table. They may be different: the first
//   one counts the active number of columns, the second the total number of
//   columns. This distinction was not taken into account in E00 import: the
//   active number of columns was also taken as the total number. This resulted
//   in wrong attributes or even crashes.
//   The import now correctly parses this info.
// - Changed the error message when an unrecognized tag is found to be clearer
// - Removed a bug in parsing of the boundary rectangle coordinates: the
//   double precision case was not handled properly.
// - Added detection of yet another tag: MSK. This section is skipped
// - Added support for single table import. The E00 file only contains a single
//   table in the INFO section. It is read into a regular ILWIS table.
//
// Revision 1.8  1998/06/11 19:27:52  Willem
// Attributes are now linked correctly to the segment maps. Superfluous
// domain items are also removed from the segment map domain.
//
// Revision 1.7  1998/05/12 19:30:21  Willem
// - Updated support for GRD (Raster grid): it is now included
// - The import will recognize TX7 tag
// - Attributes are now also imported
// - Segments are now scanned for duplicate points, that can be created
//   when very small details are in the E00 file
// - E00 file without attributes, will still give attribute tables in ILWIS,
//   with only one column: <filename>_ID
//
// Revision 1.6  1998/03/30 16:28:18  Willem
// Added the GRD-EOG recognition (Grid file): it is now skipped
//
// Revision 1.5  1998/03/23 20:51:27  Willem
// Several changes, see RLOG of revision 1.7 of impe00.c
//
// Revision 1.4  1998/03/13 15:28:48  Willem
// Many changes in E00 import: see description in impe00.c for revision 1.6
//
// Revision 1.3  1997/11/27 22:14:46  Willem
// Double precision takes now the correct number of digits for a double;
// Added TXT file E00 tag (was undocumented)
//
// Revision 1.2  1997/08/09 19:49:04  Willem
// Extended for reading attributes
//
/* impe00.h
   Import from Arc/Info segments .E00
   by Willem Nieuwenhuis, march 27, 1997
   ILWIS Department ITC
   Last change:  WN   12 May 98   11:15 am
*/

/*
  Revision history
  27-03-97: Inception
*/
#ifndef IMPORTE00_H
#define IMPORTE00_H

#include "Engine\DataExchange\IMPVEC.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\DataExchange\RECSTRUC.H"

enum Actions    { actGetHeader, actGetData, actGetDetail, actNextFile, actStoreElement, actFlush };
enum AIFileType { aiEXP, aiARC, aiCNT, aiGRD, aiLAB, aiLOG, aiMSK, aiPAL, aiPAR, aiPFF, aiPRJ, aiSIN, aiTOL, aiTXT,
aiIFO, aiAAT, aiACODE, aiBND, aiPAT, aiPCODE, aiTIC, aiEOF, aiNONE };

struct AIBaseInfo {
	long            iArcID;        // internal arc/info ID
	virtual void    Init() { iArcID = -1; }
};

struct AISegInfo : public AIBaseInfo {
	long            iTopID;     // user defined ID
	long            iBeginNode;
	long            iEndNode;
	long            iPolLeft;
	long            iPolRight;
	long            iNrCoord;
	long            iCount;     // Coordinate counter for the segment
};

struct AIPntInfo : public AIBaseInfo {
	long            iEnclosingPol;
	Coord           crdPoint;
};

struct AIPolInfo : public AIBaseInfo {  // interpret iArcID to be the number of arc's
	CoordBounds     cbPol;                // bounding rectangle of polygon
};

class ArcInfoE00
{
public:
	ArcInfoE00(const FileName fnFile, const String& sOptions, Tranquilizer&, CWnd*);
	~ArcInfoE00();
	
	long              ScanInfo();
	void              Convert(const FileName&);
private:
	void              MarkErase(bool);
	void              ReadAndCheckLine(String& sLn);
	String            sDecodeToken(String sNumToken, char cType);
	void              GetAndDecompressLine();
	void              GetNextLine();
	void              ReStart();
	void              SkipHeader();
	long              iLinesRead()         { return _iLins; }
	long              iLinesTotal()        { return _iLinsTot; }
	AIFileType        aiDetermineFile();
	
	void              AddDomainToAttrib(TObjectInfo&);
	short             SetupMaps(const FileName&);
	
	void              AddCode(long, ObjectKind);
	void              AddIndexCode(long, ObjectKind);
	int               ExtractUniqueRowCols(long, ObjectKind);
	void              E00SegmentSingle(AISegInfo&);
	bool              WriteSegment(long, long);
	bool              WriteTopology(AISegInfo& si);
	void              WritePolygon(Array<short>&, long);
	
	void              E00Segments();            // process the ARC file
	void              E00Topologies();          // process the ARC file for polygons
	void              E00Centroids();           // skip the Centroids
	void              E00ScanGrid();            // Scan the GRD file
	void              E00Grid();                // process the GRD file
	void              E00LabelPoints();         // Get label point info
	void              E00LogFile();             // skip the logfile
	void              SkipMSKSection();         // skip the MSK section
	void              E00Polygon();             // read polygons
	void              SkipPolygonSection();     // read and skip over the polygon section
	void              E00ParFile();             // skip the PAR file
	void              E00Projection();          // for now no projection yet
	void              E00SpacialIndex();        // skip, because always empty (probably)
	void              E00Tolerances();          // skip tolerance info
	void              E00TextSection();         // skip text section
	void              E00PFFSection();          // skip PFF section (unknown what it menas)
	void              E00InfoFile();            // attribute info
	
	void              DoAttributes(ObjectKind);
	void              GetEntireRecord(String&, short, short);
	void              GetFieldWidthAndType(char, short, FieldInfo&);
	// character positions in E00 file for a field
	void              atiFieldInfo(AttribInfo&);  // scan the attribute header
	void              SetMinMax(const String&, FieldInfo&);
	void              GetAttributes(ObjectKind);  // get attributes
	void              ScanAttributes(AttribInfo&); // scan attributes table info
	void              SkipAttributes();   // pass over unrecognized info tables
	
	ValueRange        vrField(const FieldInfo&);  // determine ValueRange for field
	void              MakeTable(const Domain&, Table&);
	void              MakeColumn(FieldInfo&, Table&);
	
	File              *fileE00;
	FileName          fnE00;         //
	short             iCurE00Part;   // counter for .Enn files;
	FileName          fnIlwis;
	Tranquilizer&     trq;
	TObjectInfo       oiMaps[5];     // segment, polygon, point, raster, table
	ArrayLarge<bool>  afSegWritten;  // store if segment has been written
	
	CoordBounds       cbGlobal;
	CoordBuf          cbuf;
	RowColBuf         rcbuf;
	CoordSystem       csGlobal;
	long              _iLins, _iLinsTot, m_iDecompPos;
	AIFileType        aiFType;
	String            sLine, m_sDecompLine;
	Actions           action;
	bool              fScanning, m_fIsCompressed, m_fReadingRecords;
	bool              fDblPrecision;
	bool              fInfoSection;
	CWnd              *m_wnd;
	ObjectCollection collection;
};

#endif IMPORTE00_H
