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
/* RepresentationItems
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK   13 May 98    5:21 pm
*/

#include "Engine\Representation\Rpritems.h"
#include "Engine\Table\Rec.h"
#include "Engine\Table\Colbinar.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Drawers\Drawer_n.h"

const double RepresentationItems::rDefaultLineWidth = 0.2;

RepresentationItems::RepresentationItems(const FileName& fn)
: RepresentationPtr(fn)
{
}

RepresentationItems::RepresentationItems(const FileName& fn, const Domain& dom)
: RepresentationPtr(fn,dom)
{
}

String RepresentationItems::sType() const
{
  return "Representation Items";
}

RepresentationItems::~RepresentationItems()
{
}

void RepresentationItems::GetColors(ColorBuf& clr) const
{
  GetColorLut(clr);
}

void RepresentationItems::GetColorLut(ColorBuf& clr) const
{
  if (!colColor.fValid())
    return;
  int iOff = colColor->iOffset();
  int iPlus = iOff > 0 ? iOff : 0;
  int iSize = colColor->iRecs();
  int iMax = iOff + iSize;
//  LongBuf buf(iSize+iPlus);
//  colColor->GetBufRaw(buf);
  clr.Size(iSize+iPlus);
  for (int i = iOff; i < iMax; ++i)
    clr[i] = colColor->iRaw(i); //buf[i];
}

Color RepresentationItems::clrDefault[16] = 
  { Color(0,0,0),     Color(255,0,0),   Color(255,255,0),   Color(0,0,255), 
    Color(255,0,255), Color(0,255,255), Color(0,255,0),     Color(255,255,255),
    Color(127,0,0),   Color(127,127,0), Color(0,0,127),     Color(127,0,127), 
    Color(0,127,127), Color(0,127,0),   Color(127,127,127), Color(255,127,0) };

Color RepresentationItems::clrRawDefault(long iRaw) const
{
	return clrDefault[abs(iRaw) % 16];
}

Color RepresentationItems::clrRaw(long iRaw) const
{
  if (iUNDEF == iRaw || !colColor.fValid())
    return Color();
  long iColor = colColor->iRaw(iRaw);  
  if (iUNDEF == iColor) 
    return clrDefault[abs(iRaw) % 16];
  else
    return Color(iColor);
}

Color RepresentationItems::clrSecondRaw(long iRaw) const
{
  if (iUNDEF == iRaw || !colSecondClr.fValid())
    return Color(-2);

  long iColor = colSecondClr->iRaw(iRaw);
  if (iUNDEF == iColor) 
    return colorUNDEF;
  else
    return Color(iColor);
}

/*
short RepresentationItems::iWidth(long iRaw) const
{
  if (iUNDEF == iRaw || !colWidth.fValid())
    return 1;
  short iVal = shortConv(colWidth->iValue(iRaw));
  if (iVal < 0)
    return 1;
  return iVal;  
}
*/
short RepresentationItems::iPattern(long iRaw) const
{
  if (iUNDEF == iRaw || !colPattern.fValid())
    return 0;
  short iVal = shortConv(colPattern->iValue(iRaw));
  if (iVal < 0)
    return 0;
  return iVal;  
}

void RepresentationItems::GetPattern(long iRaw, short aPat[8]) const
{
  RepresentationPtr::GetPattern(iRaw, aPat);
  if (iUNDEF == iRaw ||
      !colPatternData.fValid() ||
      iPattern(iRaw) != 9999 ||
      !colPatternData->fBinary())
    return;
  BinMemBlock bmb;
  colPatternData->GetVal(iRaw, bmb);
  memcpy(aPat, bmb.ptr(), 16);
}

String RepresentationItems::sSymbolType(long iRaw) const
{
  if (iUNDEF == iRaw || !colSmbType2.fValid())
    return DEFAULT_POINT_SYMBOL_TYPE;
  String sVal = colSmbType2->sValue(iRaw);
  sVal = sVal.sTrimSpaces();
  if (sVal != sUNDEF)
    return sVal;
  return DEFAULT_POINT_SYMBOL_TYPE;
}

short RepresentationItems::iSymbolSize(long iRaw) const
{
  if (iUNDEF == iRaw || !colSmbSize.fValid())
    return 100;
  short iVal = shortConv(colSmbSize->iValue(iRaw));
  if (iVal < 0)
    return 100;
  return iVal;  
}

short RepresentationItems::iSymbolWidth(long iRaw) const
{
  if (iUNDEF == iRaw || !colSmbWidth.fValid())
    return 1;
  short iVal = shortConv(colSmbWidth->iValue(iRaw));
  if (iVal < 0)
    return 1;
  return iVal;  
}

Color RepresentationItems::clrSymbol(long iRaw) const
{
  if (iUNDEF == iRaw)
    return Color(-1);
  else if (colSmbClr.fValid()) {
    long iRet = colSmbClr->iRaw(iRaw);
    if (iRet != iUNDEF)
      return iRet;
  }  
  return clrRaw(iRaw);  
}

Color RepresentationItems::clrSymbolFill(long iRaw) const
{
  if (iUNDEF == iRaw)
    return Color(-2);
  else if (colSmbFC.fValid()) {
    long iRet = colSmbFC->iRaw(iRaw);
    if (iRet != iUNDEF)
      return iRet;
  }    
  return clrRaw(iRaw);  
}

String RepresentationItems::sSymbolFont(long iRaw) const
{
  if (iUNDEF == iRaw)
    return sUNDEF;
  else if (colSmbFont.fValid())
    return colSmbFont->sValue(iRaw,0);
  return sUNDEF;
}

double RepresentationItems::rSymbolRotation(long iRaw) const
{
  if (iUNDEF == iRaw)
    return 0;
  else if (colSmbRot.fValid()) {
    double rRot = colSmbRot->rValue(iRaw);
    if (rRot != rUNDEF)
      return rRot;
  }
  return 0;
}

String RepresentationItems::sHatch(long iRaw) const {
  if (iUNDEF == iRaw)
    return sUNDEF;
  else if (colHatching.fValid()) {
    String sH = colHatching->sValue(iRaw);
	sH = sH.sTrimSpaces();
    if (sH != sUNDEF)
      return sH;
  }
  return sUNDEF;
}

double RepresentationItems::rTransparencyItem(long iRaw) const{
  if (iUNDEF == iRaw)
    return 0;
  else if (colTransparency.fValid()) {
    double rT = colTransparency->rValue(iRaw);
    if (rT != rUNDEF)
      return rT;
  }
  return 1.0;
}

int RepresentationItems::iColor(double rValue) const  // 0..1 
{
  if (!colColor.fValid())
    return 0;
  long iMax = colColor->iRecs();
  rValue *= iMax;
  int iVal = (int)(floor(rValue));
  if (iVal < 0)
    iVal = 0;
  else if (iVal >= iMax) 
    iVal = iMax - 1;
  iVal += colColor->iOffset();
  return iVal;  
}

short RepresentationItems::iLine(long iRaw) const
{
  if (iUNDEF == iRaw || !colLineType.fValid())
    return 1; // ldtSingle
  short iVal = shortConv(colLineType->iValue(iRaw));
  if (iVal < 0)
    return 1;
  return iVal;  
}

Color RepresentationItems::clrLineFill(long iRaw) const
{
  if (iUNDEF == iRaw)
    return Color();
  else if (colLineClrFill.fValid()) {
    long iRet = colLineClrFill->iRaw(iRaw);
    if (iRet != iUNDEF)
      return iRet;
  }  
  return Color(-1);  // support line off.
}

double RepresentationItems::rLineWidth(long iRaw) const
{
  if (iUNDEF == iRaw)
    return 0;
  else if (colLineWidth.fValid())
    return colLineWidth->rValue(iRaw);
  return rDefaultLineWidth;
}

double RepresentationItems::rLineDist(long iRaw) const
{
  if (iUNDEF == iRaw)
    return 0;
  else if (colLineDist.fValid())
    return colLineDist->rValue(iRaw);
  return 5;
}

void RepresentationItems::getProperties(long iRaw, RepresentationProperties& props) {
	props.linewidth = rLineWidth(iRaw);
	props.lineType = iLine(iRaw); 
	props.itemTransparency = rTransparencyItem(iRaw);
	props.symbolSize = iSymbolSize(iRaw);
	props.symbolType = sSymbolType(iRaw);
	props.hatchName = sHatch(iRaw);
}






