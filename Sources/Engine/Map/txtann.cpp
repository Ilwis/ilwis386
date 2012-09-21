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
/*
// $Log: /ILWIS 3.0/BasicDataStructures/txtann.cpp $
 * 
 * 7     27-10-00 2:04p Martin
 * added getobjectstructure function
 * 
 * 6     10-01-00 4:06p Martin
 * removed internal rowcol and changed them to true coords
 * 
 * 5     29-10-99 9:20 Wind
 * thread save stuff
 * 
 * 4     22-10-99 12:56 Wind
 * thread save access (not yet finished)
 * 
 * 3     9/27/99 12:59p Wind
 * comments
 * 
 * 2     9/27/99 11:13a Wind
 * changed calls to static ObjectInfo::ReadElement and WriteElement to
 * member function calls
	Last change:  J    21 Oct 99   10:03 am
*/
// Revision 1.4  1998/10/07 12:06:36  Wim
// long iRaw is now outside if statement (syntax error)
//
// Revision 1.3  1998-10-07 12:58:10+01  Wim
// Based on a map with domain Class or ID now also a column with the map name and
// the domain of the map is created with the calues of the different items.
//
// Revision 1.2  1998-10-07 10:59:43+01  Wim
// A map will first be calculated before it will be used in the constructor.
//
// Revision 1.1  1998-09-16 18:22:46+01  Wim
// 22beta2
//
/* AnnotationText
   Copyright Ilwis System Development ITC
   may 1998, by Wim Koolhoven
	Last change:  WK    7 Oct 98    1:01 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\txtann.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Headers\Hs\DAT.hs"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

IlwisObjectPtrList AnnotationText::listTxtAnn;

AnnotationText::AnnotationText()
: IlwisObject(listTxtAnn)
{
}

AnnotationText::AnnotationText(const AnnotationText& ta)
: IlwisObject(listTxtAnn, ta.pointer())
{
}

AnnotationText::AnnotationText(const FileName& fn)
: IlwisObject(listTxtAnn)
{
  FileName filnam = fn;
  if (filnam.sExt.length() == 0)
    filnam.sExt = ".atx";
  if (!File::fExist(filnam)) { // check std dir
    filnam.Dir(getEngine()->getContext()->sStdDir());
    if (!File::fExist(filnam))
      NotFoundError(fn);
  }
  MutexFileName mut(fn);
  AnnotationTextPtr* p = pGet(filnam);
  if (0 == p)
    p = new AnnotationTextPtr(filnam);
  SetPointer(p);
}

AnnotationTextPtr* AnnotationText::pGet(const FileName& fn)
{
  return static_cast<AnnotationTextPtr*>(listTxtAnn.pGet(fn));
}

AnnotationTextPtr::AnnotationTextPtr(const FileName& fn, int)
: IlwisObjectPtr(fn, true),
  sFontName("Arial"),
  rWorkingScale(rUNDEF), fUseXY(false),
  rFontSize(10),
  fBold(false), fItalic(false), fUnderline(false), fTransparent(true),
//  fUpright(false),
  iJustification(5), color(0,0,0), rRotation(0)
{
  CreateColumns();
}

void AnnotationTextPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
  IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
  if (tbl.fValid()) {
    FileName fnDat;
    ReadElement("TableStore", "Data", fnDat);
    if (!fnDat.fValid())
      return;
    ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
  }
  if (asSection != 0) {
    (*asSection) &= "TableStore";
    (*asEntry) &= "Data";
  }
}

void AnnotationTextPtr::FromRec(long iRec)
{
  if (colFontName.fValid())
    sFontName = colFontName->sValue(iRec,0);
  if (colFontSize.fValid())
    rFontSize = colFontSize->rValue(iRec);
  if (colFontBold.fValid())
    fBold = colFontBold->iValue(iRec)!=0;
  if (colFontItalic.fValid())
    fItalic = colFontItalic->iValue(iRec)!=0;
  if (colFontUnderline.fValid())
    fUnderline = colFontUnderline->iValue(iRec)!=0;
//  if (colFontUpright.fValid())
//    fUpright = colFontUpright->iValue(iRec);
  if (colJustification.fValid()) {
    iJustification = colJustification->iValue(iRec);
    if (iJustification < 1 || iJustification > 9)
      iJustification = 5;
  }
  if (colColor.fValid())
    color = colColor->iRaw(iRec);
  if (colTransparent.fValid())
    fTransparent = colTransparent->iValue(iRec)!=0;
  if (colRotation.fValid())
    rRotation = colRotation->rValue(iRec);
}

void AnnotationTextPtr::FillRec(long iRec)
{
  if (colFontName.fValid())
    colFontName->PutVal(iRec, sFontName);
  if (colFontSize.fValid())
    colFontSize->PutVal(iRec, rFontSize);
  if (colFontBold.fValid())
    colFontBold->PutVal(iRec, (long)fBold);
  if (colFontItalic.fValid())
    colFontItalic->PutVal(iRec, (long)fItalic);
  if (colFontUnderline.fValid())
    colFontUnderline->PutVal(iRec, (long)fUnderline);
//  if (colFontUpright.fValid())
//    colFontUpright->PutVal(iRec, (long)fUpright);
  if (colJustification.fValid())
    colJustification->PutVal(iRec, (long)iJustification);
  if (colColor.fValid())
    colColor->PutRaw(iRec, color);
  if (colTransparent.fValid())
    colTransparent->PutVal(iRec, (long)fTransparent);
  if (colRotation.fValid())
    colRotation->PutVal(iRec, rRotation);
}

long AnnotationTextPtr::iAdd(const Coord& c, const String& s)
{
  if (c.fUndef() || !colX.fValid() || !colY.fValid())
    return iUNDEF;
  long iRec = tbl->iRecNew();
  colX->PutVal(iRec, c.x);
  colY->PutVal(iRec, c.y);
  colText->PutVal(iRec, s);
  FillRec(iRec);
  Updated();
  return iRec;
}

long AnnotationTextPtr::iAdd(double rRow, double rCol, const String& s)
{
  if (rRow == rUNDEF || rCol == rUNDEF ||
      !colRow.fValid() || !colCol.fValid())
    return iUNDEF;
  long iRec = tbl->iRecNew();
  colRow->PutVal(iRec, rRow);
  colCol->PutVal(iRec, rCol);
  colText->PutVal(iRec, s);
  FillRec(iRec);
  Updated();
  return iRec;
}

void AnnotationTextPtr::CreateColumns()
{
  if (tbl.fValid())
    return;
  Updated();
  FileName fn = fnObj;
  fn.sExt = ".at#";
  TablePtr* pt = new TablePtr(fnObj, fn, Domain("None"), "");
  tbl.SetPointer(pt);
  Domain dmString("string");
  colText = tbl->colNew("Text", dmString);
  colText->SetOwnedByTable(true);
  colText->SetDescription("Annotation Text");
  if (fUseXY) {
    Domain dmReal("value");
    ValueRange vr(-1e8,1e8,0.001);
    colX = tbl->colNew("X", dmReal,vr);
    colX->SetOwnedByTable(true);
    colX->SetDescription("X-coord");
    colY = tbl->colNew("Y", dmReal,vr);
    colY->SetOwnedByTable(true);
    colY->SetDescription("Y-coord");
  }
  else {
    Domain dmReal("value");
    ValueRange vr(-1e8,1e8,0.01);
    colRow = tbl->colNew("Row", dmReal,vr);
    colRow->SetOwnedByTable(true);
    colRow->SetDescription(TR("Row position of text"));
    colCol = tbl->colNew("Col", dmReal,vr);
    colCol->SetOwnedByTable(true);
    colCol->SetDescription(TR("Col position of text"));
  }
}

void AnnotationTextPtr::UseXY(const GeoRef& gr)
{
  if (fUseXY)
    return;
  fUseXY = true;
  Domain dmReal("value");
  ValueRange vr(-1e8,1e8,0.001);
  colX = tbl->colNew("X", dmReal,vr);
  colX->SetOwnedByTable(true);
  colX->SetDescription(TR("X-coord"));
  colY = tbl->colNew("Y", dmReal,vr);
  colY->SetOwnedByTable(true);
  colY->SetDescription(TR("Y-coord"));
  long iMax = iSize();
  for (long i = 1; i <= iMax; ++i) {
    double rRow, rCol;
    Coord crd;
    rRow = colRow->rValue(i);
    rCol = colCol->rValue(i);
    gr->RowCol2Coord(rRow, rCol, crd);
    colX->PutVal(i, crd.x);
    colY->PutVal(i, crd.y);
  }
  tbl->RemoveCol(colRow);
  tbl->RemoveCol(colCol);
  colRow = Column();
  colCol = Column();
}

void AnnotationTextPtr::UseRowCol(const GeoRef& gr)
{
  if (!fUseXY)
    return;
  fUseXY = false;
  Domain dmReal("value");
  ValueRange vr(-1e8,1e8,0.01);
  colRow = tbl->colNew("Row", dmReal,vr);
  colRow->SetOwnedByTable(true);
  colRow->sDescription = TR("Row position of text");
  colCol = tbl->colNew("Col", dmReal,vr);
  colCol->SetOwnedByTable(true);
  colCol->sDescription = TR("Col position of text");
  long iMax = iSize();
  for (long i = 1; i <= iMax; ++i) {
    double rRow, rCol;
    Coord crd;
    crd.x = colX->rValue(i);
    crd.y = colY->rValue(i);
    gr->Coord2RowCol(crd, rRow, rCol);
    colRow->PutVal(i, rRow);
    colCol->PutVal(i, rCol);
  }
  tbl->RemoveCol(colX);
  tbl->RemoveCol(colY);
  colX = Column();
  colY = Column();
}

void AnnotationTextPtr::CreateColumnFontName()
{
  if (colFontName.fValid())
    return;
  long iMax = iSize();
  Domain dmString("string");
  colFontName = tbl->colNew("FontName", dmString);
  for (long i = 1; i <= iMax; ++i)
    colFontName->PutVal(i, sFontName);
}

void AnnotationTextPtr::CreateColumnFontSize()
{
  if (colFontSize.fValid())
    return;
  long iMax = iSize();
  Domain dmReal("value");
  ValueRange vr(1,10000,0.1);
  colFontSize = tbl->colNew("FontSize", dmReal, vr);
  for (long i = 1; i <= iMax; ++i)
    colFontSize->PutVal(i, rFontSize);
}

void AnnotationTextPtr::CreateColumnFontBold()
{
  if (colFontBold.fValid())
    return;
  long iMax = iSize();
  Domain dmBool("yesno");
  ValueRange vr(1,10000,0.1);
  colFontBold = tbl->colNew("FontBold", dmBool);
  for (long i = 1; i <= iMax; ++i)
    colFontBold->PutVal(i, (long)fBold);
}

void AnnotationTextPtr::CreateColumnFontItalic()
{
  if (colFontItalic.fValid())
    return;
  long iMax = iSize();
  Domain dmBool("yesno");
  ValueRange vr(1,10000,0.1);
  colFontItalic = tbl->colNew("FontItalic", dmBool);
  for (long i = 1; i <= iMax; ++i)
    colFontItalic->PutVal(i, (long)fItalic);
}

void AnnotationTextPtr::CreateColumnFontUnderline()
{
  if (colFontUnderline.fValid())
    return;
  long iMax = iSize();
  Domain dmBool("yesno");
  ValueRange vr(1,10000,0.1);
  colFontUnderline = tbl->colNew("FontUnderline", dmBool);
  for (long i = 1; i <= iMax; ++i)
    colFontUnderline->PutVal(i, (long)fUnderline);
}

void AnnotationTextPtr::CreateColumnJustification()
{
  if (colJustification.fValid())
    return;
  long iMax = iSize();
  Domain dmReal("value");
  ValueRange vr(1,9);
  colJustification = tbl->colNew("Justification", dmReal, vr);
  for (long i = 1; i <= iMax; ++i)
    colJustification->PutVal(i, (long)iJustification);
}

void AnnotationTextPtr::CreateColumnColor()
{
  if (colColor.fValid())
    return;
  long iMax = iSize();
  Domain dmColor("color");
  ValueRange vr(1,10000,0.1);
  colColor = tbl->colNew("Color", dmColor);
  for (long i = 1; i <= iMax; ++i)
    colColor->PutRaw(i, color.iVal());
}

void AnnotationTextPtr::CreateColumnTransparent()
{
  if (colTransparent.fValid())
    return;
  long iMax = iSize();
  Domain dmBool("yesno");
  ValueRange vr(1,10000,0.1);
  colTransparent = tbl->colNew("Transparent", dmBool);
  for (long i = 1; i <= iMax; ++i)
    colTransparent->PutVal(i, (long)fTransparent);
}

void AnnotationTextPtr::CreateColumnRotation()
{
  if (colRotation.fValid())
    return;
  long iMax = iSize();
  Domain dmReal("value");
  ValueRange vr(-360,360,0.1);
  colRotation = tbl->colNew("Rotation", dmReal, vr);
  for (long i = 1; i <= iMax; ++i)
    colRotation->PutVal(i, rRotation);
}



AnnotationTextPtr::AnnotationTextPtr(const FileName& fn, const PointMap& mp, eCodeName ecn)
: IlwisObjectPtr(fn, true),
  sFontName("Arial"),
  rWorkingScale(rUNDEF), fUseXY(true),
  rFontSize(10),
  fBold(false), fItalic(false), fUnderline(false), fTransparent(true),
//  fUpright(false),
  iJustification(5), color(0,0,0), rRotation(0)
{
  CreateColumns();

  if (!mp->fCalculated()) {
    mp->Calc();
    if (!mp->fCalculated())
      return;
  }

  Column colName;
  DomainSort* ds = mp->dm()->pdsrt();
  if (ds)
    colName = tbl->colNew(mp->sName(), mp->dm());

  long iPoints = mp->iFeatures();
  tbl->iRecNew(iPoints);
  for (long i = 1; i <= iPoints; ++i) {
    long iRaw;
    Coord c = mp->cValue(i);
    colX->PutVal(i, c.x);
    colY->PutVal(i, c.y);
    String s = mp->sValue(i);
    if (ds) {
      iRaw = mp->iRaw(i);
      if (iUNDEF != iRaw)
        switch (ecn) {
          case eCODE:
            s = ds->sCodeByRaw(iRaw);
            break;
          case eNAME:
            s = ds->sNameByRaw(iRaw);
            break;
          case eBOTH:
            s = ds->sValueByRaw(iRaw,0);
            break;
        }
      }
    colText->PutVal(i, s);
    if (ds)
      colName->PutRaw(i, iRaw);
  }
}

AnnotationTextPtr::AnnotationTextPtr(const FileName& fn, const SegmentMap& mp, eCodeName ecn)
: IlwisObjectPtr(fn, true),
  sFontName("Arial"),
  rWorkingScale(rUNDEF), fUseXY(true),
  rFontSize(10),
  fBold(false), fItalic(false), fUnderline(false), fTransparent(true),
  iJustification(5), color(0,0,0), rRotation(0)
{
	CreateColumns();

	if (!mp->fCalculated()) {
		mp->Calc();
		if (!mp->fCalculated())
			return;
	}

	Column colName;
	DomainSort* ds = mp->dm()->pdsrt();
	if (ds)
		colName = tbl->colNew(mp->sName(), mp->dm());

	long iSeg = mp->iFeatures();
	tbl->iRecNew(iSeg);
	long iRec = 0;
	CoordBuf buf(1000);
	long iNr;
	for (int i = 0; i < iSeg; ++i) {
		ILWIS::Segment *seg = (ILWIS::Segment *)mp->getFeature(i);
		if ( seg == NULL || seg->fValid() == false)
			continue;
		long iRaw;
		CoordinateSequence *buf = seg->getCoordinates();
		iNr = buf->size();
		iNr /= 2;
		Coord crd = buf->getAt(iNr);
		String sName = seg->sValue(mp->dvrs());
		if (ds) {
			iRaw = seg->iValue();
			if (iUNDEF != iRaw)
				switch (ecn) {
		  case eCODE:
			  sName = ds->sCodeByRaw(iRaw);
			  break;
		  case eNAME:
			  sName = ds->sNameByRaw(iRaw);
			  break;
		  case eBOTH:
			  sName = ds->sValueByRaw(iRaw,0);
			  break;
			}
		}
		if ("?" == sName)
			continue;
		iRec += 1;
		colX->PutVal(iRec, crd.x);
		colY->PutVal(iRec, crd.y);
		colText->PutVal(iRec, sName);
		if (ds)
			colName->PutRaw(iRec, iRaw);
		delete buf;
	}
	if (iRec < iSeg)
		tbl->DeleteRec(iRec+1, iSeg-iRec);
}

AnnotationTextPtr::AnnotationTextPtr(const FileName& fn, const PolygonMap& mp, eCodeName ecn)
: IlwisObjectPtr(fn, true),
  sFontName("Arial"),
  rWorkingScale(rUNDEF), fUseXY(true),
  rFontSize(10),
  fBold(false), fItalic(false), fUnderline(false), fTransparent(true),
  iJustification(5), color(0,0,0), rRotation(0)
{
  CreateColumns();

  if (!mp->fCalculated()) {
    mp->Calc();
    if (!mp->fCalculated())
      return;
  }

  Column colName;
  DomainSort* ds = mp->dm()->pdsrt();
  if (ds)
    colName = tbl->colNew(mp->sName(), mp->dm());

  long iPol = mp->iFeatures();
  tbl->iRecNew(iPol);
  long iRec = 0;
  for (int i = 0; i < iPol; ++i) {
	  ILWIS::Polygon *pol = (ILWIS::Polygon *) mp->getFeature(i);
	  if ( pol == false || pol->fValid() == false)
		  continue;
    if (pol->rArea() < 0)
      continue;
    long iRaw;
    Coord crd = pol->crdFindPointInPol();
	String sName = pol->sValue(mp->dvrs());
    if (ds) {
      iRaw = pol->iValue();
      if (iUNDEF != iRaw)
        switch (ecn) {
          case eCODE:
            sName = ds->sCodeByRaw(iRaw);
            break;
          case eNAME:
            sName = ds->sNameByRaw(iRaw);
            break;
          case eBOTH:
            sName = ds->sValueByRaw(iRaw,0);
            break;
        }
      }
    if ("?" == sName)
      continue;
    iRec += 1;
    colX->PutVal(iRec, crd.x);
    colY->PutVal(iRec, crd.y);
    colText->PutVal(iRec, sName);
    if (ds)
      colName->PutRaw(iRec, iRaw);
  }
  if (iRec < iPol)
    tbl->DeleteRec(iRec+1, iPol-iRec);
}

AnnotationTextPtr::AnnotationTextPtr(const FileName& fn)
: IlwisObjectPtr(fn),
  sFontName("Arial"),
  rWorkingScale(rUNDEF), fUseXY(false),
  rFontSize(10),
  fBold(false), fItalic(false), fUnderline(false), fTransparent(true),
  iJustification(5), color(0,0,0), rRotation(0)
{
  ReadElement("AnnotationText", "Working Scale", rWorkingScale);
  ReadElement("AnnotationText", "Font Name", sFontName);
  ReadElement("AnnotationText", "Font Size", rFontSize);
  ReadElement("AnnotationText", "Bold", fBold);
  ReadElement("AnnotationText", "Italic", fItalic);
  ReadElement("AnnotationText", "Underline", fUnderline);
//  ReadElement("AnnotationText", "Upright", fUpright);
  ReadElement("AnnotationText", "Transparent", fTransparent);
  iJustification = iReadElement("AnnotationText", "Justification");
  ReadElement("AnnotationText", "Color", color);
  ReadElement("AnnotationText", "Rotation", rRotation);

  try {
    tbl = Table(fn);
  }
  catch (ErrorObject& err) {
    tbl = Table();
    err.Show();
  }
  if (tbl.fValid()) {
    tbl->fChanged = false;
    colX = tbl->col("X");
    colY = tbl->col("Y");
    colRow = tbl->col("Row");
    colCol = tbl->col("Col");
    if (colX.fValid() && colY.fValid())
      fUseXY = true;
    colText = tbl->col("Text");
    colFontName = tbl->col("FontName");
    colFontSize = tbl->col("FontSize");
    colFontBold = tbl->col("FontBold");
    colFontItalic = tbl->col("FontItalic");
    colFontUnderline = tbl->col("FontUnderline");
//    colFontUpright = tbl->col("FontUpright");
    colJustification = tbl->col("Justification");
    colColor = tbl->col("Color");
    colTransparent = tbl->col("Transparent");
    colRotation = tbl->col("Rotation");
  }
}

AnnotationTextPtr::~AnnotationTextPtr()
{
}

void AnnotationTextPtr::Store()
{
  IlwisObjectPtr::Store();
  if (tbl.fValid())
    tbl->Store();
  WriteElement("Ilwis", "Type", "AnnotationText");
  WriteElement("AnnotationText", "Working Scale", rWorkingScale);
  WriteElement("AnnotationText", "Font Name", sFontName);
  WriteElement("AnnotationText", "Font Size", rFontSize);
  WriteElement("AnnotationText", "Bold", fBold);
  WriteElement("AnnotationText", "Italic", fItalic);
  WriteElement("AnnotationText", "Underline", fUnderline);
//  WriteElement("AnnotationText", "Upright", fUpright);
  WriteElement("AnnotationText", "Transparent", fTransparent);
  WriteElement("AnnotationText", "Justification", iJustification);
  WriteElement("AnnotationText", "Color", color);
  WriteElement("AnnotationText", "Rotation", rRotation);
}

String AnnotationTextPtr::sType() const
{
  return "Annotation Text";
}

void AnnotationTextPtr::Delete(long iRec)
{
  tbl->DeleteRec(iRec);
  Updated();
}


void AnnotationTextPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	os.AddFile(fnObj, "TableStore", "Data");
	if ( os.fGetAssociatedFiles() )
	{
		int iNr=0;
		ObjectInfo::ReadElement("Table", "Columns", fnObj, iNr);
		for(int i=0; i<iNr; ++i)
		{
			String sCol("Col%d", i);
			String sKey;
			ObjectInfo::ReadElement("TableStore", sCol.c_str(), fnObj, sKey);
			os.AddFile(fnObj, String("Col:%S", sKey), "Domain");
		}			
	}
}	



