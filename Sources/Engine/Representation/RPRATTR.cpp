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
/* RepresentationAttribute
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK   24 Apr 96   11:31 am
*/

#include "Engine\Representation\RPRATTR.H"

RepresentationAttribute::RepresentationAttribute(const FileName& fn)
: RepresentationItems(fn)
{
}

RepresentationAttribute::RepresentationAttribute(const Representation& rpr, const Table& table)
: RepresentationItems(rpr->fnObj), tbl(table)
{
  String sColor, sWidth, sPattern, sSmbType, sSmbSize, sSmbWidth, sSmbClr, sSmbFC;
  if (ReadElement("RepresentationAttribute", "Color", sColor))
    colColor = Column(tbl, "Color", sColor);
  //if (ReadElement("RepresentationAttribute", "Width", sWidth))
  //  colWidth = Column(tbl, "Width", sWidth);
  if (ReadElement("RepresentationAttribute", "Pattern", sPattern))
    colPattern = Column(tbl, "Pattern", sPattern);
  if (ReadElement("RepresentationAttribute", "SymbolType", sSmbType))
    colSmbType = Column(tbl, "SymbolType", sSmbType);
  if (ReadElement("RepresentationAttribute", "SymbolSize", sSmbSize))
    colSmbSize = Column(tbl, "SymbolSize", sSmbSize);
  if (ReadElement("RepresentationAttribute", "SymbolWidth", sSmbWidth))
    colSmbWidth = Column(tbl, "SymbolWidth", sSmbWidth);
  if (ReadElement("RepresentationAttribute", "SymbolColor", sSmbClr))
    colSmbClr = Column(tbl, "SymbolColor", sSmbClr);
  if (ReadElement("RepresentationAttribute", "SymbolFillColor", sSmbFC))
    colSmbFC = Column(tbl, "SymbolFillColor", sSmbFC);
}

void RepresentationAttribute::Store()
{
  RepresentationItems::Store();
  WriteElement("Representation", "Type", "RepresentationAttribute");
}

String RepresentationAttribute::sType() const
{
  return "Representation Attribute";
}

RepresentationAttribute::~RepresentationAttribute()
{
}






