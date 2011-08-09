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
/* CoordSystemCTP
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   17 Sep 98    2:34 pm
*/

#include "Headers\toolspch.h"
#include "Engine\SpatialReference\csctp.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Map\basemap.h"
#include "Headers\Hs\DAT.hs"

#define EPS10 1.e-10

CoordSystemCTP::CoordSystemCTP(const FileName& fn, bool fUseColZ)
: CoordSystemDirect(fn)
{
  _fValid = false;
  tblCTP.SetPointer(new TablePtr(fn, Table::sSectionPrefix(".csy")));
  tblCTP->Load(); // load column data
  colX = tblCTP->col("X");
  colY = tblCTP->col("Y");
  colRefX = tblCTP->col("RelX");
  if (!colRefX.fValid())
    colRefX = tblCTP->col("RefX");
  colRefY = tblCTP->col("RelY");
  if (!colRefY.fValid())
    colRefY = tblCTP->col("RefY");
  if ( fUseColZ) {
    colRefZ = tblCTP->col("RelZ");
    if (!colRefZ.fValid())
      colRefZ = tblCTP->col("RefZ");
  }
  colAct = tblCTP->col("Active");
  ReadElement("CoordSystemCTP", "Background Map", fnBackgroundMap);
  if (!fnBackgroundMap.fValid()) // compatibility with 2.2 !
    ReadElement("CoordSystemTiePoints", "Background Map", fnBackgroundMap);
}

CoordSystemCTP::CoordSystemCTP(const FileName& fn, const CoordSystem& csRef, const FileName& fnBackgrMap, bool fUseColZ)
: CoordSystemDirect(fn, csRef),
  fnBackgroundMap(fnBackgrMap)
{
  _fValid = false;
  Domain dm("none");
  tblCTP.SetPointer(new TablePtr(fn, FileName(fn, ".cs#"), dm, Table::sSectionPrefix(".csy")));
  tblCTP->fUpdateCatalog = false; // was set by constructor
  DomainValueRangeStruct dvsReal(-1e9,1e9,0.001);
  Domain dmBool("bool");
  colX = tblCTP->colNew("X", dvsReal);
  colX->SetOwnedByTable(true);
  colX->SetDescription(TR("X-coord"));
  colY = tblCTP->colNew("Y", dvsReal);
  colY->SetOwnedByTable(true);
  colY->SetDescription(TR("Y-coord"));
  colRefX = tblCTP->colNew("RelX", dvsReal);
  colRefX->SetOwnedByTable(true);
  colRefX->SetDescription(TR("Reference X-coord"));
  colRefY = tblCTP->colNew("RelY", dvsReal);
  colRefY->SetOwnedByTable(true);
  colRefY->SetDescription(TR("Reference Y-coord"));
  if (fUseColZ) {
    colRefZ = tblCTP->colNew("RelZ", dvsReal);
    colRefZ->SetOwnedByTable(true);
    colRefZ->SetDescription(TR("Reference Z-coord"));
  }
  colAct = tblCTP->colNew("Active", dmBool);
  colAct->SetOwnedByTable(true);
  colAct->SetDescription(TR("Active point"));
  fChanged = true;

  try {
    BaseMap map(fnBackgrMap);
    if (map.fValid())
      cb = map->cb();
  }
  catch (ErrorObject&) {
  }
}


String CoordSystemCTP::sType() const
{
	return "CoordSystem CTP";
}

CoordSystemCTP::~CoordSystemCTP()
{
  if (fErase)
    tblCTP->fErase = true;
}

void CoordSystemCTP::Store()
{
  tblCTP->Store();
  CoordSystemDirect::Store();
  WriteElement("CoordSystemCTP", "Background Map", fnBackgroundMap);
}


void CoordSystemCTP::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".cs#", true);
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

int CoordSystemCTP::iRec(const Coord& crdVal) const
{
  for (int i = 1; i <= iNr(); ++i)
    if (crd(i) == crdVal)
      return i;
  return -1;    
}

int CoordSystemCTP::AddRec(const Coord& crd, const Coord& crdRef)
{ 
  int i = iRec(crd);
  if (i <= 0) {
    i = tblCTP->iRecNew(); 
    SetCoordRef(i, crdRef);
  }  
  SetCoord(i, crd); 
  SetActive(i, true);
  Updated();
  return i; 
}

bool CoordSystemCTP::fInverse() const
{
  return fValid();
}

void CoordSystemCTP::GetObjectStructure(ObjectStructure& os)
{
	CoordSystemDirect::GetObjectStructure( os );
	os.AddFile(fnObj, "TableStore", "Data");	
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "CoordSystemCTP", "Background Map");
	}		
}



