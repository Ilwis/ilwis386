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
/* Digitizer
   by Wim Koolhoven
   (c) ILWIS System Department ITC
	Last change:  WK    3 Mar 97    3:01 pm
*/
#ifndef DIGITIZER_H
#define DIGITIZER_H

#define DIGBUFSIZE 256

class Digitizer;
class DigitizerWizard;
class FormUsingDigitizer;

class _export Digitizer : public CWnd
{
	friend class DigSetPortForm;
  friend class MapReferenceForm;
  friend class DigAutoForm;
  friend class DigConfigForm;
  friend class DigConfigSizeForm;
	friend class DigPortSettings;
	friend UINT ProcessRecordsInThread(LPVOID lp);
	friend UINT ComPortReadThread(LPVOID lp );

public:
  enum ACT { actPASSIVE, actACTIVE, actREFERENCING, actSETUP, actBUSY } act;

  Digitizer(CWnd*);
  ~Digitizer();
  int                   Configure(Event*);
  int                   Reference(Event*);
  void                  Activate();
  void                  Enable(bool fReportErrors=true, bool fDoRefCount=true);
  void                  Disable(bool fDoRefCount=true);
  bool                  fActive();
	bool                  fValid();
	bool                  fUsesWinTab();
  bool                  fMapRef;
  bool                  fSetup;
  float                 rUnit() const { return rUnitSize; }
  void                  ActStore();
	bool									fSetupAllowed();
	bool									fMapReferenceAllowed();
  FormUsingDigitizer*   mrf;

private: 
  void                  OpenComPort(const String& sPort);
  void                  Load();
  void                  Store();
  void                  RefLoad();
  void                  RefStore();
  void                  Interpret(const char*);  // sets iCurrX, iCurrY, iButton
  void                  SendInfo();
  void                  TimerFunc();

  double                x0, y0, x10, x01, y10, y01;
  int                   iErr;
	DigitizerWizard* digwiz;
  bool                  fAffine;
  CoordWithCoordSystem  cwcs;
  void                  ProcessRecord(string sRecord);
  LRESULT                   OnReceiveWTPacket(WPARAM wParm, LPARAM lParm);
  LRESULT                   OnCloseWTContext(WPARAM wParm, LPARAM lParm);
  LRESULT                   OnOverlapWTContext(WPARAM wParm, LPARAM lParm);
  LRESULT                   OnChangeWTContext(WPARAM wParm, LPARAM lParm);
  bool                  fWinTab;
  HANDLE                hTab;
  HANDLE                hComPort;
  bool                  fKillThreads;
  CCriticalSection      guard;
  DCB                   dcb;
  CWnd*                 wPar;
	int iBaudRate, iByteSize, iStopBits, iParity;
  int                   iPort, iMaxX, iMaxY, iBufPos,
                        iNrChars, iFlagPos, iXPos, iXLen, iYPos, iYLen;
  float                 rUnitSize;
  char                  sCodes[5];
  char                  sBuf[DIGBUFSIZE];
  double                rCurrX, rCurrY;
  int                   iButton, iDummy;
	int										iRefCount;

	CWinThread* threadDig;
	DECLARE_MESSAGE_MAP()
};


#endif // DIGITIZER_H
