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
/* Interface for FieldSemiVariogram
   by Wim Koolhoven, march 1998
   (c) Ilwis System Development ITC
	Last change:  WK    8 Sep 98   12:17 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldsmv.h"
#include "Headers\Hs\Table.hs"

const int iSMVS = 7;

class _export FieldSemiVarModelSimple: public FieldOneSelect
{
public:
  FieldSemiVarModelSimple(FormEntry* par, SemiVariogramModel** smvm)
  : FieldOneSelect(par, (long*)smvm, true)
  {
    svmod = *smvm;
    SetWidth(75);
    int i = 0;
    mt[i].svm = svmSPHERICAL;
    mt[i++].sName = TR("Spherical Model");
    mt[i].svm = svmEXPONENTIAL;
    mt[i++].sName = TR("Exponential Model");
    mt[i].svm = svmGAUSSIAN;
    mt[i++].sName = TR("Gaussian Model");
    mt[i].svm = svmWAVE;
    mt[i++].sName = TR("Wave Model");
    mt[i].svm = svmCIRCULAR;
    mt[i++].sName = TR("Circular Model");
    mt[i].svm = svmPOWER;
    mt[i++].sName = TR("Power Model");
    mt[i].svm = svmRATQUAD;
    mt[i++].sName = TR("Rational Quadratic Model");
  }
  void create() {
    FieldOneSelect::create();
    for (int i = 0; i < iSMVS; ++i) {
      String s("%S.svm", mt[i].sName);
      int id = ose->AddString(s.c_str());
      if (*svmod == mt[i].svm)
        ose->SetCurSel(id);
      ose->SetItemData(id, (long)&mt[i].svm);
    }
  }
private:
  struct SmvStruct {
    SemiVariogramModel svm;
    String sName;
  };
  SmvStruct mt[iSMVS];
  SemiVariogramModel* svmod;
};

class FieldSemiVarModel: public FieldGroup
{
public:
  FieldSemiVarModel(FormEntry* parent, const String& sQuestion, SemiVariogramModel** smvm)
  : FieldGroup(parent)
  {
    StaticTextSimple* st = 0;
    if (sQuestion.length() != 0)
      st = new StaticTextSimple(this, sQuestion);
    fsvms = new FieldSemiVarModelSimple(this, smvm);
    if (st)
      fsvms->Align(st, AL_AFTER);
  }
  void SetCallBack(NotifyProc np, CallBackHandler* cb)
  { 
		FieldGroup::SetCallBack(np, cb);
		fsvms->SetCallBack(np,cb); 
	}
private:
  FieldSemiVarModelSimple* fsvms;
};

FieldSemiVariogram::FieldSemiVariogram(FormEntry* parent, const String& sQuestion,
     SemiVariogram* semivar)
: FieldGroup(parent), smv(semivar)
{
  svm = &smv->svm;
  fsvm = new FieldSemiVarModel(this, sQuestion, &svm);
  fsvm->SetCallBack((NotifyProc)&FieldSemiVariogram::CallBack, this);
  frNugget =new FieldReal(this, TR("&Nugget"), &smv->rNugget); //, ValueRange(0,1e20,0));
  frNugget->Align(fsvm, AL_UNDER);
  frSill = new FieldReal(this, TR("&Sill"), &smv->rSill, ValueRange(0,1e20,0));
  frRange = new FieldReal(this, TR("&Range"), &smv->rRange); //, ValueRange(0,1e20,0));
  // power model:
  frSlope = new FieldReal(this, TR("&Slope"), &smv->rSlope); //, ValueRange(0,1e20,0));
  frSlope->Align(frNugget, AL_UNDER);
  frPower = new FieldReal(this, TR("&Power"), &smv->rPower); // ValueRange(0,10,0));
}

int FieldSemiVariogram::CallBack(Event*)
{
  fsvm->StoreData();
  switch (*svm) {
    case svmSPHERICAL:
    case svmEXPONENTIAL:
    case svmGAUSSIAN:
    case svmWAVE:
    case svmCIRCULAR:
    case svmRATQUAD:
      frSill->Show();
			frSill->CallCallBacks();
      frRange->Show();
      frSlope->Hide();
      frPower->Hide();
      break;
    case svmPOWER:
      frSill->Hide();
      frRange->Hide();
      frSlope->Show();
      frPower->Show();
      break;
  }
  return 1;
}

void FieldSemiVariogram::StoreData()
{
  FieldGroup::StoreData();
  smv->svm = *svm;
}


FieldSemiVariogramList::FieldSemiVariogramList(FormEntry* parent,
                         int iNr, const String* sQuestion, SemiVariogram* semivar)
: FieldGroup(parent), iNrSmv(iNr), smv(semivar)
{
  svm = new SemiVariogramModel*[iNrSmv];
  for (int i = 0; i < iNrSmv; ++i)
    svm[i] = &smv[i].svm;
  StaticText* st = new StaticText(this, " ");
  FormEntry* feLast = st;
  FieldBlank* fb = new FieldBlank(this);
  fb->Align(st, AL_AFTER);
  StaticText* st1 = new StaticText(this, TR("Nugget"));
  st1->Align(fb, AL_AFTER);
  StaticText* st2 = new StaticText(this, TR("Sill/Slope"));
  st2->Align(st1, AL_AFTER);
  StaticText* st3 = new StaticText(this, TR("Range/Power"));
  st3->Align(st2, AL_AFTER);

  for (int i = 0; i < iNrSmv; ++i) {
    FieldSemiVarModel* fsvm = new FieldSemiVarModel(this, sQuestion[i], &svm[i]);
    fsvm->Align(feLast, AL_UNDER);
    feLast = fsvm;
    FieldReal* frNugget =new FieldReal(this, "", &smv[i].rNugget);
    frNugget->Align(fsvm, AL_AFTER);
    FieldReal* frSill = new FieldReal(this, "", &smv[i].rSill);
    frSill->Align(frNugget, AL_AFTER);
    FieldReal* frRange = new FieldReal(this, "", &smv[i].rRange, ValueRange(0,1e20,0));
    frRange->Align(frSill, AL_AFTER);
  }
}

FieldSemiVariogramList::~FieldSemiVariogramList()
{
  delete [] svm;
}

void FieldSemiVariogramList::StoreData()
{
  FieldGroup::StoreData();
  for (int i = 0; i < iNrSmv; ++i) {
    smv[i].svm = *svm[i];
    smv[i].rSlope = smv[i].rSill;
    smv[i].rPower = smv[i].rRange;
  }
}



