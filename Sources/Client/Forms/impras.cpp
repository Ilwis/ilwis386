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
/* 
  Import Raster General
  by Wim Koolhoven
  (c) ILWIS System Development, ITC
	Last change:  WK   18 Sep 98    6:03 pm
*/

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Headers\hourglss.h"
#include "Headers\Hs\IMPEXP.hs"
#include "Headers\Htp\Ilwismen.htp"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Forms\IMPORT.H"
#include "Client\Forms\GeneralImportForm.h" 
#include "Client\Forms\IMPRAS.H"
#include "Engine\DataExchange\CONV.H"
#include "Client\ilwis.h"

ImportRasterForm::ImportRasterForm(CWnd* wPar, const FileName& fn, const String& sDef)
: FormExtraImportOptions(wPar, SIETitleImportRaster),
  fnMap(fn)
{
	iHeaderSize = 0;
	iNrBands = 1;
	iNrCols = 1024;
	iPixelStructure = 1;
	iNrBytes = 0;
	iRealType = 1;
	iFileStructure = 0;  
	fByteSwap = false;
	fCopyData = false;
	iCopyUseAs = 0;
	fUseAs = true;
	fCreateMPR = true;
	
	new FieldInt(root, SIERasHeaderSize, &iHeaderSize, ValueRange(0,64000));
	fiBands = new FieldInt(root, SIERasNrBands, &iNrBands, ValueRange(1,9999), true);  // use with spin control
	fiBands->SetCallBack((NotifyProc)&ImportRasterForm::CallBackBands);
	new FieldInt(root, SIERasNrCols, &iNrCols, ValueRange(2,1000000));
	
	FieldGroup* fg = new FieldGroup(root, true);
	rgPix = new RadioGroup(fg, SIERasPixStruct, &iPixelStructure);
	rgPix->SetCallBack((NotifyProc)&ImportRasterForm::CallBackPixelStructure);
	new RadioButton(rgPix, SIERasBit);
	new RadioButton(rgPix, SIERasByte);
	RadioButton* rbInt = new RadioButton(rgPix, SIERasInt);
	cbSwap = new CheckBox(rbInt, SIERasHighEndian, &fByteSwap);
	RadioButton* rbFlt = new RadioButton(rgPix, SIERasFlt);
	new CheckBox(rbFlt, SIERasHighEndian, &fByteSwap);
	rbFlt->Align(rbInt, AL_UNDER);
	
	rgInt = new RadioGroup(rbInt, SIERasBytesPerPixel, &iNrBytes, true);
	rgInt->SetCallBack((NotifyProc)&ImportRasterForm::CallBackPixelStructure);
	rgInt->Align(rbFlt, AL_UNDER);
	rgInt->SetIndependentPos();
	new RadioButton(rgInt, "&1");
	new RadioButton(rgInt, "&2");
	new RadioButton(rgInt, "&4");
	
	RadioGroup* rgFlt = new RadioGroup(rbFlt, SIERasBytesPerPixel, &iRealType, true);
	rgFlt->Align(rbFlt, AL_UNDER);
	rgFlt->SetIndependentPos();
	new RadioButton(rgFlt, "&4");
	new RadioButton(rgFlt, "&8");
	
	rgFile = new RadioGroup(fg, SIERasFileStruct, &iFileStructure);
	new RadioButton(rgFile, SIERasBIL);
	new RadioButton(rgFile, SIERasBSQ);
	new RadioButton(rgFile, SIERasPixelInterleaved);
	
	rgCopyUseAs = new RadioGroup(fg, SIERasUseAsOrCopy, &iCopyUseAs);
	rgCopyUseAs->SetCallBack((NotifyProc)&ImportRasterForm::CallBackBands);
	RadioButton *rbUseAs = new RadioButton(rgCopyUseAs, SIERasUseOriginal);
	new RadioButton(rgCopyUseAs, SIERasConvertToILWIS);
	cbCreateMPR = new CheckBox(rbUseAs, SIERasCreateMPR, &fCreateMPR);
	cbCreateMPR->Align(rbUseAs, AL_AFTER);
	
	new StaticText(root, SIEUiDescription);
	FieldString * fsDesc = new FieldString(root, "", &sDescr);
	fsDesc->SetWidth(200);
	fsDesc->SetIndependentPos();

	SetMenHelpTopic(htpImportGenRas);
}

BOOL ImportRasterForm::OnInitDialog()
{
	FormWithDest::OnInitDialog();

	int iImg = IlwWinApp()->iImage("ExeMap16Ico");
	HICON hIco = IlwWinApp()->ilSmall.ExtractIcon(iImg);
	SetIcon(hIco,FALSE);

	return TRUE;
}

int ImportRasterForm::CallBackPixelStructure(Event*)
{
	rgPix->StoreData();
	rgInt->StoreData();

	switch (iPixelStructure)
	{
		case 0:
		case 1: break;
		case 2: if (iNrBytes >= 1)  // Note: this is the option, not the actual number of bytes!
							cbSwap->Show();
						else
							cbSwap->Hide();
						break;
		case 3: cbSwap->Hide();
						break;
	}
	return 1;
}

int ImportRasterForm::CallBackBands(Event*)
{
	if (!fiBands->fShow())
		return 1;

	fiBands->StoreData();
	if (iNrBands > 1)
	{
		rgCopyUseAs->StoreData();
		if (iCopyUseAs == 0)
			cbCreateMPR->Show();
		else
			cbCreateMPR->Hide();
		rgFile->Show();
	}
	else
	{
		cbCreateMPR->Hide();
		rgFile->Hide();

	}
	return 1;
}

int ImportRasterForm::exec()
{
	HourglassCursor cur(this);
	FormWithDest::exec();

	/*String sCmd = sBuildCommandLine();
	if (sCmd.length() > 0)
	{
		char* str = sCmd.sVal();
		IlwWinApp()->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);
	}*/
	
	return 0;
}

bool ImportRasterForm::fValidData() {
	return true;
}

String ImportRasterForm::sGetExtraOptionsPart(const String& currentExp) {
	String sCmd;
	if (iNrBands > 1)
		sCmd &= "=maplist(";
	else
		sCmd &= "=map(";

	// Use the string sQuote function instead of FileName sFullNameQuoted
	// because the extension may also contain quotable characters.
	sCmd &= fnMap.sFullName().sQuote();
	sCmd &= ",genras";
	
	fUseAs = iCopyUseAs == 0;
	if (fUseAs)
		sCmd &= ",UseAs";
	else
		sCmd &= ",Convert";

	sCmd &= String(",%d", iNrCols);
	if (iNrBands > 1)
		sCmd &= String(",%d", iNrBands);

	sCmd &= String(",%d", iHeaderSize);

	if (iNrBands > 1)
	{
		switch (iFileStructure)
		{
		    case 0:
				sCmd &= ",BIL"; // BANDINTERLEAVED;
				break;
		    case 1:
				sCmd &= ",BSQ"; // BANDSEQUENTIAL;
				break;  
		    case 2:
				sCmd &= ",BIP"; // PIXELINTERLEAVED;
				break;  
		}
	}
	int iNrBytesPerPixel;
	switch (iPixelStructure)
	{
	    case 0:  // bit
			sCmd &= ",Bit,1";
			break;
	    case 1:  // byte
			sCmd &= ",Byte,1";
			break;
	    case 2:  // int
			iNrBytesPerPixel = 1 << iNrBytes;
			sCmd &= String(",Int,%d", iNrBytesPerPixel);
			break;
		case 3:
			iNrBytesPerPixel = 4 << iRealType;
			sCmd &= String(",Real,%d", iNrBytesPerPixel);
			break;
	}
	if (fByteSwap)
		sCmd &= ",SwapBytes";
	else
		sCmd &= ",NoSwap";

	if (iNrBands > 1)
	{
		// Convert: always create MPR
		// UseAs:   create MPR when selected by user
		if (!fUseAs || (fUseAs && fCreateMPR))
			sCmd &= ",CreateMpr";
		else
			sCmd &= ",NoMPR";
	}
	if (sDescr.length() > 0)
		sCmd &= String(",\"%S\"", sDescr);  // quote the description
	sCmd &= ")";

	return sCmd;
}