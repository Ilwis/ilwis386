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
/* $Log: /ILWIS 3.0/BasicDataStructures/dm.cpp $
 * 
 * 40    1/15/02 16:13 Willem
 * Converting a domain from class to ID or vice versa now handles deleted
 * items properly
 * 
 * 39    12/14/01 16:25 Willem
 * - Added constructor to create a new bool domain
 * - Added a function to remove duplicate code (for creation of
 * Representation)
 * - Chnaged some code that still used char*
 * 
 * 38    8/23/01 17:08 Willem
 * GetObjectStructure does not need to add the representation for anything
 * else than COPY or DELETE (not for SetReadOnly for instance)
 * 
 * 37    6/15/01 10:32 Willem
 * Removed unnecessary code that was able to crash the Class to ID
 * conversion if the representation is missing
 * 
 * 36    14-03-01 4:04p Martin
 * the storetype of int is limited 32768. USHRT_MAX was not a good
 * solution because than fasle offset occur when values are betweem
 * SHRT_MAX and USHRT_MAX. Values in this range are always stored as long
 * and need no offset
 * 
 * 35    22/01/01 11:46 Willem
 * The number of classes is one less than the number of colors in the
 * picture map, because value zero will become UNDEF. Previously ILWIS
 * created one class item too much.
 * 
 * 34    10-01-01 18:21 Hendrikse
 * added 	case dmtUNIQUEID: in constructor
 * 
 * 33    29/11/00 15:57 Willem
 * Added messages in the tranquilizer for ClassToIdent and IdentToClass
 * functions, when they are loading the domain tables.
 * 
 * 32    22/11/00 14:27 Willem
 * The storetype is now also properly calculated for 2 byte storage
 * 
 * 31    13-11-00 10:05a Martin
 * rpr is now mandatory when getting the object structure
 * 
 * 30    3/11/00 17:12 Willem
 * - Removed old commented code
 * - Name/ID, code and decsription columns are now all copied in domain
 * conversion (ID <-->class). Also the object description, the sort order
 * as well as the attribute table are copied
 * - Adjusted the check on the domain reference count to larger than 1
 * (again). This is possible because domain sort now does not have the
 * attribute table as a member.
 * 
 * 29    23-10-00 4:10p Martin
 * added getobjecsstructure function
 * 
 * 28    11/10/00 12:53 Willem
 * SetRepresentation is now protected against an invalid rpr
 * 
 * 27    25-09-00 10:20a Martin
 * change the order of returns in the dmt function, dmUNIQUEID was always
 * recognized as a dmID
 * 
 * 26    9/18/00 3:42p Martin
 * prevent setrepresentation form changing the odf when it is not needed
 * 
 * 25    4-09-00 3:15p Martin
 * add tranquilizer for clastoid/ idtoclass
 * 
 * 24    1-09-00 2:06p Martin
 * check to prevent classtoid/idtoclass when other objects are open
 * 
 * 23    3-08-00 13:11 Koolhoven
 * protect constructor Domain(sExpr) against empty sExpr
 * 
 * 22    17-07-00 8:06a Martin
 * changed sCurDir to sGetCurDir from IlwsWinApp
 * 
 * 21    5-07-00 4:35p Martin
 * retrieve rpr name from registry without extension
 * 
 * 20    27-06-00 12:40p Martin
 * added reading from the catalog for systemdomains rpr
 * 
 * 19    21/06/00 18:11 Willem
 * The three dmt() function now all handle the same domain types
 * 
 * 18    19/05/00 17:09 Willem
 * Removed duplicate copy of sDomainType
 * 
 * 17    19/05/00 17:05 Willem
 * Renamed DomainTypeAsString to sDomainType
 * 
 * 16    19/05/00 14:00 Willem
 * Added static member function Domain::DomainTypeAsString() returning the
 * string representation of the DomainType.
 * 
 * 15    6-03-00 12:28 Wind
 * don't call store if object has no name
 * 
 * 14    21-02-00 4:35p Martin
 * Added a function to quickly add a whole set of values (AddValues)
 * 
 * 13    10/02/00 16:20 Willem
 * Updated rpr() and rprLoad() member functions. The fShowOnError
 * parameter is now only valid for rprLoad().
 * 
 * 12    10/02/00 15:20 Willem
 * Improved the check on delete representation in Domain destructor
 * 
 * 11    10/02/00 12:14 Willem
 * Added default parameter fShowOnError to rpr() and rprLoad() member
 * functions. When the flag is false no error message will be shown. This
 * behaviour is only used in the destructor of DomainPtr to be able a.o.
 * to delete a Domain and Representation pair without unnecessary
 * messages.
 * 
 * 10    22-12-99 18:13 Koolhoven
 * Derived ElementContainer from FileName
 * it has ElementMap* as a member.
 * In ObjectInfo::ReadElement and ObjectInfo::WriteElement the ElementMap
 * is used when available instead of the file.
 * ElementMap is initialized in MapCompositionDoc::Serialize to load/store
 * a MapView.
 * Further has IlwisObjectPtr now a SetFileName() function to prevent
 * everythere the const_casts of fnObj.
 * 
 * 9     21-12-99 12:58p Martin
 * added a domain and column Coordbuf based on domain binary to be able to
 * read and store dynamically coordbufs in a table
 * 
 * 8     3-11-99 12:31 Wind
 * case insensitive comparison
 * 
 * 7     29-10-99 13:00 Wind
 * case sensitive stuff
 * thread safe stuff
 * 
 * 6     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 5     10/12/99 10:06a Wind
 * debugged for internal domains and representation
 * 
 * 4     9/08/99 1:02p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 3     9/08/99 10:10a Wind
 * adapted to use of quoted file names
 * 
 * 2     3/11/99 12:15p Martin
 * Added support for Case insesitive 
// Revision 1.9  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.8  1997/09/18 20:13:19  Willem
// DomainPicture was created with internal representation. Not any more.
//
// Revision 1.7  1997/09/12 18:37:35  Wim
// Only create an internal representation when the extension is !=
// ".dom"
//
// Revision 1.6  1997-09-08 18:11:29+02  Wim
// Only delete internal representation when deleting the domain,
// not all of them
//
// Revision 1.5  1997-09-08 18:09:34+02  Wim
// Correctly delete internal representation when domain is deleted
//
// Revision 1.4  1997-08-21 10:33:36+02  Wim
// NotThisObjectTypeError
//
// Revision 1.3  1997-08-20 09:31:42+02  martin
// In the domainToID and IDToDomain codes are correctly added to a class
//
// Revision 1.2  1997/08/14 18:50:32  Wim
// Added sName() to report always the extension when different from
// the normal one.
//
/* Domain, DomainPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/

#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\domaintime.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Domain\dmclass.h"
#include "Engine\Domain\dmident.h"
#include "Engine\Domain\dmstring.h"
#include "Engine\Domain\dmcolor.h"
#include "Engine\Domain\dmpict.h"
#include "Engine\Domain\dmnone.h"
#include "Engine\Domain\dmgroup.h"
#include "Engine\Domain\dmbinary.h"
#include "Engine\Domain\dmcoord.h"
#include "Engine\Domain\dmcoordbuf.h"
#include "Engine\Domain\DomainUniqueID.h"
#include "Headers\Hs\DOMAIN.hs"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\mutex.h"
#include <map>


// ---- IlwisObjectPtrList ---
IlwisObjectPtrList Domain::listDom;


Domain::Domain()
	: IlwisObject(listDom)
{
}

Domain::Domain(const Domain& dm)
	: IlwisObject(listDom, dm.pointer())
{
}

Domain::Domain(const FileName& fn, double rMin, double rMax, double rStep)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	SetPointer(new DomainValueReal(fn, rMin, rMax, rStep));
}

Domain::Domain(const FileName& fn, long iMin, long iMax)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	SetPointer(new DomainValueInt(fn, iMin, iMax));
}

// DomainBool
Domain::Domain(const FileName& fn, const String& sFalse, const String& sTrue)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	FileName fnDom(fn, ".dom", false);
	SetPointer(new DomainBool(fnDom, sFalse, sTrue));
}

Domain::Domain(const FileName& fn, long iNr, DomainType dmt)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	FileName fnDom(fn, ".dom", false);
	switch (dmt)
	{
		case dmtCLASS:
			SetPointer(new DomainClass(fnDom, iNr));
			InitRepresentation(fnDom);
			break;
		case dmtGROUP:
			SetPointer(new DomainGroup(fnDom, iNr));
			InitRepresentation(fnDom);
			break;
		case dmtPICTURE:
			SetPointer(new DomainPicture(fnDom, iNr));
			InitRepresentation(fnDom);
			break;
		case dmtID:
			SetPointer(new DomainIdentifier(FileName(fn.sFullNameQuoted(true), ".dom", false), iNr));
			break;
		case dmtUNIQUEID:
			SetPointer(new DomainUniqueID(fn, "", iNr));
			break;
		default:
			assert(0 == 1); // no other types allowed
	}
	if (ptr()->sDescription.length() == 0)
		ptr()->sDescription = ptr()->sTypeName();
}

void Domain::InitRepresentation(const FileName& fnDom)
{
	ptr()->Store();
	FileName fnRpr = fnDom;
	if (fCIStrEqual(fnRpr.sExt, ".dom"))
		fnRpr.sExt = ".rpr";
	Representation rpr(fnRpr, *this);
	ptr()->SetRepresentation(rpr);
}

Domain::Domain(const FileName& fn, const CoordSystem& csy, bool threeD)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	SetPointer(new DomainCoord(fn, csy,threeD));
	ptr()->Store();
}

Domain::Domain(const FileName& fn, const ILWIS::TimeInterval& intervm ,ILWIS::Time::Mode m)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	SetPointer(new DomainTime(fn,intervm,m));
	ptr()->Store();
}

Domain::Domain(const FileName& fn, long iNr, DomainType dmt, const String& sPrefix)
	: IlwisObject(listDom)
{
	MutexFileName mut(fn);
	FileName fnDom(fn, ".dom", false);
	switch (dmt)
	{
		case dmtCLASS:
			SetPointer(new DomainClass(fnDom, iNr, sPrefix));
			InitRepresentation(fnDom);
			break;
		case dmtID:
			SetPointer(new DomainIdentifier(FileName(fn.sFullNameQuoted(true), ".dom", false), iNr, sPrefix));
			break;
		case dmtUNIQUEID:
			SetPointer(new DomainUniqueID(fn, sPrefix, iNr));
			break;
		default:
			assert(0 == 1); // no other types allowed
	}
	if (ptr()->sDescription.length() == 0)
		ptr()->sDescription = ptr()->sTypeName();
}

Domain::Domain(const FileName& fn)
	: IlwisObject(listDom)
{
	DomainPtr* p = Domain::pGet(fn);
	if (p)
	{
		SetPointer(p);
		return;
	}
	MutexFileName mut(fn);
	p = Domain::pGet(fn);
	if (p)
	{
		SetPointer(p);
		return;
	}
	SetPointer(DomainPtr::create(fn));
}

Domain::Domain(const String& sExpr)
	: IlwisObject(listDom)
{
	if (sExpr.length() == 0)
		return;

	Array<String> arParts ;
	Split(sExpr, arParts, "(,");
	if ( arParts[0] == "DomainUniqueID" )
	{
		DomainPtr* p = Domain::pGet(arParts[1]);
		if ( p )
			SetPointer(p);
		else
		{
			Domain dm(arParts[1], arParts[2].iVal(), dmtUNIQUEID, arParts[3]);
			SetPointer(dm.ptr());
		}
	}
	else
	{
		FileName fn(sExpr, ".dom", false); 
		Domain dm(fn);
		SetPointer(dm.ptr());
	}
}

Domain::Domain(const char * sExpr)
	: IlwisObject(listDom)
{
	FileName fn(String(sExpr), ".dom", false); 
	Domain dm(fn);
	SetPointer(dm.ptr());
}

Domain::Domain(const String& sExpression, const String& sPath)
	: IlwisObject(listDom)
{ 
	if (sExpression.find('(') != String::npos) // check for inline domain
		SetPointer(DomainPtr::create(sExpression));
	else
	{
		FileName fn(sExpression, ".dom", false);
		if (sExpression.find(':') == String::npos)  // no path set
			fn.Dir(sPath); 
		Domain dm(fn);
		SetPointer(dm.ptr());
	}  
	if (ptr()->sDescription.length() == 0)
		ptr()->sDescription = ptr()->sTypeName();
}

DomainPtr* Domain::pGet(const FileName& fn)
{
	return static_cast<DomainPtr*>(listDom.pGet(fn));
}

DomainType Domain::dmt(const FileName& fn)
{
	String sDomType;
	ObjectInfo::ReadElement("Domain", "Type", fn, sDomType);
	if (fCIStrEqual(sDomType , "DomainValue"))
		return dmtVALUE;
	else if (fCIStrEqual(sDomType , "DomainImage"))
		return dmtIMAGE;
	else if (fCIStrEqual(sDomType , "DomainBool"))
		return dmtBOOL;
	else if (fCIStrEqual(sDomType , "DomainBit"))
		return dmtBIT;
	else if (fCIStrEqual(sDomType , "DomainClass"))
		return dmtCLASS;
	else if (fCIStrEqual(sDomType , "DomainGroup"))
		return dmtGROUP;
	else if (fCIStrEqual(sDomType , "DomainIdentifier"))
		return dmtID;
	else if (fCIStrEqual(sDomType , "DomainPicture"))
		return dmtPICTURE;
	else if (fCIStrEqual(sDomType , "DomainCoord"))
		return dmtCOORD;
	else if ( fCIStrEqual(sDomType , "DomainCoordBuf"))
		return dmtCOORDBUF;
	else if ( fCIStrEqual(sDomType , "DomainUniqueID"))
		return dmtUNIQUEID;
	else if ( fCIStrEqual(sDomType , "DomainString"))
		return dmtSTRING;
	else if ( fCIStrEqual(sDomType , "DomainColor"))
		return dmtCOLOR;
	else if ( fCIStrEqual(sDomType , "DomainBinary"))
		return dmtBINARY;
	else if ( fCIStrEqual(sDomType , "DomainTime"))
		return dmtTIME;
	return dmtNONE;
}

DomainType DomainPtr::dmt() const 
{
	if (0 != pdi())
		return dmtIMAGE;
	if (0 != pdbit())
		return dmtBIT;
	if (0 != pdbool())
		return dmtBOOL;
	if ( 0 != pdtime())
		return dmtTIME;
	if (0 != pdv())
		return dmtVALUE;
	if (0 != pdgrp())
		return dmtGROUP;
	if ( 0 != pdUniqueID() )
		return dmtUNIQUEID;	
	if (0 != pdid())
		return dmtID;
	if (0 != pdc())
		return dmtCLASS;
	if (0 != pdp())
		return dmtPICTURE;
	if (0 != pds())
		return dmtSTRING;
	if (0 != pdcrd())
		return dmtCOORD;
	if (0 != pdbin())
		return dmtBINARY;
	if ( 0 != pdmcb())
		return dmtCOORDBUF;
	if (0 != pdcol())
		return dmtCOLOR;
	return dmtNONE;
}

DomainPtr::DomainPtr(const FileName& fn, bool fCreate)
	: IlwisObjectPtr(fn, fCreate, ".dom")
{
	if (fCreate)
		_iWidth = 10;
	else
	{
		if (!fCIStrEqual(fnObj.sExt,".dom"))
			ReadBaseInfo("Domain");  
		_iWidth = iReadElement("Domain", "Width");
	}  
	_fRawAvail = true;
}

DomainPtr::~DomainPtr()
{
	if (fErase)
	{
		try
		{
			FileName fnRpr;
			ReadElement("Domain", "Representation", fnRpr);
			if (fnRpr == fnObj)
			{
				Representation rp(fnRpr);
				rp->fErase = true;
			}
		}
		catch (const ErrorObject&)
		{
		}
		if (!fCIStrEqual(fnObj.sExt,".dom"))
		{
			WriteElement("Domain", (const char*)0, (const char *)0);
			fErase = false;
		}
	}
}

String DomainPtr::sType() const
{
	return "Domain";
}

void DomainPtr::Store()
{
  if (fCIStrEqual(fnObj.sExt, ".dom")) {
    IlwisObjectPtr::Store();
    WriteElement("Ilwis", "Type", "Domain");
    WriteElement("Ilwis", "ContainsDom", (char*)0);
  }
  else {
    WriteBaseInfo("Domain");  
    WriteElement("Ilwis", "ContainsDom", "True");
  }
  WriteElement("Domain", "Width", (long)iWidth());
}

Representation DomainPtr::rpr() const
{
	DomainPtr *dp = const_cast<DomainPtr*>(this);
  return dp->rprLoad();
}

Representation DomainPtr::rprLoad(bool fShowOnError)
{
  Representation rpr;
  try 
	{
		IlwisSettings settings("DefaultSettings");
		String sName = String(fnObj.sFile).toLower();
		String sDefaultRpr = settings.sValue( sName, "");
		if ( sDefaultRpr != "")
		{
			if (File::fExist(sDefaultRpr))
			{
					rpr = Representation(sDefaultRpr);
					return rpr;
			}
		}
		ReadElement("Domain", "Representation", rpr);
  }
  catch (const ErrorObject& err) 
	{
		if (fShowOnError) err.Show();
  }
  return rpr;
}

void DomainPtr::SetRepresentation(const Representation& rpr)
{
	FileName fnFile;
	ReadElement("Domain",  "Representation", fnFile);
	if (!rpr.fValid() || (rpr.fValid() && fnFile != rpr->fnObj ))
		WriteElement("Domain", "Representation", rpr);
}

DomainPtr* DomainPtr::create(const FileName& fn)
{
  FileName filnam = fn;
  if (filnam.sExt.length() == 0)
    filnam.sExt = ".dom";
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
      //throw ErrorNotFound(fn);
  }
  DomainPtr* p = Domain::pGet(filnam);
  if (p)
    return p;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
      //throw ErrorNotFound(fn);
  }
  p = Domain::pGet(filnam);
  if (p)
    return p;
  if (fCIStrEqual(fn.sExt, ".csy")) {
    MutexFileName mut(fn);
    return new DomainCoord(FileName(), CoordSystem(fn));
  }
  String sType;
  if (ObjectInfo::ReadElement("Domain", "Type", filnam, sType)) {
    MutexFileName mut(filnam);
    if (fCIStrEqual("DomainValue" , sType))
      return DomainValue::create(filnam);
    else if (fCIStrEqual("DomainClass" , sType))
      return new DomainClass(filnam);
    else if (fCIStrEqual("DomainGroup" , sType))
      return new DomainGroup(filnam);
    else if (fCIStrEqual("DomainIdentifier" , sType))
      return new DomainIdentifier(filnam);
    else if (fCIStrEqual("DomainColor" , sType)) {
      DomainPtr* p = new DomainColor();			 
			p->SetFileName(filnam);
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
      return p;
    }
     else if (fCIStrEqual("DomainBinary" , sType)) {
      DomainPtr* p = new DomainBinary();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
    }
    else if (fCIStrEqual("DomainCoordBuf" , sType)) {
      DomainPtr* p = new DomainCoordBuf();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
		}
    else if (fCIStrEqual("DomainCoord" , sType))
      return new DomainCoord(filnam);
    else if (fCIStrEqual("DomainImage" , sType)) {
      DomainPtr* p = new DomainImage();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
    }
    else if (fCIStrEqual("DomainBit" , sType)) {
      DomainPtr* p = new DomainBit();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
    }
    else if (fCIStrEqual("DomainBool" , sType)) {
      DomainPtr* p = 0;
      try {
        p = new DomainBool(filnam);
      }
      catch (const ErrorObject&) {
        p = new DomainBool();
        ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
				p->SetFileName(filnam);
      }
      return p;
    }
    else if (fCIStrEqual("DomainNone" , sType)) {
      DomainPtr* p = new DomainNone();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
    }
    else if (fCIStrEqual("DomainPicture" , sType))
      return new DomainPicture(filnam);
	else if (fCIStrEqual("DomainTime" , sType))
      return new DomainTime(filnam);
    else if (fCIStrEqual("DomainString" , sType)) {
      DomainPtr* p = new DomainString();
      ObjectInfo::ReadElement("Ilwis", "Description", filnam, p->sDescription);
			p->SetFileName(filnam);
      return p;
    }
		else if (fCIStrEqual("DomainUniqueID" , sType))
		{
			DomainPtr* p = new DomainUniqueID(filnam);
			return p;
		}			
    InvalidTypeError(fn, "Domain", sType);
  }
  else
    NotThisObjectTypeError(fn, "Domain");
  return 0;
}

DomainPtr* DomainPtr::create(const String& sExpression)
{
  if (fCIStrEqual(sExpression.sRight(4), ".csy"))// coordinate system
    return new DomainCoord(FileName(), CoordSystem(sExpression));
  const char *p = sExpression.scVal();
  String s = p;
  char *pparm = s.strchrQuoted('(');
  if (pparm != 0) 
	{
    *pparm =0; pparm++;
    if ((_stricmp(p, "Value") == 0) || (_stricmp(p, "Real") == 0)) {
      double rMin, rMax, rStep = 1;
      int i = sscanf(pparm, "%lg,%lg,%lg", &rMin, &rMax, &rStep);
      if (i == 2)
        rStep = 1;
      return new DomainValueReal(FileName("value"), rMin, rMax, rStep);
    }
    else if (_stricmp(p, "Int") == 0) {
      long iMin, iMax;
      sscanf(pparm, "%li,%li", &iMin, &iMax);
      return new DomainValueReal(FileName("value"), iMin, iMax, 1.0);
    }
    else if (_stricmp(p, "Binary") == 0) {
      return new DomainBinary;
    }
		else
		{
			Array<String> arParts ;
			Split(sExpression, arParts, "(,)");
			if ( arParts[0] == "DomainUniqueID" )
			{
				DomainPtr* p = Domain::pGet(arParts[1]);
				if ( p )
					return p;
				else
				{
					return new DomainUniqueID(arParts[1], arParts[2], arParts[3].iVal());
				}
			}
		}
  }
  else {
//    String sExt = ".dom";
    FileName filnam(sExpression);
    if (filnam.sExt.length() == 0)
      filnam.sExt = ".dom";
    return DomainPtr::create(filnam);
  }
  NotFoundError(String("Domain: %S", sExpression));
  return 0;
}

StoreType DomainPtr::st(unsigned long iNr)
{
  if (iNr <= 2)
    return stBIT;
  if (iNr <= 4)
    return stDUET;
  if (iNr <= 16)
    return stNIBBLE;
  if (iNr <= 256)
    return stBYTE;
  if (iNr <= SHRT_MAX)
    return stINT;
  return stLONG;
}

DomainValue* DomainPtr::pdv() const
{
  return dynamic_cast<DomainValue*>
    (const_cast<DomainPtr*>(this));
}

DomainValueInt* DomainPtr::pdvi() const
{
  return dynamic_cast<DomainValueInt*>
    (const_cast<DomainPtr*>(this));
}

DomainValueReal* DomainPtr::pdvr() const
{
  return dynamic_cast<DomainValueReal*>
    (const_cast<DomainPtr*>(this));
}

DomainClass* DomainPtr::pdc() const
{
  return dynamic_cast<DomainClass*>
    (const_cast<DomainPtr*>(this));
}

DomainIdentifier* DomainPtr::pdid() const
{
  return dynamic_cast<DomainIdentifier*>
    (const_cast<DomainPtr*>(this));
}

DomainString* DomainPtr::pds() const
{
  return dynamic_cast<DomainString*>
    (const_cast<DomainPtr*>(this));
}

DomainImage* DomainPtr::pdi() const
{
  return dynamic_cast<DomainImage*>
    (const_cast<DomainPtr*>(this));
}

DomainBit* DomainPtr::pdbit() const
{
  return dynamic_cast<DomainBit*>
    (const_cast<DomainPtr*>(this));
}

DomainPicture* DomainPtr::pdp() const
{
  return dynamic_cast<DomainPicture*>
    (const_cast<DomainPtr*>(this));
}

DomainSort* DomainPtr::pdsrt() const
{
  return dynamic_cast<DomainSort*>
    (const_cast<DomainPtr*>(this));
}

DomainGroup* DomainPtr::pdgrp() const
{
  return dynamic_cast<DomainGroup*>
    (const_cast<DomainPtr*>(this));
}

DomainColor* DomainPtr::pdcol() const
{
  return dynamic_cast<DomainColor*>
    (const_cast<DomainPtr*>(this));
}

DomainTime* DomainPtr::pdtime() const
{
  return dynamic_cast<DomainTime*>
    (const_cast<DomainPtr*>(this));
}

DomainNone* DomainPtr::pdnone() const
{
  return dynamic_cast<DomainNone*>
    (const_cast<DomainPtr*>(this));
}

DomainBinary* DomainPtr::pdbin() const
{
  return dynamic_cast<DomainBinary*>
    (const_cast<DomainPtr*>(this));
}

DomainBool* DomainPtr::pdbool() const
{
  return dynamic_cast<DomainBool*>
    (const_cast<DomainPtr*>(this));
}

DomainCoord* DomainPtr::pdcrd() const
{
  return dynamic_cast<DomainCoord*>
    (const_cast<DomainPtr*>(this));
}

DomainCoordBuf* DomainPtr::pdmcb() const
{
  return dynamic_cast<DomainCoordBuf*>
    (const_cast<DomainPtr*>(this));
}

DomainUniqueID* DomainPtr::pdUniqueID() const
{
	return dynamic_cast<DomainUniqueID *>(const_cast<DomainPtr *>(this));
}

String DomainPtr::sValueByRaw(long, short, short) const
{
  return String();
}

long DomainPtr::iRaw(const String&) const
{
  return iUNDEF;
}

bool DomainPtr::fValid(const String&) const
{
  return false;
}

bool DomainPtr::fValidDomain() const
{
  if (pdbit()) // 'bit' domain has no file name
    return true;
  if (pdcol()) // 'color' domain has no file name
    return true;
	if ( pdUniqueID() )
		return true;
  return fnObj.sFile != "";
}

       
Domain Domain::dmFindSameOnDisk(const Domain& dm, const String& sSearchDir)
{
    WIN32_FIND_DATA findData;
    HANDLE handle;
    String sSearchFile;
    if (sSearchDir.length())
        sSearchFile = sSearchDir;
    else
        sSearchFile = getEngine()->sGetCurDir();
    sSearchFile &= "*.dom";
    if ((handle=FindFirstFile(sSearchFile.scVal(), &findData))!=INVALID_HANDLE_VALUE)
    {
        do 
        {
            String s(findData.cFileName);
            FileName fn(s);
            if (sSearchDir.length() != 0)
                fn.Dir(sSearchDir);
            Domain dmExist(fn);
            if (dmExist == dm)
            return dmExist;
        } while (FindNextFile(handle, &findData)!=FALSE);
        FindClose(handle);
    }
    return Domain();
}

void Domain::ClassToIdent()
{
	DomainClass* pdc = ptr()->pdc();
	if (pdc == 0)
		return;

	if ( pdc->iRef > 1) 
		throw ErrorObject(String(SDMErrObjectsStillOpen_S.scVal(), pdc->sName()));	

	DomainIdentifier* pdi;
	FileName fnAttr;
	if (pdc->fTblAtt())
		fnAttr = pdc->tblAtt()->fnObj;

	if (!pdc->tbl.fValid())
		pdi = new DomainIdentifier(pdc->fnObj, pdc->iSize(), pdc->sPrefix);
	else
	{
		pdi = new DomainIdentifier(pdc->fnObj, pdc->iSize());
		pdi->dsType = DomainSort::dsMANUAL;
		Tranquilizer trq(SDMTitleConvertingClassID);
		trq.SetDelayShow(false);
		trq.SetNoStopButton(true);
		trq.fText(SDMMsgLoadingDomain);
		bool fHasCode = pdc->colCode.fValid();
		bool fHasDescr = pdc->colDesc.fValid();

		pdc->Load(); // force the table load here to be able to set the tranquilizer
		trq.fText(SDMMsgCreateNewDomain);
		pdi->SetVal(1, "--_Dummy_--"); // force creation of new domain table

		trq.SetText(SDMConverting);
		trq.SetNoStopButton(false);
		for (long i=1; i <= pdc->iSize(); i++) 
		{
			if (trq.fUpdate(i, pdc->iSize()))
				return;

			pdi->SetVal(i, pdc->sNameByRaw(i, 0));
			if (fHasCode)
				pdi->SetCode(i, pdc->sCodeByRaw(i, 0));
			if (fHasDescr)
				pdi->SetDescription(i, pdc->sDescriptionByRaw(i));
			long iO = pdc->iOrd(i);
			if (iO != iUNDEF)
				pdi->SetOrd(i, iO);  // if iO is undef the domain gets messed up
			else
			{
				pdi->SetOrd(i, i);
				pdi->Delete(i);
			}
		}
		trq.fUpdate(pdc->iSize(), pdc->iSize());
		pdi->dsType = pdc->dsType;
	} 
	pdi->sDescription = pdc->sDescription;
	pdi->SetRepresentation(Representation()); // remove reference to rpr from ODF

	if (pdc->fTblAtt())
		pdc->SetAttributeTable(Table());     // reduce ref count to domain class
	pdc->SetFileName(FileName());    // detach original class domain from filename
	SetPointer(pdi);                 // and attach the new domainID to the name 
	if (fnAttr.fValid())
	{
		Table tbl(fnAttr);
		pdi->SetAttributeTable(tbl); // re-attach the attribute table
	}
	pdi->Store();
}

void Domain::IdentToClass()
{
	DomainIdentifier* pdi = ptr()->pdid();
	if (pdi == 0)
		return;
	
	if ( pdi->iRef > 1) 
		throw ErrorObject(String(SDMErrObjectsStillOpen_S.scVal(), pdi->sName()));

	FileName fnAttr;
	if (pdi->fTblAtt())
		fnAttr = pdi->tblAtt()->fnObj;

	DomainClass* pdc;
	if (!pdi->tbl.fValid())
		pdc = new DomainClass(pdi->fnObj, pdi->iSize(), pdi->sPrefix);
	else
	{
		pdc = new DomainClass(pdi->fnObj, pdi->iSize());
		pdc->dsType = DomainSort::dsMANUAL;
		pdc->sDescription = pdi->sDescription;
		Tranquilizer trq(SDMTitleConvertingIDClass);
		trq.SetDelayShow(false);
		trq.SetNoStopButton(true);
		trq.fText(SDMMsgLoadingDomain);
		bool fHasCode = pdi->colCode.fValid();
		bool fHasDescr = pdi->colDesc.fValid();

		pdi->Load();             // force the table load here to be able to set the tranquilizer
		trq.fText(SDMMsgCreateNewDomain);
		pdc->SetVal(1, "--_Dummy_--"); // force creation of new domain table

		trq.SetText(SDMConverting);
		trq.SetNoStopButton(false);
		for (long i=1; i <= pdi->iSize(); i++) 
		{
			if (trq.fUpdate(i, pdi->iSize()))
				return;

			pdc->SetVal(i, pdi->sNameByRaw(i,0));
			if (fHasCode)
				pdc->SetCode(i, pdi->sCodeByRaw(i,0));
			if (fHasDescr)
				pdc->SetDescription(i, pdi->sDescriptionByRaw(i));
			long iO = pdi->iOrd(i);
			if (iO != iUNDEF)
				pdc->SetOrd(i, iO);  // if iO is undef the domain gets messed up
			else
			{
				pdc->SetOrd(i, i);
				pdc->Delete(i);
			}
		}
		trq.fUpdate(pdi->iSize(), pdi->iSize());
		pdc->dsType = pdi->dsType;
	}  
	pdi->SetFileName(FileName());
	SetPointer(pdc);
	if (fnAttr.fValid())
	{
		Table tbl(fnAttr);
		pdc->SetAttributeTable(tbl);
	}
	pdc->Store();
	Representation rpr(FileName(pdc->fnObj, ".rpr", fCIStrEqual(pdc->fnObj.sExt, ".dom")), *this);
	pdc->SetRepresentation(rpr);
}

void Domain::PictureToClass()
{
	DomainPicture* pdp = ptr()->pdp();
	if (pdp == 0)
		return;

	if ( pdp->iRef > 1 )
		throw ErrorObject(String(SDMErrObjectsStillOpen_S.scVal(), pdp->sName()));
	
	Representation rprPict = pdp->rpr();
	DomainClass* pdc;
	pdc = new DomainClass(pdp->fnObj, max(0, pdp->iColors() - 1), "Class");
	pdc->sDescription = pdp->sDescription;
	pdp->SetFileName(FileName());
	SetPointer(pdc);
	pdc->Store();
	Representation rpr(FileName(pdc->fnObj, ".rpr", fCIStrEqual(pdc->fnObj.sExt, ".dom")), *this);
	pdc->SetRepresentation(rpr);
	RepresentationClass* rprc = rpr->prc();
	for (int i=1; i <= pdc->iSize(); ++i)
		rprc->PutColor(i, rprPict->clrRaw(i));
	pdc->Store();
	rpr->Store();
}

DomainType Domain::dmt(const String& sDomType)
{
	if (fCIStrEqual(sDomType , "class"))
		return dmtCLASS;
	if (fCIStrEqual(sDomType , "id"))
		return dmtID;
	if (fCIStrEqual(sDomType , "value"))
		return dmtVALUE;
	if (fCIStrEqual(sDomType , "image"))
		return dmtIMAGE;
	if (fCIStrEqual(sDomType , "picture"))
		return dmtPICTURE;
	if (fCIStrEqual(sDomType , "bit"))
		return dmtBIT;
	if (fCIStrEqual(sDomType , "bool"))
		return dmtBOOL;
	if (fCIStrEqual(sDomType , "string"))
		return dmtSTRING;
	if (fCIStrEqual(sDomType , "group"))
		return dmtGROUP;
	if (fCIStrEqual(sDomType , "coord"))
		return dmtCOORD;
	if (fCIStrEqual(sDomType , "coordbuf"))
		return dmtCOORDBUF;
	if (fCIStrEqual(sDomType , "UniqueID"))
		return dmtUNIQUEID;
	if ( fCIStrEqual(sDomType , "color"))
		return dmtCOLOR;
	if ( fCIStrEqual(sDomType , "binary"))
		return dmtBINARY;
	if ( fCIStrEqual(sDomType , "time"))
		return dmtTIME;
	return dmtNONE;
}

String Domain::sDomainType(DomainType dmt)
{
  switch (dmt) {
    case dmtCLASS: 
      return "class";
    case dmtID:
      return "id";
    case dmtGROUP : 
      return "group";
    case dmtPICTURE : 
      return "picture";
    case dmtIMAGE : 
      return "image";
    case dmtSTRING : 
      return "string";
    case dmtBIT : 
      return "bit";
    case dmtBOOL: 
      return "bool";
    case dmtNONE : 
      return "none";
    case dmtCOLOR: 
      return "color";
    case dmtCOORD: 
      return "coord";
    case dmtCOORDBUF: 
      return "coordbuf";
    case dmtVALUE : 
      return "value";
	case dmtTIME : 
      return "time";
    case dmtBINARY : 
      return "binary";
		case dmtUNIQUEID:
			return "UniqueID";
    default : /* nothing */ ;
  }
  return sUNDEF;
}

String Domain::sStoreType(StoreType st)
{
  switch (st) {
    case stBIT: 
      return "Bit";
    case stDUET:
    case stNIBBLE:
    case stBYTE:
      return "Byte";
    case stINT: 
      return "Int";
    case stLONG:
      return "Long";
    case stREAL: 
      return "Real";
    case stCRD: 
      return "Coord";
	case stCRD3D: 
      return "Coord3D";
	case stCOORDBUF3D: 
      return "Coord3D";
    case stSTRING:
      return "String";
    case stBINARY:
      return "Binary";
    default : /* nothing */ ;
  }
  return sUNDEF;
}

StoreType Domain::st(const String& sStoreType)
{
  if (sStoreType == "bit")
    return stBIT;
  if (sStoreType == "byte")
    return stBYTE;
  if (sStoreType == "int")
    return stINT;
  if (sStoreType == "long")
    return stLONG;
  if (sStoreType == "real")
    return stREAL;
  if (sStoreType == "coord")
    return stCRD;
  if (sStoreType == "coord3D")
    return stCRD3D;
  if (sStoreType == "coordbuf3D")
    return stCOORDBUF3D;
  if (sStoreType == "coordbuf")
    return stCOORDBUF;
  if (sStoreType == "string")
    return stSTRING;
  if (sStoreType == "binary")
    return stBINARY;
  if ( sStoreType == "?") 
    return stUNKNOWN;
  return stBYTE; // what else ???
}

long DomainPtr::iMaxStringWidth() const
{
  return _iWidth;
}

void DomainPtr::Rename(const FileName& fnNew)
{
}

String DomainPtr::sObjectSection() const
{
  if (!fCIStrEqual(fnObj.sExt,".dom"))
    return "Domain";
  return IlwisObjectPtr::sObjectSection();
}

String DomainPtr::sName(bool fExt, const String& sDirRelative) const
{
  if (!fCIStrEqual(fnObj.sExt,".dom"))
    fExt = true;
  return IlwisObjectPtr::sName(fExt, sDirRelative);
}

String DomainPtr::sNameQuoted(bool fExt, const String& sDirRelative) const
{
  String s;
  if (fnObj.fValid() && !fCIStrEqual(fnObj.sExt,".dom")) 
    s = IlwisObjectPtr::sName(false, sDirRelative);
  else
    s = sName(false, sDirRelative);
  if (fnObj.fValid()) {
    s = s.sQuote(false);
    if (fExt || !fCIStrEqual(fnObj.sExt, ".dom"))
      s &= fnObj.sExt;
  }  
  return s;
}

void DomainPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure( os );

	// Add representation to the list of object only for Copy or Delete
	// but not for setting ReadOnly flag!
	if (os.caGetCommandAction() == ObjectStructure::caCOPY ||
		os.caGetCommandAction() == ObjectStructure::caDELETE)
		os.AddFile(fnObj, "Domain", "Representation", ".rpr" );
}






