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
/* Interface for FieldJustification
   by Wim Koolhoven, january 1997
   (c) Ilwis System Development ITC
	Last change:  WK   12 Jun 98    4:05 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldjust.h"

FieldJustification::FieldJustification(FormEntry* fe, const String& sQuestion, int* iJust)
: FieldGroup(fe, true), iJustification(iJust)
{
  iLeftRight = iTopBottom = 1;
  switch (*iJustification) {
    case 1: iLeftRight = 0; iTopBottom = 2; break;
    case 2: iLeftRight = 0; iTopBottom = 1; break;
    case 3: iLeftRight = 0; iTopBottom = 0; break;
    case 4: iLeftRight = 1; iTopBottom = 2; break;
    case 5: iLeftRight = 1; iTopBottom = 1; break;
    case 6: iLeftRight = 1; iTopBottom = 0; break;
    case 7: iLeftRight = 2; iTopBottom = 2; break;
    case 8: iLeftRight = 2; iTopBottom = 1; break;
    case 9: iLeftRight = 2; iTopBottom = 0; break;
  }
  StaticText* st = new StaticText(this, sQuestion);
  st->SetIndependentPos();
  RadioGroup* rgTB = new RadioGroup(this, "", &iTopBottom, true);
  new RadioButton(rgTB, SDUIJstTop);
  new RadioButton(rgTB, SDUIJstBaseline);
  new RadioButton(rgTB, SDUIJstBottom);
  RadioGroup* rgLR = new RadioGroup(this, "", &iLeftRight, true);
  new RadioButton(rgLR, SDUIJstLeft);
  new RadioButton(rgLR, SDUIJstCenter);
  new RadioButton(rgLR, SDUIJstRight);
}

void FieldJustification::StoreData()
{
  FieldGroup::StoreData();
  *iJustification = 3 * iLeftRight + 3 - iTopBottom;
}





