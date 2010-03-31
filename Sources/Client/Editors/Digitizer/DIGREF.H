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
/* digref.h
// FieldRefPoint
// MapReferenceForm
// by Wim Koolhoven
// (c) ILWIS Department ITC
	Last change:  WK    3 Mar 97    2:28 pm
*/
#ifndef DIGREF_H
#define DIGREF_H
#include "Client\Editors\Digitizer\DIGINFO.H"
#include "Client\Editors\Digitizer\DIGTRAN.H"

class _export FormUsingDigitizer : public FormWithDest
{
protected:
  FormUsingDigitizer(CWnd*, Digitizer*, const String& sTitle, bool fModal=true);
public:  
  ~FormUsingDigitizer();
  void create() { FormWithDest::create(); }
  DigitizerInfoWindow* inf() { return info; }
  void ProcessInfo(double rX, double rY, int iButton);
  bool fOk;
protected:  
  virtual void HandleInfo()=0; // called by ProcessInfo
	virtual void OnOK();		 // OnOK and OnCancel clean up info before FormUsingDigitizer
	virtual void OnCancel(); // disappears from the screen otherwise white area remains (b3376)
  double rCurrX, rCurrY;
  int iButton;
  Digitizer* dig;
  DigitizerInfoWindow *info;
};

class _export FieldRefPoint;

class _export MapReferenceForm : public FormUsingDigitizer
{
public:
  MapReferenceForm(CWnd*, Digitizer*, bool fMetric);
  ~MapReferenceForm();
  FieldRefPoint* frpCurr() const { return frp[iActDigPnt]; }
  FieldRefPoint* frpNext();
  int calc(Event*);
  double x0, y0, x10, x01, y10, y01;
	afx_msg HBRUSH OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor);
  int iAffine;
protected:  
  virtual void HandleInfo();
private:
  int initMRF(Event*);
  StaticText *stSigma, *stScale, *stRot;
  RadioGroup *rgAffine;
  FieldRefPoint *frp[6];
  int iActDigPnt;
  DigTransform dtf;
  Color clrNormal, clrActive, clrBackNormal, clrBackActive;
  CBrush *brNormal, *brActive;
  DECLARE_MESSAGE_MAP()
};

class _export FieldRefPoint: public FieldGroup
{
  friend MapReferenceForm::MapReferenceForm(CWnd* par, Digitizer* dg, bool fMetric);
public:
  FieldRefPoint(FormEntry* par, int iNr, bool fMetric, const CoordSystem&);
  virtual FormEntry* CheckData();            
  void Store(int iNr);
  void ShowDig();
  void Activate();
  void ShowVals();
  void ReadCoord();
  Coord coord;
  double rDigX, rDigY;
  double rDX, rDY;
  bool fActDig, fDig, fOk;
  bool fDigHandle(HANDLE);
private:
  void Load(int iNr);
  FormEntry *fCrd;
  StaticTextSimple *stDigX, *stDigY, *stDX, *stDY;
  String sDigX, sDigY, sDX, sDY;
};



#endif // DIGREF_H
