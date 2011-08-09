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
/* $Log: /ILWIS 3.0/Digitizer/DIGITIZR.cpp $
 * 
 * 38    25-01-02 9:27 Retsios
 * Now the behavior of the destructor is what it should be: clean up the
 * COM port anyway.
 * 
 * 37    24-01-02 19:28 Retsios
 * Added reference counter to keep track of digitizer use, and also a
 * parameter to Enable() and Disable() when needed to bypass checks on the
 * reference counter (the Digitizer Setup wizard needs this)
 * 
 * 36    18-01-02 18:41 Retsios
 * Configure and Reference preserve the previous digitizer situation
 * instead of the default "enable at completion". The digieditor takes
 * care of enabling / disabling the digitizer on demand, so that the COM
 * port is not occupied all the time.
 * 
 * 35    4-12-01 16:53 Retsios
 * Prevent unknown error due to division by zero when using wintab and
 * there is (physically) no digitizer.
 * 
 * 34    26-11-01 16:06 Retsios
 * Set act to passive if WinTab is used and tablet is disconnected
 * 
 * 33    22-11-01 17:18 Retsios
 * fSetup is now also true when using WinTab (I see no reason to
 * distinguish it in this boolean), herewith solving bug#5412: WinTab
 * doesn't work
 * 
 * 32    4/06/01 20:37 Retsios
 * Improved robuustness by adding try/catch blocks around digitizer
 * settings
 * 
 * 31    3/15/01 16:07 Retsios
 * a) Registry key for digitizer configuration is now public
 * read/writable.
 * b) Added robustness to the check if the digitizer retup is allowed.
 * 
 * 30    3/12/01 19:14 Retsios
 * Added member functions that check if digitizer setup / mapreferencing
 * is allowed
 * Ensure correct behavior of Load(), Store(), RefLoad(), Configure(),
 * Reference()
 * Reference() hasn't been tested with a digitizer yet.
 * 
 * 29    1-03-01 18:16 Koolhoven
 * removed limitation on type of coordsystem in StartDigRefForm because
 * for some special purposes (monoplotting) special things have to be
 * possible
 * 
 * 28    10-11-00 17:14 Koolhoven
 * in StartDigRefForm only allow BoudnsOnly and Projection coordinate
 * system
 * 
 * 27    3-11-00 18:39 Koolhoven
 * reading from ilwis.ini now takes into account change of meaning of
 * positions (now starts counting at 1)
 * 
 * 26    3-11-00 18:29 Koolhoven
 * default settings when reading from config file are now the same as from
 * reading from registry
 * 
 * 25    3-10-00 19:26 Koolhoven
 * set fSetup on true at the end of Configure()
 * 
 * 24    3-10-00 15:03 Koolhoven
 * changed caption of MapReference forms
 * 
 * 23    8/30/00 9:20a Martin
 * added access function to check if wintab is activated
 * 
 * 22    28-07-00 11:06 Koolhoven
 * corrected defaults for port settings
 * 
 * 21    27-07-00 18:19 Koolhoven
 * layout improvements, text changes, error handling improved
 * 
 * 20    29-05-00 15:25 Koolhoven
 * The digitizer setup is now a wizard instead of a sequence of dialog
 * boxes
 * 
 * 19    22-05-00 3:03p Martin
 * different order in the messagebox with the properties of the com port
 * 
 * 18    26-04-00 18:39 Koolhoven
 * registry storage of metric/degrees corrected
 * Parent of forms is now window of current thread
 * 
 * 17    25-04-00 16:20 Koolhoven
 * Use registry settings instead of ilwis.ini file
 * 
 * 16    25-04-00 14:01 Koolhoven
 * Some prevents against deadlocks
 * 
 * 15    25-04-00 10:41 Koolhoven
 * set dcf on 0 before it is deleted
 * 
 * 14    25-04-00 10:24 Koolhoven
 * Added Sleep(1) to prevent deadlocks
 * 
 * 13    7-02-00 9:38a Martin
 * prevented empty record checking
 * 
 * 12    28-01-00 11:06a Martin
 * forgot to read databits outof the reigstry
 * 
 * 11    14-12-99 9:19a Martin
 * changed locking of digitizer object
 * 
 * 10    29-11-99 9:01a Martin
 * clarified error message(s)
 * 
 * 9     11/25/99 3:38p Martin
 * port number corrected
 * 
 * 8     25-11-99 10:31a Martin
 * 
 * 7     19-11-99 11:17a Martin
 * changed the opening sequence of ports, the port is now opened once and
 * changed according to settings. The config form loop has changed to a
 * regeular loop instead of a goto
 * 
 * 6     23-09-99 8:11a Martin
 * misspelled 'none'
 * 
 * 5     6-09-99 12:24p Martin
 * Wintab now works
 * 
 * 4     1-09-99 11:13a Martin
 * savedCOM port  settings to registry
 * 
 * 3     27-08-99 10:27a Martin
 * 
 * 2     25-08-99 12:56p Martin
 * digitizer setup and mapreferencing functions
/* 
 * 1     20-08-99 11:07a Martin
// Revision 1.6  1998/09/16 17:36:10  Wim
// 22beta2
//
// Revision 1.5  1998/03/19 10:09:45  Wim
// Digitizer::Configure() now initializes iPrt properly.
// (old code seemed to be not incorrect, but did not work properly.
//
// Revision 1.4  1997-09-29 12:32:21+01  martin
// Store() is now correctly called in the configure function.
//
// Revision 1.3  1997/09/29 10:53:08  martin
// value of strcmp for the detection of affine mapreference correctly transformed to bool
//
// Revision 1.2  1997/08/25 08:16:56  Wim
// Store WinTab setting
//
/* Digitizer
// by Wim Koolhoven
// (c) ILWIS System Development ITC
	Last change:  WK   19 Mar 98   11:06 am
*/
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldcs.h"
#include "Client\ilwis.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Client\Editors\Digitizer\DIGREF.H"
#include "Client\Editors\Digitizer\DigitizerWizard.h"
#include "Client\Editors\Digitizer\DIGREF.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Hs\Digitizr.hs"
#include "Client\Editors\Digitizer\WINTAB.H"
#define PACKETDATA (PK_X | PK_Y | PK_BUTTONS)
#define PACKETMODE 0
#include "Client\Editors\Digitizer\PKTDEF.H"

const static int iMAX_BYTES_READ = 100;

UINT ComPortReadThread(LPVOID lp )
{
	char chRead, sCoord[iMAX_BYTES_READ];
	Digitizer *dig = (Digitizer *) lp;
	DWORD dwRead;

	int i=0;
	char cXOff = 0x13;
 	int j=0;

	for ( ; !dig->fKillThreads; ) 
	{
		long iRet = ReadFile(dig->hComPort, &chRead, 1, &dwRead, NULL);
		if ( iRet != 0 )
		{
				if ( dwRead != 1 ) 
        {
					Sleep(1);
					continue;
				}
				if ( chRead == 10 || chRead == 13 )
				{
					if ( j++ != 0 ) // first record ignores, contains extra info
          {
						sCoord[i]=0;
						if (i != 0 ) // no empty record
							dig->ProcessRecord(sCoord);
          } 
					i=0;
				}
				else
				{
					if ( i < iMAX_BYTES_READ)
						sCoord[i++] = chRead;
					else
						i=0; // try again
				}
		}
	}
	return 1;
}

//static InfoLine* infDig = 0;

BEGIN_MESSAGE_MAP( Digitizer, CWnd )
	//{{AFX_MSG_MAP( Digitizer )
	ON_MESSAGE(WT_PACKET, OnReceiveWTPacket)
	ON_MESSAGE(WT_CTXCLOSE, OnCloseWTContext)
	ON_MESSAGE(WT_CTXOVERLAP, OnOverlapWTContext)
	ON_MESSAGE(WT_CTXUPDATE, OnChangeWTContext)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

Digitizer::Digitizer(CWnd* par)
: wPar(par), rCurrX(0), rCurrY(0),
  cwcs(crdUNDEF),
	fKillThreads(true), threadDig(0),
	hComPort(0),
	fSetup(false),
	fMapRef(false),
	fWinTab(false),
	mrf(NULL),
	digwiz(NULL),
	iRefCount(0)
{
	DWORD iRet = CWnd::Create(NULL, "Digitizer window", WS_CHILD , CRect(0,0,0,0), wPar, 1001);

	sCodes[4]=0;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(DCB);
  Load();
  //Store();
  act = actPASSIVE;
  if (fSetup)
    RefLoad();
}

Digitizer::~Digitizer()
{
  Disable(false); // Force the Disable(); actually no editors should be open - take care of it before deleting the Digitizer pointer, or re-count them in the constructor
  if (mrf) delete mrf;
  if (digwiz) delete digwiz;
//  if (infDig) delete infDig;
}

void Digitizer::Load()
{
	IlwisSettings settings("Digitizer\\Configuration", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADONLY);
	if (settings.fKeyExists())
  {

		iPort    = settings.iValue("CommPort"    , 0);
		iMaxX    = settings.iValue("TabletSizeX" , 4500);
		iMaxY    = settings.iValue("TabletSizeY" , 3500);
		iNrChars = settings.iValue("NrChars"     , 13 );
		iFlagPos = settings.iValue("FlagPos"     , 12);
		iXPos    = settings.iValue("XPos"        , 1);
		iXLen    = settings.iValue("XLen"        , 5);
		iYPos    = settings.iValue("YPos"        , 7);
		iYLen    = settings.iValue("YLen"        , 5);
		rUnitSize= settings.rValue("UnitSize"    , 1.0);
		fWinTab  = settings.fValue("WinTab"      , false);
		iBaudRate = settings.iValue("BaudRate", 9600);
		iByteSize = settings.iValue("DataBits", 8);
		iStopBits = settings.iValue("StopBits", 0);
		iParity = settings.iValue("Parity", 1);
		String sT = settings.sValue("Codes", "1248");
		strcpy(sCodes,sT.c_str());
	}
	else
  {
		iPort    = GetPrivateProfileInt("Digitizer","CommPort",0,"ilwis.ini");
		iMaxX    = GetPrivateProfileInt("Digitizer","TabletSizeX",4500,"ilwis.ini");
		iMaxY    = GetPrivateProfileInt("Digitizer","TabletSizeY",3000,"ilwis.ini");
		iNrChars = GetPrivateProfileInt("Digitizer","NrChars",13,"ilwis.ini");
		iFlagPos = GetPrivateProfileInt("Digitizer","FlagPos",11,"ilwis.ini") + 1;
		iXPos    = GetPrivateProfileInt("Digitizer","XPos",0,"ilwis.ini") + 1;
		iXLen    = GetPrivateProfileInt("Digitizer","XLen",5,"ilwis.ini");
		iYPos    = GetPrivateProfileInt("Digitizer","YPos",6,"ilwis.ini") + 1;
		iYLen    = GetPrivateProfileInt("Digitizer","YLen",5,"ilwis.ini");
		GetPrivateProfileString("Digitizer","Codes","1248",sCodes,5,"ilwis.ini");
		GetPrivateProfileString("Digitizer","UnitSize","1.0",sBuf,80,"ilwis.ini");
		rUnitSize = atof(sBuf);
		GetPrivateProfileString("Digitizer","WinTab","0",sBuf,80,"ilwis.ini");
		fWinTab = 0 != strcmp(sBuf, "0");
		iBaudRate = 9600;
		iByteSize = 8;
		iStopBits = 0;
		iParity = 0;
	}

	IlwisSettings settings2("Digitizer\\DigRef", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADONLY);
	fSetup = (iPort != 0 || fWinTab) && settings2.fKeyExists();
}

void Digitizer::Store()
{
	// As we arrived here, it means the user has sufficient rights to the Digitizer key (under normal circumstances).
	// Work around NT 4.0 problem: only creator can read his own keys
	// First remove the old key
	// IlwisSettings settings1("Digitizer", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADWRITE);
	// settings1.DeleteKey(); // Recursive => all is lost
	// NOTE!! As of 6 April 2001 the above is removed: it is only needed for some Ilwis 3.0 Beta testers that have
	// created their registry keys with the wrong rights
	
	// If the key does not exist, create it and give read-write access to yourself and read access to all.
	// Unfortunately it is not easy to use the ACL's, so we give read/write to all for now.
	
	try
	{
		IlwisSettings settings("Digitizer\\Configuration", IlwisSettings::pkuMACHINE, true, IlwisSettings::omREADWRITE, IlwisSettings::arALLUSERS);
		settings.SetValue("WinTab"       , fWinTab);
		settings.SetValue("CommPort"     , iPort);
		settings.SetValue("TabletSizeX"  , iMaxX );
		settings.SetValue("TabletSizeY"  , iMaxY);
		settings.SetValue("NrChars"      , iNrChars);
		settings.SetValue("FlagPos"      , iFlagPos);
		settings.SetValue("XPos"         , iXPos);
		settings.SetValue("XLen"         , iXLen);
		settings.SetValue("YPos"         , iYPos);
		settings.SetValue("YLen"         , iYLen);
		settings.SetValue("Codes"        , String(sCodes));
		settings.SetValue("UnitSize"     , rUnitSize);
		settings.SetValue("BaudRate"     , iBaudRate);
		settings.SetValue("DataBits"     , iByteSize);
		settings.SetValue("StopBits"     , iStopBits);
		settings.SetValue("Parity"       , iParity);
	}
	catch (RegistryError&)
	{
		// Will never occur under normal circumstances
		MessageBox("Unable to store digitizer settings");
	}
	// Remove old Digitizer\\DigRef key (and its access control list that we may not like)
	IlwisSettings settings3("Digitizer\\DigRef", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADWRITE);
	settings3.DeleteKey();
	// Make sure Digitizer\\DigRef exists and is public writable
	try
	{
		IlwisSettings settings2("Digitizer\\DigRef", IlwisSettings::pkuMACHINE, true, IlwisSettings::omREADWRITE, IlwisSettings::arALLUSERS);
	}
	catch (RegistryError&)
	{
		// MessageBox already given above; this is a similar "access rights" problem that should never occur
	}
}

void Digitizer::RefLoad()
{

	IlwisSettings settings("Digitizer\\DigRef", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADONLY);

	act            = (Digitizer::ACT)settings.iValue("Active"    , 0);
	fAffine        = settings.fValue("Affine"    , true);
  x0             = settings.rValue("x0"        , rUNDEF);
  y0             = settings.rValue("y0"        , rUNDEF);
  x01            = settings.rValue("x01"       , rUNDEF);
  y01            = settings.rValue("y01"       , rUNDEF);
  x10            = settings.rValue("x10"       , rUNDEF);
  y10            = settings.rValue("y10"       , rUNDEF);

	fMapRef = ( x0  != rUNDEF && y0 != rUNDEF && x01 != rUNDEF && y01 != rUNDEF && 
	            y10 != rUNDEF && x10!= rUNDEF ) ? true : false;

}

void Digitizer::ActStore()
{
	IlwisSettings settings("Digitizer\\DigRef", IlwisSettings::pkuMACHINE);

	settings.SetValue("Active", (act == actACTIVE) ? true : false );
}

void Digitizer::RefStore()
{
  ActStore();

	IlwisSettings settings("Digitizer\\DigRef", IlwisSettings::pkuMACHINE);

	settings.SetValue("Affine", fAffine);
	settings.SetValue("x0"    , x0);
	settings.SetValue("y0"    , y0);
	settings.SetValue("x01"   , x01);
	settings.SetValue("y01"   , y01);
	settings.SetValue("x10"   , x10);
	settings.SetValue("y10"   , y10);

}

void Digitizer::ProcessRecord(string sRecord)
{

	switch ( act)
  {
		case actACTIVE:
			{
				double rOldX = rCurrX;
        double rOldY = rCurrY;
				int iOldButton = iButton;
				Interpret(sRecord.c_str());
				if (rCurrX != rOldX || rCurrY != rOldY || iOldButton != iButton)
					SendInfo();
			}
			break;
    case actSETUP:
			{
				if (digwiz)
					digwiz->SendMessage(ILWM_SENDDIGSTRING, 0, (LPARAM)sRecord.c_str());
			}
			break;
    case actREFERENCING:
			Interpret(sRecord.c_str());
			SendInfo();
			break;
	}
}

void Digitizer::Activate()
{
  if (fWinTab && 0 != hTab)
  {
  //WTOverlap((HCTX)hTab, TRUE); // to top
  }
}

void Digitizer::Enable(bool fReportErrors, bool fDoRefCount)
{
	if (fDoRefCount) // The setup wizard is allowed to proceed (will come with fDoRefCount=false)
	{
		++iRefCount;					// increase refcounter
		if (act != actPASSIVE) // if already enabled, simply return, otherwise attempt enabling
			return;					// also ignore the call if we're busy in the Setup or Reference Wizard (and e.g. we opened a DigiEditor)
	}
  if (fWinTab) 
	{
    LOGCONTEXT lc;
    WTInfo(WTI_DEFCONTEXT, 0, &lc);
    strcpy(lc.lcName, "ILWIS Digitizer");
    lc.lcOptions = CXO_MESSAGES;
    lc.lcPktData = PACKETDATA;
    lc.lcPktMode = PACKETMODE;
    lc.lcMoveMask = PACKETDATA;
    lc.lcOutOrgX = 0;
    lc.lcOutOrgY = 0;
    
    AXIS axX, axY;
		bool faxXRead, faxYRead;
    UINT wDevice;
    FIX32 fx;
    WTInfo(WTI_DEFCONTEXT, CTX_DEVICE, &wDevice);
    faxXRead = 0 != WTInfo(WTI_DEVICES+wDevice, DVC_X, &axX);
    faxYRead = 0 != WTInfo(WTI_DEVICES+wDevice, DVC_Y, &axY);
		if (faxXRead && axX.axResolution != 0) // test so that we dont get division by zero (hTab will become 0 anyway)
			FIX_DIV(fx, CASTFIX32(1000), axX.axResolution);
    if (axX.axUnits == TU_INCHES)
      FIX_MUL(fx, fx, CASTFIX32(2.54));
    lc.lcOutExtX = INT(fx * lc.lcInExtX);
		if (faxYRead && axY.axResolution != 0) // test so that we dont get division by zero (unknown error)
			FIX_DIV(fx, CASTFIX32(1000), axY.axResolution);
    if (axY.axUnits == TU_INCHES)
      FIX_MUL(fx, fx, CASTFIX32(2.54));
    lc.lcOutExtY = INT(fx * lc.lcInExtY);
      
    hTab = (HANDLE) WTOpen(*this, &lc, TRUE);
		if (hTab != 0)
			act = actACTIVE;
		else
			act = actPASSIVE;
  }
  else if (iPort > 0)
	{ 
		try
    {
			Disable(false); // a.o. clean up COM port thread

			String sCom("COM%d:", iPort);

			OpenComPort(String("COM%d:", iPort));

			dcb.BaudRate = iBaudRate;
			dcb.fParity = true;
			dcb.fInX = true;
			dcb.XonLim = 20;	 // could use finetuning
			dcb.XoffLim = 100;
			dcb.ByteSize = iByteSize;
			dcb.Parity = iParity;
			dcb.StopBits = iStopBits;
			SetCommState(hComPort, &dcb);
			
			char cXOn = 0x11;
			unsigned long iNoBw;

			DWORD iRet = WriteFile(hComPort, &cXOn, 1, &iNoBw, NULL);
			if (iRet != 0)
				act = actACTIVE;
			else
				act = actPASSIVE;

			COMMTIMEOUTS commtimeouts;

			GetCommTimeouts(hComPort, &commtimeouts); 

			commtimeouts.ReadIntervalTimeout         = 250; 
			commtimeouts.ReadTotalTimeoutMultiplier  = 10; 
			commtimeouts.ReadTotalTimeoutConstant    = 100; 
			commtimeouts.WriteTotalTimeoutMultiplier = 10; 
			commtimeouts.WriteTotalTimeoutConstant   = 100; 
 
			SetCommTimeouts(hComPort, &commtimeouts);
			

			if (hComPort)
			{
				fKillThreads = false;
				threadDig = AfxBeginThread(ComPortReadThread, this, THREAD_PRIORITY_BELOW_NORMAL);
			}
		}
		catch(ErrorObject& err)
		{
			if (fReportErrors)
				err.Show();
			else
				throw;
		}
	}
	rCurrX = rUNDEF; //-1;
	rCurrY = rUNDEF; //-1;
}

void Digitizer::Disable(bool fDoRefCount)
{
	if (fDoRefCount) // The setup wizard is allowed to proceed (will come with fDoRefCount=false)
	{
		--iRefCount;
		if ((iRefCount > 0) || (act != actACTIVE))	// return if still referenced by editor or other thing - just decrease refcounter
			return;					// also ignore the call if we're in the Setup or Reference Wizard (and e.g. we closed a DigiEditor)
	}
  if (fWinTab)
	{
    WTClose((HCTX)hTab);
    hTab = 0;
  }
  else {
    if (threadDig) 
		{
				char cXOff = 0x13;
				unsigned long iNoBw;

				fKillThreads = true;
				WaitForSingleObject(threadDig->m_hThread,INFINITE);
				threadDig = 0;

				WriteFile(hComPort, &cXOff, 1, &iNoBw, NULL);
				CloseHandle(hComPort);
				hComPort = 0;
    }  
  }  
  act = actPASSIVE;  
}

void Digitizer::Interpret(const char* s)
{
  char sTmp[20];
  int i, iLen;
  char cFlag;
  iLen = strlen(s);
  if (iLen < iNrChars) return;
  for (i = 0; i < iXLen; ++i)
    sTmp[i] = s[iXPos + i - 1];
  sTmp[iXLen] = 0;
  rCurrX = atof(sTmp);
  if (rCurrX == rUNDEF)
    rCurrX = 0;
  for (i = 0; i < iYLen; ++i)
    sTmp[i] = s[iYPos + i - 1];
  sTmp[iYLen] = 0;
  rCurrY = atof(sTmp);
  if (rCurrY == rUNDEF)
    rCurrY = 0;
  cFlag = s[iFlagPos - 1];
  iButton = -1;
  for (i = 0; i <= 3; ++i)
    if (cFlag == sCodes[i]) {
      iButton = i;
      break;
    }
  rCurrX *= rUnitSize;  
  rCurrY *= rUnitSize;  
}



int Digitizer::Configure(Event*)
{
	String sParity;
  if (act >= actSETUP) {
    MessageBeep(MB_ICONEXCLAMATION);
		return 0;
	}

	// we need digitizer to be completely disabled
	if (iRefCount >= 1)
		Disable(false);

	digwiz = new DigitizerWizard(wPar, this);

  act = actSETUP;
	int iRes = digwiz->DoModal();
  act = actPASSIVE;
	if (ID_WIZFINISH == iRes)
	{
		Store();
		IlwisSettings settings2("Digitizer\\DigRef", IlwisSettings::pkuMACHINE, false, IlwisSettings::omREADONLY);
		fSetup = (iPort != 0 || fWinTab) && settings2.fKeyExists();
		// Perhaps unnecessary, but it is the right thing to do
		// in case we decide not to empty DigRef in the future
	  if (fSetup)
			RefLoad();
	}
	else
		Load();

	delete digwiz;
	digwiz = 0;
	if (iRefCount >= 1)
		Enable(true, false);

  return 0;
}

LRESULT Digitizer::OnReceiveWTPacket(WPARAM wParm, LPARAM lParm)
{
  PACKET pkt;
  if (WTPacket((HCTX)lParm, wParm, &pkt)) {
    short iB = pkt.pkButtons;
    iButton=-1;
    if      ( iB == 1 ) iButton=0;
    else if ( iB == 2 ) iButton=1;
    else if ( iB == 4 ) iButton=2;
    else if ( iB == 8 ) iButton=3;
    rCurrX = pkt.pkX / 100.0;
    rCurrY = pkt.pkY / 100.0;
    SendInfo();
  }
  return 0;
}

LRESULT Digitizer::OnCloseWTContext(WPARAM wParm, LPARAM lParm)
{
  if ((WORD)hTab == wParm)
    hTab = 0;
  return 0;
}

LRESULT Digitizer::OnOverlapWTContext(WPARAM wParm, LPARAM lParm)
{
  
  return 0;
}

LRESULT Digitizer::OnChangeWTContext(WPARAM wParm, LPARAM lParm)
{
  return 0;
}



void Digitizer::SendInfo()
{
  try {
    switch (act) {
      case actPASSIVE:
        break;
      case actACTIVE: 
        if (fMapRef) {
          cwcs.c().x = x0 + x10 * rCurrX + x01 * rCurrY;
          cwcs.c().y = y0 + y10 * rCurrX + y01 * rCurrY;
					IlwWinApp()->SendUpdateCoordMessages((CoordMessage)(100+iButton), &cwcs);
         }
        break;
      case actREFERENCING: //{
        if (mrf)
          mrf->ProcessInfo(rCurrX, rCurrY, iButton);
        break;
      case actSETUP: // never comes here
        break;
    }
  }
  catch (...) {}
}

class StartDigRefForm: public FormWithDest
{
public:
  StartDigRefForm(CWnd* w, String* sCsy, int* iOption)
  : FormWithDest(w, TR("Map Referencing - Coordinate System"))
  {
    FormEntry* fe = new FieldCoordSystemC(root, TR("&Coordinate System"), sCsy);
    fe->SetIndependentPos();
    RadioGroup* rg = new RadioGroup(root, TR("Use for Control Points:"), iOption);
    new RadioButton(rg, TR("&Metric (Projected) Coordinates"));
    new RadioButton(rg, TR("&Geographic Coordinates"));
    SetMenHelpTopic("ilwismen\\digitizer_map_referencing_coordsys.htm");
    create();
  }
};

int Digitizer::Reference(Event*)
{
	Enable(); // we need to use the digitizer .. if port is busy we'll get a message

	ACT actOld = act;

  if (actOld != actACTIVE && actOld != actPASSIVE)
    MessageBeep(MB_ICONEXCLAMATION);
	else
	{
		// Remark: incorrect behavior when clicking "Cancel"
		/*
		x0 = y0 = 0;
		x01 = x10 = y01 = y10 = 0;
		fMapRef = false;
		*/
		act = actBUSY;
 
		IlwisSettings settings("Digitizer\\DigRef", IlwisSettings::pkuMACHINE);

		String sCsy; 
		int iOption = 0;
		char sBuf[200];
		sCsy = settings.sValue("CoordSystem","unknown");
		FileName fn(sCsy);
		sCsy = fn.sFile;
		String sTmp = settings.sValue("Use","meters");
		if ("degrees" == sBuf)
			iOption = 1;
		StartDigRefForm frm(0, &sCsy, &iOption);  
		if (frm.fOkClicked())
		{
			fn = FileName(sCsy);
			cwcs = CoordSystem(fn);
			sCsy = fn.sFullName();
			settings.SetValue("CoordSystem", sCsy);
			switch (iOption) {
				case 0:
					settings.SetValue("Use", String("metres"));
					break;
				case 1:  
					settings.SetValue("Use", String("degrees"));
					break;
			}
			act = actREFERENCING;
			MapReferenceForm* mrffrm = new MapReferenceForm(AfxGetMainWnd(),this,0==iOption);
			mrf = mrffrm;
			mrf->create();
			act = actOld; // restore state
			if (mrf->fOkClicked() && mrf->fOk) {
				fAffine = mrffrm->iAffine == TRUE;
				x0 = mrffrm->x0;
				y0 = mrffrm->y0;
				x01 = mrffrm->x01;
				x10 = mrffrm->x10;
				y01 = mrffrm->y01;
				y10 = mrffrm->y10;
				// act = actACTIVE; commented out! if we did the MapRef with actPASSIVE, store it as such
				fMapRef = true;
				RefStore();
			}
			mrf = 0;
			delete mrffrm;
		}
		else
			act = actOld; // restore state (user clicked cancel)
	}

	Disable(); // release the COM port

  return 0;
}

bool Digitizer::fSetupAllowed()
{
	try
	{
		IlwisSettings settings ("Digitizer\\Configuration", IlwisSettings::pkuMACHINE, true, IlwisSettings::omREADWRITE, IlwisSettings::arALLUSERS);
	}
	catch (RegistryError&)
	{
	}
	// If we have sufficient rights, the key will exist by now
	// Now still do the following test (if the key does not exist it returns false)
	return (IlwisSettings::fWritingAllowed("Digitizer\\Configuration", IlwisSettings::pkuMACHINE));
}

bool Digitizer::fMapReferenceAllowed()
{
	return (IlwisSettings::fWritingAllowed("Digitizer\\DigRef", IlwisSettings::pkuMACHINE));
}

bool Digitizer::fActive() 
{ 
  return fSetup && fMapRef && (act == actACTIVE); 
//  return fSetup && fMapRef && (idComm > 0) && (act == actACTIVE); 
}

void Digitizer::OpenComPort(const String& sPort)
{
	if ( hComPort == 0 )
		hComPort = CreateFile(sPort.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);

	if (hComPort == INVALID_HANDLE_VALUE) 
	{
		act = actPASSIVE;  
		hComPort = 0;
		String sErr(TR("Error opening COM port %i\n%S").c_str(), iPort, TR("HARDWARE: The hardware is not available\n(is locked by another device)"));
		ErrorObject err(sErr);
		err.SetTitle(TR("Digitizer Error").c_str());
		throw err;
	}
}

bool Digitizer::fValid()
{
	return hComPort != 0;
}

bool Digitizer::fUsesWinTab()
{
	return fWinTab;
}
