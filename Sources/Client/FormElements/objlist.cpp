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
  BOOL fFound = finder.FindFile(sMask.c_str());
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
	fFound = finder.FindFile(sMask.c_str());
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

bool MapValueRangeLister::fOK(const FileName& fn, const String&) {
	if (fn.sExt != ".mpr")
		return false;

	if (! valuerange.fValid())
		return false;

	ValueRange range;
	ObjectInfo::ReadElement("BaseMap","Range",fn, range);

	if ( !range.fValid())
		return false;
	return valuerange->fContains(range);
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
  if ((dmTypes & dmTIME) && (fCIStrEqual("DomainTime" , sType))) return true;
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
  if ((dmTypes & dmTIME) && (fCIStrEqual("DomainTime" , sType))) return true;
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
			return true; //(fnDom == dm->fnObj);
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
		ObjectInfo::ReadElement("MapList", sKey.c_str(), fn, fnBand);
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
		ObjectInfo::ReadElement("MapList", sKey.c_str(), fnMap, fnBand);
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
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).c_str(), "Domain", fnAttrib, fnDom);
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
		ObjectInfo::ReadElement("TableStore", String("col%d", i).c_str(), fnAttrib, sCol);
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
		ObjectInfo::ReadElement("MapList", sKey.c_str(), fnMap, fnBand);
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
		ObjectInfo::ReadElement("MapList", sKey.c_str(), fnMap, fnBand);
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
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).c_str(), "Domain", fnAttrib, fnDom);
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
		ObjectInfo::ReadElement("TableStore", String("col%d", i).c_str(), fnAttrib, sCol);
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
		ObjectInfo::ReadElement("MapList", sKey.c_str(), fnMap, fnBand);
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
		ObjectInfo::ReadElement(String("Col:%S", sCol.sQuote()).c_str(), "Domain", fnAttrib, fnDom);
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
		ObjectInfo::ReadElement("TableStore", String("col%d", i).c_str(), fnAttrib, sCol);
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
