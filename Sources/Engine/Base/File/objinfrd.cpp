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
/* $Log: /ILWIS 3.0/BasicDataStructures/objinfrd.cpp $
 * 
 * 40    22-03-02 10:39 Koolhoven
 * after some thinking made check in ObjectInfo::ReadElement() on ec equal
 * to check in base function
 * 
 * 39    21-03-02 19:11 Koolhoven
 * Correction of change 37. Domain in an ElementContainer has to be read.
 * why 37 is there at all is not clear to me, so I do not dare to remove
 * it all together
 * 
 * 38    11/21/01 9:37 Willem
 * The discrimination between GeoRef expression was not complete:
 * There are three input possibilities:
 *    1. Regular GeoRef with extension
 *    2. Regular GeoRef without extension
 *    3. Internal GeoRef
 * 3. is tried first, if it fails 1. and 2. are combined by setting the
 * extension and tried
 * 
 * 37    10/30/01 12:33p Martin
 * from non existing files the domains section should not be read.
 * 
 * 36    16-08-01 19:01 Koolhoven
 * ReadElement() of RealMatrix now does not crash anymore on zero size
 * matrix
 * 
 * 35    7-08-01 11:26 Koolhoven
 * ObjectInfo::ReadElement(): replace invalid georef by none.grf
 * 
 * 34    6/21/01 9:55 Willem
 * ReadElement for GeoRefs now first checks if the xtension of the value
 * in the ODF exist. If not the extension is set to  .GRF. This is
 * necessary because only then the system directory is checked for this
 * georef.
 * 
 * 33    26/02/01 15:22 Willem
 * ReadEelement(): When reading a filename from a MapView ODF the path
 * information was lost (current directory was set instead)
 * 
 * 32    12-02-01 9:22a Martin
 * throws an error instead of showing it locally in read. Better handling
 * is than possible
 * 
 * 31    6-11-00 8:36a Martin
 * ReadElement checks if it is an ilwisobject before attempting toe read
 * (else it will try to access huge binary files, consumes too much time).
 * 
 * 30    31-10-00 12:54 Koolhoven
 * in ReadElement(...,fn) if fnObj has no valid path use current directory
 * as default
 * 
 * 29    25-09-00 15:30 Koolhoven
 * just to be sure sSize.scVal() instead of sVal()
 * 
 * 28    20-09-00 19:19 Koolhoven
 * replaced short by int, because sscanf(  %i  ) asks for an int
 * 
 * 27    19-07-00 9:25a Martin
 * forgot one to check one find error
 * 
 * 26    17-07-00 1:43p Martin
 * forgot to block one find error (if needed)
 * 
 * 25    17-07-00 9:05a Martin
 * changed the handling of Current Directory. Is now stored in thread
 * local storage through Tls mechanism. IlwisWinApp has now some access
 * function to Set/Get/Initialize and remove the thread local. AppContext
 * is no longer used for this
 * 
 * 24    19/05/00 12:50 Willem
 * Added #pragma for 4503 and 4786 to suppress the very long warning
 * messages
 * 
 * 23    17-05-00 19:58 Koolhoven
 * :ReadElement(    fn)
 * should take fnObj directory as default
 * 
 * 22    3/27/00 4:04p Wind
 * added functions to read/write a CObject (stored in hex string)
 * 
 * 21    14-03-00 19:03 Koolhoven
 * Added ReadElement and WriteElement for a char* and a length
 * (storage as hexstring)
 * 
 * 20    9-03-00 10:47 Koolhoven
 * Erroneaously the ReadElement() for RowCol and MinMax was fallen away
 * 
 * 19    8-03-00 18:47 Koolhoven
 * Added ReadElement() and WriteElement() for CSize and CRect
 * 
 * 18    6-03-00 12:28 Wind
 * change title of error message when loading non existing domain
 * 
 * 17    22-12-99 18:13 Koolhoven
 * Derived ElementContainer from FileName
 * it has ElementMap* as a member.
 * In ObjectInfo::ReadElement and ObjectInfo::WriteElement the ElementMap
 * is used when available instead of the file.
 * ElementMap is initialized in MapCompositionDoc::Serialize to load/store
 * a MapView.
 * Further has IlwisObjectPtr now a SetFileName() function to prevent
 * everythere the const_casts of fnObj.
 * 
 * 16    21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 15    13-12-99 12:46 Wind
 * arrays are now zero based, ReadElement and WriteElement adapted
 * 
 * 14    29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 13    20-10-99 15:55 Wind
 * 
 * 12    10/18/99 1:03p Wind
 * made thread save by removing static variable
 * 
 * 11    9/29/99 11:00a Wind
 * added support for use of FileName::sSectionPostFix
 * 
 * 10    23-09-99 12:26 Koolhoven
 * Corrected protection against null pointers in ReadElement()
 * 
 * 9     22-09-99 15:51 Koolhoven
 * ReadElement() protected against empty strings
 * 
 * 8     9/08/99 10:08a Wind
 * adpated to use of quoted file and column names
 * 
 * 7     13/07/99 12:50 Willem
 * The atof() strtod() fucntion gave strange problems. (also during
 * linking). The functions have been renamed to be unique. They are now
 * called by their real name. The C library functions atof() and strtod()
 * are not used directly anymore
 * 
 * 6     26/05/99 8:52 Willem
 * ReadElement for string values now leaves the capitalization as is.
 * 
 * 5     15-03-99 17:04 Willem
 * Adjusted to write "Yes"/"No" to ODF for boolean values; ILWIS 2.x does
 * not recognize "true" and "false"
 * 
 * 4     3/11/99 12:15p Martin
 * Added support for Case insesitive 
 * 
 * 3     3/10/99 11:28a Martin
 * further case insesitive support added
 * 
 * 2     3/10/99 9:27a Martin
 * Added case insensitive string compare
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/04 08:13:59  Wim
// ReadElement(  String) had a maximum of 255 characters. Elarged till 32000.
//
/* ObjectInfo ReadElement functions
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK    4 Aug 97   10:11 am
*/

#pragma warning( disable : 4503 )
#pragma warning( disable : 4786 )

#include "Engine\Domain\dm.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Table\Col.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Table\tbl.h"
#include "Engine\Function\FILTER.H"
#include "Engine\Function\CLASSIFR.H"
#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\strtod.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Hs\DAT.hs"
#include "Engine\Base\File\ElementMap.h"
#include <sys/stat.h>
#include <direct.h>

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, String& sValue)
{
  char sBuf[32000];
	if (sSection == 0 || *sSection == 0)
		return 0;
	if (sEntry && *sEntry == 0)
		return 0;
	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&filename);
	if (ec && 0 != ec->em) {
		ElementMap& em = const_cast<ElementMap&>(*ec->em);
		sValue = em(sSection, sEntry);
		return sValue.length();
	}
  if (!filename.fValid())
    return 0;
	if ( IlwisObject::iotObjectType( filename) == IlwisObject::iotANY )
		return 0;
  String s = filename.sFullName();
  const char* sFileName = s.scVal();
  int iRet;
  if (filename.sSectionPostFix.length() == 0)
    iRet = GetPrivateProfileString(sSection, sEntry, sValue.scVal(),
                                     sBuf, 32000, sFileName);
  else {
    String sSec(const_cast<char*>(sSection));
    sSec &= filename.sSectionPostFix;
    iRet = GetPrivateProfileString(sSec.scVal(), sEntry, sValue.scVal(),
                                     sBuf, 32000, sFileName);
  }
  sValue = String(sBuf);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, int& iValue)
{
  String sValue;
  int iRet = ReadElement(sSection, sEntry, filename, sValue);
  if (iRet) 
    iValue = sValue.iVal();
  else
    iValue = shUNDEF;
  return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, long& iValue)
{
  String sValue;
  int iRet = ReadElement(sSection, sEntry, filename, sValue);
  if (iRet) 
    iValue = sValue.iVal();
  else
    iValue = iUNDEF;
  return iRet;  
}                         

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, double& rValue)
{
  String sValue;
  int iRet = ReadElement(sSection, sEntry, filename, sValue);
  if (iRet) 
    rValue = sValue.rVal();
  else
    rValue = rUNDEF;
  return iRet;  
}                         
                         
int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                         const FileName& filename, bool& fValue)
{
  String s;
  int iRet = ReadElement(sSection, sEntry, filename, s);
  if (iRet) 
    fValue = fCIStrEqual(s, "yes")  || fCIStrEqual(s, "true");
  else
    fValue = false;
  return iRet;  
}                         
                         
int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                            const FileName& fnObj, FileName& fn)
{
	String sNam;
	int iRet = ReadElement(sSection, sEntry, fnObj, sNam);
	if (iRet != 0)
	{
		FileName filnam(sNam, fnObj);
		FileName fnRead = filnam;
		if ((filnam.sExt.length() != 0) && (sNam.strchrQuoted(':') == 0))
		{
			// first check in system directory
			filnam.Dir(getEngine()->getContext()->sStdDir());
			if (!File::fExist(filnam))
				filnam = fnRead;  // restore actual filename read from ODF
		}
		fn = filnam;
	}
	else
		fn = FileName();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, RangeInt& rng)
{
  String s;
  int iRet = ReadElement(sSection, sEntry, fnObj, s);
  if (iRet > 0)
    rng = RangeInt(s);
  else
    rng = RangeInt();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, RangeReal& rng)
{
  String s;
  int iRet = ReadElement(sSection, sEntry, fnObj, s);
  if (iRet > 0)
    rng = RangeReal(s);
  else
    rng = RangeReal();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, Time& tim)
{
  String sValue;
  int iRet = ReadElement(sSection, sEntry, fnObj, sValue);
  long l =sValue.iVal();
  if (l != iUNDEF)
    tim = Time(l);
  else
    tim = Time(0);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, RealArray& arr)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  if (iRet == 0)
    return iRet;
  char *p = s.sVal();
  p = strtok(s.sVal(), " ");
  long iSize = atol(p);
  p = strtok(0, " ");
  long iOffset = atol(p);
  p = strtok(0, " ");
//  arr.Resize(iSize, iOffset);
  arr.Resize(iSize+iOffset); // arrays now are zero based
  for (int j=iOffset; j < iOffset+iSize; j++ ) {
    arr[j] = atofILW(p);
    p = strtok(0, " ");
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, RealMatrix& mat)
{
  int iRet;
  String sEnt = sEntry;
  sEnt &= "_Size";
  String sSize;
  iRet = ReadElement(sSection, sEnt.scVal(), fnObj, sSize);
  if (iRet == 0)
    return iRet;
  int iRows, iCols;
  sscanf(sSize.scVal(), "%i %i", &iRows, &iCols);
  if (iRows <= 0 || iCols <= 0) 
  {
    mat = RealMatrix();
    return 0;
  }
  mat = RealMatrix(iRows, iCols);
  for (int i=0; i < mat.iRows(); i++) {
    sEnt = sEntry;
    sEnt &= String("_Row%i", i);
    String sRow;
    iRet += ReadElement(sSection, sEnt.scVal(), fnObj, sRow);
    char * p = strtok(sRow.sVal(), " ");
    for (int j=0; j < mat.iCols(); j++ ) {
      mat(i, j) = atofILW(p);
      p = strtok(0, " ");
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, IntMatrix& mat)
{
  int iRet;
  String sEnt = sEntry;
  sEnt &= "_Size";
  String sSize;
  iRet = ReadElement(sSection, sEnt.scVal(), fnObj, sSize);
  int iRows, iCols;
  sscanf(sSize.scVal(), "%i %i", &iRows, &iCols);
  mat = IntMatrix(iRows, iCols);
  for (int i=0; i < mat.iRows(); i++) {
    sEnt = sEntry;
    sEnt &= String("_Row%i", i);
    String sRow;
    iRet += ReadElement(sSection, sEnt.scVal(), fnObj, sRow);
    char * p = strtok(sRow.sVal(), " ");
    for (int j=0; j < mat.iCols(); j++ ) {
      mat(i, j) = atoi(p);
      p = strtok(0, " ");
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, Coord& crd)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%lg %lg", &crd.x, &crd.y);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, LatLon& ll)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%lg %lg", &ll.Lat, &ll.Lon);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, RowCol& rc)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%li %li", &rc.Row, &rc.Col);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, MinMax& mm)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%li %li %li %li",
         &mm.rcMin.Row, &mm.rcMin.Col,
         &mm.rcMax.Row, &mm.rcMax.Col);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, CSize& sz)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%li %li", &sz.cx, &sz.cy);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, CRect& rect)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%li %li %li %li",
         &rect.top, &rect.left, 
         &rect.bottom, &rect.right);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, CoordBounds& cb)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(), "%lg %lg %lg %lg",
         &cb.cMin.x, &cb.cMin.y,
         &cb.cMax.x, &cb.cMax.y);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                            const FileName& fnObj, GeoRef& gr)
{
	String sGr;
	int iRet = ReadElement(sSection, sEntry, fnObj, sGr);
	if (iRet)
	{
		try
		{
			// There are three input possibilities:
			// 1. Regular GeoRef with extension
			// 2. Regular GeoRef without extension
			// 3. Internal GeoRef
			// 3. is tried first, if it fails 1. and 2. are combined by setting the extension and tried
			FileName fnGrf(sGr, fnObj);
			// try to open internal GeoRef
			try
			{
				gr = GeoRef(sGr, fnObj.sPath());
				return iRet;
			}
			catch (...)
			{
			}
			// It is no internal georef, add extension and try to open as regular georef
			fnGrf.sExt = ".grf";
			if (!File::fExist(fnGrf))  // try in current directory
				fnGrf.Dir(getEngine()->getContext()->sStdDir());
			if (File::fExist(fnGrf))   // try in system directory
				gr = GeoRef(fnGrf);
			else 
			{
				bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
				if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					ErrorNotFound(FileName(sGr, fnObj)).Show();
				gr = GeoRef(FileName("none.grf"));
			}  
		}
		catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
				err.Show();
			gr = GeoRef();
		}
	}
	else
		gr = GeoRef();
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                            const FileName& fnObj, Domain& dm)
{
  String sDom;
	const ElementContainer* ec = dynamic_cast<const ElementContainer*>(&fnObj);
	if ((0 == ec || 0 == ec->em) && !fnObj.fExist())
	{
    DomainInfo dinf(fnObj, sSection);
    dm = dinf.dmUnknown();		
		return 0;
	}		
  int iRet = ReadElement(sSection, sEntry, fnObj, sDom);
	bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
  if (iRet) {
    FileName fnDom(sDom, fnObj);
    try {
      if (fCIStrEqual(fnDom.sExt, ".DOM")) {
        if (!File::fExist(fnDom))
          fnDom.Dir(getEngine()->getContext()->sStdDir());
        if (File::fExist(fnDom))
          dm = Domain(fnDom);
        else {
          if (fCIStrEqual(fnDom.sFile.c_str() , "int") ||
              fCIStrEqual(fnDom.sFile.c_str() , "long")  ||
              fCIStrEqual(fnDom.sFile.c_str() , "real")) {
            dm = Domain("value.dom");
          }
          else {
            ErrorObject err = ErrorNotFound(WhereError(FileName(sDom, fnObj), "Domain"));
            err.SetTitle(fnObj.sFullName());
						if ( fDoNotShowError == 0 || *fDoNotShowError == false)
	            err.Show();
            DomainInfo dinf(fnObj, sSection);
            dm = dinf.dmUnknown();
          }
        }
      }
      else
        dm = Domain(sDom, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
      DomainInfo dinf(fnObj, sSection);
      dm = dinf.dmUnknown();
    }
  }
  else 
	{
    throw ErrorObject(WhatError(String(SDATErrEmptySection_ss.sVal(), sEntry, sSection), errOPEN), fnObj); //.Show();
    DomainInfo dinf(fnObj, sSection);
    dm = dinf.dmUnknown();
  }  
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                           const FileName& fnObj, Representation& rpr)
{
  String sRpr;
  int iRet = ReadElement(sSection, sEntry, fnObj, sRpr);
  if (iRet) {
    try {
      FileName fnRpr(sRpr, fnObj);
      if (fCIStrEqual(fnRpr.sExt , ".RPR")) {
        if (!File::fExist(fnRpr))
          fnRpr.Dir(getEngine()->getContext()->sStdDir());
        rpr = Representation(fnRpr);
      }
      else
        rpr = Representation(sRpr);
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
      rpr = Representation();
    }
  }
  else
    rpr = Representation();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                            const FileName& fnObj, CoordSystem& cs)
{
  String sCS;
  int iRet = ReadElement(sSection, sEntry, fnObj, sCS);
  if (iRet){
    try {
      FileName fnCSy(sCS, fnObj);
      if (fCIStrEqual(fnCSy.sExt, ".csy")) {
        if (!File::fExist(fnCSy))
          fnCSy.Dir(getEngine()->getContext()->sStdDir());
        cs = CoordSystem(fnCSy);
      }
      else
        cs = CoordSystem(sCS);
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
      cs = CoordSystem();
    }
  }
  else
    cs = CoordSystem();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          Map& mp)
{
  mp = Map();
  String sMap;
  int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
  if (iRet) {
    try {
      FileName fnMap(sMap, fnObj);
      if (fCIStrEqual(fnMap.sExt, ".mpr")) {
        if (!File::fExist(fnMap))
          fnMap.Dir(getEngine()->getContext()->sStdDir());
        mp = Map(fnMap);
      }
      else
        mp = Map(sMap, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          SegmentMap& mp)
{
  mp = SegmentMap();
  String sMap;
  int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
  if (iRet) {
    try {
      FileName fnMap(sMap, fnObj);
      if (fCIStrEqual(fnMap.sExt, ".mps")) {
        if (!File::fExist(fnMap))
          fnMap.Dir(getEngine()->getContext()->sStdDir());
        mp = SegmentMap(fnMap);
      }
      else
        mp = SegmentMap(sMap, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          PolygonMap& mp)
{
  mp = PolygonMap();
  String sMap;
  int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
  if (iRet) {
    try {
      FileName fnMap(sMap, fnObj);
      if (fCIStrEqual(fnMap.sExt, ".mpa")) {
        if (!File::fExist(fnMap))
          fnMap.Dir(getEngine()->getContext()->sStdDir());
        mp = PolygonMap(fnMap);
      }
      else
        mp = PolygonMap(sMap, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          PointMap& mp)
{
  mp = PointMap();
  String sMap;
  int iRet = ReadElement(sSection, sEntry, fnObj, sMap);
  if (iRet) {
    try {
      FileName fnMap(sMap, fnObj);
      if (fCIStrEqual(fnMap.sExt, ".mpp")) {
        if (!File::fExist(fnMap))
          fnMap.Dir(getEngine()->getContext()->sStdDir());
        mp = PointMap(fnMap);
      }
      else
        mp = PointMap(sMap, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          Table& tbl)
{
  tbl = Table();
  String sTbl;
  int iRet = ReadElement(sSection, sEntry, fnObj, sTbl);
  if (iRet) {
    try {
      FileName fnTbl(sTbl, fnObj);
      if (fCIStrEqual(fnTbl.sExt, ".tbt")) {
        if (!File::fExist(fnTbl))
          fnTbl.Dir(getEngine()->getContext()->sStdDir());
        tbl = Table(fnTbl);
      }
      else
        tbl = Table(sTbl, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj,
                          MapList& mpl)
{
  mpl = MapList();
  String sMpl;
  int iRet = ReadElement(sSection, sEntry, fnObj, sMpl);
  if (iRet) {
    try {
      FileName fnMpl(sMpl, fnObj);
      if (fCIStrEqual(fnMpl.sExt, ".mpl")) {
        if (!File::fExist(fnMpl))
          fnMpl.Dir(getEngine()->getContext()->sStdDir());
        mpl = MapList(fnMpl);
      }
      else
        mpl = MapList(sMpl, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
    }
  }
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, Filter& flt)
{
  String sFlt;
  int iRet = ReadElement(sSection, sEntry, fnObj, sFlt);
  if (iRet) {
    try {
      FileName fnFlt(sFlt, fnObj);
      if (fCIStrEqual(fnFlt.sExt, ".fil")) {
        if (!File::fExist(fnFlt))
          fnFlt.Dir(getEngine()->getContext()->sStdDir());
        flt = Filter(fnFlt);
      }
      else
        flt = Filter(sFlt, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
      flt = Filter();
    }
  }
  else
    flt = Filter();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, Classifier& clf)
{
  String sClf;
  int iRet = ReadElement(sSection, sEntry, fnObj, sClf);
  if (iRet) {
    try {
      FileName fnClf(sClf, fnObj);
      if (fCIStrEqual(fnClf.sExt.c_str() , ".clf")) {
        if (!File::fExist(fnClf))
          fnClf.Dir(getEngine()->getContext()->sStdDir());
        clf = Classifier(fnClf);
      }
      else
        clf = Classifier(sClf, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
      err.Show();
      clf = Classifier();
    }
  }
  else
    clf = Classifier();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                          const FileName& fnObj, SampleSet& sms)
{
  String sSms;
  int iRet = ReadElement(sSection, sEntry, fnObj, sSms);
  if (iRet) {
    try {
      FileName fnSms(sSms, fnObj);
      if (fCIStrEqual(fnSms.sExt, ".sms")) {
        if (!File::fExist(fnSms))
          fnSms.Dir(getEngine()->getContext()->sStdDir());
        sms = SampleSet(fnSms);
      }
      else
        sms = SampleSet(sSms, fnObj.sPath());
    }
    catch (const ErrorObject& err) 
		{
			bool *fDoNotShowError = (bool *)(getEngine()->pGetThreadLocalVar(IlwisAppContext::tlvDONOTSHOWFINDERROR));
			if ( fDoNotShowError == 0 || *fDoNotShowError == false)
					err.Show();
      sms = SampleSet();
    }
  }
  else
    sms = SampleSet();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, ValueRange& vr)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  if (iRet) {
    vr = ValueRange(s);
//    ValueRangeReal* vrr = vr->vrr();
//    if (0 != vrr)
//      vrr->AdjustRangeToStep();
  }
  else
    vr = ValueRange();
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                                const FileName& fnObj, Color& col)
{
  int iRet;
  String s;
  iRet = ReadElement(sSection, sEntry, fnObj, s);
  sscanf(s.sVal(),"%lx",&col);
  return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry,
                   const FileName& fnObj, char* pc, int iLen)
{
	String sVal;
	int iRet = ReadElement(sSection, sEntry, fnObj, sVal);
	const char* s = sVal.scVal();

	for (int i = 0; i < iLen; ++i) 
	{
		if (i > iRet) {
			*pc++ = 0;
			continue;
		}
		int iVal;
		char c = *s++; 
		if (isdigit(c))
			iVal = c - '0';
		else
			iVal = c - 'A' + 10;
		iVal *= 16;
		c = *s++; 
		if (isdigit(c))
			iVal += c - '0';
		else
			iVal += c - 'A' + 10;
		*pc++ = iVal;
	}
	return iRet;
}

int ObjectInfo::ReadElement(const char* sSection, const char* sEntry, const FileName& fnObj, CObject* cobj)
{
	int iRead, iLen;
	String sEntryLen("%sLen", sEntry);
  ObjectInfo::ReadElement(sSection, sEntryLen.scVal(), fnObj, iLen);
	if (iLen > 0) {
		char* sMem = new char[iLen];
		iRead = ObjectInfo::ReadElement(sSection, sEntry, fnObj, sMem, iLen);
		CMemFile memobj;
		memobj.Write(sMem, iLen);
		memobj.Seek(0, CFile::begin);
		CArchive ca(&memobj, CArchive::load);
		cobj->Serialize(ca);
		return iRead;
	}
	else
		return 0;
}
