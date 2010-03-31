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
/* $Log: /ILWIS 3.0/FormElements/fentvalr.cpp $
 * 
 * 10    2/22/02 12:20p Martin
 * FieldValueRange did not put its show state correctly when it was turned
 * on. Its elements (valuerange, stepsize) showed correctly but the whole
 * formelement was "hidden". That meant that storedata was not called
 * 
 * 9     26-09-01 19:28 Koolhoven
 * made Domain dm a member, because otherwise the pointer dv becomes
 * invalid
 * 
 * 8     17-08-01 15:11 Koolhoven
 * take fFixedRange() and fFixedPrecision() of domain into account by
 * making these fields read only
 * 
 * 7     3-08-00 13:07 Koolhoven
 * protect against sDom is empty in DomainCallBack()
 * 
 * 6     27/07/00 11:58 Willem
 * Added Enable() and Disable() functions for FieldRangeInt,
 * FieldRangeReal and FieldValueRange
 * 
 * 5     18-10-99 3:44p Martin
 * Visibility is now default off unless turned on.
 * 
 * 4     11-10-99 11:34a Martin
 * valueranges become visible when needed
 * 
 * 3     3-09-99 11:57a Martin
 * //-> /*
 * 
 * 2     3-09-99 11:21a Martin
 * Callback changed, must now check if something may be shown
 * (fieldValueRange)
// Revision 1.13  1998/10/09 16:19:33  Wim
// Changed sequence of include files to prevent problems
//
// Revision 1.12  1998-10-07 10:14:02+01  Wim
// Step size of precision in FieldValueRange is now never larger than the precision,
// this caused roundign to 0.000 in the display.
//
// Revision 1.11  1998-09-16 18:37:53+01  Wim
// 22beta2
//
// Revision 1.10  1997/09/22 12:10:42  Wim
// Corrected FieldValueRange::SetVal() so that rng is only changed
// by rStep when needed and not always
//
// Revision 1.9  1997-09-12 17:05:05+02  Wim
// Correct rng in SetVal() for precision to prevent making the range smaller
//
// Revision 1.8  1997-09-10 18:52:59+02  Wim
// Hide on a domain bool or image
//
// Revision 1.7  1997-09-01 17:39:37+02  Wim
// SetVal(rng) should never put larger range than -1e100:+1e100
//
// Revision 1.6  1997-08-28 20:40:41+02  Wim
// Set precision of stepsize when valuerange is set on itself.
// With limitation 1e-6 and 0.1
//
// Revision 1.5  1997-08-13 17:44:35+02  Wim
// Corrected SetCallBack() functions so that step is included
//
// Revision 1.4  1997-08-04 12:11:42+02  Wim
// Precision now again allows decimals behind the point
//
// Revision 1.3  1997-07-28 15:26:39+02  Wim
// Precision only allows positive numbers now
//
// Revision 1.2  1997/07/24 16:03:53  Wim
// ShowHide() function added, same as DomainCallBack() but no SetVal()
// called for vr.
//
/* ui/fentvalr.c
   ValueRangeReal formentries
   by Wim Koolhoven, oct 1996
   (c) Ilwis System Development ITC
	Last change:  WK    9 Oct 98    5:19 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fentvalr.h"
#include "Engine\Domain\Dmvalue.h"

FieldValueRange::FieldValueRange(FormEntry* parent, const String& sQuestion,
       ValueRange *rng, FieldDataType* _fdc, const ValueRange& vrRange)
: FieldGroup(parent), vr(rng), fdc(_fdc),
  frr(0), dv(0)
{
  if ((*vr).fValid()) {  
    rStep = (*vr)->rStep();
    rr = (*vr)->rrMinMax();
  }  
  frr = new FieldRangeReal(this, (sQuestion != " " ? sQuestion : SUIUiValRange), &rr, vrRange);
  if (0 != fdc)
    frr->Align(fdc, AL_UNDER);
  frStep = new FieldReal(this, SUIUiStepSize, &rStep, ValueRange(0,1e9,0));
  if (rStep < 0.001)
    frStep->SetStepSize(rStep); // prevent rounding to 0 in display!
  frStep->Align(frr, AL_UNDER);  
  frStep->SetCallBack((NotifyProc)&FieldValueRange::StepChangeCallBack,this);
}       

int FieldValueRange::StepChangeCallBack(void *)
{
	if (!rr.fValid())
		return 0;
  bool fShowFrr = frr->fShow();
  frStep->StoreData();
  frr->SetStepSize(rStep);
  return 0;  
}

int FieldValueRange::DomainCallBack(Event *)
{
  frr->Hide();
  frStep->Hide();
  fdc->StoreData();
  String sDom = fdc->sName();
	if ("" == sDom)
		return 0;
  try {
    dm = Domain(sDom);
  }
  catch (const ErrorObject&) {
    return 0;
  }
  if (!dm.fValid())
    return 0;

  dv = dm->pdv();
  if (0 == dv)
    return 0;

  ValueRange vr(dm);
  if (!vr.fValid())
    return 0;

	Show();
  RangeReal rr = vr->rrMinMax();
  double rStep = vr->rStep();
  frr->SetVal(rr);    
  frr->SetReadOnly(dv->fFixedRange());
  frStep->SetVal(rStep);
  frStep->SetReadOnly(dv->fFixedPrecision());

  return 0;  
}

void FieldValueRange::Show()
{
	FieldGroup::Show();
	if ( frStep)
		frStep->Show();
	if ( frr)
		frr->Show();	
}
void FieldValueRange::SetVal(const ValueRange& rng)
{ 
  if (!rng.fValid())
    return;
  double rStep = rng->rStep();
  RangeReal rr = rng->rrMinMax();

  if (dv) {
    if (dv->fFixedPrecision())
      rStep = dv->rStep();
    if (dv->fFixedRange())
      rr = dv->rrMinMax();
  }  

  double r = rStep;
  if ((r > 0.1) && (floor(r)+1e-6 > r))
    r = 0.1;
  else if (r < 1e-6)
    r = 1e-6;
  frStep->SetStepSize(r);
  frStep->SetVal(rStep);
  frr->SetStepSize(rStep);
  if (rr.rLo() < -1e100)
    rr.rLo() = -1e100;
  if (rr.rHi() > 1e100)
    rr.rHi() = 1e100;

  if (rStep < 1e-5)
    r = 1e-3;

  double rFrac = rr.rLo();
  rFrac -= floor(rFrac);
  rFrac /= r;
  if (rFrac > round(rFrac))
    rr.rLo() -= r;
  rFrac = rr.rHi();
  rFrac -= floor(rFrac);
  rFrac /= r;
  if (rFrac > round(rFrac))
    rr.rHi() += r;

  frr->SetVal(rr);
}

void FieldValueRange::StoreData()
{
  frStep->StoreData();
  frr->StoreData();
  (*vr) = ValueRange(rr.rLo(), rr.rHi(), rStep);
  (*vr)->AdjustRangeToStep();
}

void FieldValueRange::Align(FormEntry* fe, enumAlign al, int iOffSet)
{
  frr->Align(fe, al, iOffSet);
}

void FieldValueRange::Enable()
{
	frr->Enable();
	frStep->Enable();
}

void FieldValueRange::Disable()
{
	frr->Disable();
	frStep->Disable();
}

