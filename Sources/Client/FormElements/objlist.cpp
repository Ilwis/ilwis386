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
/* $Log: /ILWIS 3.0/FormElements/objlist.cpp $
 * 
 * 61    19-05-05 17:49 Retsios
 * [bug=6424] columns with spaces were not listed by some of the listers
 * 
 * 60    14-10-03 10:58 Retsios
 * Repaired errors from merging
 * 
 * 59    10/10/03 2:44p Lichun
 * Merging from $/ILWIS Hydrologic Flow/FormElements/objlist.cpp
 * 
 * 59    6/13/03 6:26p Lichun
 * Added code to list the maps by name of the Domain in FieldDataType   
 * 
 * 58    4/22/03 3:13p Lichun
 * Added a class MapListerDomainName to list only maps with the specified
 * domain name. 
 * 
 * 58    2-10-03 12:21 Retsios
 * Merging from $/ILWIS SMCE/FormElements/objlist.cpp
 * Read FileName and not String because ObjectInfo assigns the matching
 * path from the map to the filename, while (see previous solution) the
 * FileName created from String did not guarantee this. As a result
 * non-existing domain filenames were constructed.
 * 
 * 57    9/24/03 4:19p Martin
 * [bug 4216] added handling of the string domain for Mapdomainlister
 * 
 * 56    2/06/03 12:00p Lichun
 * 
 * 55    9/06/02 4:43p Lichun
 * implemented TableListerDomainType 
 * 
 * 55    19-11-02 16:09 Retsios
 * 
 * 58    19-11-02 15:04 Retsios
 * Merge from Main
 * 
 * 57    18-11-02 20:00 Retsios
 * Reorganized the fAcceptAttributeTable a bit because more objectlisters
 * need it - now ObjectLister has a public virtual member fHasChildren
 * that (currently) does the same as the old
 * MapListerDomainType::fAcceptAttributeTable - this public function is
 * used in ObjectTreeCtrl to decide if there should be an 'expansion'
 * symbol (a  '+') before the labels.
 * 
 * 55    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 54    14-11-02 18:35 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 55    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 54    14-11-02 18:35 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 55    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 54    14-11-02 18:35 Retsios
 * Merge from SMCE - Added MapListerDomainTypeAndGeoRef to filter on a
 * domain type and a georef
 * 
 * 55    9-07-02 19:46 Retsios
 * Added MapListerDomainTypeAndGeoRef to filter on a domain type and a
 * georef (was handy for SMCE)
 * 
 * 53    21-02-02 12:18 Willem
 * Accept any domain when the MapListerDomainAndGeoRef is created with an
 * empty domain filename (needed when adding into empty Maplist, because
 * no domain restrictions exist at that moment)
 * 
 * 52    19-11-01 14:45 Retsios
 * solved LFN bug (#5386): "path widh dot is causing problems" because it
 * is quoted.
 * 
 * 51    13-08-01 16:57 Koolhoven
 * readability: replaced "== false" by a "!"
 * 
 * 50    8-08-01 9:48 Koolhoven
 * MapListerDomainType allow default value to be a maplist
 * 
 * 49    7-08-01 19:45 Koolhoven
 * MapListerDomainType has extra boolean to allow also a maplist instead
 * of a raster map
 * 
 * 48    2-08-01 17:14 Koolhoven
 * added GraphLister::fOK()
 * 
 * 47    5/31/01 18:15 Hendrikse
 * FillDir() now also clears the ObjectTreeCtrl
 * 
 * 46    20/03/01 18:49 Willem
 * MapListerDomainType::fOK(): 
 * - For maplists the SectionPostfix must also be checked to see if the
 * maplist itself is passed or an internal band.
 * - The sCHECK_OBJECT_ONLY string will be passed in case a MapList is
 * clicked in a ObjectTreeControl; in that particular case the check will
 * fail
 * 
 * 45    19-03-01 18:13 Koolhoven
 * let sDefaultSelectedValue() return also the extension of the file,
 * often it is needed
 * 
 * 44    16-03-01 14:03 Koolhoven
 * DomainLister and DomainAllExtLister only allow "domainunqiueid" when it
 * specifically requested. Because it may not normally be used, only
 * during the creation of a Pojnt, Segment, PolygonMap.
 * 
 * 43    15-03-01 2:57p Martin
 * added extra virtual function it will return for a given filename a
 * possible default choice with that filename. e.g. for maps it will
 * return the map as possible choice or if that is not possible a column
 * from a possible attrib table
 * 
 * 42    13-03-01 15:46 Koolhoven
 * DomainLister::fOk() now allows domains which have another extension
 * then .dom (needed for defaults of an internal (uniqueID) domain in some
 * cases)
 * 
 * 41    8/03/01 17:47 Willem
 * - Added MapListerDomainAndGeoRef to check both Domain and Georef of
 * rastermaps
 * - Improved Maplist handling in Domain and Georef Map listers
 * 
 * 40    7/03/01 16:32 Willem
 * Rewrote  MapListerDomainType::fOK() to handle bands in maplists
 * properly (both internal and extrernal bands).
 * 
 * 39    22/02/01 15:12 Willem
 * The TableLister, RprLister and RprListerExt now checks if the domain
 * was relative to the filename to check. If so the fOK accepts the file
 * fn.
 * 
 * 38    16-02-01 17:24 Koolhoven
 * "DomainUniqueID" is now also allowed as type when dmIDENT is specified,
 * because it is really just a special case 
 * 
 * 37    16/02/01 16:12 Willem
 * MapListerDomainType::fAcceptDomain now also accepts DomainUniqueID
 * 
 * 36    9-02-01 11:28a Martin
 * maplisterdomaintype now checks the full range of option (map plus
 * atrrib column) to decide if a object is allowed
 * 
 * 35    15-01-01 4:31p Martin
 * added UniqueID to domain for DomainAllExtension lister
 * 
 * 34    11-01-01 12:32 Koolhoven
 * GeoRefLister will no longer accept georefNone when gr3DONLY is
 * specified
 * 
 * 33    16-11-00 4:00p Martin
 * multiple columns where no correctly checkec if they are allowed
 * 
 * 32    10-11-00 17:12 Koolhoven
 * added enum csType
 * and class CoordSystemLister
 * 
 * 31    24-10-00 14:58 Koolhoven
 *  RepresentationLister::fOK() for images was not always working
 * correctly
 * 
 * 30    11/10/00 10:47 Willem
 * Changed DomainUniqueIdentifier to DomainUniqueID
 * 
 * 29    10-08-00 15:07 Koolhoven
 * MapListerDomainType is now derived from ObjectExtensionLister, so it
 * now accepts more than 1 extension
 * 
 * 28    27-07-00 15:16 Koolhoven
 * Added UniqueID option to Create PntMap and SegMap forms
 * 
 * 27    18-07-00 2:25p Martin
 * Forgot one check in maplister
 * 
 * 26    18-07-00 12:54p Martin
 * only legal maps are selectable in th objecttree control. Illegal maps
 * with a legal attrib table are visible but not selectable
 * 
 * 25    14-07-00 4:29p Martin
 * sGetCurDir (from IlwisWinApp) instead of sCurDir
 * 
 * 24    25/05/00 15:18 Willem
 * - Case of filenames is now preserved, disabled the ToLower function
 * - Removed a lot of commented code
 * 
 * 23    5/23/00 12:53p Wind
 * MapListerDomainType: if dmTypes is 0 all domains are accepted
 * 
 * 22    17-05-00 12:47p Martin
 * added virtual function for objectextensionlister sFileExt
 * 
 * 21    4-05-00 12:39p Martin
 * added a second parm to the fOK function (default empty). Is used for
 * columns checking. Added column and attribute checking to the
 * MapDomainTypeLister
 * 
 * 20    27-04-00 2:43p Martin
 * objectcollection are visible in the objectree control
 * 
 * 19    3/22/00 5:10p Wind
 * add dmCOORD type for domainlister
 * 
 * 18    31-01-00 11:53 Koolhoven
 * Quicker opening of ObjectTreeCtrl by earlier filtering on extension
 * 
 * 17    14-12-99 18:44 Koolhoven
 * Allow in ObjectExtensionLister also a MapList (.mpl) when only .mpr is
 * allowed
 * 
 * 16    6-12-99 10:19 Wind
 * change grType enum for GoeRefLister and fOK functions now only checks
 * .mpr files for "ContainsGeoRef' (performance improvement)
 * 
 * 15    29-11-99 10:25 Wind
 * adapted for UNC (use now SetCurrentDirectory and GetCurrentDirectory)
 * 
 * 14    17-11-99 8:15a Martin
 * fOk function of MapLister.. now also checks for correct extension
 * 
 * 13    22-10-99 12:59 Wind
 * CString not allowed in FileName constructor
 * 
 * 12    21/10/99 15:40 Willem
 * Undid previous change: class FileName now accepts a CString in the
 * constructor
 * 
 * 10    19-10-99 14:30 Koolhoven
 * Allow MapLists as container of raster maps to show in tree list and
 * nameedit
 * 
 * 9     18-10-99 17:54 Koolhoven
 * NameEdit now no longer shows a ListBox when clicked but a TreeCtrl.
 * 
 * 8     6-10-99 17:49 Koolhoven
 * Removed nameedit.h and objlist.h from formelementspch.h
 * 
 * 7     20-08-99 15:39 Koolhoven
 * Use FindClose() often enough
 * 
 * 6     20-08-99 15:17 Koolhoven
 * AddObjects() now closes the findHandles, to prevent that they stay in
 * use.
 * 
 * 5     11/08/99 9:28 Willem
 * The ObjectLister now counts the number of extensions correctly.
 * 
 * 4     30/03/99 16:55 Willem
 * - The strings compares are now case insensitive again
 * 
 * 3     3/10/99 9:22a Martin
 * 
 * 2     3/10/99 9:22a Martin
 * Case insesitive string comapre added
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/22 13:20:58  Wim
// reread now takes path from ne
//
// Revision 1.2  1997-07-28 19:04:12+02  Wim
// Add DomainAllExtLister. DomainLister only lists .DOM domains
// Added RepresentationAllExtLister. RepresentationLister only lists .RPR representations.
//
/* ObjectLister
   by Wim Koolhoven
   (c) Computer Department ITC
	Last change:  WK   16 Feb 98    5:15 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\nameedit.h"
#include "Client\ilwis.h"
#include "Client\FormElements\objlist.h"

String ObjectLister::sFileExt()
{				
	return ".*";
}

String ObjectLister::sDefaultSelectedValue(const FileName& fn)
{
	return fn.sShortName(true);
}	

void ObjectLister::FillDir()
{
  String sCurDir = IlwWinApp()->sGetCurDir();
  if (ne) {
    String sDir = ne->sPath();

    SetCurrentDirectory(sDir.sVal());
    IlwWinApp()->SetCurDir(sDir);
  }
  HCURSOR hCur = SetCursor(LoadCursor(NULL, IDC_WAIT));
	if (ne)
	{
		ne->ResetContent();
		ne->ResetObjectTreeCtrl();
	}

  AddObjects();
// ObjectTreeCtrl should be the only method to change
// the directory, so exclude here
//  ne->addDir(DRIVES|DIRS|ONLY,"*.*");
  SetCursor(hCur);	    
  IlwWinApp()->SetCurDir(sCurDir);
}

bool ObjectLister::fHasChildren(const FileName& fn)
{
	return false; // default implementation .. override if needed in derived classes
}

ObjectExtensionLister::ObjectExtensionLister(NameEdit* ne, const String& sExtensions)
: ObjectLister(ne)
{
  int iNrExt = 0;
  for (int i=0; i < sExtensions.length(); i++) {
    if (sExtensions[i] == '.') {
      iNrExt++;
      asExt &= new String();
    }
    (*asExt[iNrExt - 1]) &= sExtensions[i];
  }
  for (int i=0; i < iNrExt; i++)
    asExt[i]->toLower();
}

ObjectExtensionLister::~ObjectExtensionLister()
{
  for (int i = 0; i < asExt.iSize(); i++)
    delete asExt[i];
}

void ObjectExtensionLister::AddObjects()
{
  for (int i=0; i < asExt.iSize(); i++) 
    AddObjects(*asExt[i]);
}

String ObjectExtensionLister::sFileExt()
{
	String sResult;
  for (int i=0; i < asExt.iSize(); i++) 
		sResult &= *(asExt[i]);

	return sResult;
}

void ObjectExtensionLister::AddObjects(const String& sExt)
{
  String sMask;
	sMask = ne->sPath();
	sMask &= "\\*";
  sMask &= sExt;

  CFileFind finder;
  BOOL fFound = finder.FindFile(sMask.scVal());
	while (fFound) 
	{
		fFound = finder.FindNextFile();
		if (finder.IsHidden())
			continue;
		if (finder.IsDirectory()) 
			continue;
    String s = finder.GetFilePath(); 
		FileName fn(s); 
		if (!fOK(fn))
			continue;
		ne->AddString(finder.GetFileName());
	}

	sMask = IlwWinApp()->Context()->sStdDir();
  sMask &= "\\*";
  sMask &= sExt;

	finder.Close();
	fFound = finder.FindFile(sMask.scVal());
	while (fFound) 
  {
		fFound = finder.FindNextFile();
		if (finder.IsHidden())
			continue;
		if (finder.IsDirectory()) 
			continue;
		String s = finder.GetFilePath(); 
		FileName fn(s); 
		if (!fOK(fn))
			continue;
		ne->AddString(finder.GetFileName());
  }
}

bool ObjectExtensionLister::fOK(const FileName& fn, const String&)
{
	for (int i = 0; i < asExt.iSize(); ++i)
		if (fCIStrEqual(fn.sExt , *asExt[i]))
			return true;

	// Do a separate check for container objects
	// Object collection may contain anything; a Maplist only raster maps
	if (fCIStrEqual(".ioc", fn.sExt))
		return true;
	if (fCIStrEqual(".mpl", fn.sExt)) 
		for (int i = 0; i < asExt.iSize(); ++i)
			if (fCIStrEqual(".mpr", *asExt[i]))
				return true;
			
	return false;
}


ObjectExtLister::~ObjectExtLister()
{}

String ObjectExtLister::sFileExt()
{				
	return sExt;
}

void ObjectExtLister::AddObjects()
{
  HANDLE findHandle;
  WIN32_FIND_DATA findData;
  String sMask = "*";
  sMask &= sExt;
  if ((findHandle = FindFirstFile(sMask.sVal(),&findData)) != INVALID_HANDLE_VALUE) 
  {
    do {
      String s(findData.cFileName);
//      s.toLower();
      FileName fn(s);
      if (fOK(fn))
        ne->AddString(s.sVal());
    } while (FindNextFile(findHandle, &findData)==TRUE);
  }
	FindClose(findHandle);
  String sStdDir = IlwWinApp()->Context()->svl()->sGet("IlwStdDir");
  sMask = sStdDir;
  sMask &= "\\*";
  sMask &= sExt;
  if ((findHandle = FindFirstFile(sMask.sVal(),&findData)) != INVALID_HANDLE_VALUE)
  {
    do {
      String s(findData.cFileName);
//      s.toLower();
      FileName fn(s);
      fn.Dir(sStdDir);
      if (fOK(fn))
        ne->AddString(s.sVal());
    } while (FindNextFile(findHandle, &findData)==TRUE);
  }
	FindClose(findHandle);
}

bool TableLister::fOK(const FileName& fn, const String&)
{
	String sDom;
	if (!ObjectInfo::ReadElement("Table", "Domain", fn, sDom))
		return false;
	if (sDom.length() == 0)  
		return false;

	FileName fnDom(FileName(sDom, ".dom", false).sShortNameQuoted(true), fn);
	if (!File::fExist(fnDom))
	{
		fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
		if (!File::fExist(fnDom))  //  - No!, try the system dir
			fnDom.Dir(IlwWinApp()->Context()->sStdDir());
	}
	return (fnDom == dm->fnObj);
}

bool TableListerDomainType::fOK(const FileName& fn, const String&)
{
	String sDom;
	if (!ObjectInfo::ReadElement("Table", "Domain", fn, sDom))
		return false;
	if (sDom.length() == 0)  
		return false;

	FileName fnDom(FileName(sDom, ".dom", false).sShortNameQuoted(true), fn);
	if (!File::fExist(fnDom))
	{
		fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
		if (!File::fExist(fnDom))  //  - No!, try the system dir
			fnDom.Dir(IlwWinApp()->Context()->sStdDir());
	}
	int dmType = dmtype(Domain::dmt(fnDom));
	if (dmType == 0)  // check for DomainBinary or DomainNone
		return false;

	return ((dmType & m_dmTypes) == dmType);
}

bool TableAsLister::fOK(const FileName& fn, const String&)
{
  return ObjectInfo::fTable(fn); 
}

bool DomainLister::fOK(const FileName& fn, const String&)
{
  if ((fn.sExt != ".dom") && (fn.sExt != ".csy")) 
		if (!ObjectInfo::fDomain(fn))
			return false;
  if (0 == dmTypes)
    return true;  
	if (fn.sExt == ".csy" && (dmTypes & dmCOORD))
		return true;
  String sType;
  ObjectInfo::ReadElement("Domain", "Type", fn, sType);
  if ((dmTypes & dmCLASS) && (fCIStrEqual("DomainClass" , sType))) return true;
  if ((dmTypes & dmIDENT) && (fCIStrEqual("DomainIdentifier" , sType))) return true;
  if (((dmTypes & dmCLASS) || (dmTypes & dmGROUP)) && (fCIStrEqual("DomainGroup" , sType))) return true;
  if ((dmTypes & dmVALUE) && (fCIStrEqual("DomainValue" , sType))) return true;
  if ((dmTypes & dmIMAGE) && (fCIStrEqual("DomainImage" , sType))) return true;
  if ((dmTypes & dmNONE)  && (fCIStrEqual("DomainNone" , sType))) return true;
  if ((dmTypes & dmPICT)  && (fCIStrEqual("DomainPicture" , sType))) return true;
  if ((dmTypes & dmCOLOR) && (fCIStrEqual("DomainColor" , sType))) return true;
  if ((dmTypes & dmBOOL)  && (fCIStrEqual("DomainBool" , sType))) return true;
  if ((dmTypes & dmBIT)   && (fCIStrEqual("DomainBit" , sType))) return true;
  if ((dmTypes & dmSTRING) && (fCIStrEqual("DomainString" , sType))) return true;
  if (((dmTypes & dmIDENT) || (dmTypes & dmUNIQUEID)) && (fCIStrEqual("DomainUniqueId", sType))) 
	{
		// new UniqueID is created by selecting uniqueid.dom, this should only be possible if asking for it explicitly
		if (fn.sExt == ".dom" && !(dmTypes & dmUNIQUEID)) 
			return false;
		else
			return true;
	}
  return false;
}

bool DomainAllExtLister::fOK(const FileName& fn, const String&)
{
 	if (fn.sExt == ".csy" && (dmTypes & dmCOORD))
		return true;
  if (fn.sExt != ".dom") {
    if (!ObjectInfo::fDomain(fn))
      return false;
  }
  if (0 == dmTypes)
    return true;  
  String sType;
  ObjectInfo::ReadElement("Domain", "Type", fn, sType);
  if ((dmTypes & dmCLASS) && (fCIStrEqual("DomainClass" , sType))) return true;
  if ((dmTypes & dmIDENT) && (fCIStrEqual("DomainIdentifier" , sType))) return true;
  if (((dmTypes & dmCLASS) || (dmTypes & dmGROUP)) && (fCIStrEqual("DomainGroup", sType))) return true;
  if ((dmTypes & dmVALUE) && (fCIStrEqual("DomainValue" , sType))) return true;
  if ((dmTypes & dmIMAGE) && (fCIStrEqual("DomainImage" , sType))) return true;
  if ((dmTypes & dmNONE)  && (fCIStrEqual("DomainNone" , sType))) return true;
  if ((dmTypes & dmPICT)  && (fCIStrEqual("DomainPicture" , sType))) return true;
  if ((dmTypes & dmCOLOR) && (fCIStrEqual("DomainColor" , sType))) return true;
  if ((dmTypes & dmBOOL)  && (fCIStrEqual("DomainBool" , sType))) return true;
  if ((dmTypes & dmBIT)   && (fCIStrEqual("DomainBit" , sType))) return true;
  if ((dmTypes & dmSTRING) && (fCIStrEqual("DomainString" , sType))) return true;
  if (((dmTypes & dmIDENT) || (dmTypes & dmUNIQUEID)) && (fCIStrEqual("DomainUniqueId", sType))) 
	{
		// new UniqueID is created by selecting uniqueid.dom, this should only be possible if asking for it explicitly
		if (fn.sExt == ".dom" && !(dmTypes & dmUNIQUEID)) 
			return false;
		else
			return true;
	}

  return false;
}

bool RepresentationLister::fOK(const FileName& fn, const String&)
{
	if (!dm.fValid())
		return false;
	String sType;
	String sDomain;
	if (!fCIStrEqual(fn.sExt , ".rpr"))
	{
		if (!ObjectInfo::fRepresentation(fn))
			return false;
	}
	ObjectInfo::ReadElement("Representation", "Type", fn, sType);
	if (dm->pdsrt())
	{
		if (dm->pdc())
		{
			if (fCIStrEqual("RepresentationClass" , sType))
			{
				ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
				FileName fnDom (sDomain, fn); // always create fnDom relative to fn
				if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
				{
					fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
					if (!File::fExist(fnDom))  //  - No!, try the system dir
						fnDom.Dir(IlwWinApp()->Context()->sStdDir());
				}
				return (fnDom == dm->fnObj);
			}  
		}
		if (fRprAttr && fCIStrEqual("RepresentationAttribute" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}    
	else if (dm->pdv())
	{
		if (fCIStrEqual("RepresentationGradual" , sType))
			return true;
		else if (fCIStrEqual("RepresentationValue" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			if (dm->pdi() && fCIStrEqual(sDomain , "image"))
				return true;
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}
	else if (dm->pdp())
	{
		if (fCIStrEqual("RepresentationClass" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}
	return false;
}

void RepresentationLister::SetDomain(const Domain& dom)
{
  if (dom.fValid() && dm.fValid() && dm == dom)
    return;
  dm = dom;
  FillDir();
}

bool RepresentationAllExtLister::fOK(const FileName& fn, const String&)
{
	if (!dm.fValid())
		return false;
	String sType;
	String sDomain;
	if (!fCIStrEqual(fn.sExt , ".rpr"))
	{
		if (!ObjectInfo::fRepresentation(fn))
			return false;
	}
	ObjectInfo::ReadElement("Representation", "Type", fn, sType);
	if (dm->pdsrt())
	{
		if (dm->pdc())
		{
			if (fCIStrEqual("RepresentationClass" , sType))
			{
				ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
				FileName fnDom (sDomain, fn); // always create fnDom relative to fn
				if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
				{
					fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
					if (!File::fExist(fnDom))  //  - No!, try the system dir
						fnDom.Dir(IlwWinApp()->Context()->sStdDir());
				}
				return (fnDom == dm->fnObj);
			}  
		}
		if (fRprAttr && fCIStrEqual("RepresentationAttribute" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}    
	else if (dm->pdv())
	{
		if (fCIStrEqual("RepresentationGradual" , sType))
			return true;
		else if (fCIStrEqual("RepresentationValue" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			if (dm->pdi())
				return (fCIStrEqual(sDomain , "image"));
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}
	else if (dm->pdp())
	{
		if (fCIStrEqual("RepresentationClass" , sType))
		{
			ObjectInfo::ReadElement("Representation", "Domain", fn, sDomain);
			FileName fnDom (sDomain, fn); // always create fnDom relative to fn
			if (!File::fExist(fnDom)) // the following 2 lines became obsolete; cleanup sometime
			{
				fnDom.Dir(fn.sPath());     // Was the Domain relative to fn?
				if (!File::fExist(fnDom))  //  - No!, try the system dir
					fnDom.Dir(IlwWinApp()->Context()->sStdDir());
			}
			return (fnDom == dm->fnObj);
		}  
	}
	return false;
}

void RepresentationAllExtLister::SetDomain(const Domain& dom)
{
	if (dom.fValid() && dm.fValid() && dm == dom)
		return;
	dm = dom;
	FillDir();
}

bool GeoRefLister::fOK(const FileName& fn, const String&)
{
	FileName fnBand = fn;
	if (fCIStrEqual(fn.sExt, ".mpl") && fnBand.sSectionPostFix.length() == 0)
	{
		long iBands = 0;
		ObjectInfo::ReadElement("MapList", "Maps", fn, iBands);
		if (iBands == 0)
			return false;

		// Select the first band in the maplist as Map to check the domain
		long iOffset = 0;  // Initialize to get rid of warning
		ObjectInfo::ReadElement("MapList", "Offset", fn, iOffset);  // Offset is read properly
		String sKey = String("Map%li", iOffset);
		ObjectInfo::ReadElement("MapList", sKey.scVal(), fn, fnBand);
	}

	// Check for raster maps (also in maplists): only they can contain georeferences
	//   fnBand: If the extension = ".mpl" the section postfix is set
	if (fCIStrEqual(fnBand.sExt , ".mpr") || fCIStrEqual(fnBand.sExt , ".mpl"))
	{
		String s = "False";
		int iRet = ObjectInfo::ReadElement("Ilwis", "ContainsGeoRef", fnBand, s);
		if (0 == iRet || !fCIStrEqual(s , "True"))
			return false;
	}
	else if (!fCIStrEqual(fnBand.sExt , ".grf")) 
		return false;

	String sType;
	ObjectInfo::ReadElement("GeoRef", "Type", fnBand, sType);
	if (fCIStrEqual("GeoRef3D" , sType))
	{
		if (grTypes & gr3DONLY) 
			return true;
		else if (grTypes & grNO3D) 
			return false;
		return grTypes & grALL;
	}
	if (grTypes & gr3DONLY)
		return false;
	if (fCIStrEqual("GeoRefNone" , sType))
		return !(grTypes & grNONONE);
	return (grTypes & grALL);
}

bool FilterLister::fOK(const FileName& fn, const String&)
{
  if (!fCIStrEqual(fn.sExt , ".fil")) 
		return false;
  String sType;
  ObjectInfo::ReadElement("Filter", "Type", fn, sType);
  if ((filTypes & filLINEAR) && (fCIStrEqual("FilterLinear" , sType))) return true;
  if ((filTypes & filRANK) && (fCIStrEqual("FilterRankOrder" , sType))) return true;
  if ((filTypes & filMAJORITY) && (fCIStrEqual("FilterMajority" , sType))) return true;
  if ((filTypes & filRADAR) && (fCIStrEqual("FilterRadar" , sType))) return true;
  if ((filTypes & filBINARY) && (fCIStrEqual("FilterBinary" , sType))) return true;
  return false;
}

// The sColName parameter is used to check columns, except when
// the special string sCHECK_OBJECT_ONLY is used. In this case fOK() should
// only look at the object itself not at contained objects or columns.
bool MapListerDomainType::fOK(const FileName& fnMap, const String& sColName)
{
	if (!ObjectExtensionLister::fOK(fnMap))
		return false;

	// Reject any object that is not a Map or MapList
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mps") ||
	       fCIStrEqual(fnMap.sExt , ".mpa") ||
	       fCIStrEqual(fnMap.sExt , ".mpp") ||
	       fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
		return false;

	FileName fnBand = fnMap;
	bool fCheckObjectSelf = sCHECK_OBJECT_ONLY == sColName;
	if (fCIStrEqual(fnMap.sExt , ".mpl") && fnBand.sSectionPostFix.length() == 0)
	{
		if (fCheckObjectSelf && !fAllowMapList)
			return false;

		long iBands = 0;
		ObjectInfo::ReadElement("MapList", "Maps", fnMap, iBands);
		if (iBands == 0)
			return false;

		// Select the first band in the maplist as Map to check the domain
		long iOffset = 0;  // Initialize to get rid of warning
		ObjectInfo::ReadElement("MapList", "Offset", fnMap, iOffset);  // Offset is read properly
		String sKey = String("Map%li", iOffset);
		ObjectInfo::ReadElement("MapList", sKey.scVal(), fnMap, fnBand);
	}
	FileName fnDom;
	ObjectInfo::ReadElement("BaseMap", "Domain", fnBand, fnDom);

	if (!fCheckObjectSelf && sColName != "" && fUseAttribColumns)
	{
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fnBand);
		return fAcceptColumn(fnAttrib, sColName);
	}		
	if (fCheckObjectSelf && !fUseAttribColumns) // special condition, in case an illegal map has been selected with legal columns
		return fAcceptDomain(fnDom);
	
	return fAcceptDomain(fnDom) || fAcceptAttributeTable(fnBand);
}

bool MapListerDomainType::fAcceptColumn(const FileName& fnAttrib, const String& sCol)
{
	if ( sCol != "")
	{
		FileName fnDom;
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).scVal(), "Domain", fnAttrib, fnDom);
		return fAcceptDomain(fnDom);
	}
	return false;
}

String MapListerDomainType::sDefaultSelectedValue(const FileName& fn)
{
	BaseMap mp;
  if (".mpl" == fn.sExt)
  {
    MapList mpl(fn);
    mp = mpl[mpl->iLower()];
  }
  else
    mp = BaseMap(fn);
	Domain dm = mp->dm();
	if (fAcceptDomain(dm->fnObj))
		return fn.sShortName(true);
	else
	{
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fn);
		if (fnAttrib == FileName())
			return "";
		Table tbl(fnAttrib);
		if (tbl.fValid())
		{
			for(int i=0; i < tbl->iCols() ; ++i)
			{
				String sName = tbl->col(i)->sName();
				if (fAcceptColumn(fnAttrib, sName))
					return String("%S|%S|%S.clm", fn.sFullPath(), fnAttrib.sFullPath(), sName);
			}			
		}			
	}		
	return "";
}

bool MapListerDomainType::fHasChildren(const FileName& fn)
{
	return fAcceptAttributeTable(fn);
}

bool MapListerDomainType::fAcceptAttributeTable(const FileName& fn)
{
	if ( !fUseAttribColumns) return false;

	FileName fnAttrib = ObjectInfo::fnAttributeTable(fn);
	if ( fnAttrib == FileName() )
		return false;
	int iCols;
	ObjectInfo::ReadElement("Table", "Columns", fnAttrib, iCols);
	bool fOK = false;
	for ( int i=0; i< iCols; ++i)
	{
		String sCol;
		ObjectInfo::ReadElement("TableStore", String("col%d", i).scVal(), fnAttrib, sCol);
		fOK |= fAcceptColumn(fnAttrib, sCol);

	}
	return fOK;
}

bool MapListerDomainType::fAcceptDomain(FileName fnDom)
{
	if (fnDom.sExt.length() == 0)
    fnDom.sExt = ".dom";
  if (!File::fExist(fnDom)) {
    fnDom.Dir(IlwWinApp()->Context()->sStdDir());
    if (!File::fExist(fnDom)) 
      return false;
  }
	if (0 == dmTypes)
		return true;
  String sType;
  ObjectInfo::ReadElement("Domain", "Type", fnDom, sType);
  if ((dmTypes & dmCLASS) && (fCIStrEqual("DomainClass" , sType))) return true;
  if ((dmTypes & dmIDENT) && (fCIStrEqual("DomainIdentifier" , sType))) return true;
  if (((dmTypes & dmCLASS) || (dmTypes & dmGROUP)) && (fCIStrEqual("DomainGroup" , sType))) return true;
  if ((dmTypes & dmVALUE) && (fCIStrEqual("DomainValue" , sType))) return true;
  if ((dmTypes & dmIMAGE) && (fCIStrEqual("DomainImage" , sType))) return true;
  if ((dmTypes & dmNONE)  && (fCIStrEqual("DomainNone" , sType))) return true;
  if ((dmTypes & dmPICT)  && (fCIStrEqual("DomainPicture" , sType))) return true;
  if ((dmTypes & dmCOLOR) && (fCIStrEqual("DomainColor" , sType))) return true;
  if ((dmTypes & dmBOOL)  && (fCIStrEqual("DomainBool" , sType))) return true;
  if ((dmTypes & dmBIT)   && (fCIStrEqual("DomainBit" , sType))) return true;
  if ((dmTypes & dmUNIQUEID) && (fCIStrEqual("DomainUniqueID" , sType))) return true;
  if ((dmTypes & dmSTRING) && (fCIStrEqual("DomainString" , sType))) return true;
  return false;
}

///////////////////////////////////////////////////////////////
bool MapListerDomainTypeAndGeoRef::fOK(const FileName& fnMap, const String& sColName)
{
	if (!MapListerDomainType::fOK(fnMap, sColName))
		return false;
	// The following code is copied in its entirety from MapListerGeoRef::fOK
	// Perhaps it could be cleaned up, but no real gain ...

	// Reject any object that is not a Map or MapList
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mps") ||
	       fCIStrEqual(fnMap.sExt , ".mpa") ||
	       fCIStrEqual(fnMap.sExt , ".mpp") ||
	       fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
		return false;

	FileName fnBand = fnMap;
	if (fCIStrEqual(fnMap.sExt , ".mpl") && fnBand.sSectionPostFix.length() == 0)
	{
		long iBands = 0;
		ObjectInfo::ReadElement("MapList", "Maps", fnMap, iBands);
		if (iBands == 0)
			return false;

		// Select the first band in the maplist as Map to check the domain
		long iOffset = 0;  // Initialize to get rid of warning
		ObjectInfo::ReadElement("MapList", "Offset", fnMap, iOffset);  // Offset is read properly
		String sKey = String("Map%li", iOffset);
		ObjectInfo::ReadElement("MapList", sKey.scVal(), fnMap, fnBand);
	}

	// fnBand is now a Map or a band in a maplist
	FileName fnGeoRef;
	ObjectInfo::ReadElement("Map", "GeoRef", fnBand, fnGeoRef);
	
	return (fnGrf == fnGeoRef);
}
///////////////////////////////////////////////////////////////

bool MapListerGeoRef::fOK(const FileName& fnMap, const String&)
{
	// Reject any object that is not a Map or MapList
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mps") ||
	       fCIStrEqual(fnMap.sExt , ".mpa") ||
	       fCIStrEqual(fnMap.sExt , ".mpp") ||
	       fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
		return false;

	FileName fnBand = fnMap;
	if (fCIStrEqual(fnMap.sExt , ".mpl") && fnBand.sSectionPostFix.length() == 0)
	{
		long iBands = 0;
		ObjectInfo::ReadElement("MapList", "Maps", fnMap, iBands);
		if (iBands == 0)
			return false;

		// Select the first band in the maplist as Map to check the domain
		long iOffset = 0;  // Initialize to get rid of warning
		ObjectInfo::ReadElement("MapList", "Offset", fnMap, iOffset);  // Offset is read properly
		String sKey = String("Map%li", iOffset);
		ObjectInfo::ReadElement("MapList", sKey.scVal(), fnMap, fnBand);
	}

	// fnBand is now a Map or a band in a maplist
	FileName fnGeoRef;
	ObjectInfo::ReadElement("Map", "GeoRef", fnBand, fnGeoRef);
	
	return (fnGrf == fnGeoRef);
}

bool MapListerDomainName::fOK(const FileName& fnMap, const String&)
{
	// Reject any object that is not a Map or MapList
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
		return false;

	FileName fnDomain;
	ObjectInfo::ReadElement("BaseMap", "Domain", fnMap, fnDomain);
	
	return (fnDom == fnDomain);
}

// MapListerDomainAndGeoRef checks for Domain and GeoRef equality
bool MapListerDomainAndGeoRef::fOK(const FileName& fnMap, const String& sColName)
{
	// Reject any object that is not a Map
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mps") ||
	       fCIStrEqual(fnMap.sExt , ".mpa") ||
	       fCIStrEqual(fnMap.sExt , ".mpp")) )
		return false;

	bool fCheckObjectSelf = sCHECK_OBJECT_ONLY == sColName;
	// fCheckObjectSelf == true means the user clicked (selected) an item
	// fCheckObjectSelf == false means the tree is filling/expanding
	
	FileName fnDom;
	ObjectInfo::ReadElement("BaseMap", "Domain", fnMap, fnDom);
	if (fnDom.sExt.length() == 0)
    fnDom.sExt = ".dom";
	fnDom = IlwisObjectPtr::fnCheckPath(fnDom);

	// Accept any domain when the MapListerDomainAndGeoRef is created with
	// an empty domain filename (needed when adding into empty Maplist, because no domain 
	// restrictions exist at that moment)
	if (m_fnDom.fValid())
	{
		if (!fCheckObjectSelf && sColName != "" && m_fUseAttribColumns)
		{
			FileName fnAttrib = ObjectInfo::fnAttributeTable(fnMap);
			if (!fAcceptColumn(fnAttrib, sColName))
				return false;
				// otherwise "true" as far as the domain is concerned, continue checking the georef
				// perhaps all this should be solved by deriving from BaseMapListerDomain and calling
				// its fOK
		}
		else
		{
			if (fCheckObjectSelf && !m_fUseAttribColumns) // special condition, in case an illegal map has been selected with legal columns
			{
				if (!fAcceptDomain(fnDom))
					return false;
			}
			else
			{
				if (fCheckObjectSelf)
				{
					if (!fAcceptDomain(fnDom)) // user clicked - refuse map selection if domain isn't suitable
					return false;
				}
				else
				{
					if (!(fAcceptDomain(fnDom) || fAcceptAttributeTable(fnMap))) // fill table - include map if attribtable contains suitable col
					return false; 
				}
			}
		}
	}

	GeoRef grMap;
	ObjectInfo::ReadElement("Map", "GeoRef", fnMap, grMap);
	FileName fnGeoRef = grMap->fnObj;

	if (m_fnGrf == fnGeoRef)
	{
		// Check RowCol size for GeoRefNone
		if (grMap->fGeoRefNone())
		{
			Map mp(fnMap);
			return mp->rcSize() == m_rcSize;
		}
		else
			return true;
	}

	return false;
}

bool MapListerDomainAndGeoRef::fAcceptColumn(const FileName& fnAttrib, const String& sCol)
{
	if ( sCol != "")
	{
		FileName fnDom;
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).scVal(), "Domain", fnAttrib, fnDom);
		return fAcceptDomain(fnDom);
	}
	return false;
}

bool MapListerDomainAndGeoRef::fHasChildren(const FileName& fn)
{
	return fAcceptAttributeTable(fn);
}

bool MapListerDomainAndGeoRef::fAcceptAttributeTable(const FileName& fn)
{
	if ( !m_fUseAttribColumns) return false;

	FileName fnAttrib = ObjectInfo::fnAttributeTable(fn);
	if ( fnAttrib == FileName() )
		return false;
	int iCols;
	ObjectInfo::ReadElement("Table", "Columns", fnAttrib, iCols);
	bool fOK = false;
	for ( int i=0; i< iCols; ++i)
	{
		String sCol;
		ObjectInfo::ReadElement("TableStore", String("col%d", i).scVal(), fnAttrib, sCol);
		fOK |= fAcceptColumn(fnAttrib, sCol);
	}
	return fOK;
}

bool MapListerDomainAndGeoRef::fAcceptDomain(FileName fnDom)
{
	if (fnDom.sExt.length() == 0)
    fnDom.sExt = ".dom";
  if (!File::fExist(fnDom)) {
    fnDom.Dir(IlwWinApp()->Context()->sStdDir());
    if (!File::fExist(fnDom)) 
      return false;
  }
	return (m_fnDom == fnDom);
}

bool BaseMapListerDomain::fOK(const FileName& fnMap, const String& sColName)
{
	// Reject any object that is not a Map or MapList
	if (! (fCIStrEqual(fnMap.sExt , ".mpr") ||
	       fCIStrEqual(fnMap.sExt , ".mps") ||
	       fCIStrEqual(fnMap.sExt , ".mpa") ||
	       fCIStrEqual(fnMap.sExt , ".mpp") ||
	       fCIStrEqual(fnMap.sExt , ".mpl")) )  // allow MapList bands
		return false;

	bool fCheckObjectSelf = sCHECK_OBJECT_ONLY == sColName;
	// fCheckObjectSelf == true means the user clicked (selected) an item
	// fCheckObjectSelf == false means the tree is filling/expanding

	FileName fnBand = fnMap;
	if (fCIStrEqual(fnMap.sExt , ".mpl") && fnBand.sSectionPostFix.length() == 0)
	{
		long iBands = 0;
		ObjectInfo::ReadElement("MapList", "Maps", fnMap, iBands);
		if (iBands == 0)
			return false;

		// Select the first band in the maplist as Map to check the domain
		long iOffset = 0;  // Initialize to get rid of warning
		ObjectInfo::ReadElement("MapList", "Offset", fnMap, iOffset);  // Offset is read properly
		String sKey = String("Map%li", iOffset);
		ObjectInfo::ReadElement("MapList", sKey.scVal(), fnMap, fnBand);
	}

	FileName fnDomain;
	ObjectInfo::ReadElement("BaseMap", "Domain", fnBand, fnDomain);

	if (!fCheckObjectSelf && sColName != "" && m_fUseAttribColumns)
	{
		FileName fnAttrib = ObjectInfo::fnAttributeTable(fnBand);
		return fAcceptColumn(fnAttrib, sColName);
	}		
	if (fCheckObjectSelf && !m_fUseAttribColumns) // special condition, in case an illegal map has been selected with legal columns
		return fAcceptDomain(fnDomain);

	if (fCheckObjectSelf)
		return fAcceptDomain(fnDomain); // user clicked - refuse map selection if domain isn't suitable
	else	
		return fAcceptDomain(fnDomain) || fAcceptAttributeTable(fnBand); // fill table - include map if attribtable contains suitable col
}

bool BaseMapListerDomain::fAcceptColumn(const FileName& fnAttrib, const String& sCol)
{
	if ( sCol != "")
	{
		FileName fnDom;
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).scVal(), "Domain", fnAttrib, fnDom);
		return fAcceptDomain(fnDom);
	}
	return false;
}

bool BaseMapListerDomain::fHasChildren(const FileName& fn)
{
	return fAcceptAttributeTable(fn);
}

bool BaseMapListerDomain::fAcceptAttributeTable(const FileName& fn)
{
	if ( !m_fUseAttribColumns) return false;

	FileName fnAttrib = ObjectInfo::fnAttributeTable(fn);
	if ( fnAttrib == FileName() )
		return false;
	int iCols;
	ObjectInfo::ReadElement("Table", "Columns", fnAttrib, iCols);
	bool fOK = false;
	for ( int i=0; i< iCols; ++i)
	{
		String sCol;
		ObjectInfo::ReadElement("TableStore", String("col%d", i).scVal(), fnAttrib, sCol);
		fOK |= fAcceptColumn(fnAttrib, sCol);
	}
	return fOK;
}

bool BaseMapListerDomain::fAcceptDomain(FileName fnDom)
{
	if (fnDom.sExt.length() == 0)
    fnDom.sExt = ".dom";
  if (!File::fExist(fnDom)) {
    fnDom.Dir(IlwWinApp()->Context()->sStdDir());
    if (!File::fExist(fnDom)) 
      return false;
  }
	return (m_fnDom == fnDom);
}

bool CoordSystemLister::fOK(const FileName& fn, const String&)
{
  if (!fCIStrEqual(fn.sExt , ".csy")) 
		return false;
  String sType;
  ObjectInfo::ReadElement("CoordSystem", "Type", fn, sType);
  if ((csTypes & csBOUNDSONLY) && (fCIStrEqual("BoundsOnly" , sType))) return true;
  if ((csTypes & csPROJ) && (fCIStrEqual("Projection" , sType) || "" == sType)) return true;
  if ((csTypes & csLATLON) && (fCIStrEqual("LatLon" , sType))) return true;
  if ((csTypes & csFORMULA) && (fCIStrEqual("Formula" , sType))) return true;
  if ((csTypes & csTIEPOINTS) && (fCIStrEqual("TiePoints" , sType))) return true;
  if ((csTypes & csORTHOPHOTO) && (fCIStrEqual("OrthoPhoto" , sType))) return true;
  if ((csTypes & csDIRECTLINEAR) && (fCIStrEqual("DirectLinear" , sType))) return true;
  return false;
}

bool GraphLister::fOK(const FileName& fn, const String&)
{
  if (!fCIStrEqual(fn.sExt , ".grh")) 
		return false;
  String sType;
  ObjectInfo::ReadElement("Graph", "Type", fn, sType);
  if ((grTypes & grhGRAPH) && (fCIStrEqual("StandardGraph", sType))) return true;
  if ((grTypes & grhROSEDIAGRAM) && (fCIStrEqual("RoseDiagram", sType))) return true;
  return false;
}
