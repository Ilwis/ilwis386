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

#include "Engine\Domain\dm.h"
#include "Engine\Domain\dminfo.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Representation\RPRATTR.H"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Representation\Rprgrad.h"
#include "Engine\Representation\RPRVALUE.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Drawers\Drawer_n.h"

IlwisObjectPtrList Representation::listRpr;

RepresentationProperties::RepresentationProperties() {
	linewidth = rUNDEF;
	lineType = ILWIS::NewDrawer::ldtSingle;
	itemTransparency = 1.0;
	symbolType = DEFAULT_POINT_SYMBOL_TYPE;
	symbolSize = 100.0;

}

RepresentationProperties::RepresentationProperties(const RepresentationProperties& p){
	linewidth = p.linewidth;
	lineType = p.lineType;
	itemTransparency = p.itemTransparency;
	symbolType = p.symbolType;
	symbolSize = p.symbolSize;
}

map< int, vector<Color> > Representation::colorSets;

Representation::Representation()
: IlwisObject(listRpr)
{}

Representation::Representation(const Representation& rpr)
: IlwisObject(listRpr, rpr.pointer())
{}

Representation::Representation(const String& sExpression)
: IlwisObject(listRpr, RepresentationPtr::create(sExpression))
{
  if (ptr() && ptr()->sDescription == "")
    ptr()->sDescription = ptr()->sTypeName();
}

Representation::Representation(const FileName& fn)
: IlwisObject(listRpr, FileName(fn.sFullNameQuoted(true), ".rpr", false))
{ if (!pointer())
    SetPointer(RepresentationPtr::create(FileName(fn.sFullNameQuoted(true), ".rpr", false)));
}

Representation::Representation(const FileName& fn, const Domain& dm, bool fCreateGradual)
: IlwisObject(listRpr, RepresentationPtr::create(FileName(fn.sFullNameQuoted(true), ".rpr", false), dm, fCreateGradual))
{
  if (ptr()->sDescription == "")
    ptr()->sDescription = ptr()->sTypeName();
}

/*
Representation::Representation(const Representation& rpr, const Table& tblAttr)
: IlwisObject(listRpr, new RepresentationAttribute(rpr, tblAttr))
{
}
*/
RepresentationPtr* Representation::pGet(const FileName& fn)
{
  return static_cast<RepresentationPtr*>(listRpr.pGet(fn));
}

Domain Representation::dmGet(const FileName& fn)
{
  String sDom;
  ObjectInfo::ReadElement("Representation", "Domain", fn, sDom);
  Domain dm;
//  try {
    dm = Domain(sDom);
/*  }
  catch (const ErrorObject& err) {
    err.Show();
  }*/
  return dm;
}

bool Representation::fHasDomainValue(const FileName& fn)
{
  Domain dm = dmGet(fn);
  if (dm.fValid())
    return dm->pdv() != 0;
  return false;
}

bool Representation::fHasSameDomain(const FileName& fn, const Domain& dmCompare)
{
  Domain dm = dmGet(fn);
  if (dm.fValid())
    return dm == dmCompare;
  return false;
}

RepresentationPtr::RepresentationPtr(const FileName& fn)
: IlwisObjectPtr(fn)
{
  if (fnObj.sExt != ".rpr")
    ReadBaseInfo("Representation");
  ReadElement("Representation", "Domain", _dm);
  ReadElement("Representation", "BoundaryColor", clrBoundaryColor);
  iBoundaryWidth = iReadElement("Representation", "BoundaryWidth");
  if (iBoundaryWidth < 1)
    iBoundaryWidth = 1;
  else if (iBoundaryWidth > 100)
    iBoundaryWidth = 100;
}

RepresentationPtr::RepresentationPtr(const FileName& fn, const Domain& dom)
: IlwisObjectPtr(fn, true, ".rpr")
{
  _dm = dom;
  clrBoundaryColor = Color(0,0,0);
  iBoundaryWidth = 1;
}

void RepresentationPtr::Store()
{
 if (fCIStrEqual(fnObj.sExt,".rpr")) {
    IlwisObjectPtr::Store();
    WriteElement("Ilwis", "Type", "Representation");
    WriteElement("Ilwis", "ContainsRpr", (char*)0);
  }  
  else {
    WriteBaseInfo("Representation");  
    WriteElement("Ilwis", "ContainsRpr", "True");
  }
  WriteElement("Representation", "Domain", dm());
  WriteElement("Representation", "BoundaryColor", clrBoundaryColor);
  WriteElement("Representation", "BoundaryWidth", iBoundaryWidth);
}

RepresentationPtr::~RepresentationPtr()
{
  if (fErase && (fnObj.sExt != ".rpr")) {
    WriteElement("Representation", (const char*)0, (const char *)0);
    fErase = false;
  } 
}

String RepresentationPtr::sType() const
{
  return "Representation";
}

RepresentationPtr* RepresentationPtr::create(const FileName& fn)
{
  if (fn.sFile == "")
    return 0;
  MutexFileName mut(fn);
  RepresentationPtr* p = Representation::pGet(fn);
  if (p)
    return p;
  FileName filnam = fn;
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
      //throw ErrorNotFound(fn);
  }
  String sType;
  if (ObjectInfo::ReadElement("Representation", "Type", filnam, sType)) {
    if (fCIStrEqual("RepresentationClass" , sType))
      return new RepresentationClass(filnam);
//    if ("RepresentationAttribute" == sType)
//      return new RepresentationAttribute(filnam);
    if (fCIStrEqual("RepresentationGradual" , sType))
      return new RepresentationGradual(filnam);
    if (fCIStrEqual("RepresentationValue" , sType))
      return new RepresentationValue(filnam);
    InvalidTypeError(fn, "Representation", sType);
  }
  else
    NotThisObjectTypeError(fn, "Representation");
  return 0;
}

RepresentationPtr* RepresentationPtr::create(const FileName& fn, const Domain& dm, bool fCreateGradual)
{
  if (dm->pdc() || dm->pdp())  // Class or Pictures
    return new RepresentationClass(fn,dm);
  else if (dm->pdv())          // Value
    if (dm->sName() == "value" && fCreateGradual)
      return new RepresentationGradual(fn,dm);
    else
      return new RepresentationValue(fn,dm);
  else
    return 0;    
}

RepresentationPtr* RepresentationPtr::create(const String& sExpression)
{
  // checken op haakes enzo
  FileName fn(sExpression, ".rpr", false);
  MutexFileName mut(fn);
  RepresentationPtr* p = Representation::pGet(fn);
  if (p)
    return p;
  return create(fn);
}

/*
RepresentationItems* RepresentationPtr::pri() const
{
  return dynamic_cast<RepresentationItems*>
    (const_cast<RepresentationPtr*>(this));
}
*/
/*
RepresentationAttribute* RepresentationPtr::pra() const
{
  return dynamic_cast<RepresentationAttribute*>
    (const_cast<RepresentationPtr*>(this));
}
*/
RepresentationClass* RepresentationPtr::prc() const
{
  return dynamic_cast<RepresentationClass*>
    (const_cast<RepresentationPtr*>(this));
}

RepresentationGradual* RepresentationPtr::prg() const
{
  return dynamic_cast<RepresentationGradual*>
    (const_cast<RepresentationPtr*>(this));
}

RepresentationValue* RepresentationPtr::prv() const
{
  return dynamic_cast<RepresentationValue*>
    (const_cast<RepresentationPtr*>(this));
}

void RepresentationPtr::GetColors(ColorBuf& buf) const
{
  buf.Size(0);
}

void RepresentationPtr::GetColorLut(ColorBuf& buf) const
{
  buf.Size(0);
}

Color RepresentationPtr::clrRaw(long) const
{
  return Color();
}

Color RepresentationPtr::clrSecondRaw(long) const
{
  return Color();
}

/*
short RepresentationPtr::iWidth(long) const
{
  return 1;
}
*/

short RepresentationPtr::iPattern(long) const
{
  return 0;
}

void RepresentationPtr::GetPattern(long, short aPat[8]) const
{
  for (int i = 0; i < 8; ++i)
    aPat[i] = 0;
}

short RepresentationPtr::iSymbolType(long) const
{
  return 0;
}

short RepresentationPtr::iSymbolSize(long) const
{
  return 1;
}

short RepresentationPtr::iSymbolWidth(long) const
{
  return 5;
}

Color RepresentationPtr::clrSymbol(long) const
{
  return Color(-1);
}

Color RepresentationPtr::clrSymbolFill(long) const
{
  return Color(-2);
}

String RepresentationPtr::sSymbolFont(long) const
{
  return sUNDEF;
}

double RepresentationPtr::rSymbolRotation(long) const
{
  return 0;
}

byte RepresentationPtr::iColor(double rValue) const  // 0..1 
{
  return 0;
}

byte RepresentationPtr::iColor(long iValue, RangeInt ri) const
{
  if (iValue == iUNDEF)
    return 0;
  double rValue = iValue;  
  rValue -= ri.iLo();
  rValue /= ri.iWidth();
  return iColor(rValue);
}

byte RepresentationPtr::iColor(double rValue, RangeReal rr) const
{
  if (rValue == rUNDEF)
    return 0;
  rValue -= rr.rLo();                                  
  rValue /= rr.rWidth();
  return iColor(rValue);
}

Color RepresentationPtr::clr(double rValue) const            // 0..1
{
  return clrRaw(iColor(rValue));  
}

Color RepresentationPtr::clr(double rValue, RangeReal rr) const // stretch
{
  return clrRaw(iColor(rValue, rr));  
}

short RepresentationPtr::iLine(long) const
{
  return 1; // ldtSingle
}

Color RepresentationPtr::clrLineFill(long) const
{
  return Color();
}

double RepresentationPtr::rLineWidth(long) const
{
  return 0;
}

double RepresentationPtr::rLineDist(long) const
{
  return 0;
}

short* RepresentationPtr::pPattern(const String& sPattern)
{
  static short pBuf[8];
  char sBuf[100];
  GetPrivateProfileString("Patterns", sPattern.c_str(), "", sBuf, 100, "control.ini");
  int iRet = sscanf(sBuf, "%i %i %i %i %i %i %i %i", 
                    &pBuf[0], &pBuf[1], &pBuf[2], &pBuf[3], 
                    &pBuf[4], &pBuf[5], &pBuf[6], &pBuf[7]);
  if (iRet != 8)
    return 0;
  return pBuf;  
}

void RepresentationPtr::GetPatternNames(Array<String>& sa)
{
  sa.Reset();
  char* sBuf = new char[16000];
  GetPrivateProfileString("Patterns", NULL, "", sBuf, 16000, "control.ini");
  for (char* s = sBuf; *s; ++s) {
    String sPat = s;
    if (0 != pPattern(sPat))
      sa &= sPat;
    while (*s) ++s;
  }
  delete sBuf;
}

String RepresentationPtr::sPattern(const byte aPat[8])
{
  short pPat[8];
  for (int i = 0; i < 8; ++i)
    pPat[i] = aPat[i];
  String sRet;
  char* sBuf = new char[16000];
  GetPrivateProfileString("Patterns", NULL, "", sBuf, 16000, "control.ini");
  for (char* s = sBuf; *s; ++s) {
    String sPat = s;
    const short* ptr = pPattern(sPat);
    if (0 != ptr)
      if (0 == memcmp(ptr, pPat, 16)) {
        sRet = sPat;
        break;
      }
    while (*s) ++s;
  }
  delete sBuf;
  return sRet;
}

DomainInfo RepresentationPtr::dminf() const
{
  return DomainInfo(fnObj, "Representation");
}

void RepresentationPtr::Rename(const FileName& fnNew)
{
}

String RepresentationPtr::sObjectSection() const
{
  if (fnObj.sExt != ".rpr")
    return "Representation";
  return IlwisObjectPtr::sObjectSection();
}

String RepresentationPtr::sName(bool fExt, const String& sDirRelative) const
{
  if (fnObj.sExt != ".rpr")
    fExt = true;
  return IlwisObjectPtr::sName(fExt, sDirRelative);
}

String RepresentationPtr::sNameQuoted(bool fExt, const String& sDirRelative) const
{
  String s;
  if (fnObj.fValid() && fnObj.sExt != ".rpr") 
    s = IlwisObjectPtr::sName(false, sDirRelative);
  else
    s = sName(false, sDirRelative);
  if (fnObj.fValid()) {
    s = s.sQuote(false);
    if (fExt || fnObj.sExt != ".rpr")
      s &= fnObj.sExt;
  }  
  return s;
}

void RepresentationPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure( os );
	os.AddFile(fnObj, "TableStore", "Data");
	if ( os.fGetAssociatedFiles() )
		os.AddFile(fnObj, "Representation", "Domain", ".dom");
}

void RepresentationPtr::getProperties(long iRaw, RepresentationProperties *props) {
}

void Representation::loadColorSets(const String& folder) {
	String pathToColorSet;
	if ( folder == "") {
		String ilwDir = getEngine()->getContext()->sIlwDir();
		pathToColorSet = ilwDir + "\\System\\*.ics";
	} else
		pathToColorSet = folder;

	CFileFind finder;
	BOOL fFound = finder.FindFile(pathToColorSet.c_str());

	while(fFound) {
		fFound = finder.FindNextFile();
		if (finder.IsDirectory())
		{
			FileName fnFolder(finder.GetFilePath());
			if ( fnFolder.sFile != "." && fnFolder.sFile != ".." && fnFolder.sFile != "")
				loadColorSets(fnFolder.sFullPath());
		}
		else {
			FileName fnCS(finder.GetFilePath());
			parseFile(fnCS);
		}
	}
}

void Representation::parseFile(const FileName& fn) {
	ifstream file(fn.sPhysicalPath().c_str());
	if ( file.is_open()) {
		int i = 0;
		int index = iUNDEF;
		while(!file.eof()) {
			String line;
			file >> line;
			if ( line.size() < 7)
				continue;
			String head = line.sHead("=");
			String tail=line.sTail("=");
			if ( head == "index") {
				index = tail.iVal();
				if (index == iUNDEF)
					throw ErrorObject(TR(String("Illegal color definition in %S", fn.sRelative())));
				continue;
			}


			Array<String> parts;
			Split(tail, parts,",");
			Color clr;
			if ( parts.size() == 4) {
				clr = Color(parts[0].iVal(), parts[1].iVal(),parts[2].iVal(),parts[3].iVal());
			}else {
				clr = Color(0,0,0,0);
			}
			colorSets[index].push_back(clr);
			++i;
		}
		if ( i != 32) {
			throw ErrorObject(TR(String("Illegal color definition in %S", fn.sRelative())));
		}
	}
}
Color Representation::clrPrimary(int iNr, int set) 
{
	if ( colorSets.size() == 0) {
		loadColorSets();
	}
	int index = iNr % 32;
	map< int, vector<Color> >::const_iterator iter = colorSets.find(set);
	if ( iter != colorSets.end())
		return (*iter).second[index];
	if ( colorSets.size() == 0)
		throw ErrorObject(TR(String("missing color definition for id domain")));

	return colorSets[0][index];
}