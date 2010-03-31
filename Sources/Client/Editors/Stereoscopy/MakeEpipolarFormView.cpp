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
// MakeEpipolarFormView.cpp: implementation of the MakeEpipolarFormView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\FieldRowColFiducial.h"
#include "Client\Editors\Stereoscopy\MakeEpiPolarFormView.h"
#include "Client\Editors\Stereoscopy\MakeEpipolarDocument.h"
#include "Engine\SpatialReference\GrEpipolar.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Headers\Hs\stereoscopy.hs"
#include "Engine\Base\System\RegistrySettings.h"

IMPLEMENT_DYNCREATE(MakeEpipolarFormView, FormBaseView)

BEGIN_MESSAGE_MAP(MakeEpipolarFormView, FormBaseView)
	ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

const int MakeEpipolarFormView::iFieldSize = 30;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MakeEpipolarFormView::MakeEpipolarFormView()
: fInCalc(false), fInUpdate(false), sInputImageName("")
{
  clrTextNormal = GetSysColor(COLOR_WINDOWTEXT);
  clrBackNormal = GetSysColor(COLOR_WINDOW);
  // clrCurrentlyActive = GetSysColor(COLOR_HIGHLIGHTTEXT);
  // clrBackCurrentlyActive = GetSysColor(COLOR_HIGHLIGHT);
	clrCurrentlyActive = Color(0,0,0);
  clrBackCurrentlyActive = Color(255,255,196);

	IlwisSettings settings("Stereo Maker");
	// Now read the customizable items from registry
	clrBackCurrentlyActive = settings.clrValue("Background Active", clrBackCurrentlyActive); // entry does not exist => default color

  brNormal = new CBrush(clrBackNormal);
  brCurrentlyActive = new CBrush(clrBackCurrentlyActive);

	fbs |= fbsNOOKBUTTON | fbsNOCANCELBUTTON | fbsNOBEVEL;

	FieldGroup * fgLeft = new FieldGroup(root);
	FieldGroup * fgRight = new FieldGroup(root);
	fgRight->Align(fgLeft, AL_AFTER);

	stDummy = new StaticText(fgLeft, "");
	stDummy->psn->SetBound(0,0,0,0);
	// FieldBlank *fbDummy = new FieldBlank(fgLeft);

	stFiducials = new StaticText(fgLeft, SStcUiFiducials);
	stFiducials->Align(stDummy, AL_AFTER);
	stFiducials->psn->SetBound(0,0,0,0);
	int i;
  for (i = 0; i < 4; ++i)
	{
    fFm[i] = new FieldRowColFiducial(fgLeft, 1+i);
		if (i>0)
			fFm[i]->Align(fFm[i-1], AL_UNDER);
		else // ==0
			fFm[i]->Align(stDummy, AL_UNDER);
	}

	stDummy = new StaticText(fgLeft, "");
	stDummy->psn->SetBound(0,0,0,0);
	stDummy->Align(fFm[3], AL_UNDER);

	stOffFlight = new StaticText(fgLeft, SStcUiOffFLScalePts);
	stOffFlight->Align(stDummy, AL_AFTER);
	stOffFlight->psn->SetBound(0,0,0,0);
	for (i = 0; i < 2; ++i)
	{
		sFm[i] = new FieldRowColFiducial(fgLeft, 1+i);
	}
	sFm[0]->Align(stDummy, AL_UNDER);
	sFm[1]->Align(sFm[0], AL_UNDER);

	stPP = new StaticText(fgRight, SStcUiPrincPoint);
	stPP->psn->SetBound(0,0,0,0);
	
	String sDummy = "1234.5"; // To reserve space in PP, rotation and SF
	
	//fgUserPivot = new FieldGroup(fgRight);
	//stUPP = new StaticText(fgUserPivot, SStcUiUserPP);

	frcPP = new FieldRowCol(fgRight, "", &rcPP, true);
	frcPP->psn->SetBound(0,0,0,0);
	// frcPP->SetCallBack((NotifyProc)calc,this);
	frcPP->SetWidth(iFieldSize);
	//frcPP->Align(stPP, AL_UNDER);

	stTPP = new StaticText(fgRight, SStcUiTransfPP);
	stTPP->psn->SetBound(0,0,0,0);
	//stTPP->Align(frcPP, AL_UNDER);
	frcTPP = new FieldRowCol(fgRight, "", &rcTPP, true);
	frcTPP->psn->SetBound(0,0,0,0);
	// frcTPP->SetCallBack((NotifyProc)calc,this);
	frcTPP->SetWidth(iFieldSize);
	//frcTPP->Align(stTPP, AL_UNDER);
 
	stRot = new StaticText(fgRight, SStcUiRotation);
	stRot->psn->SetBound(0,0,0,0);
	//stRot->Align(frcTPP, AL_UNDER);
	stfRot = new StaticText(fgRight, sDummy);
	stfRot->psn->SetBound(0,0,0,0);
	stfRot->SetWidth(iFieldSize * 2);
	//stfRot->Align(stRot, AL_UNDER);

	stScf = new StaticText(fgRight, SStcUiTotDistfrFline);
	stScf->psn->SetBound(0,0,0,0);
	//stScf->Align(stfRot, AL_UNDER);
	stfScf = new StaticText(fgRight, sDummy);
	stfScf->psn->SetBound(0,0,0,0);
	stfScf->SetWidth(iFieldSize);
	//stfScf->Align(stScf, AL_UNDER);

	stInputTxt = new StaticText(fgRight, SStcUIInputImage);
	stInputTxt->psn->SetBound(0,0,0,0);
	//stScf->Align(stfRot, AL_UNDER);
	stInputName = new StaticText(fgRight, sDummy);
	stInputName->psn->SetBound(0,0,0,0);
	stInputName->SetWidth(iFieldSize);
	
	// Re-set values so that dummy isn't displayed
	//stfPPRow->SetVal("");
	//stfPPCol->SetVal("");
	//stPP->SetVal("");
	stfRot->SetVal("");
	stfScf->SetVal("");
	stInputName->SetVal("");

//	SetCallBack((NotifyProc)calc);

	// SetState(ifsFIDUCIALS); // is default already
}

MakeEpipolarFormView::~MakeEpipolarFormView()
{
  delete brNormal;
  delete brCurrentlyActive;
}

MakeEpipolarDocument *MakeEpipolarFormView::GetDocument()
{
	return (MakeEpipolarDocument*)m_pDocument;
}

void MakeEpipolarFormView::SetActiveColor(Color &clr)
{
	// update the color
	clrBackCurrentlyActive = clr;
	// update the brush
	brCurrentlyActive->DeleteObject(); // free the GDI
	brCurrentlyActive->CreateSolidBrush(clr); // then use the new color
}

Color MakeEpipolarFormView::GetActiveColor()
{
	return clrBackCurrentlyActive;
}

BOOL MakeEpipolarFormView::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if ((0 == HIWORD(wParam)) || (EN_SETFOCUS == HIWORD(wParam))) // not sure what 0 is called, but now it reacts well on the "checkbox flip"
	{
		MakeEpipolarDocument *med = GetDocument();
		// To find the current element: you have to go through all elements of
		// the form: iId will be between two form id's
		int iId = LOWORD(wParam);
		MakeEpipolarDocument::iFormStateTP iCandidateState = med->ifsGetState(); // default
		int iCandidateSubState = med->iGetSubState(); // default : 
		int i;
		for (i = 0; i < 4; ++i)
		{
			if (iId > fFm[i]->Id())
			{
				iCandidateState = MakeEpipolarDocument::ifsFIDUCIALS;
				iCandidateSubState = i;
			}
		}
		for (i = 0; i < 2; ++i)
		{
			if (iId > sFm[i]->Id())
			{
				iCandidateState = MakeEpipolarDocument::ifsOFFFLIGHTPTS;
				iCandidateSubState = i;
			}
		}
		if (iId > frcPP->Id())
		{
			iCandidateState = MakeEpipolarDocument::ifsPP;
			iCandidateSubState = 0;
		}
		if (iId > frcTPP->Id())
		{
			iCandidateState = MakeEpipolarDocument::ifsTPP;
			iCandidateSubState = 0;
		}
		int lHint = med->iGetHintFromState();
		med->SetState(iCandidateState, iCandidateSubState);
		FieldRowCol *frc = frcFieldRowColFromHint(lHint);
		FormEntryPositioner * psn;
		CRect rect;
		if (frc)
		{
			psn = frc->psn;
			rect = CRect(psn->iPosX, psn->iPosY, psn->iPosX + psn->iWidth, psn->iPosY + psn->iHeight);
			InvalidateRect(rect);
		}
		frc = frcFieldRowColFromCurrentState();
		if (frc)
		{
			psn  = frc->psn;
			rect  = CRect(psn->iPosX, psn->iPosY, psn->iPosX + psn->iWidth, psn->iPosY + psn->iHeight);
			InvalidateRect(rect);
		}
		if (0 == HIWORD(wParam))
			return FormBaseView::OnCommand(wParam, lParam); // otherwise the DataChanged() isn't called
		else
			return TRUE;
	}
	else
		return FormBaseView::OnCommand(wParam, lParam);
}

FieldRowCol * MakeEpipolarFormView::frcFieldRowColFromHint(int lHint)
{
	FieldRowCol * frcRet;
		// decode lHint
	int iIndex = lHint - 10001; // iIndex is now between 0 and 63
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point

	switch (iIndex)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			frcRet = fFm[iIndex]->frc;
			break;
		case 4:
		case 5:
			frcRet = sFm[iIndex-4]->frc;
			break;
		case 6:
			frcRet = frcPP;
			break;
		case 7:
			frcRet = frcTPP;
			break;
		default :
			frcRet = 0;
	}
	return frcRet;
}

FieldRowCol * MakeEpipolarFormView::frcFieldRowColFromCurrentState()
{
	FieldRowCol * frcRet;
	MakeEpipolarDocument *med = GetDocument();
	MakeEpipolarDocument::iFormStateTP ifsState = med->ifsGetState();
	int iSubState = med->iGetSubState();

	switch (ifsState)
	{
		case MakeEpipolarDocument::ifsFIDUCIALS :
			frcRet = fFm[iSubState]->frc;
			break;
		case MakeEpipolarDocument::ifsPP :
			frcRet = frcPP;
			break;
		case MakeEpipolarDocument::ifsTPP :
			frcRet = frcTPP;
			break;
		case MakeEpipolarDocument::ifsOFFFLIGHTPTS :
			frcRet = sFm[iSubState]->frc;
			break;
		default :
			frcRet = 0;
	}
	return frcRet;
}

CheckBox * MakeEpipolarFormView::cbCheckBoxFromHint(int lHint)
{
	CheckBox * cbRet;
		// decode lHint
	int iIndex = lHint - 10001; // iIndex is now between 0 and 63
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point

	switch (iIndex)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			cbRet = fFm[iIndex]->cb;
			break;
		case 4:
		case 5:
			cbRet = sFm[iIndex-4]->cb;
			break;
		default :
			cbRet = 0;
	}
	return cbRet;
}

int MakeEpipolarFormView::DataChanged(Event*)
{
	if (fInCalc)  // prevent calc() from being called when FieldRowCol for Principle Point is updated
		return 0;
	
	MakeEpipolarDocument *med = GetDocument();
	stfRot->SetVal("");
	stfRot->Show();
	stfScf->SetVal("");
	stfScf->Show();
	int i;
  for (i = 0; i < 4; ++i)
	{
    fFm[i]->StoreData(); // user sees offsef (1,1)
		fFm[i]->rcRowCol = Decreased(fFm[i]->rcRowCol); // we use offset (0,0) internally
	}
	for (i = 0; i < 2; ++i)
	{
		sFm[i]->StoreData();
		sFm[i]->rcRowCol = Decreased(sFm[i]->rcRowCol);
	}
	frcPP->StoreData();
	rcPP = Decreased(rcPP);
	frcTPP->StoreData();
	rcTPP = Decreased(rcTPP);
	for (i = 0; i < 4; ++i)
		med->SetFiducial(fFm[i]->fOk, fFm[i]->rcRowCol, i);
	med->SetScale(1);
	for (i = 0; i < 2; ++i)
		med->SetOffFlightLinePt(sFm[i]->fOk, sFm[i]->rcRowCol, i);
	med->SetPP(rcPP);
	// med->UpdateAllEpipolarViews(this, 10007); // PP may just have changed
	med->SetTPP(rcTPP);
	// send an update command if a user was typing in an editbox
	if (!fInUpdate)
		med->UpdateAllEpipolarViews(this, med->iGetHintFromState());
	int iErr = med->Compute();
	// check validity of PrincPoint computation:
	fPPDefined = false; // Default!! if first or second case is not valid, this stays
	if (med->iGetNrFiducials() < 2)
	{
		frcPP->SetReadOnly(false);
		fPPDefined = !rcPP.fUndef();
		stfRot->SetVal("");
	}
	else if (iErr <= -16 || iErr >= -10)
	{
		rcPP = med->rcGetPP();
		frcPP->SetReadOnly(true);
		fInCalc = true;
		frcPP->SetVal(Increased(rcPP));
		fInCalc = false;
		fPPDefined = true;
		stfRot->SetVal("");
	}
	else // >= 2 fiducials, but principle point can't be calculated => undefine it !!
	{
		fInCalc = true;
		frcPP->SetVal(rcUNDEF);
		fInCalc = false;
		med->SetPP(rcUNDEF);
	}

	med->UpdateAllEpipolarViews(this, 10007); // Refresh Principle Point

	// check whether the rotation is valid:
	if (!rcTPP.fUndef() && ((iErr < -17) || (iErr == 0)))
		stfRot->SetVal(String(SStcUiDegrees_F.scVal(), med->rGetAngle() * 180 / M_PI));
	else
		stfRot->SetVal("");
	stfRot->Show();

	// check whether the scaling is valid:
	if (med->iGetNrOffFlightPts() > 0 && iErr == 0)
		stfScf->SetVal(String(SStcUIPixels_F.scVal(), med->rGetScale()));
	else
		stfScf->SetVal("");
	stfScf->Show();

	stInputName->SetVal(sInputImageName);

	/*
	if (med->MakeEpipolarDocument()->)
		stInputName->SetVal("naam");
	else
		stInputName->SetVal("");
	stInputName->Show(); */
	
	return 0;
}

void MakeEpipolarFormView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	if (fInUpdate)
		return; // The calc below might trigger more updates
	FormBaseView::OnUpdate(pSender, lHint, pHint);
	if (10000 == lHint)
	{
		// update everything .. the method is not so handy but will do for now
		MakeEpipolarDocument *med = GetDocument();
		int i;
		FieldRowCol * frc;
		CheckBox * cb;
		fInCalc = true;
		for (i = 10001; i < 10009; ++i)
		{
			frc = frcFieldRowColFromHint(i);
			cb = cbCheckBoxFromHint(i);
			if (frc)
			{
				Coord crd (med->crdGetRCFromHint(i));
				if (!crd.fUndef())
					frc->SetVal(Increased(RowCol(crd.x, crd.y)));
				else
					frc->SetVal(rcUNDEF);
			}
			if (cb)
				cb->SetVal(med->fGetSelectedFromHint(i));
		}
		fInCalc = false;
	}
	else
	{
		// update specific field
		FieldRowCol * frc = frcFieldRowColFromHint(lHint);
		CheckBox * cb = cbCheckBoxFromHint(lHint);
		MakeEpipolarDocument *med = GetDocument();
		fInCalc = true;
		if (frc)
		{
			Coord crd (med->crdGetRCFromHint(lHint));
			if (!crd.fUndef())
				frc->SetVal(Increased(RowCol(crd.x, crd.y)));
			else
				frc->SetVal(rcUNDEF);
		}
		if (cb)
			cb->SetVal(med->fGetSelectedFromHint(lHint));
		fInCalc = false;
	}
	fInUpdate = true;
	DataChanged(0);
	fInUpdate = false;
}

HBRUSH MakeEpipolarFormView::OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_EDIT)
	{
		// if pWnd is a "currently active" element, return "currently active" colors,
		// otherwise "normal"

		// compare the m_hWnd's to check
		FieldRowCol *frc = frcFieldRowColFromCurrentState();
		if (frc->fIncludesHandle(pWnd->m_hWnd))
		{
			cdc->SetTextColor(clrCurrentlyActive);
			cdc->SetBkColor(clrBackCurrentlyActive);
			return *brCurrentlyActive;
		}
		else
		{
			cdc->SetTextColor(clrTextNormal);
			cdc->SetBkColor(clrBackNormal);
			return *brNormal;
		}
	}
	return FormBaseView::OnCtlColor(cdc, pWnd, nCtlColor);
}

RowCol MakeEpipolarFormView::Decreased(const RowCol& rc)
{
	RowCol rcRet;
	if ((rc.Row == 0) || (rc.Col == 0) || rc.fUndef())
		rcRet = rcUNDEF;
	else
		rcRet = RowCol(rc.Row - 1, rc.Col - 1);
	return rcRet;
}

RowCol MakeEpipolarFormView::Increased(const RowCol& rc)
{
	RowCol rcRet (rc);
	if (rcRet.Row != iUNDEF) ++rcRet.Row; // preserve partial rcUNDEF
	if (rcRet.Col != iUNDEF) ++rcRet.Col;
	return rcRet;
}

void MakeEpipolarFormView::SetInputImageName(const String& _sInputImageName)
{
	sInputImageName = FileName(_sInputImageName).sShortName(false);
}
