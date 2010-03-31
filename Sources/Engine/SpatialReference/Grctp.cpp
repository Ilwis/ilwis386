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
/* GeoRefCONTROLPOINTS
   Copyright Ilwis System Development ITC
   january 1997, by Jan Hendrikse
	Last change:  WK   28 Apr 98    3:02 pm
*/

#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\AssertD.h"
#include "Engine\SpatialReference\GRCTP.H"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"

GeoRefCTP::GeoRefCTP(const FileName& fn, bool fUseColZ)
: GeoRefPtr(fn)
{
  _fValid = false;
  MutexFileName mut(fn);
  tblCTP.SetPointer(new TablePtr(fn, Table::sSectionPrefix(".grf")));
  tblCTP->Load(); // load column data

  colX = tblCTP->col("X");
  colY = tblCTP->col("Y");
  if (fUseColZ)
    colZ = tblCTP->col("Z");
  colRow = tblCTP->col("Row");
  colCol = tblCTP->col("Col");
  colAct = tblCTP->col("Active");

  ISTRUE(fIEqual, colX.fValid(), true);
  ISTRUE(fIEqual, colY.fValid(), true);
  ISTRUE(fIEqual, colRow.fValid(), true);
  ISTRUE(fIEqual, colCol.fValid(), true);
  ISTRUE(fIEqual, colAct.fValid(), true);

  ReadElement("GeoRefCTP", "Background Map", fnBackgroundMap);
  if (!fnBackgroundMap.fValid()) // compatibility with 2.02 !
    ReadElement("GeoRefCTP", "Reference Map", fnBackgroundMap);
	ReadElement("GeoRefCTP", "SubPixelPrecision", fSubPixelPrecision);

  // next statements for compat. with 2.02:
  colX->SetOwnedByTable(true);
  colX->SetDescription("X-coord");
  colY->SetOwnedByTable(true);
  colY->SetDescription("Y-coord");
  if ( fUseColZ && colZ.fValid() ) {
    colZ->SetOwnedByTable(true);
    colZ->SetDescription("Z-coord");
  }
  colRow->SetOwnedByTable(true);
  colRow->SetDescription("Pixel row");
  colCol->SetOwnedByTable(true);
  colCol->SetDescription("Pixel column");
  colAct->SetOwnedByTable(true);
  colAct->SetDescription("Active point");
}

GeoRefCTP::GeoRefCTP(const FileName& fn, const CoordSystem& cs, RowCol rc, bool fUseColZ, bool fSubPixelPr)
: GeoRefPtr(fn,cs,rc)
{
  _fValid = false;
  Domain dm("none");
  tblCTP.SetPointer(new TablePtr(fn, FileName(fn, ".gr#"), dm, Table::sSectionPrefix(".grf")));
  tblCTP->fUpdateCatalog = false; // was set by constructor
  DomainValueRangeStruct dvsRealRow(-rc.Row, 2*rc.Row,0.01);
  DomainValueRangeStruct dvsRealCol(-rc.Col, 2*rc.Col,0.01);
  DomainValueRangeStruct dvsRow(-rc.Row, 2*rc.Row);
  DomainValueRangeStruct dvsCol(-rc.Col, 2*rc.Col);
  DomainValueRangeStruct dvsReal(-1e9,1e9,0.001);
  Domain dmBool("bool");
  colX = tblCTP->colNew("X", dvsReal);
  colX->SetOwnedByTable(true);
  colX->SetDescription("X-coord");
  colY = tblCTP->colNew("Y", dvsReal);
  colY->SetOwnedByTable(true);
  colY->SetDescription("Y-coord");
  if (fUseColZ) {
    Column colZdtm = tblCTP->colNew("Z_dtm", dvsReal);
    colZdtm->SetReadOnly(true);
    colZ = tblCTP->colNew("Z", dvsReal);
    colZ->SetOwnedByTable(true);
    colZ->SetDescription("Z-coord");
  }
	fSubPixelPrecision = fSubPixelPr; // for the time being
	if(fSubPixelPrecision)
	{
		colRow = tblCTP->colNew("Row", dvsRealRow);
		colCol = tblCTP->colNew("Col", dvsRealCol);
	}
	else
	{
		colRow = tblCTP->colNew("Row", dvsRow);
		colCol = tblCTP->colNew("Col", dvsCol);
	}
  colRow->SetOwnedByTable(true);
  colRow->SetDescription("Pixel row");
  colCol->SetOwnedByTable(true);
  colCol->SetDescription("Pixel column");
  colAct = tblCTP->colNew("Active", dmBool);
  colAct->SetOwnedByTable(true);
  colAct->SetDescription("Active point");
  fChanged = true;
}

void GeoRefCTP::Store()
{
  tblCTP->Store();
  GeoRefPtr::Store();
  WriteElement("GeoRefCTP", "Background Map", fnBackgroundMap);
	WriteElement("GeoRefCTP", "SubPixelPrecision", fSubPixelPrecision);
}

GeoRefCTP::~GeoRefCTP()
{
  if (fErase)
    tblCTP->fErase = true;
}

int GeoRefCTP::iRec(RowCol rcVal) const  
{
  for (int i = 1; i <= iNr(); ++i)
    if (rc(i) == rcVal)
      return i;
  return -1;    
}

int GeoRefCTP::iRec(Coord rcVal) const  
{
  for (int i = 1; i <= iNr(); ++i)
    if (crdRC(i) == rcVal)
      return i;
  return -1;    
}

int GeoRefCTP::AddRec(RowCol rc, Coord crd)  
{ 
  int i = iRec(rc);
  if (i <= 0) {
    i = tblCTP->iRecNew(); 
    SetRowCol(i, rc); 
  }  
  SetCoord(i, crd); 
  SetActive(i, true);
  Updated();
  return i; 
}

int GeoRefCTP::AddRec(RowCol rc, LatLon ll)  
{ 
  int i = iRec(rc);
  if (i <= 0) {
    i = tblCTP->iRecNew(); 
    SetRowCol(i, rc); 
  }  
  SetLatLon(i, ll); 
  SetActive(i, true);
  Updated();
  return i; 
}

int GeoRefCTP::AddRec(Coord crdRC, Coord crd)  
{ 
  int i = iRec(crdRC);
  if (i <= 0) {
    i = tblCTP->iRecNew(); 
    SetcrdRC(i, crdRC); 
  }  
  SetCoord(i, crd); 
  SetActive(i, true);
  Updated();
  return i; 
}

int GeoRefCTP::AddRec(Coord crdRC, LatLon ll)  
{ 
  int i = iRec(crdRC);
  if (i <= 0) {
    i = tblCTP->iRecNew(); 
    SetCoord(i, crdRC); 
  }  
  SetLatLon(i, ll); 
  SetActive(i, true);
  Updated();
  return i; 
}

void GeoRefCTP::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  FileName fnDat(fnObj, ".gr#", true);
  ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  if (asSection != 0) {
    (*asSection) &= String("%STableStore", tblCTP->sSectionPrefix);
    (*asEntry) &= "Data";
  }
}

bool GeoRefCTP::fDependent() const
{
  return false;
}

void GeoRefCTP::DoNotUpdate()
{
	GeoRefPtr::DoNotUpdate();
	tblCTP->DoNotUpdate();
	
}

void GeoRefCTP::GetObjectStructure(ObjectStructure& os)
{
	GeoRefPtr::GetObjectStructure( os );
	os.AddFile(fnObj, String("%STableStore", tblCTP->sSectionPrefix), "Data");	
	if ( os.fGetAssociatedFiles() )
	{
		os.AddFile(fnObj, "GeoRefCTP", "Background Map");
		os.AddFile(fnObj, "GeoRefCTP", "Reference Map");
	}		
}




