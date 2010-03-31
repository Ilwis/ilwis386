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
#ifndef DIGCONF_H
#define DIGCONF_H

#if !defined(AFX_DIGITIZERWIZARD_H__5A0F82E6_32DA_11D4_B8BA_00A0C9D5342F__INCLUDED_)
#include "Client\Editors\Digitizer\DigitizerWizard.h"
#endif

class DigSetPortForm : public DigitizerWizardPage
{
public:
	DigSetPortForm(DigitizerWizard* dw, Digitizer* dig);
private:
	int CallBack(Event*);
	int exec();
	RadioGroup* rg;
	RadioButton* rbWinTab;
	int iPrt;
};

class DigPortSettings : public DigitizerWizardPage
{
public:
	DigPortSettings(DigitizerWizard* dw, Digitizer* dig);
	LRESULT OnWizardNext();

private:
  int FillAll(Event *);
	int exec();

	FieldOneSelectTextOnly *foData;
	FieldOneSelectTextOnly *foStop;
	FieldOneSelectTextOnly *foBaud;
	FieldOneSelectTextOnly *foParity;

  String sBaud, sData, sParity, sStopBits;
	bool fAdded;
};

class DigAutoForm: public DigitizerWizardPage
{
public:
  DigAutoForm(DigitizerWizard* dw, Digitizer* dig);

	void Interpret(const char* s);
	virtual BOOL OnSetActive();
	int Manual(Event*);

  bool fManual; 

private:
  StaticText *stReceive, *stMessage;
  enum { stINIT, stSTART, stBUTTON } status;
  int iCnt, iLength, iFlagPos;
  char cNone;
  char sCodes[4];	
};

class DigConfigForm: public DigitizerWizardPage
{
public:
  DigConfigForm(DigitizerWizard* dw, Digitizer* dig);

  void Interpret(const char* s);
	int CallBack(Event*);

private:
  int exec(); 
  String sCodes; 
  StaticText* stReceive;
};

class DigConfigSizeForm: public DigitizerWizardPage
{
public:
  DigConfigSizeForm(DigitizerWizard* dw, Digitizer* dig);  
  void Interpret(const char* s);
	virtual BOOL OnSetActive();
private:
  int exec(); 
  int CallBack(Event*);
  int MetricCallBack(Event*);
  bool fMetric;
  double rXSizeM, rYSizeM, rUnitSizeM, rXSizeInch, rYSizeInch, rUnitSizeInch;
  CheckBox* cbMetric;
  FieldReal *frXM, *frYM, *frUM, *frXI, *frYI, *frUI;
  StaticText *stX, *stY, *stButton, *stXmm, *stYmm;
};

#endif
