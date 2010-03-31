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
// MakeEpipolarDocument.cpp: implementation of the MakeEpipolarDocument class.
//
//////////////////////////////////////////////////////////////////////

//#include <afxwin.h>         // MFC core and standard components
#include "Client\Headers\formelementspch.h"
#include "Client\Editors\Stereoscopy\MakeEpipolarDocument.h"
#include "Client\Editors\Stereoscopy\PreStereoMateView.h"
#include "Engine\SpatialReference\GrEpipolar.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GRNONE.H"
#include "Headers\constant.h"

IMPLEMENT_DYNCREATE(MakeEpipolarDocument, CDocument)

BEGIN_MESSAGE_MAP(MakeEpipolarDocument, CDocument)
	//{{AFX_MSG_MAP(MakeEpipolarDocument)
	ON_COMMAND(ID_SELECTFIDUCIALS, SetStateFiducials)
	ON_COMMAND(ID_SELECTPPOINT, SetStatePPoint)
	ON_COMMAND(ID_SELECTTPPOINT, SetStateTPPoint)
	ON_COMMAND(ID_SELECTSCALINGPTS, SetStateScalingPts)
	ON_UPDATE_COMMAND_UI(ID_SELECTFIDUCIALS, OnUpdateFiducials)
	ON_UPDATE_COMMAND_UI(ID_SELECTPPOINT, OnUpdatePPoint)
	ON_UPDATE_COMMAND_UI(ID_SELECTTPPOINT, OnUpdateTPPoint)
	ON_UPDATE_COMMAND_UI(ID_SELECTSCALINGPTS, OnUpdateScalingPts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MakeEpipolarDocument::MakeEpipolarDocument()
: mpv(0), grEpi(0)
{
	for (int i=0; i<=3; ++i)
		iFormSubState[i] = 0; // reset all states to 0
	SetState(ifsFIDUCIALS);
}

MakeEpipolarDocument::~MakeEpipolarDocument()
{
}

void MakeEpipolarDocument::SetGrEpiMember(GeoRefEpipolar *gr_In)
{
	grEpi = gr_In;
}

void MakeEpipolarDocument::UpdateAllEpipolarViews(CView* pSender, LPARAM lHint, CObject* pHint)
{
	// SetModifiedFlag();
	if (mpv && pSender != mpv)
		mpv->OnUpdate(pSender, lHint, pHint);
	UpdateAllViews(pSender, lHint, pHint);
}

void MakeEpipolarDocument::SetView(PreStereoMateView * mvIn)
{
	mpv = mvIn; // set the view that for the time being can't follow the doc/view principle
}


void MakeEpipolarDocument::SetState(iFormStateTP iState, int iSubState)
{
	// dont change the state if we want to go to ifsPP and there are 2 or more fiducials
	if (iState != ifsPP || iGetNrFiducials() < 2)
	{
		iFormState = iState;
		iFormSubState[iFormState] = iSubState;
		if (mpv && (mpv->iActiveTool))
			mpv->OnNoTool();
	}
}

void MakeEpipolarDocument::SetState(iFormStateTP iState)
{
	// dont change the state if we want to go to ifsPP and there are 2 or more fiducials
	if (iState != ifsPP || iGetNrFiducials() < 2)
	{
		iFormState = iState;
		if (mpv && (mpv->iActiveTool))
			mpv->OnNoTool();
	}
}

void MakeEpipolarDocument::SetSubState(int iSubState)
{
	iFormSubState[iFormState] = iSubState;
}

void MakeEpipolarDocument::AdvanceSubState()
{
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point
	switch (iFormState)
	{
		case ifsFIDUCIALS :
			++iFormSubState[iFormState] %= 4;
			break;
		case ifsOFFFLIGHTPTS :
			++iFormSubState[iFormState] %= 2;
			break;
	}
}

int MakeEpipolarDocument::iGetHintFromState()
{
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point
	int lHint;
	switch (iFormState)
	{
		case ifsFIDUCIALS :
			lHint = iFormSubState[iFormState];
			break;
		case ifsOFFFLIGHTPTS :
			lHint = 4 + iFormSubState[iFormState];
			break;
		case ifsPP :
			lHint = 6;
			break;
		case ifsTPP :
			lHint = 7;
			break;
		default : // Nothing updated
			lHint = 8;
			break;
	}
	return 10001 + lHint;
}

MakeEpipolarDocument::iFormStateTP MakeEpipolarDocument::ifsGetState()
{
	return iFormState;
}

int MakeEpipolarDocument::iGetSubState()
{
	return iFormSubState[iFormState];
}

void MakeEpipolarDocument::SetRowCol(const RowCol &rc)
{
	double *pCrdX = 0, *pCrdY = 0;
	bool *pfSelected = 0;
	if (grEpi)
	{
		switch (iFormState)
		{
			case ifsFIDUCIALS :
				pCrdX = &grEpi->rScannedFidRow[iFormSubState[iFormState]];
				pCrdY = &grEpi->rScannedFidCol[iFormSubState[iFormState]];
				pfSelected = &grEpi->fSelectedFiducial[iFormSubState[iFormState]];
				break;
			case ifsOFFFLIGHTPTS :
				pCrdX = &grEpi->rScannedScalingPntRow[iFormSubState[iFormState]];
				pCrdY = &grEpi->rScannedScalingPntCol[iFormSubState[iFormState]];
				pfSelected = &grEpi->fSelectedScalingPoint[iFormSubState[iFormState]];
				break;
			case ifsPP :
				pCrdX = &grEpi->cScanPrincPoint.x;
				pCrdY = &grEpi->cScanPrincPoint.y;
				grEpi->rScanPrincPointRow = rc.Row; // copy here coz we might wanna draw the new point
				grEpi->rScanPrincPointCol = rc.Col; 
				break;
			case ifsTPP :
				pCrdX = &grEpi->cScanTransfPrincPoint.x;
				pCrdY = &grEpi->cScanTransfPrincPoint.y;
				break;
		}
	}
	if (pCrdX) // test if null!!
	{
		Coord crd;
		if (!rc.fUndef())
			crd = Coord(rc.Row, rc.Col);
		else
			crd = crdUNDEF;
		if (*pCrdX != crd.x || *pCrdY != crd.y) // check if the point changed
		{
			if (*pCrdX != rUNDEF && *pCrdY != rUNDEF)
				oldRC = Coord(*pCrdX, *pCrdY); // backup for update purposes
			else
				;//oldRC = crdUNDEF;
		}
		*pCrdX = crd.x;
		*pCrdY = crd.y;
	}
	if (pfSelected) // all except PP and TPP have a checkbox
		*pfSelected = true;
}

Coord MakeEpipolarDocument::crdGetOldRC()
{
	return oldRC; // the one that was backed up, in order to remember where to refresh the screen
}

list <Element> MakeEpipolarDocument::ElementList()
{
	list <Element> eList;
	eList.clear();
	int i, iNrFiducials = 0;

	if (grEpi)
	{
		for (i = 0; i < 4; ++i)
		{
			if (grEpi->fSelectedFiducial[i] && grEpi->rScannedFidRow[i] != rUNDEF && grEpi->rScannedFidCol[i] != rUNDEF)
			{
				eList.push_back(Element(RowCol(grEpi->rScannedFidRow[i], grEpi->rScannedFidCol[i]), 0, String ("%1i", 1+i)));
				++iNrFiducials;
			}
		}

		for (i = 0; i < 2; ++i)
		{
			if (grEpi->fSelectedScalingPoint[i] && grEpi->rScannedScalingPntRow[i] != rUNDEF && grEpi->rScannedScalingPntCol[i] != rUNDEF)
				eList.push_back(Element(RowCol(grEpi->rScannedScalingPntRow[i], grEpi->rScannedScalingPntCol[i]), 3, String ("S%1i", 1+i)));
		}
		if (grEpi->rScanPrincPointRow != rUNDEF && grEpi->rScanPrincPointCol != rUNDEF) eList.push_back(Element(Coord(grEpi->rScanPrincPointRow, grEpi->rScanPrincPointCol), (iNrFiducials>=2)?1:4, "PP"));
		if (grEpi->cScanTransfPrincPoint != crdUNDEF) eList.push_back(Element(grEpi->cScanTransfPrincPoint, 2, "TP"));
	}
	return eList;
}

Coord MakeEpipolarDocument::crdGetRCFromHint(int lHint)
{
	// decode lHint
	int iIndex = lHint - 10001; // iIndex is now between 0 and 63
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point
	Coord crd;

	if (grEpi)
	{
		switch (iIndex)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				crd = Coord(grEpi->rScannedFidRow[iIndex], grEpi->rScannedFidCol[iIndex]);
				break;
			case 4:
			case 5:
				crd = Coord(grEpi->rScannedScalingPntRow[iIndex-4], grEpi->rScannedScalingPntCol[iIndex-4]);
				break;
			case 6:
				crd = Coord(grEpi->rScanPrincPointRow, grEpi->rScanPrincPointCol);
				break;
			case 7:
				crd = grEpi->cScanTransfPrincPoint;
				break;
		}
	}
	return crd;
}

bool MakeEpipolarDocument::fGetSelectedFromHint(int lHint)
{
	// decode lHint
	int iIndex = lHint - 10001; // iIndex is now between 0 and 63
	// 0..3: one of the fiducials
	// 4..5: one of the off-flight-line-pts
	// 6	 : the principle point
	// 7	 : the transferred principle point
	bool fSelected;

	if (grEpi)
	{
		switch (iIndex)
		{
			case 0:
			case 1:
			case 2:
			case 3:
				fSelected = grEpi->fSelectedFiducial[iIndex];
				break;
			case 4:
			case 5:
				fSelected = grEpi->fSelectedScalingPoint[iIndex-4];
				break;
			// no more cases !!! return value should be ignored by recepient
		}
	}
	return fSelected;
}

void MakeEpipolarDocument::SetFiducial(bool fSelected, const RowCol &rc, int iIndex)
{
	if (grEpi)
	{
		if (grEpi->rScannedFidRow[iIndex] != (rc.fUndef()?rUNDEF:rc.Row) || grEpi->rScannedFidCol[iIndex] != (rc.fUndef()?rUNDEF:rc.Col)) // check if the point changed
			oldRC = Coord(grEpi->rScannedFidRow[iIndex], grEpi->rScannedFidCol[iIndex]); // then backup the RC
		grEpi->rScannedFidRow[iIndex] = (rc.fUndef()?rUNDEF:rc.Row);
		grEpi->rScannedFidCol[iIndex] = (rc.fUndef()?rUNDEF:rc.Col);
		grEpi->fSelectedFiducial[iIndex] = fSelected;
	}
}

void MakeEpipolarDocument::SetOffFlightLinePt(bool fSelected, const RowCol &rc, int iIndex)
{
	if (grEpi)
	{
		if (grEpi->rScannedScalingPntRow[iIndex] != (rc.fUndef()?rUNDEF:rc.Row) || grEpi->rScannedScalingPntCol[iIndex] != (rc.fUndef()?rUNDEF:rc.Col)) // check if the point changed
			oldRC = Coord(grEpi->rScannedScalingPntRow[iIndex], grEpi->rScannedScalingPntCol[iIndex]);
		grEpi->rScannedScalingPntRow[iIndex] = (rc.fUndef()?rUNDEF:rc.Row);
		grEpi->rScannedScalingPntCol[iIndex] = (rc.fUndef()?rUNDEF:rc.Col);
		grEpi->fSelectedScalingPoint[iIndex] = fSelected;
	}
}

void MakeEpipolarDocument::SetPP(const RowCol &rc)
{
	if (grEpi)
	{
		if (!rc.fUndef()) // New PP NOT UNDEF
		{
			if (!grEpi->cScanPrincPoint.fUndef()) // back up previous PP properly
			{
				if (rc != RowCol(grEpi->cScanPrincPoint.x, grEpi->cScanPrincPoint.y)) // check if the point changed
					oldRC = grEpi->cScanPrincPoint;
			}
			else
				; //oldRC = crdUNDEF;
			grEpi->cScanPrincPoint = Coord(rc.Row, rc.Col); // Set the PP to the new one
			grEpi->rScanPrincPointRow = rc.Row; // copy here coz we might wanna draw the new point
			grEpi->rScanPrincPointCol = rc.Col; // and rScanPrincPointRow/Col isn't set if compute quits
		}
		else // New PP is UNDEF
		{
			if (!grEpi->cScanPrincPoint.fUndef()) // back up previous PP properly
				oldRC = grEpi->cScanPrincPoint;
			else
				; //oldRC = crdUNDEF;
			grEpi->cScanPrincPoint = crdUNDEF; // Set the PP to the new one
			grEpi->rScanPrincPointRow = rUNDEF; // copy here coz we might wanna draw the new point
			grEpi->rScanPrincPointCol = rUNDEF; // and rScanPrincPointRow/Col isn't set if compute quits
		}
	}
}

RowCol MakeEpipolarDocument::rcGetPP()
{
	if (grEpi)
	{
		if (!grEpi->cScanPrincPoint.fUndef())
			return RowCol(grEpi->cScanPrincPoint.x, grEpi->cScanPrincPoint.y);
		else
			return rcUNDEF;
	}
	else
		return rcUNDEF;
}

void MakeEpipolarDocument::SetTPP(const RowCol &rc)
{
	if (grEpi)
	{
		if (!rc.fUndef()) // New TPP NOT UNDEF
		{
			if (!grEpi->cScanTransfPrincPoint.fUndef()) // back up previous TPP properly
			{
				if (rc != RowCol(grEpi->cScanTransfPrincPoint.x, grEpi->cScanTransfPrincPoint.y)) // check if the point changed
					oldRC = grEpi->cScanTransfPrincPoint;
			}
			else
				; //oldRC = crdUNDEF;
			grEpi->cScanTransfPrincPoint = Coord(rc.Row, rc.Col);
		}
		else // New PP is UNDEF
		{
			if (!grEpi->cScanTransfPrincPoint.fUndef()) // back up previous TPP properly
				oldRC = grEpi->cScanTransfPrincPoint;
			else
				; //oldRC = crdUNDEF;
			grEpi->cScanTransfPrincPoint = crdUNDEF; // Set the TPP to the new one
		}
	}
}

void MakeEpipolarDocument::SetScale(double rSc)
{
	if (grEpi)
		grEpi->rScale = rSc;
}

int MakeEpipolarDocument::iGetNrFiducials()
{
	if (grEpi)
		return grEpi->iNrFiduc; // usually a Compute() will happen before this will have a chance to be called
	else
		return 0;
}

int MakeEpipolarDocument::iGetNrOffFlightPts()
{
	if (grEpi)
		return grEpi->iNrOffFlight; // usually a Compute() will happen before this will have a change to be called
	else
		return 0;
}

double MakeEpipolarDocument::rGetAngle()
{
	if (grEpi)
		return grEpi->rAngle;
	else
		return 0;
}

double MakeEpipolarDocument::rGetScale()
{
	if (grEpi)
		return grEpi->rScale;
	else
		return 0;
}

int MakeEpipolarDocument::Compute()
{
	if (grEpi)
	{
		oldRC = Coord(grEpi->rScanPrincPointRow, grEpi->rScanPrincPointCol);
		return grEpi->Compute();
	}
	else
		return -1; // georef not yet defined ...
}

void MakeEpipolarDocument::SetStateFiducials()
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	int lHint = iGetHintFromState();
	if (fToolActive || iFormState!=ifsFIDUCIALS)
		SetState(ifsFIDUCIALS);
	else
		AdvanceSubState();
	UpdateAllEpipolarViews(0, lHint);
}

void MakeEpipolarDocument::SetStatePPoint()
{
	int lHint = iGetHintFromState();
	SetState(ifsPP);
	UpdateAllEpipolarViews(0, lHint);
}

void MakeEpipolarDocument::SetStateTPPoint()
{
	int lHint = iGetHintFromState();
	SetState(ifsTPP);
	UpdateAllEpipolarViews(0, lHint);
}

void MakeEpipolarDocument::SetStateScalingPts()
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	int lHint = iGetHintFromState();
	if (fToolActive || iFormState!=ifsOFFFLIGHTPTS)
		SetState(ifsOFFFLIGHTPTS);
	else
		AdvanceSubState();
	UpdateAllEpipolarViews(0, lHint);
}

void MakeEpipolarDocument::OnUpdateFiducials(CCmdUI* pCmdUI)
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	pCmdUI->SetRadio((ifsGetState() == ifsFIDUCIALS) && !fToolActive);
}

void MakeEpipolarDocument::OnUpdatePPoint(CCmdUI* pCmdUI)
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	pCmdUI->Enable(iGetNrFiducials()<2);
	pCmdUI->SetRadio((ifsGetState() == ifsPP) && !fToolActive);
}

void MakeEpipolarDocument::OnUpdateTPPoint(CCmdUI* pCmdUI)
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	pCmdUI->SetRadio((ifsGetState() == ifsTPP) && !fToolActive);
}

void MakeEpipolarDocument::OnUpdateScalingPts(CCmdUI* pCmdUI)
{
	bool fToolActive = false;
	if (mpv)
		fToolActive = 0 != mpv->iActiveTool;
	pCmdUI->SetRadio((ifsGetState() == ifsOFFFLIGHTPTS) && !fToolActive);
}
