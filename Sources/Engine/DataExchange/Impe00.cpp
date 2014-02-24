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
/* $Log: /ILWIS 3.0/Import_Export/Impe00.cpp $
 * 
 * 22    8/16/01 15:26 Willem
 * The end-of-file detection mechanism skipped empty lines. However this
 * is not allowed in fixed size records (the entire IFO section). In the
 * IFO section empty lines are now properly interpreted.
 * 
 * 21    8/16/01 12:19 Willem
 * - Added error message in case something is wrong in compressed tokens
 * - Found and implemented the holes left in the documentation of the
 * compressed E00 format
 * 
 * 20    8/15/01 14:12 Willem
 * Special case of '~' characters after compressed numbers is now
 * correctly handled
 * 
 * 19    7/25/01 10:39 Willem
 * Added try..catch around parsing code with sSub function. In case of
 * format errors in the E00 file this function may cause an exception
 * 
 * 18    7/24/01 12:00 Willem
 * - E00 import now also handles compressed E00 files properly
 * - Added saveguard against files that end with a line only containing an
 * EOF character (important in case of split e00 files using .e01 .e02
 * files etc)
 * - It is possible in e00 attribute table that duplicate fiels are
 * included. The import will now make the column names unique before
 * adding them
 * 
 * 17    8/11/00 17:24 Willem
 * Extended e00 format command line with etxra option. This allows to
 * specify on the command line wether to create polygons or not.
 * 
 * 16    1/11/00 14:34 Willem
 * Determination of section type is now case insensitive
 * 
 * 15    26/10/00 14:56 Willem
 * The messagebox function now use the desktop window as parent. This
 * prevents the tranquilizer from disappearing behind the mainwindow
 * 
 * 14    20/10/00 14:41 Willem
 * Segment coordinates are now read as doubles instead of integer values.
 * This prevents rounding errors
 * 
 * 13    19/10/00 17:54 Willem
 * - Very large negative ID's overlapping other text are now properly
 * scanned
 * - Coordinates in the E00 file that do not follow the specifications are
 * now recognized as well
 * 
 * 12    10/05/00 10:26 Willem
 * Attribute header line is now analyzed better
 * 
 * 11    3/03/00 18:17 Willem
 * Reversed the arguments of the pow() function. It seems that the Borland
 * version and the MS version reverse the arguments...
 * 
 * 10    29/02/00 11:35 Willem
 * MessageBox is now shown using as parent the mainWindow
 * 
 * 9     21/02/00 12:43 Willem
 * Removed creation of CoordSystemBoundsOnly. All maps are now created
 * with csUnknown (again)
 * 
 * 8     17/02/00 14:14 Willem
 * The messagebox function now use the impexp.win member as the window
 * instead of the desktop; this prevents the messagebox to disappear
 * behind other windows.
 * 
 * 7     17/02/00 13:10 Willem
 * - Vector maps are now created with a CoordSystemBoundsOnly
 * - Info table scan is now more robust
 * - Tranquilizer text during scan phase is now properly initialized
 * - Array fill routine does not pass boundary anymore
 * 
 * 6     10-01-00 4:05p Martin
 * removed vector rowcol and changed them to true coords
 * 
 * 5     9-09-99 4:36p Martin
 * correct some ported 2.22 linking problems
 * 
 * 4     9-09-99 2:40p Martin
 * ported 2.22 stuff
// Revision 1.25  1999/07/05 16:39:55  willem.ilwis.itc
// The import tried to create columns for a non-existing map. The map
// was not created because it would be empty. The validity of the map is
// checked before columns (that is attributes) are created.
//
// Revision 1.24  1999/05/12 10:06:30  willem.ilwis.itc
// Forgot to remove extra statement to find the bug sooner.
//
// Revision 1.23  1999/05/12 09:39:44  willem.ilwis.itc
// IFO tables that were skipped during the convert (as opposed to the
// scan phase) were not skipped properly: the record size was calculated
// incorrectly; the record size is noew calculated properly again.
//
// Revision 1.22  1999/03/19 12:11:24  willem.ilwis.itc
// - When creating polygons, the file is now correctly read. In this case
//   the segment attributes are not needed, but they were not skipped
//   properly.
//
// Revision 1.21  1999/03/03 16:21:47  willem.ilwis.itc
// Adjusted the unrecognized tag error to also display the TAG name
//
// Revision 1.20  1999/03/03 10:11:10  willem.ilwis.itc
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
// Revision 1.19  1998/10/19 09:39:08  Wim
// In SetMinMax() do not check on longs, only on real
// Undone revision 1.18 because caused not to recognize -1 as end of
// segments indicator anymore
//
// Revision 1.18  1998-10-16 18:13:33+01  Wim
// Also allow negative ID numbers in E00SegmentSingle()
//
// Revision 1.17  1998-10-16 16:46:47+01  Wim
// Use longs instead of shorts for recordnumbers in atiFieldInfo(),
// ScanAttributes() and GetAttributes()
//
// Revision 1.16  1998-09-17 10:12:41+01  Wim
// 22beta2
//
// Revision 1.15  1998/06/18 19:08:58  Willem
// Solved two problems:
// - determining valuerange with min value equal to max value caused
//   'log10: SING' error. This is now checked.
// - Arc/Info ID codes can be larger than 32767, causing the ARC scan to stop
//   prematurely. The code are now cast to long values.
//
// Revision 1.14  1998/06/11 19:26:23  Willem
// Attributes are now linked correctly to the segment maps. Superfluous
// domain items are also removed from the segment map domain.
//
// Revision 1.13  1998/06/10 13:12:44  Willem
// Two problems:
// - In some cases the attribute table was created with too many records
// - For some valueranges ILWIS calculates wrong values. Although the
//   requested values are within the range, UNDEF's are returned. Added a
//   workaround to always result in the proper values returned.
//
// Revision 1.12  1998/06/05 14:02:16  Willem
// Solved two problems:
// - In case wrong data is collected in a column, a valuerange was attempted
//   ranging from rUNDEF to rUNDEF: this is now detected and an exception is
//   thrown.
// - In case a segment section in the E00 file had no attributes, and also the
//   segment codes are meaningless, that is ranging from zero to zero, a SING
//   error resulted. An extra check has now been built in to detect this case.
//
// Revision 1.11  1998/06/02 18:56:25  Willem
// A segment map with attributes did not get the attributes imported. A flag
// was inappropriately checked, falsely assuming a polygon map was available.
//
// Revision 1.10  1998/05/26 15:15:09  Willem
// The ExtractUniqueRowCols worked to good: it removed ALL duplicate RowCols.
// This caused closed segments to loose the closing node... Changed the code
// to only remove runs with duplicate RowCols.
//
// Revision 1.9  1998/05/12 19:05:35  Willem
// - Updated support for GRD (Raster grid): it is now included
// - The import will recognize TX7 tag
// - Attributes are now also imported
// - Segments are now scanned for duplicate points, that can be created
//   when very small details are in the E00 file
// - E00 file without attributes, will still give attribute tables in ILWIS,
//   with only one column: <filename>_ID
//
// Revision 1.8  1998/03/30 16:27:21  Willem
// Added the GRD-EOG recognition (Grid file): it is now skipped
//
// Revision 1.7  1998/03/23 20:28:16  Willem
// Changes:
// - the default name of the attribute columns is now Arcinfo_ID
// - Added code to check the number of segments: when this is larger than
//   32766 a warning will be displayed; only a max of 32766 segments will
//   be imported.
// - Added code to clean up the intermediate files when the stop button is pressed
// - Changed the label points domain to reflect the position within polygons:
//   linkage to the (polygon) attributes will now be correct.
//
// Revision 1.6  1998/03/13 15:14:03  Willem
// - Added detection of TX6 and PFF sections; they are now skipped
// - Sections are now read separarely; any superfluous lines between them are
//   ignored ("Jabberwocky" problem).
// - Improved recognition of IFO double precision numbers (single = 14,
//   double = 24 characters)
// - When creating map domains the fnUnique function now also receives the
//   extension .DOM to find unique domain names
// - Splittedfiles (E00, E01, E02 etc) are now read sequentially, assuming
//   they all have the same filename part.
// - The user is now presented with a choice to let the import create a
//   polygon map in case a PAL section is found (default = no)
// - The segment/point ID's are now stored in an attribute table. The table
//   will get the domain based on the internal Arc/Info code (as are the map domains)
// - A check is added to detect compressed E00 file; when found the import will stop.
// - The tranquilizer will now also display a progress bar during the scan phase.
// - The stop button on the tranquilizer now works.
//
// Revision 1.5  1997/11/27 22:13:58  Willem
// Double precision section take now the correct number of digits for a double
//
// Revision 1.4  1997/09/25 17:10:06  Willem
// During skipping of ILWIS::Polygon section (.PAL) a string array was not reset,
// causing it to grow very large, eventually resulting in a GPF. Added the code
// to resetbthe length of the array to zero.
//
// Revision 1.3  1997/08/09 19:48:31  Willem
// Extended forreading attributes
//
/* impe00.c
   Import Segments from Arc/Info E00
   by Willem Nieuwenhuis, march 27, 1997
   ILWIS Department ITC
   Last change:  WK   19 Oct 98   10:05 am
*/

/*
  Revision history:
  WN 27-03-97: Inception
  WN 06-05-97: Segment import ready
  WN 24-07-97: Solved a problem that occurred when negative numbers were
               appearing in coordinates.
*/

#define IMPE00_C
#include "Headers\toolspch.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\IMPE00.H"
#include "Engine\SpatialReference\csbonly.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\Map\Raster\Map.h"

#define FORWARD        true
#define BACKWARD       false
#define SKIP           true

/*-------------*/

ArcInfoE00::ArcInfoE00(const FileName fnFile, const String& sOptions, Tranquilizer& _trq, CWnd* w)
  : trq(_trq), m_wnd(w)
{
	fileE00 = new File(fnFile);
	fInfoSection = false;
	fnE00 = fnFile;
	iCurE00Part = 0;
	m_iDecompPos = 0;
	m_fIsCompressed = false;
	m_fReadingRecords = false;
	cbuf.Size(500);
	rcbuf.Size(500);

	Array<String> as;
	Split(sOptions, as);
	bool fCreatePol = false;
	for (unsigned int i = 0; i < as.size(); i++)
		if (as[i].toLower() == "createpol")
			fCreatePol = true;

	TObjectInfo& oi = oiMaps[okPOLYGON];
	oi.fCreatePolygons = fCreatePol; // Do create polgon map
}

ArcInfoE00::~ArcInfoE00() {
  if (fileE00) delete fileE00;
}

/*
  Set the fErase flag for all open ILWIS objects to fRemove
  Function is called first just before the conversion starts, but 
  after the scan phase and the creation of all object. This first
  time all objects are marked as erasable. In case of an exception 
  nothing will remain on disk.
  When the conversion is finished succesfully, the function is called 
  once more to mark all objects as NOT erasable, to preserve the
  newly imported data.
*/
void ArcInfoE00::MarkErase(bool fRemove) {
  for (short ok = okSEGMENT; ok <= okPOINT; ok++) {
    TObjectInfo& oi = oiMaps[ok];
    if (!oi.mp.fValid())
      continue;
    oi.mp->fErase = fRemove;
    if (!oi.mp->dm()->fSystemObject())
      oi.mp->dm()->fErase = fRemove;
    Table& tbl = oiMaps[ok].tblAttrib;
    if (tbl.fValid()) {
      tbl->fErase = fRemove;
      for (short i = 0; i < tbl->iCols(); i++)
        if (!tbl->col(i)->dm()->fSystemObject())
          tbl->col(i)->dm()->fErase = fRemove;
    }
  }
}

// E00 compressed numbers consist of a code and a compressed numberstring
// The code which is a single ASCII character, determines the type:
// CODE NUMBER_TYPE         DIGITS  EXPONENT
// '1'  floating point      even    positiv
// '@'  floating point      even    negativ
// '^'  floating point      odd     positiv
// 'm'  floating point      odd     negativ
// '0'  integer             even    positiv
// '?'  integer             odd     negativ
// '!'  integer             even    no
// 'N'  integer             odd     no
// '"' to and including '/'
//      fixed point         even    no
// 'O' to and including ']'
//      fixed point         odd     no
// In case of fixed point the code also determines the position of the 
// decimal point: this is calculated as (type - <first type> + 1)
//
// The compressed number string consists of ASCII characters. These are 
// turned into numbers by taking the corresponding hexadecimal notation
// and then calculate the string representation:
// Case 1: character = '}', this is an escape character
//   1. fetch the next character
//   2. subtract 0x21 (hex notation for '!')
//   3. add 0x5c (hex notation for '}')
//   4. the result can now be turned into a string by simply using printf("%d")
// Case 2: other characters
//   1. subtract 0x21 (hex notation for '!')
//   2. the result can now be turned into a string by simply using printf("%d")
//
String ArcInfoE00::sDecodeToken(String sNumToken, char cType)
{
	String sOut;
	unsigned int i = 0;
	while (i < sNumToken.length())
	{
		if (sNumToken[i] == '}')
		{
			i++;
			sOut &= String("%02d", (int)sNumToken[i] - 33 + 92);
		}
		else
			sOut &= String("%02d", (int)sNumToken[i] - 33);
		i++;
	}
	// Check for odd number of digits, if so remove last digit
	if (cType == '^' || cType == 'm' ||  // for floating point
		cType == '?' || cType == 'N' ||  // for integer types
		cType == ']' || cType == 'l' ||  // for integer types with exponent
		(cType >= 'O' && cType <= '\\')   // for fixed point
		)
		sOut = sOut.sSub(0, sOut.length() - 1);

	// Check for number types with positiv exponents
	if (cType == '1' ||  // floating point (even)
		cType == '^' ||  // floating point (odd)
		cType == '0' ||  // integer        (even)
		cType == ']'     // integer        (odd)
		)
		sOut.insert(sOut.length() - 2, "E+");

	// Check for number types with negativ exponents
	if (cType == '@' ||  // floating point (even)
		cType == 'm' ||  // floating point (odd)
		cType == '?' ||  // integer        (even)
		cType == 'l'     // integer        (odd)
		)
		sOut.insert(sOut.length() - 2, "E-");

	// Add dot for floating point numbers
	if (cType == '1' || cType == '@' ||  // for floating point
		cType == '^' || cType == 'm'     // for floating point
		)
		sOut.insert(1, ".");

	// Add the dot for fixed point numbers
	if ((cType >= 'O' && cType <= '\\') ||  // for fixed point, odd number of digits
		(cType >= '"' && cType <= '/')     // for fixed point, even number of digits
		)
	{
		unsigned int iPos;
		if (cType >= '"' && cType <= '/')
			iPos = (int)cType - (int)'"' + 1;
		else
			iPos = (int)cType - (int)'O' + 1;
		if (iPos > sOut.length())
		{
			String sErr = String(TR("Unrecognized compression token in %S, in line %li").c_str(), fileE00->sName(), iLinesRead());
			throw ErrorObject(WhatError(sErr, errFORMERROR));
		}
		else
			sOut.insert(iPos, ".");
	}

	return sOut;
}

void ArcInfoE00::ReadAndCheckLine(String& sLn)
{
	bool fOk = false;
	do
	{
		if (fileE00->fEof()) {   // this is the only place for acheck on end of file
			iCurE00Part++;
			fnE00.sExt = String(".e%02d", iCurE00Part);
			if (fnE00.fExist()) {
				if (fileE00) delete fileE00;   // close the current open E00 part
				fileE00 = new File(fnE00);    // and open the next part
			}
			else
				UnexpectedEOF(fileE00->sName());
		}
		fileE00->ReadLnAscii(sLn);
		fOk = (sLn.length() > 0 && sLn[0] != '\x1a');
	}
	while (!fOk && !m_fReadingRecords);
}

// E00 compressed files consist of ASCII compressed characters with
// equal length lines of 80 characters each, except for the first line
// which is not compressed.
// All characters are copied from the input until the compression token ('~')
// is encountered. This token can be followed by:
// 1. '}'        : end of line token, copy '\n to output
// 2. ' <count>' : space sequence, the number of spaces is calculated as the
//                 hex value of the count character subtracted by 0x20
// 3. '~'        : The escape character is copied to the output
// 4. '<number code><number string>'
//               : The number string ends when a '~' or a space is encountered.
//                 For the details on numbers see function sDecodeToken() above.
// Note: a number string sequence ends when a space or a '~' is encountered. In case 
//       a '~' is found, it is ONLY a compression token when followed by a space or a '}'.
//       In all other cases it just needs to be ignored.
void ArcInfoE00::GetAndDecompressLine()
{
	if (!m_fIsCompressed)
	{
		ReadAndCheckLine(sLine);
		return;
	}

	// Handle compressed case
	sLine = "";
	String sNumber;
	bool fIsCompToken = false;  // used to determine if a compressed token is found
	bool fIsSpaces = false;     // a space sequence is detected, mutually exclusive with fIsNumber
	bool fIsNumber = false;     // a number token  is detected, mutually exclusive with fIsSpaces
	bool fNumberReady = false;    // if a '~' is detected immediately after a number it needs to be ignored
	char cType;  // the type of the number token
	while (true)
	{
		if (m_iDecompPos >= m_sDecompLine.length())
		{
			ReadAndCheckLine(m_sDecompLine);
			m_iDecompPos = 0;
		}
		char cCur = m_sDecompLine[m_iDecompPos];

		if (!fIsCompToken)   // are we in the regular character stream?
		{
			if (cCur != '~')
				sLine &= cCur;  // copy regular characters to output
			else   // we found start of a compressed token
				fIsCompToken = true;

			m_iDecompPos++;
		}
		else  // here the decoding takes place
		{
			// fNumberReady = true, when a compressed number was read immediately followed
			// by an escape character '~'. The only escape sequences allowed in this
			// case are space sequences and end-of-line. in all other case the '~'
			// is ignored and the current character is copied to the output
			if (fNumberReady && !(cCur == '}' || cCur == ' '))
			{
				sLine &= cCur;
				m_iDecompPos++;
				fNumberReady = false;  // now needs to be reset
				fIsCompToken = false;
				continue;
			}
			fNumberReady = false;  // now needs to be reset

			// Now regular parsing continues
			if (fIsSpaces)
			{
				sLine &= String("%*s", ((int)cCur - 32), "");
				fIsSpaces = false;
				fIsCompToken = false;
				m_iDecompPos++;
			}
			else if (fIsNumber)
			{
				if (cCur == '~' || cCur == ' ')  // end of number token
				{
					fIsNumber = false;
					fIsCompToken = false;
					sLine &= sDecodeToken(sNumber, cType);
					sNumber = "";
					fNumberReady = cCur == '~';
					// do not move the position counter, cCur needs to be evaluated again
				}
				else
				{
					sNumber &= cCur;
					m_iDecompPos++;
				}
			}
			else if (cCur == '~')  // the tilde character was escaped
			{
				sLine &= cCur;  // the tilde character itself is now added
				fIsCompToken = false;
				m_iDecompPos++;
			}
			else if (cCur == '}')  // end of line found
			{
				fIsCompToken = false;
				m_iDecompPos++;
				break;    // Leave the loop, line is read
			}
			else if (cCur == ' ')  // space sequence detected
			{
				fIsSpaces = true;
				m_iDecompPos++;
			}
			else if (cCur == '1' || cCur == '@'    ||  // floating point, even number of digits
				     cCur == '^' || cCur == 'm'    ||  // floating point, odd number of digits
					 cCur == '0' || cCur == '?'    ||  // integer value with exponent, even number of digits
					 cCur == ']' || cCur == 'l'    ||  // integer value with exponent, odd number of digits
					 cCur == '!' || cCur == 'N'    ||  // integer value without exponent
					 (cCur >= '"' && cCur <= '/')  ||  // fixed point, even number of digits
					 (cCur >= 'O' && cCur <= '\\')     // fixed point, odd number of digits
				    )
			{
				fIsNumber = true;
				cType = cCur;
				m_iDecompPos++;
			}
			else
			{
				String sErr = String(TR("Unrecognized compression token in %S, in line %li").c_str(), fileE00->sName(), iLinesRead());
				throw ErrorObject(WhatError(sErr, errFORMERROR));
			}
		}
	}
}


/* Then GetNextLine routine while read an ASCII line from the Arc/Info export file.
   It also handles splitted export files: E00, E01 etc. by detecting the physical
   end of the file. Assuming the splitted parts have the same base file name it
   searches for .E01 .E02 extension to continue.
*/
void ArcInfoE00::GetNextLine()
{
	GetAndDecompressLine();  // into sLine
	_iLins++;
	if (_iLins % 10 == 0)
		if (!fScanning) {
			if (trq.fUpdate(_iLins, iLinesTotal()))
				UserAbort(fileE00->sName());
		}
		else
			if (trq.fUpdate(fileE00->iLoc() / 1000, fileE00->iSize() / 1000))
				UserAbort(fileE00->sName());
}

void ArcInfoE00::ReStart() {
  if (iCurE00Part == 0)
    fileE00->Seek(0);
  else {
    if (fileE00) delete fileE00;   // if not the first Enn file
    fnE00.sExt = ".e00";           // then
    fileE00 = new File(fnE00);     //   reopen the first E00 file
    iCurE00Part = 0;               //
  }
  _iLins = 0;
}

void ArcInfoE00::SkipHeader()
{
	// skip first line and determine if file is compressed or not
	m_fIsCompressed = false;  // make sure the first line is just skipped
	GetNextLine();
	GetNextLine();
	bool fESC = sLine.find("~") != string::npos;
	ReStart();
	GetNextLine();
	m_fIsCompressed = fESC;
}

AIFileType ArcInfoE00::aiDetermineFile()
{
	Array<String> as;
	bool fErrDetect = false;
	String sSubSection;
	do
	{
		as.Reset();
		GetNextLine();
		Split(sLine, as, " ");
		
		if (as.iSize() == 2)
		{
			fErrDetect = as[0].rVal() != rUNDEF;  // the string is a number, impossible here
			// so skip this line
			fDblPrecision = (3 == as[1].iVal());
		}
		sSubSection = as[0].toUpper();
	}
	while (fErrDetect || (as.iSize() != 2 && sSubSection != "EOS"));
	if (sSubSection == "ARC")       return aiARC;
	else if (sSubSection == "CNT")  return aiCNT;
	else if (sSubSection == "GRD")  return aiGRD;
	else if (sSubSection == "LAB")  return aiLAB;
	else if (sSubSection == "LOG")  return aiLOG;
	else if (sSubSection == "MSK")  return aiMSK;
	else if (sSubSection == "PAL")  return aiPAL;
	else if (sSubSection == "PAR")  return aiPAR;
	else if (sSubSection == "PFF")  return aiPFF;
	else if (sSubSection == "PRJ")  return aiPRJ;
	else if (sSubSection == "SIN")  return aiSIN;
	else if (sSubSection == "TOL")  return aiTOL;
	else if (sSubSection == "TX6")  return aiTXT;
	else if (sSubSection == "TX7")  return aiTXT;
	else if (sSubSection == "TXT")  return aiTXT;
	else if (sSubSection == "IFO")  return aiIFO;
	else if (sSubSection == "EOS")  return aiEOF;
	else
		UnrecognizedTag(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()), sSubSection);
	return aiNONE;    // cannot come here
}

/* Fill the array of Arc/Info codes for each maptype. This information
   will be turned into the DOMAIN column in the attribute table
*/
void ArcInfoE00::AddCode(long iCode, ObjectKind ok) {
  oiMaps[ok].iCount++;
  if (oiMaps[ok].fUnique)
    for (unsigned long i = 0; i < oiMaps[ok].aiCodes.iSize(); i++) {
      if (oiMaps[ok].aiCodes[i] == iCode) {
         oiMaps[ok].fUnique = false;
      }
    }
  oiMaps[ok].aiCodes &= iCode;
}

/* Fill the array of Arc/Info index codes for a maptype; Here it is only used
   for label points. This information will be used to correctly link
   the attributes to the ILWIS point map, in case the PAL section is found
   but no polygons are created.
*/
void ArcInfoE00::AddIndexCode(long iCode, ObjectKind ok) {
  oiMaps[ok].aiIndexCodes &= iCode;
}

void ArcInfoE00::E00ScanGrid() {
  TObjectInfo& oi = oiMaps[okRASTER];
  Array<String> as;
  GetNextLine();
  Split(sLine, as, " -");  // number indicating undefined may be negative
  bool fNotKnown = as.iSize() != 4;
  long iLines = as[1].iVal();
  long iCols  = as[0].iVal();
  oi.rcSize = RowCol(iLines, iCols);
  oi.fIntType = as[2].iVal() == 1;
  as.Reset();
  GetNextLine();
  Split(sLine, as);
  fNotKnown = fNotKnown && (as.iSize() != 2);
// It has not yet been established whether pixel width comes before height
// Apparently they are equal most of the time
//  double rXPix = double(as[0]);
//  double rYPix = double(as[1]);

  as.Reset();
  GetNextLine();
  Split(sLine, as);
  fNotKnown = fNotKnown && (as.iSize() != 2);
  oi.cbMap.cMin = Coord(as[0].rVal(), as[1].rVal());
  as.Reset();
  GetNextLine();
  Split(sLine, as);
  fNotKnown = fNotKnown && (as.iSize() != 2);
  oi.cbMap.cMax = Coord(as[0].rVal(), as[1].rVal());

  long iPos = 0;
  long iCurRow = 0;
  GetNextLine();
  while (sLine.sLeft(3) != "EOG") {
    if (fNotKnown) continue;
    as.Reset();
    Split(sLine, as);
    for (unsigned int j = 0; j < as.iSize(); j++) {
      if (iPos + j >= iCols)
        continue;    // only take into account actual pixels
      double rV = as[j].rVal();
      if (rV == rUNDEF) continue;
      if (rV > oi.rMax)
        oi.rMax = rV;
      if (rV < oi.rMin)
        oi.rMin = rV;
    }
    iPos += as.iSize();
    if (iPos >= iCols) {
      iPos = 0;
      iCurRow++;
    }
    GetNextLine();
  }
  oi.fValid = true;   // indicate map presence
}


void ArcInfoE00::E00Grid() {
  Array<String> as;
  GetNextLine();
  Split(sLine, as, " -");  // number indicating undefined may be negative
  bool fNotKnown = as.iSize() != 4;
//  long iLines = (long)as[1];
  long iCols  = as[0].iVal();
  bool fIntType = as[2].iVal() == 1;
  GetNextLine();           // skip cell sizes
  GetNextLine();           // skip minX minY
  GetNextLine();           // skip maxX maxY
  TObjectInfo& oi = oiMaps[okRASTER];
  LongBuf ibuf;
  RealBuf rbuf;
  if (fIntType)
    ibuf.Size(iCols + 5);   // Integer values
  else
    rbuf.Size(iCols + 5);   // floating point values
  long iPos = 0;
  long iCurRow = 0;
  MapPtr* pmp = dynamic_cast<MapPtr*>(oi.mp.ptr());
  GetNextLine();
  while (sLine.sLeft(3) != "EOG") {
    if (fNotKnown) continue;
    as.Reset();
    Split(sLine, as);
    for (unsigned int j = 0; j < as.iSize(); j++) {
      if (iPos + j >= iCols)
        continue;    // only take into account actual pixels
      double rV = as[j].rVal();
      if (fIntType)
        ibuf[iPos + j] = (long)rV;
      else
        rbuf[iPos + j] = rV;
    }
    iPos += as.iSize();
    if (iPos >= iCols) {
      if (fIntType)
        pmp->PutLineVal(iCurRow, ibuf);
      else
        pmp->PutLineVal(iCurRow, rbuf);
      iPos = 0;
      iCurRow++;
    }
    GetNextLine();
  }
}

void ArcInfoE00::E00SegmentSingle(AISegInfo& si) {
	GetNextLine();         // get info for next segment
	Array<String> asSeg;
	try
	{
		asSeg &= sLine.sLeft(10);
		asSeg &= sLine.sSub(10, 10);
		asSeg &= sLine.sSub(20, 10);
		asSeg &= sLine.sSub(30, 10);
		asSeg &= sLine.sSub(40, 10);
		asSeg &= sLine.sSub(50, 10);
		asSeg &= sLine.sSub(60, 10);
	}
	catch (exception&) // sSub will sometimes fail, with wrongly formatted E00 files
	{
	}

	if (asSeg.iSize() != 7)
		FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
	si.iCount = 0;
	si.iArcID = asSeg[0].iVal();
	if (si.iArcID == -1)
		return;
	si.iTopID     = asSeg[1].iVal();
	si.iBeginNode = asSeg[2].iVal();
	si.iEndNode   = asSeg[3].iVal();
	si.iPolLeft   = asSeg[4].iVal();
	si.iPolRight  = asSeg[5].iVal();
	si.iNrCoord   = asSeg[6].iVal();
	short iChars = fDblPrecision ? 21 : 14;   // number of characters for a number
	short iLast = (fDblPrecision ? 1 : 2);    // one or two coordinates per line?
	while ( si.iCount < si.iNrCoord) {
		GetNextLine();         // get info for next coordinate(s)
		unsigned short iPos = 0;
		//    for (short i = 0; i < iLast; i++) {
		while (iPos < sLine.length())
		{
			String sCrd = sLine.sSub(iPos, iChars);
			iPos += iChars;
			Coord crd;
			crd.x = sCrd.rVal();
			sCrd = sLine.sSub(iPos, iChars);
			iPos += iChars;
			crd.y = sCrd.rVal();
			if (crd.fUndef())
				FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
			cbuf[si.iCount] = crd;
			si.iCount++;
			if (si.iCount == si.iNrCoord)
				break;
		}
	}   // end while
}

void ArcInfoE00::E00Segments() {
  CoordBounds cbE00;
  AISegInfo   si;
  si.iArcID = 0;
  TObjectInfo& oi = oiMaps[okSEGMENT];
  oi.iCur = 0;
  if (!fScanning) {
    afSegWritten.Resize(oi.iCount);
    for (int i = 0; i < oi.iCount; i++)
      afSegWritten[i] = false;
  }
  
  do {
    E00SegmentSingle(si);

    oi.iCur++;

// avoid having more than MAX_SEGMENTS segments, but do read
// through the input file
    if ((si.iArcID < 0) || (oi.iCur > MAX_SEGMENTS)) {
      if (fScanning && (oi.iCur == MAX_SEGMENTS + 1)) {
        int iAns = MessageBox(GetDesktopWindow(), String(TR("Too many segments in input, only first %ld will be converted.\nContinue ?").c_str(), MAX_SEGMENTS).c_str(),
                                 TR("Importing from Arc/Info E00").c_str(),
                                 MB_YESNO | MB_ICONEXCLAMATION);
        if (iAns == IDNO)
          UserAbort(fileE00->sName());
      }
      continue;
    }   

    if (fScanning) {
      AddCode(si.iTopID, okSEGMENT);
      if (si.iArcID > oi.iIDMax)
        oi.iIDMax = si.iArcID;
      for (short i = 0; i < si.iNrCoord; i++)
        cbE00 += cbuf[i];
      oi.cbMap = cbE00;
    }
    else 
      afSegWritten[oi.iCur - 1] = WriteSegment(oi.iCur, si.iNrCoord);
  }   // end do
  while (si.iArcID >= 0); // end of segment file is indicated with ID == -1
  if (fScanning)
    oi.fValid = oi.iCount > 0;
}

/* Copy Coords to RowCols and remove duplicate coordinates;
   Return the number of coordinates still there.
*/
int ArcInfoE00::ExtractUniqueRowCols(long iNrCoords, ObjectKind ok) {
  if (ok == okSEGMENT) {
    SegmentMapPtr *psm = dynamic_cast<SegmentMapPtr *>(oiMaps[ok].mp.ptr());
    if (psm == NULL)
      return 0;
  }
  else if (ok == okPOLYGON) {
    PolygonMapPtr* ppm = dynamic_cast<PolygonMapPtr*>(oiMaps[ok].mp.ptr());
    if (ppm == NULL)
      return 0;
  }
  int iPos = 0, iLast = iNrCoords - 1;
  int i, iRun;

  // check for (adjacent) duplicate RowCols in the buffer 
  // and if found remove all except one of them.
  while (iPos < iLast) {
    i = iPos + 1;
    iRun = 0;
    while (i <= iLast && cbuf[i] == cbuf[iPos]) {
      i++;
      iRun++;
    }
    if (iRun > 0) {
      for (int j = iPos + 1; j <= iLast - iRun; j++)
        cbuf[j] = cbuf[j + iRun];
      iLast -=iRun;
    }
    iPos++;
  }
  return iLast + 1;
}

/* only attempt writing to existing map;
   a segment section exists WITHOUT a valid segment map
   in case the E00 file contains polygon info
*/
bool ArcInfoE00::WriteSegment(long iCode, long iNrCoord) {
  long iNrC = ExtractUniqueRowCols(iNrCoord, okSEGMENT);
  if ( iNrC < 2)
    return false;
  SegmentMapPtr *psm = dynamic_cast<SegmentMapPtr *>(oiMaps[okSEGMENT].mp.ptr());
  if (psm == NULL)
    return false;
  ILWIS::Segment *segCur = CSEGMENT(psm->newFeature());
  segCur->PutCoords(iNrC, cbuf);
  segCur->PutVal(iCode);    // Domain takes care of prefix
  AddIndexCode(psm->iFeatures(), okSEGMENT);
  return true;
}

void ArcInfoE00::E00Centroids() {
  int iCnt = 0;
  while (iCnt >= 0) {
    GetNextLine();
    sscanf(sLine.c_str(), "%d", &iCnt);
  }
}

void ArcInfoE00::E00LabelPoints() {
  CoordBounds cbE00;
  Array<String> asLine;
  long iPntCode = 0;
  short iRealChars = fDblPrecision ? 21 : 14;  // number of characters for a real number
  short iIntChars = 10;
  long iCurPnt = 0;
  while (iPntCode >= 0) {
    GetNextLine();
    short iPos = 0;
    String sTmp = sLine.sSub(iPos, iIntChars); // read a long (label ID)
    iPos += iIntChars;
    iPntCode = sTmp.iVal();                    // Arc/Info coverage ID: Ilwis Point label
    if (iPntCode < 0)
      continue;
    iCurPnt++;
    sTmp = sLine.sSub(iPos, iIntChars);       // read another long (enclosing polygon)
    long iPntInPol = sTmp.iVal();              // use Arc/Info polygon userID for domain
    iPos += iIntChars;
    sTmp = sLine.sSub(iPos, iRealChars);
    iPos += iRealChars;
    Coord crd;
    crd.x = sTmp.rVal();
    sTmp = sLine.sSub(iPos, iRealChars);
    crd.y = sTmp.rVal();
    if (crd.fUndef())
      FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
    GetNextLine();                            // skip the dummy coordinates
    if (fDblPrecision)
      GetNextLine();                          // skip the dummy coordinates

    if (fScanning) {
      AddCode(iPntCode, okPOINT);
      AddIndexCode(iPntInPol, okPOINT);
      if (iPntCode > oiMaps[okPOINT].iIDMax)    // coverage ID's are stored as attribute column
        oiMaps[okPOINT].iIDMax = iPntCode;      // follow the maximum label ID number
      if (iPntInPol > oiMaps[okPOINT].iMaxIdx)  // polygon ID's as index
        oiMaps[okPOINT].iMaxIdx = iPntCode;     // follow the maximum polygon number
      cbE00 += crd;
    }
    else {
      PointMapPtr *ppm = dynamic_cast<PointMapPtr *>(oiMaps[okPOINT].mp.ptr());
      if (ppm == NULL)
        return;
      ppm->iAddRaw(crd, iCurPnt);
    }
  }  // end while

  // only setup a valid map if there the number of points >0
  if (fScanning && (iCurPnt > 0)) {  
    double rXMargin = cbE00.cMax.x * 0.05;     //
    double rYMargin = cbE00.cMax.y * 0.05;     // extend the boundary a bit
    cbE00 += Coord(cbE00.cMin.x - rXMargin, cbE00.cMin.y - rYMargin);
    cbE00 += Coord(cbE00.cMax.x + rXMargin, cbE00.cMax.y + rYMargin);
    oiMaps[okPOINT].cbMap = cbE00;
    oiMaps[okPOINT].fValid = true;
  }
}

void ArcInfoE00::E00LogFile() {
  while (sLine.sLeft(3) != "EOL") {
    GetNextLine();
  }
}

void ArcInfoE00::SkipMSKSection() {
  bool fEndMsk = false;
  long iVal;
  ULONGLONG iPos;
  // try to find the next section: the MSK section is assumed to
  // include only numbers. The end of the section is found by locating
  // the next section tag. 
  while (!fEndMsk) {
    iPos = fileE00->iLoc();
    GetNextLine();
    // sscanf result: 0 == no conversion, -1 (EOF) == error in conversion
    fEndMsk = (sscanf(sLine.c_str(), "%d", &iVal) <= 0);
  }
  // set file position to first line of next section.
  fileE00->Seek(iPos);
}

void ArcInfoE00::E00Topologies() {
  CoordBounds cbE00;
  AISegInfo   si;
  si.iArcID = 0;
  do {
    E00SegmentSingle(si);

    if (si.iArcID < 0)
      continue;        // end of section reached
    if (fScanning) {
      AddCode(si.iTopID, okSEGMENT);     // the topologiy code is also stored
      if (si.iArcID > oiMaps[okSEGMENT].iIDMax)
        oiMaps[okSEGMENT].iIDMax = si.iArcID;
      for (short i = 0; i < si.iNrCoord; i++)
        cbE00 += cbuf[i];
      oiMaps[okSEGMENT].cbMap = cbE00;
    }
    else
      WriteTopology(si);
  }   // end do
  while (si.iArcID >= 0);  // end of segment file is indicated with ID == -1
}

/*
  Create a new topology; only the coordinates and the value are filled in.
  The polygonization is left to PolygonMapStore->Polygonize().
*/
bool ArcInfoE00::WriteTopology(AISegInfo& si) {
  long iNrC = ExtractUniqueRowCols(si.iNrCoord, okPOLYGON);
  if ( iNrC < 2)
    return false;
  PolygonMapPtr* ppm = dynamic_cast<PolygonMapPtr*>(oiMaps[okPOLYGON].mp.ptr());
  if (ppm == NULL)
    return false;
  PolygonMap pm;
  pm.SetPointer(ppm);
  vector<Coordinate> *coords = new vector<Coordinate>();
  for(int i = 0; i < cbuf.iSize(); ++i) {
	  coords->push_back(cbuf[i]);		
  }
  CoordinateSequence *seq = new CoordinateArraySequence(coords);
  LinearRing * ring = new LinearRing(seq,new GeometryFactory());
  ILWIS::Polygon *pol = CPOLYGON(pm->newFeature());
  pol->addBoundary(ring);
  AddIndexCode(pm->iFeatures(), okSEGMENT);
  return true;
}

// Skip the PAL section in the E00 file
void ArcInfoE00::SkipPolygonSection() {
  oiMaps[okPOLYGON].fValid = true;
  Array<String> asLine;
  do {
    GetNextLine();
    asLine.Reset();
    Split(sLine, asLine);
  } while (asLine.iSize() != 7);
}

/*
  Read ILWIS::Polygon information; this function is only called in the scan phase.
  In the scanning phase the number of polygons is counted, to determine if a polygonmap
  needs to be created.
  Polygons are not created by building topology here. After the import is complete
  the polygonisation will start, when the user so desires.
*/
void ArcInfoE00::E00Polygon() {
  long iTopCnt = 0;      // store the number of tops/pol, -1 if end of polygons
  long iNrTops;
  long iNrPols = -1;
  Array<String> asTopInfo;
  Array<String> asBounds;
  CoordBounds cbE00;
  while (iTopCnt >= 0) {
    GetNextLine();
    asBounds.Reset();
    Split(sLine, asBounds);
    iTopCnt = asBounds[0].iVal();
    iNrTops = iTopCnt;
    if (iNrTops < 0) continue;
    iNrPols++;

// Ignore codes for polygons, as the attributes will be linked to the label points
//    if (iNrPols > 0)
//      AddCode(iNrPols, okPOLYGON);   // don't count the universe polygon
    if (asBounds.iSize() != 5)
      FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
    cbE00 += Coord(asBounds[1].rVal(), asBounds[2].rVal());
    cbE00 += Coord(asBounds[3].rVal(), asBounds[4].rVal());
    Array<short> aiTops;
    Array<short> aiRightPol;   // In Arc/Info left pol!
    while (iNrTops > 0) {      // Read all toplogies, both during scanning and during import
      GetNextLine();
      asTopInfo.Reset();
      Split(sLine, asTopInfo);
      if (asTopInfo.iSize() != 3 && asTopInfo.iSize() != 6)
        FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
      aiTops &= asTopInfo[0].iVal();
      aiRightPol &= asTopInfo[2].iVal();
      iNrTops--;
      if (asTopInfo.iSize() == 6) {
        aiTops &= asTopInfo[3].iVal();
        aiRightPol &= asTopInfo[5].iVal();
        iNrTops--;
      }
    }
  }
  if (fScanning)
    oiMaps[okPOLYGON].cbMap = cbE00;
}

void ArcInfoE00::E00ParFile() {
  int iCnt = 0;
  while (iCnt >= 0) {
    GetNextLine();
    sscanf(sLine.c_str(), "%d", &iCnt);
  }
}

void ArcInfoE00::E00Projection() {
  while (sLine.sLeft(3) != "EOP") {
    GetNextLine();
  }
}

void ArcInfoE00::E00SpacialIndex() {
  while (sLine.sLeft(3) != "EOX") {
    GetNextLine();
  }
}


// TXT section ends with "-1 0 0 0 0 0 0"; read first argument on the line
// to find the "-1"
void ArcInfoE00::E00TextSection() {
  int iCnt = 0;
  int i2, i3, i4, i5, i6, i7;
  while (iCnt >= 0) {
    GetNextLine();
    if (7 != sscanf(sLine.c_str(), "%d %d %d %d %d %d %d", &iCnt, &i2, &i3, &i4, &i5, &i6, &i7))
      iCnt = 0; 
  }
}

// PFF section ends with "-1 0 0 0 0 0 0"; read first argument on the line
// to find the "-1"
void ArcInfoE00::E00PFFSection() {
  int iCnt = 0;
  int i2, i3, i4, i5, i6, i7;
  while (iCnt >= 0) {
    GetNextLine();
    if (7 != sscanf(sLine.c_str(), "%d %d %d %d %d %d %d", &iCnt, &i2, &i3, &i4, &i5, &i6, &i7))
      iCnt = 0; 
  }
}

void ArcInfoE00::E00Tolerances() {
  int iCnt = 0;
  while (iCnt >= 0) {
    GetNextLine();
    sscanf(sLine.c_str(), "%d", &iCnt);
  }
}

void ArcInfoE00::AddDomainToAttrib(TObjectInfo& oi) {
  FieldInfo tiDomain;
  long iMin = oi.aiCodes[0], iMax = oi.aiCodes[0];
  for (int i = 1; i < oi.aiCodes.iSize(); i++) {
    if (oi.aiCodes[i] < iMin)
      iMin = oi.aiCodes[i];
    else if (oi.aiCodes[i] > iMax)
      iMax = oi.aiCodes[i];
  }
  tiDomain.sName = String("%S_ID", fnE00.sFile);    // Arc/Info user ID
  tiDomain.atctCol = atctVALUE;
  tiDomain.iDecimal = 0;
  if (iMax < -iMin)
    iMax = -iMin;
  tiDomain.rMin = iMin;
  tiDomain.rMax = iMax;
  tiDomain.iWidth = 1 + (iMax == 0 ? 0 : (short)log10((double)(iMax)));
  MakeColumn(tiDomain, oi.tblAttrib);
  Column col = (oi.tblAttrib)->col(tiDomain.sName);
  for (short iRec = 0; iRec < oi.aiCodes.iSize(); iRec++)
    col->PutVal( iRec + 1, oi.aiCodes[iRec] );
}

/*
  Take the necessary steps to get attribute information (scan phase) or
  the attributes themselves (convert phase)
*/
void ArcInfoE00::DoAttributes(ObjectKind ok) {
  if (fScanning) {
    atiFieldInfo(oiMaps[ok].atiInfo);
    ScanAttributes(oiMaps[ok].atiInfo);
  }
  else {
    if (!oiMaps[ok].fValid) {
      SkipAttributes();
      return;
    }
    if (oiMaps[ok].atiInfo.iNrRecords > 0)
      GetAttributes(ok);
  }
}

void ArcInfoE00::E00InfoFile() {
  Array<String> as;
  Array<String> asFile;
  while (sLine.sLeft(3) != "EOI") {
    GetNextLine();
    as.Reset();
    asFile.Reset();
    Split(sLine, as);
    Split(as[0], asFile, ".");
    if (asFile[0] == "EOI")
      continue;
    if (asFile[1] == "BND") {  // Info section has extent information
      for (short i = 0; i < as[2].iVal(); i++)
        GetNextLine();         // skip lines with structure info
        
      short iChars = fDblPrecision ? 24 : 14;
      short iLinesPerRecord = 1 + (iChars * 4 - 1) / 80;
      String sRecord;
      GetEntireRecord(sRecord, iChars * 4, iLinesPerRecord);
      short iPos = 0;
      for (short iBnd = 0; iBnd < 4; iBnd++) {
        String sTmp = sRecord.sSub(iPos, iChars);
        iPos += iChars;
        double rTmp = sTmp.rVal();
        switch (iBnd) {
          case 0 : cbGlobal.cMin.x = rTmp; break;
          case 1 : cbGlobal.cMin.y = rTmp; break;
          case 2 : cbGlobal.cMax.x = rTmp; break;
          case 3 : cbGlobal.cMax.y = rTmp; break;
        }
      }
      if (cbGlobal.cMin.fUndef() || cbGlobal.cMax.fUndef())
        FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
    }
    else if (asFile[1] == "AAT") {
      trq.SetText(TR("Segment attributes"));

      DoAttributes(okSEGMENT);
    }
    else if (asFile[1] == "PAT") {
// polygons can optionally be created in a later stadium by Ilwis, so 
// attributes will be linked to the labels, not to polygons.
			String sPoint("Point");
      trq.SetText(String(TR("%S attributes").c_str(), sPoint));

      DoAttributes(okPOINT);
    }
    else if (asFile.iSize() == 1) { // no info type indicated => plain table
      trq.SetText(TR("Plain table"));
      DoAttributes(okTABLE);
      if (fScanning)
        oiMaps[okTABLE].fValid = true;  // valid table found
    }
    else // IFO files not recognized or dealt with
         // Assumes table layout
      SkipAttributes();
  }
}

/* --------------------------- */
/* Attribute related functions */
/* The table below describes how the number of characters to be read
   from the E00 file is determined.
   A typical field definition:
     SOIL    4-1   14-1   12 3 60-1   -1 -1-1       1
     ^^^^1   ^2                ^3
   The marked fields are used to make the determination. The first field
   only describes the name though.
     E00 type   E00 width      To be
     (field 3)  (field 2)      Read     What is it?
         1        8             8       Date field
         2        n             n       Character string
         3        n             n       Integer
         4        n             14      n will be less than 13: Floating point (single)
         5        4             11      Special integers
         6        4             14      Floating point (single)
         6        8             24      Floating point (double)
*/
void ArcInfoE00::GetFieldWidthAndType(char cType, short iWid, FieldInfo& fi) {
  fi.atctCol = atctVALUE;
  fi.iFieldSize = iWid;   // default case
  fi.iWidth = iWid;       // true for: '1' (date), '2' (string), '3' (integer)
  fi.iDecimal = 0;
  switch (cType) {
    case '1' :        // Date
      fi.iFieldSize = 8;
      break;
    case '2' :       // String, variable length
      fi.iFieldSize = iWid;
      fi.atctCol = atctSTRING;
      break;
    case '3' :      // Integer, variable length
      fi.iFieldSize = iWid;
      break;
    case '4' :      // Single float
      fi.iFieldSize = 14;
      fi.iWidth = 14;
      fi.iDecimal = 6;
      break;
    case '5' :      // Long (ID's)
      fi.iFieldSize = 11;
      fi.iWidth = 10;
      break;
    case '6' : {    // Floats
      switch (iWid) {
        case 4 :    // single precision
          fi.iFieldSize = 14;  // characters to read from the file 
          fi.iWidth = 14;
          break;
        case 8 :    // double precision
          fi.iFieldSize = 24;  // characters to read from the file
          fi.iWidth = 14;
          break;
      }
      fi.iDecimal = 6;
      break;
    }
  }
}

void ArcInfoE00::atiFieldInfo(AttribInfo& ati) {
	Array<String> as;

	try
	{
		as &= sLine.sLeft(32);
		as &= sLine.sSub(32,  2);
		as &= sLine.sSub(34,  4);
		as &= sLine.sSub(38,  4);
		as &= sLine.sSub(42,  4);
		as &= sLine.sSub(46, 10);
	}
	catch (exception&) // sSub will sometimes fail, with wrongly formatted E00 files
	{
	}
	
	if (as.iSize() != 5 && as.iSize() != 6)  
		FormatProblem(String(TR("%S, in line %ld").c_str(), fileE00->sName(), iLinesRead()));
	
	bool fNative = as.iSize() == 6;
	ati.iNrActiveFields = as[fNative?2:1].iVal();
	ati.iNrFields = as[fNative?3:2].iVal();
	ati.iNrRecords = as[as.iSize() - 1].iVal();
	ati.fields.Resize(ati.iNrActiveFields);
	ati.iRecordSize = 0;
	long iCurField = 0;   // iterator for the active fields
	for (short i = 0; i < ati.iNrFields; i++) {
		GetNextLine();
		
		String sTmp = sLine.sSub(65, 5).sTrimSpaces();
		if (sTmp[0] == '-')
			continue;   // skip inactive fields
		
		FieldInfo& field = ati.fields[iCurField];
		iCurField++;
		field.sName = sLine.sSub(0, 16).sTrimSpaces();
		for (short j = 0; j < field.sName.length(); j++) {
			if (!isalnum(field.sName[j]))
				field.sName[j] = '_';
		}
		
		short iWid = sLine.sSub(16, 3).iVal();
		GetFieldWidthAndType(sLine[35], iWid, field);
		ati.iRecordSize += field.iFieldSize;
	}
}

void ArcInfoE00::MakeColumn(FieldInfo& fi, Table& tbl) {
  ValueRange vr;
  String sDomain;
  switch (fi.atctCol) {
    case atctVALUE :
      sDomain = "value";
      vr = vrField(fi);
      break;
    default :            // should be only atctSTRING
      sDomain = "string";
      break;
  }
  FileName fnDom(sDomain, ".dom", true);
  Domain dm(fnDom);
  int i = 0;
  while (i < tbl->iCols())
  {
	  if (fCIStrEqual(fi.sName, tbl->col(i)->sName()))
	  {
		  fi.sName = String("%S_", fi.sName);  // try to make the column name unique
		  i = 0; // check all column names again
	  }
	  else
		  i++;
  }
  tbl->colNew(fi.sName, dm, vr);
}

/* Temporary check routine. Used as a workaround for
   a flaw in ValueRange. The mean value of the MIN
   and MAX values divided by the STEP is not checked to fit
   in the storetype selected. This is done here.
   Code is added for Patch 3, Alpha i7 (2.2 Beta)
*/
bool fIsVROK(const FieldInfo& fi) {
  double rMiddle = fi.rMax - fi.rMin;
  int iNum = 0;
  if (abs(rMiddle) > 1e-6) 
    iNum = floor(log10(abs(rMiddle)));
  double rNorm = rMiddle / pow(10.0, iNum);
  if (iNum + fi.iDecimal == 9)   // check for stLONG
    return (abs(rNorm) < 2.1);   //   < 2.1e9  (MAXLONG)
  if (iNum + fi.iDecimal == 5)   // check fo stINT
    return (abs(rNorm) < 3.2);   //   < 3.2e4  (MAXSHORT)
  if (iNum + fi.iDecimal == 3)   // check for stBYTE
    return (abs(rNorm) < 2.5);   //   < 256    (MAXBYTE)
  return true;
}

ValueRange ArcInfoE00::vrField(const FieldInfo& fi) {
  if (fi.rMin == rUNDEF || fi.rMax == rUNDEF)
    NumericExpected(fi.sName);
  if ( fi.iDecimal != 0 ) {
    double rStep = pow(10.0, -fi.iDecimal);
    if (!fIsVROK(fi))
      rStep /= 10;  // force to next higher storetype
                    // that can handle all values
    return ValueRange(fi.rMin, fi.rMax, rStep);
  }
  return ValueRange(fi.rMin, fi.rMax, 1);
}

void ArcInfoE00::MakeTable(const Domain& dmTable, Table& tbl) {
  FileName fnTable  = FileName::fnUnique(FileName(fnIlwis, ".tbt", true));
  FileName fnTblDat = FileName(fnTable, ".tb#", true);
  if (dmTable.fValid())  // false in case of table only E00 file
    tbl.SetPointer(new TablePtr(fnTable, fnTblDat, dmTable, String()));
  else
    tbl.SetPointer(new TablePtr(fnTable, fnTblDat, oiMaps[okTABLE].atiInfo.iNrRecords, String()));
  String sD = TR("Table ");
  sD &= fnTable.sFile;
  sD &= fnTable.sExt;
  tbl->sDescription = sD;
  tbl->Store();
  collection->Add(tbl);
}

void ArcInfoE00::GetEntireRecord(String& sRec, short iRecSize, short iLines) {
  sRec = "";
  for (short i = 0; i < iLines; i++) {
    GetNextLine();
    sRec &= sLine;
    if ((i != iLines - 1) && sLine.length() != 80)
      sRec &= String("%*s", 80 - sLine.length(), "");
  }
  if (sRec.length() != iRecSize)
    sRec &= String("%*s", iRecSize - sRec.length(), "");
}

void ArcInfoE00::SetMinMax(const String& sVal, FieldInfo& fi) {
  if (fi.atctCol == atctVALUE) {
    double rVal = sVal.rVal();
    if (rVal == rUNDEF)
      return;
//    long iVal = (long)sVal;  if doubles are used anyway why to check for integers?
//    if (iVal == iUNDEF)
//      return;
    if (fi.rMin == rUNDEF || fi.rMax == rUNDEF) {
      fi.rMin = rVal;
      fi.rMax = rVal;
      return;
    }
    if (rVal < fi.rMin)
      fi.rMin = rVal;
    else if (rVal > fi.rMax)
      fi.rMax = rVal;
  }
}

// Skip E00 Info tables for unrecognized table types
void ArcInfoE00::SkipAttributes() {
  AttribInfo atiDum;
  atiFieldInfo(atiDum);
  ScanAttributes(atiDum);
}

void ArcInfoE00::ScanAttributes(AttribInfo& ati)
{
	m_fReadingRecords = true;
	
	if (ati.iNrActiveFields == 0)
		return;
	String sRecord;
	short iLinesPerRecord = 1 + (ati.iRecordSize - 1) / 80;
	ati.fOuterIncluded = false;
	for (long iRec = 0; iRec < ati.iNrRecords; iRec++) {
		GetEntireRecord(sRecord, ati.iRecordSize, iLinesPerRecord);
		if (iRec == 0 && sRecord[0] == '-') {
			ati.fOuterIncluded = true;
			continue; // skip the polygon attribute for the outer polygon.
		}
		short iOffset = 0;
		for (short iFld = 0; iFld < ati.iNrActiveFields; iFld++) {
			FieldInfo& fi = ati.fields[iFld];
			String sFld = sRecord.sSub(iOffset, fi.iFieldSize);
			iOffset += fi.iFieldSize;
			SetMinMax(sFld, fi);
		}
	}
	m_fReadingRecords = false;
}

void ArcInfoE00::GetAttributes(ObjectKind ok)
{
	AttribInfo atiDum;
	atiFieldInfo(atiDum);     // skip AttribInfo, already parsed in scan phase
	if ((ok == okSEGMENT) && (oiMaps[okPOLYGON].fValid && oiMaps[okPOLYGON].fCreatePolygons) )
	{
		ScanAttributes(atiDum);  // skip the attributes
		return;     // Do not create segment attributes for polygon map
	}
	
	AttribInfo& ati = oiMaps[ok].atiInfo;
	if (ati.iNrActiveFields == 0)
		return;
	
	int i;
	LongArrayLarge aiReverse;
	if ((ok == okPOINT) && (oiMaps[okPOLYGON].fValid)) {
		TObjectInfo& oi = oiMaps[ok];
		aiReverse.Resize(ati.iNrRecords + 2);  // 2 extra for non zero base of ArcInfo
		for (i = 0; i < oi.aiIndexCodes.iSize(); i++)
			aiReverse[oi.aiIndexCodes[i] - 1] = i + 1;  // make it zero based
	}
	
	m_fReadingRecords = true;

	String sRecord;
	int iLinesPerRecord = 1 + (ati.iRecordSize - 1) / 80;
	Table& tbl = oiMaps[ok].tblAttrib;
	long iTotRec = ati.iNrRecords; 
	if (ati.fOuterIncluded && (ok == okPOINT || ok == okPOLYGON)) {  // if universal polygon included in E00: skip it
		GetEntireRecord(sRecord, ati.iRecordSize, iLinesPerRecord);
		iTotRec = ati.iNrRecords - 1;
	}
	
	bool fFirstRecord = true;
	for (long iRec = 1; iRec <= iTotRec; iRec++) {
		GetEntireRecord(sRecord, ati.iRecordSize, iLinesPerRecord);
		
		long iOffset = 0;
		for (int iFld = 0; iFld < ati.iNrActiveFields; iFld++) {
			FieldInfo& fi = ati.fields[iFld];
			String sFld = sRecord.sSub(iOffset, fi.iFieldSize);
			iOffset += fi.iFieldSize;
			if (fFirstRecord) {
				MakeColumn(fi, oiMaps[ok].tblAttrib);
				if (!tbl->col(fi.sName).fValid())
					CreateColError(fi.sName);
			}
			Column col = tbl->col(fi.sName);
			if ((ok == okPOINT) && (oiMaps[okPOLYGON].mp.fValid()))
				col->PutVal(aiReverse[iRec], sFld );
			else if (ok == okSEGMENT) {
				if (afSegWritten[iRec - 1])
					col->PutVal(iRec, sFld);
			}
			else
				col->PutVal(iRec, sFld );
		}
		fFirstRecord = false;
	}
	m_fReadingRecords = false;
	
	/* Because possibly not all segments have been written to
	ILWIS (and therefore only a subset of the attribute records),
	The domain may contain superfluous elements. These will be removed 
	from the domain.
	*/
	if (ok == okSEGMENT) {
		DomainValueRangeStruct dvs = oiMaps[ok].mp->dvrs();
		DomainSort* pds = dvs.dm()->pdsrt();
		if (pds != NULL) {
			for (i = 0; i < oiMaps[ok].iCount; i++)
				if (!afSegWritten[i])
					pds->Delete(i + 1);
		}
	}
}

long ArcInfoE00::ScanInfo() {
	trq.SetText(TR("Scanning..."));
	ReStart();
	SkipHeader();
	fScanning = true;
	bool fEndOfFile = false;
	for (short ok = okSEGMENT; ok <= okTABLE; ok++) {
		oiMaps[ok].atiInfo.iNrActiveFields = 0;
	}
	while (!fEndOfFile) {       // check on EOS
		switch (aiDetermineFile()) {
		case aiEXP :            // header line, is skipped
			break;
		case aiARC :            // ARC file: the coordinates
			E00Segments();
			break;
		case aiCNT :            // this file is skipped
			E00Centroids();
			break;
		case aiGRD :            // E00 Grid file
			E00ScanGrid();
			break;
		case aiLAB :            // Label Points: imported as points, but in case of polygons they become label points
			E00LabelPoints();
			break;
		case aiLOG :            // Arc/Info Logfile: this file is skipped
			E00LogFile();
			break;
		case aiMSK :
			SkipMSKSection();
			break;
		case aiPAL :            // ILWIS::Polygon info: always skipped: polygons are created afterwards
			SkipPolygonSection(); // do not include polygons, they will be created from the segments
			break;
		case aiPAR :            // Unknown: this file is skipped
			E00ParFile();
			break;
		case aiPFF :            // this file is skipped
			E00PFFSection();
			break;
		case aiPRJ :            // Projection info: this file is skipped for now
			E00Projection();
			break;
		case aiSIN :            // this file is skipped
			E00SpacialIndex();
			break;
		case aiTOL :            // this file is skipped
			E00Tolerances();
			break;
		case aiTXT :            // this file is skipped
			E00TextSection();
			break;
		case aiIFO :            // only extracts boundary info
			E00InfoFile();
			break;
		case aiEOF :            // Line containing the string 'EOS'
			fEndOfFile = true;
			break;
		default :
			break;
		}
	}
	trq.fUpdate(iLinesRead(), iLinesRead());
	TObjectInfo& oi = oiMaps[okPOLYGON];
	oi.fValid = oi.fValid && oi.fCreatePolygons;  // only create polygons if specified and possible
	
	return iLinesRead();
}

short ArcInfoE00::SetupMaps(const FileName& fnObject) {
  trq.SetText(TR("Initializing maps"));
  short iMaps = 0;
  ParmList pm;
  collection = ObjectCollection(FileName(fnObject,".ioc"), "ObjectCollection", pm);
  for (short ok = okSEGMENT; ok <= okTABLE; ok++) {
    TObjectInfo& oi = oiMaps[ok];
    if (!oi.fValid)
      continue;
/* Create new maps:
   If there is a PAL section create a PolygonMap and no SegmentMap, but
   only if the user decided to create polygons;
   A LAB section always results in a PointMap
*/
    DomainValueRangeStruct dvs;
    Domain dm;
    FileName fnDom = FileName::fnUnique(FileName(fnObject, ".dom", true));
    CoordBounds cb;
		CoordSystem cs = oi.csMap;

    switch (ok) {
      case okSEGMENT :
// Create a segment map when an ARC section is found
// and (1) no PAL section is found
// or  (2) no polygon map is created when a PAL section is found
        if (!oiMaps[okPOLYGON].fValid || !oiMaps[okPOLYGON].fCreatePolygons) {
          oi.sPrefix = "Seg";
          dm = Domain(fnDom, oi.iCount, dmtID, oi.sPrefix);
          dvs = DomainValueRangeStruct(dm);
          cb = cbGlobal.fUndef() ? oi.cbMap : cbGlobal;
          oi.mp = SegmentMap(FileName(fnObject,".mps"), cs, cb, dvs);
          oi.mp->sDescription  = TR("Segment Map");
		  collection->Add(oi.mp);
		  collection->Add(oi.mp->dm());
          iMaps++;
        }
        break;
      case okPOLYGON : {
        oi.sPrefix = "Pol";
        dm = Domain(fnDom, oi.iCount, dmtID, oi.sPrefix);
        dvs = DomainValueRangeStruct(dm);
        cb = cbGlobal.fUndef() ? oi.cbMap : cbGlobal;
        oi.mp = PolygonMap(FileName(fnObject,".mpa"), cs, cb, dvs);
        oi.mp->sDescription  = TR("Polygon Map");
	    collection->Add(oi.mp);
    	  collection->Add(oi.mp->dm());
        iMaps++;
        break;
      }
      case okPOINT : {
        oi.sPrefix = "Pnt";
        dm = Domain(fnDom, oi.iCount, dmtID, oi.sPrefix);
        dvs = DomainValueRangeStruct(dm);
        cb = cbGlobal.fUndef() ? oi.cbMap : cbGlobal;
        oi.mp = PointMap(FileName(fnObject,".mpp"), cs, cb, dvs);
        oi.mp->sDescription  = TR("Point Map");
        collection->Add(oi.mp);
		  collection->Add(oi.mp->dm());
        iMaps++;
        break;
      }
      case okRASTER : {
        GeoRef gr;
        gr.SetPointer(new GeoRefCorners(FileName(fnObject, ".grf", true),
                         cs, oi.rcSize, true,   // corners of corners
                         oi.cbMap.cMin, oi.cbMap.cMax));
        ValueRange vr;
        if (oi.fIntType)
          vr = ValueRange(oi.rMin, oi.rMax, 1);
        else
          vr = ValueRange(oi.rMin, oi.rMax, 0);
        dm = Domain("value");
        dvs = DomainValueRangeStruct(dm, vr);
        cb = cbGlobal.fUndef() ? oi.cbMap : cbGlobal;
        oi.mp = Map(FileName(fnObject,".mpr"), gr, oi.rcSize, dvs);
        oi.mp->sDescription  = TR("Raster Map");
    	  collection->Add(oi.mp);
  		  collection->Add(gr);
        iMaps++;
        break;
      }
      case okTABLE : {
        MakeTable(Domain(), oi.tblAttrib);
	    iMaps++;  
        break;
      }
      default :
        break;
    }

/* Create an attribute table for:
   segment map: the E00 file is read in as if it is a segment map;
                this can occur if it really is a segment map or if
                a PAL section is found but the user decided NOT to
                create a polygon map.
   point map:   An attribute table is created for the point map, when
                it is a point map or when it is a polygon map.
   polygon map: Attributes are always passed to the polygons via the
                point map. Therefore the polygon map will NOT get an
                attribute table.
   Attribute tables are created in all cases. If attribute sections are
   found its columns will populate the tables. Otherwise the tables will
   only contain the coverage ID.
*/
    if (oi.mp.fValid()) {
      if ( (ok == okSEGMENT) || (ok == okPOINT)) {
        MakeTable(dvs.dm(), oi.tblAttrib);
        oi.mp->SetAttributeTable(oi.tblAttrib);
        // add E00 domain codes to attribute table in case
        // no attributes are specified in the E00 file
        if (oi.atiInfo.iNrActiveFields == 0)
          AddDomainToAttrib(oi);
      }
      if (ok <= okRASTER) {
        oi.mp->sDescription &= TR("imported from ");
        oi.mp->sDescription &= String("%S.e00", fnE00.sFile);
        oi.mp->Store();
      }
    }
  }
  return iMaps;
}

void ArcInfoE00::Convert(const FileName& fnObject) {
  fnIlwis = fnObject;
  if (SetupMaps(fnObject) == 0)
    return;

// Clean up garbage when the user presses stop button
  MarkErase(true);  
  
  trq.SetText(TR("Converting..."));
  _iLinsTot = iLinesRead();
  ReStart();
  SkipHeader();
  fScanning = false;
  bool fEndOfFile = false;
  while (!fEndOfFile) {       // check on EOS
    switch (aiDetermineFile()) {
      case aiEXP :            // header line, is skipped
        break;
      case aiARC :            // ARC file: the coordinates
        if (oiMaps[okPOLYGON].mp.fValid())
          E00Topologies();
        else
          E00Segments();
        break;
      case aiCNT :            // this file is skipped
        E00Centroids();
        break;
      case aiGRD :            // E00 Grid file
        E00Grid();
        break;
      case aiLAB :            // Label Points: imported as points, but in case of polygons they become label points
        E00LabelPoints();
        break;
      case aiLOG :            // Arc/Info Logfile: this file is skipped
        E00LogFile();
        break;
      case aiMSK :
        SkipMSKSection();
        break;
      case aiPAL :            // ILWIS::Polygon info: implemented when polygons are taken up
        SkipPolygonSection(); // do not include polygons, they will be created from the segments
        break;
      case aiPAR :            // Unknown: this file is skipped
        E00ParFile();
        break;
      case aiPFF :            // this file is skipped
        E00PFFSection();
        break;
      case aiPRJ :            // Projection info: this file is skipped for now
        E00Projection();
        break;
      case aiSIN :            // this file is skipped
        E00SpacialIndex();
        break;
      case aiTOL :            // this file is skipped
        E00Tolerances();
        break;
      case aiTXT :            // this file is skipped
        E00TextSection();
        break;
      case aiIFO :            // only extracts boundary info
        E00InfoFile();
        break;
      case aiEOF :            // Line containing the string 'EOS'
        fEndOfFile = true;
        break;
      default :
        break;
    }
  }
  trq.fUpdate(iLinesTotal(), iLinesTotal());
  if (oiMaps[okPOLYGON].fValid && oiMaps[okPOLYGON].fCreatePolygons) {
	  PolygonMapPtr* ppm = dynamic_cast<PolygonMapPtr*>(oiMaps[okPOLYGON].mp.ptr());
	  PolygonMap pm;
	  pm.SetPointer(ppm);             // make the PolygonMap available
	  bool fNeedCheck = true;
	  bool fAuto = true;
	  long iNrPol = pm->iFeatures();
	  pm->dm()->pdsrt()->Resize(iNrPol);
	  ILWIS::Polygon *pol;
	  for (int i = 0; i < iNrPol; i) {
		  pol = (ILWIS::Polygon *)pm->getFeature(i);
		  if ( pol == NULL || pol->fValid() == false)
			  continue;
		  if (trq.fUpdate(i, iNrPol))
			  UserAbort();
		  pol->PutVal((long)i);
	  }
  }

// Keep the imported stuff!  
  MarkErase(false);
}

void ImpExp::ImportE00(const FileName& fnFile, const FileName& fnObject, const String& sOptions)
{
	try 
	{
		trq.SetTitle(TR("Importing from Arc/Info E00"));
		ArcInfoE00 ai(fnFile, sOptions, trq, win);
		
		long iLines = ai.ScanInfo();
		if (iLines != iUNDEF) ai.Convert(fnObject);
	}
	catch (AbortedByUser& ) {  // just stop, no message
	}
	catch (ErrorObject& err) {
		err.Show();
	}
}
