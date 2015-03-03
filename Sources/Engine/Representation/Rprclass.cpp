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


#include "Engine\Representation\Rprclass.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Domain\dmcolor.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Table\Rec.h"
#include "Engine\Table\Colbinar.h"
#include "Engine\Drawers\Drawer_n.h"

const int RepresentationClass::iSIZE_FACTOR=3;

RepresentationClass::RepresentationClass(const FileName& fn)
: RepresentationItems(fn)
{
	tbl.SetPointer(new TablePtr(fn, Table::sSectionPrefix(".rpr")));
	tbl->Load(); // load column data
	tbl->DoNotStore(true);
	_dm = tbl->dm();
	colColor = tbl->col("Color");
	if (colColor.fValid())
		colColor->SetOwnedByTable(true);
	colSecondClr = tbl->col("SecondColor");
	if (colSecondClr.fValid())
		colSecondClr->SetOwnedByTable(true);
	colAlpha = tbl->col("Alpha");
	if ( colAlpha.fValid())
		colAlpha->SetOwnedByTable(true);
	Column colWidth = tbl->col("Width");
	if (colWidth.fValid()) {
		colLineWidth = tbl->colNew("LineWidth", DomainValueRangeStruct(0,100,0.1));
		colLineWidth->SetOwnedByTable(true);
		colLineWidth->SetDescription("Line Width");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i) {
			double rVal = colWidth->rValue(i);
			if (rVal <= 1) // including 0 and undef
				rVal = 0.0;  // hairline
			else
				rVal *= 0.3; // ca. 0.3 mm per pixel
			colLineWidth->PutVal(i, rVal);
		}
		tbl->RemoveCol(colWidth);
	}
	colPattern = tbl->col("Pattern");
	if (colPattern.fValid())
		colPattern->SetOwnedByTable(true);
	colPatternData = tbl->col("PatternData");
	if (colPatternData.fValid())
		colPatternData->SetOwnedByTable(true);
	colSmbType2 = tbl->col("SymbolTypeNew");
	if (colSmbType2.fValid())
		colSmbType2->SetOwnedByTable(true);
	colSmbSize = tbl->col("SymbolSize");
	if (colSmbSize.fValid())
		colSmbSize->SetOwnedByTable(true);
	colSmbWidth = tbl->col("SymbolWidth");
	if (colSmbWidth.fValid())
		colSmbWidth->SetOwnedByTable(true);
	colSmbClr = tbl->col("SymbolColor");
	if (colSmbClr.fValid())
		colSmbClr->SetOwnedByTable(true);
	colSmbFC = tbl->col("SymbolFillColor");
	if (colSmbFC.fValid())
		colSmbFC->SetOwnedByTable(true);
	colSmbFont = tbl->col("SymbolFont");
	if (colSmbFont.fValid())
		colSmbFont->SetOwnedByTable(true);
	colSmbRot = tbl->col("SymbolRotation");
	if (colSmbRot.fValid())
		colSmbRot->SetOwnedByTable(true);
	colLineType = tbl->col("Line");
	if (colLineType.fValid())
		colLineType->SetOwnedByTable(true);
	colLineClrFill = tbl->col("LineFillColor");
	if (colLineClrFill.fValid())
		colLineClrFill->SetOwnedByTable(true);
	colLineWidth = tbl->col("LineWidth");
	if (colLineWidth.fValid())
		colLineWidth->SetOwnedByTable(true);
	colLineDist = tbl->col("LineDist");
	if (colLineDist.fValid())
		colLineDist->SetOwnedByTable(true);
	colHatching = tbl->col("Hatching");
	if ( colHatching.fValid()) {
		colHatching->SetOwnedByTable(true);
	}
	tbl->DoNotStore(false);
	tbl->DoNotUpdate();				
}

RepresentationClass::RepresentationClass(const FileName& fn, const Domain& dom)
: RepresentationItems(fn,dom)
{
	tbl.SetPointer(new TablePtr(fn, FileName(fn, String(".rp%c", ObjectInfo::cLastDataFileExtChar(fnObj, ".rp#"))), 
		dom, Table::sSectionPrefix(".rpr")));
	tbl->fUpdateCatalog = false; // was set by constructor
	tbl->DoNotStore(true);
	colColor = tbl->colNew("Color", Domain("color"));
	colColor->SetOwnedByTable(true);
	colColor->SetDescription("Color");
	for (long i=tbl->iOffset(); i<tbl->iRecs()+tbl->iOffset(); i++) 
		colColor->PutRaw(i, clrDefault[i % 16]);
	tbl->DoNotStore(false);
}

RepresentationClass::RepresentationClass(const FileName& fn, long iNr)
: RepresentationItems(fn)
{
	fChanged = true;
	tbl.SetPointer(new TablePtr(fn, FileName(fn, String(".rp%c", ObjectInfo::cLastDataFileExtChar(fnObj, ".rp#"))), 
		iNr, Table::sSectionPrefix(".rpr")));
	tbl->fUpdateCatalog = false; // was set by constructor
	tbl->DoNotStore(true);

	_dm = tbl->dm();
	colColor = tbl->colNew("Color", Domain("color"));
	colColor->SetOwnedByTable(true);
	colColor->SetDescription("Color");
	//  Color clr;
	long iMax = tbl->iRecs();
	for (long i = 0; i < iMax; ++i)
		colColor->PutRaw(i, /*clr,*/ clrDefault[i % 16]);
	tbl->DoNotStore(false);
}

void RepresentationClass::Store()
{
	if ( fStore()) {
		tbl->Store();
		RepresentationItems::Store();
		WriteElement("Representation", "Type", "RepresentationClass");
	}
}

String RepresentationClass::sType() const
{
	return "Representation Class";
}

RepresentationClass::~RepresentationClass()
{
	//  if (tbl->fChanged)
	//    Store();
	if (fErase)
		tbl->fErase = true;
}

Ilwis::Record RepresentationClass::rec(long iRaw) const
{
	return tbl->rec(iRaw);
}

void RepresentationClass::PutColor(long iRaw, Color clr)
{
	colColor->PutRaw(iRaw, clr.iVal());
	Updated();
}

void RepresentationClass::PutSecondColor(long iRaw, Color clr)
{
	if (!colSecondClr.fValid()) {
		colSecondClr = tbl->colNew("SecondColor", Domain("color"));
		colSecondClr->SetOwnedByTable(true);
		colSecondClr->SetDescription("Second Color");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = 1; i <= iMax; ++i)
			colSecondClr->PutRaw(i, Color(255,255,255).iVal());
	}
	colSecondClr->PutRaw(iRaw, clr.iVal());
	Updated();
}
/*
void RepresentationClass::PutWidth(long iRaw, short iWidth)
{
if (!colWidth.fValid()) {
//    Domain dmInt("int");
colWidth = tbl->colNew("Width", DomainValueRangeStruct(0,10000));
colWidth->SetOwnedByTable(true);
colWidth->sDescription = "Line width";
long iMax = tbl->iRecs()+tbl->iOffset()-1;
for (long i = 1; i <= iMax; ++i)
colWidth->PutVal(i, 1L);
}
colWidth->PutVal(iRaw, (long)iWidth);
Updated();
}
*/
void RepresentationClass::PutPattern(long iRaw, short iPattern)
{
	if (!colPattern.fValid()) {
		//    Domain dmInt("int");
		colPattern = tbl->colNew("Pattern", DomainValueRangeStruct(0,10000));
		colPattern->SetOwnedByTable(true);
		colPattern->SetDescription("Pattern");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colPattern->PutRaw(i, 0);
	}
	colPattern->PutRaw(iRaw, iPattern);
	Updated();
}



void RepresentationClass::PutPatternData(long iRaw, const short* ptr)
{
	if (ptr == 0)
		return;
	PutPattern(iRaw, 9999);
	if (colPatternData.fValid() && !colPatternData->fBinary()) {
		tbl->RemoveCol(colPatternData);
		colPatternData = Column();
	}
	if (!colPatternData.fValid()) {
		Domain dmBinary("Binary");
		colPatternData = tbl->colNew("PatternData", dmBinary);
	}
	BinMemBlock bmb(16,(const void*)ptr);
	colPatternData->PutVal(iRaw, bmb);
	Updated();
}

void RepresentationClass::PutSymbolType(long iRaw, const String& symbName)
{
	if (!colSmbType2.fValid()) {
		//    Domain dmInt("int");
		colSmbType2 = tbl->colNew("SymbolTypeNew", DomainValueRangeStruct(Domain("String")));
		colSmbType2->SetOwnedByTable(true);
		colSmbType2->SetDescription("Symbol type");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colSmbType2->PutVal(i, DEFAULT_POINT_SYMBOL_TYPE);
	}
	colSmbType2->PutVal(iRaw, symbName);
	Updated();
}

void RepresentationClass::PutSymbolSize(long iRaw, short iSize)
{
	if (!colSmbSize.fValid()) {
		//    Domain dmInt("int");
		colSmbSize = tbl->colNew("SymbolSize", DomainValueRangeStruct(0,10000));
		colSmbSize->SetOwnedByTable(true);
		colSmbSize->SetDescription("Symbol size");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colSmbSize->PutVal(i, (long)100);
	}
	colSmbSize->PutVal(iRaw, (long)iSize);
	Updated();
}

void RepresentationClass::PutAlpha(long iRaw, double alpha){
	if ( !colAlpha.fValid()) {
		colAlpha = tbl->colNew("Alpha",DomainValueRangeStruct(0.0,1.0,0.01));
		colAlpha->SetOwnedByTable(true);
		colAlpha->SetDescription("Alpha value of items");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colAlpha->PutVal(i, 1.0);
	}
	colAlpha->PutVal(iRaw, alpha);
	Updated();
}

void RepresentationClass::PutHatchingName(long iRaw, const String& hatch){
	if ( !colHatching.fValid()) {
		colHatching = tbl->colNew("Hatching", Domain("String"));
		colHatching->SetOwnedByTable(true);
		colHatching->SetDescription(TR("Hatching patterns for polygons"));
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colHatching->PutVal(i, sUNDEF);

	}
	colHatching->PutVal(iRaw, hatch);
	Updated();
}

void RepresentationClass::PutSymbolWidth(long iRaw, short iWidth)
{
	if (!colSmbWidth.fValid()) {
		//    Domain dmInt("int");
		colSmbWidth = tbl->colNew("SymbolWidth", DomainValueRangeStruct(0,10000));
		colSmbWidth->SetOwnedByTable(true);
		colSmbWidth->SetDescription("Symbol width");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colSmbWidth->PutVal(i, 1L);
	}
	colSmbWidth->PutVal(iRaw, (long)iWidth);
	Updated();
}

void RepresentationClass::PutSymbolColor(long iRaw, Color clr)
{
	if (!colSmbClr.fValid()) {
		Domain dmCol("color");
		colSmbClr = tbl->colNew("SymbolColor", dmCol);
		colSmbClr->SetOwnedByTable(true);
		colSmbClr->SetDescription("Symbol color");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i) {
			Color c = clrRaw(i);
			colSmbClr->PutRaw(i, c.iVal());
		}  
	}
	colSmbClr->PutRaw(iRaw, clr.iVal());
	Updated();
}

void RepresentationClass::PutSymbolFill(long iRaw, Color clr)
{
	if (!colSmbFC.fValid()) {
		Domain dmCol("color");
		colSmbFC = tbl->colNew("SymbolFillColor", dmCol);
		colSmbFC->SetOwnedByTable(true);
		colSmbFC->SetDescription("Symbol fill color");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i) {
			Color c = clrRaw(i);
			colSmbFC->PutRaw(i, c.iVal());
		}  
	}
	colSmbFC->PutRaw(iRaw, clr.iVal());
	Updated();
}

void RepresentationClass::PutSymbolFont(long iRaw, const String& sFont)
{
	if (!colSmbFont.fValid()) {
		Domain dmStr("string");
		colSmbFont = tbl->colNew("SymbolFont", dmStr);
		colSmbFont->SetOwnedByTable(true);
		colSmbFont->SetDescription("Symbol Font");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
	}
	colSmbFont->PutVal(iRaw, sFont);
	Updated();
}

void RepresentationClass::PutSymbolRotation(long iRaw, double rRot)
{
	if (!colSmbRot.fValid()) {
		colSmbRot = tbl->colNew("SymbolRotation", DomainValueRangeStruct(-360,360,0.1));
		colSmbRot->SetOwnedByTable(true);
		colSmbRot->SetDescription("Symbol Rotation");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i) {
			colSmbRot->PutVal(i, 0.0);
		}  
	}
	colSmbRot->PutVal(iRaw, rRot);
	Updated();
}

long RepresentationClass::iAdd(const Color& clr)
{
	long iNewRaw = tbl->iRecNew();
	PutColor(iNewRaw, clr);
	return iNewRaw;;
}

void RepresentationClass::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
	FileName fnDat;
	ReadElement(String("%STableStore", tbl->sSectionPrefix).c_str(), "Data", fnDat);
	ObjectInfo::Add(afnDat, fnDat, fnObj.sPath());
	if (asSection != 0) {
		(*asSection) &= String("%STableStore", tbl->sSectionPrefix);
		(*asEntry) &= "Data";
	}
}

void RepresentationClass::PutLine(long iRaw, short iLine)
{
	if (!colLineType.fValid()) {
		colLineType = tbl->colNew("Line", DomainValueRangeStruct(0,10000));
		colLineType->SetOwnedByTable(true);
		colLineType->SetDescription("Line Type");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colLineType->PutRaw(i, 1);
	}
	colLineType->PutRaw(iRaw, iLine);
	Updated();
}

void RepresentationClass::PutLineFillColor(long iRaw, Color clr)
{
	if (!colLineClrFill.fValid()) {
		Domain dmCol("color");
		colLineClrFill = tbl->colNew("LineFillColor", dmCol);
		colLineClrFill->SetOwnedByTable(true);
		colLineClrFill->SetDescription("Line Fill Color");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		Color clrDflt(-1);
		for (long i = tbl->iOffset(); i <= iMax; ++i) {
			colLineClrFill->PutRaw(i, clrDflt.iVal());
		}
	}
	colLineClrFill->PutRaw(iRaw, clr.iVal());
	Updated();
}

void RepresentationClass::PutLineWidth(long iRaw, double rWidth)
{
	if (!colLineWidth.fValid()) {
		colLineWidth = tbl->colNew("LineWidth", DomainValueRangeStruct(0,100,0.1));
		colLineWidth->SetOwnedByTable(true);
		colLineWidth->SetDescription("Line Width");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colLineWidth->PutVal(i, RepresentationItems::rDefaultLineWidth);
	}
	colLineWidth->PutVal(iRaw, rWidth);
	Updated();
}

void RepresentationClass::PutLineDist(long iRaw, double rDist)
{
	if (!colLineDist.fValid()) {
		colLineDist = tbl->colNew("LineDist", DomainValueRangeStruct(-100,100,0.1));
		colLineDist->SetOwnedByTable(true);
		colLineDist->SetDescription("Line Distance");
		long iMax = tbl->iRecs()+tbl->iOffset()-1;
		for (long i = tbl->iOffset(); i <= iMax; ++i)
			colLineDist->PutVal(i, 5.0);
	}
	colLineDist->PutVal(iRaw, rDist);
	Updated();
}

void RepresentationClass::GetObjectStructure(ObjectStructure& os)
{
	RepresentationPtr::GetObjectStructure( os );
	if ( os.fGetAssociatedFiles())
		os.AddFile(fnObj, "Table", "Domain");
}
