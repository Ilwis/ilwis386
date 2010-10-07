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
// EditFiducialMarksForm.cpp: implementation of the EditFiducialMarksForm class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Engine\Map\Segment\Seg.h"
#include "engine\map\polygon\POL.H"
#include "Engine\SpatialReference\Grortho.h"
#include "Engine\SpatialReference\CoordSystemOrthoPhoto.h"
#include "Client\Editors\Digitizer\DIGITIZR.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Georef\EditFiducialMarksForm.h"
#include "Client\Editors\Georef\FieldFiducialMark.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\constant.h"
#include "Headers\Hs\Digitizr.hs"
#include "Headers\Hs\Georef.hs"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Editors\Utils\SYMBOL.H"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

EditFiducialMarksForm::EditFiducialMarksForm(MapPaneView* wnd, GeoRefOrthoPhoto* gcd)
: FormWithDest(wnd, SDGTitleLocateFiducialMarks, fbsSHOWALWAYS|fbsBUTTONSUNDER),
  grf(gcd), mpv(wnd)
{
	fGeoRef = true;
  colFidMarks = Color(255,0,255);  // default purple

	IlwisSettings settings("Map Window\\TiePoint Editor");
	colFidMarks = settings.clrValue("Fiducial Marks Color", colFidMarks);

  clrNormal = SysColor(COLOR_WINDOWTEXT);
  clrActive = SysColor(COLOR_HIGHLIGHTTEXT);
  clrBackNormal = SysColor(COLOR_WINDOW);
  brNormal = new CBrush(clrBackNormal);
  clrBackActive = SysColor(COLOR_HIGHLIGHT);
  brActive = new CBrush(clrBackActive);

  rPrincDist = 1000 * grf->rPrincDistance; // change from m to mm.
  frPrincDist = new FieldReal(root, SGRUiPrincDist, &rPrincDist, ValueRangeReal(0, 1e3, 0.001));
  frPrincDist->SetIndependentPos();
	frPrincDist->SetCallBack((NotifyProc)&EditFiducialMarksForm::ChangePDistCallBack);
	cCameraPrincPmm.x = grf->cCameraPPoint.x*1000;
	cCameraPrincPmm.y = grf->cCameraPPoint.y*1000;
	fcPrincPoint = new FieldCoord(root,SGRUiPrincPoint, &cCameraPrincPmm);
	fcPrincPoint->Align(frPrincDist, AL_AFTER);
	fcPrincPoint->SetIndependentPos();
	grf->cCameraPPoint.x = cCameraPrincPmm.x / 1000; // pass UI-entered PP to the georef (in meters)
	grf->cCameraPPoint.y = cCameraPrincPmm.y / 1000;
	fcPrincPoint->SetCallBack((NotifyProc)&EditFiducialMarksForm::ChangePPointCallBack);
  FieldBlank* fldbla = new FieldBlank(root, 0.8);
	fldbla->Align(frPrincDist, AL_UNDER);
	iTransformOption = grf->transfFid;
	rgTransf = new RadioGroup(root, "", &iTransformOption,true);
	new RadioButton(rgTransf, "Conformal");
  new RadioButton(rgTransf, "Affine");
	new RadioButton(rgTransf, "Bilinear");
	new RadioButton(rgTransf, "Projective");
	rgTransf->SetCallBack((NotifyProc)&EditFiducialMarksForm::TransformationChange);
	FieldBlank* fldbla2 = new FieldBlank(root, 1.1);
	fldbla2->Align(rgTransf, AL_UNDER);
  int i;
	
	iNrFiducialInputFields = 8;
  for (i = 0; i < iNrFiducialInputFields; ++i) {
    ffm[i] = new FieldFiducialMark(root, 1+i, fGeoRef);
		ffm[i]->psn->iBndUp = 0;
		ffm[i]->psn->iBndDown = -4;
//    ffm[i]->cb->SetCallBack((NotifyProc)&EditFiducialMarksForm::calc,this);
  }
  for (i = 1; i < iNrFiducialInputFields; ++i) {
    ffm[i]->Align(ffm[i-1], AL_UNDER);
	}
  for (i = 0; i < grf->iNrFiduc; ++i) {
    FieldFiducialMark* fld = ffm[i];
    fld->rRow() = grf->rScannedFidRow[i];
    fld->rCol() = grf->rScannedFidCol[i];
		if (!grf->fSubPixelPrecision)
			SetDirty(fld->rc());
		else
			SetDirty(fld->c());
    fld->crdPhoto.x = grf->cPhotoFid[i].x * 1000;
    fld->crdPhoto.y = grf->cPhotoFid[i].y * 1000;
    fld->fRowCol = !fld->rc().fUndef();
    fld->fPhoto = !fld->crdPhoto.fUndef();
    fld->frc->SetVal(fld->crdRowCol);
    fld->fcPhoto->SetVal(fld->crdPhoto);
    fld->fOk = fld->fRowCol && fld->fPhoto;
    fld->cb->SetVal(fld->fOk);
  }
    
  FieldGroup* fg = new FieldGroup(root);
  StaticText *st;
  int iH;
  st = new StaticText(fg, SDGRemCameraCrds);
  iH = ffm[0]->fcPhoto->psn->iHeight + ffm[0]->fcPhoto->psn->iBndDown + 20;
  st->Align(ffm[0]->fcPhoto, AL_UNDER, -iH);
	if (!grf->fSubPixelPrecision)
		st = new StaticText(fg, SDGRemRowCol);
	else
		st = new StaticText(fg, SDGRemRowColSubPix);
  iH = ffm[0]->frc->psn->iHeight + ffm[0]->frc->psn->iBndDown + 20;
  st->Align(ffm[0]->frc, AL_UNDER, -iH);
  st = new StaticText(fg, SDGRemDiffRow);
  iH = ffm[0]->stDRow->psn->iHeight + ffm[0]->stDRow->psn->iBndDown + 20;
  st->Align(ffm[0]->stDRow, AL_UNDER, -iH);
  st = new StaticText(fg, SDGRemDiffCol);
  iH = ffm[0]->stDCol->psn->iHeight + ffm[0]->stDCol->psn->iBndDown + 20;
  st->Align(ffm[0]->stDCol, AL_UNDER, -iH);
	
	String sFill11('X', 11);
	stRMSrow = new StaticText(fg, sFill11);
	stRMSrow->Align(ffm[7]->stDRow, AL_UNDER);
	String sFill6('X', 6);
	stRMScol = new StaticText(fg, sFill6);
	stRMScol->Align(ffm[7]->stDCol, AL_UNDER);
	stRMSrow->SetVal("");
	stRMScol->SetVal("");
	
  iActDigPnt = 0;
  ffmCurr()->fAct = true;
//  new FieldBlank(root);
  FieldGroup* fgInfo = new FieldGroup(root);
  fgInfo->SetIndependentPos();
  fgInfo->Align(ffm[7], AL_UNDER);

  String sFill('X', 50);
  stRem1 = new StaticText(fgInfo, sFill);
  stRem2 = new StaticText(fgInfo, sFill);
	stRem3 = new StaticText(fgInfo, sFill);
  fgInfo->SetCallBack((NotifyProc)&EditFiducialMarksForm::initFMF);

  SetMenHelpTopic(htpGrfFiducialMarks);

  for (i = 0; i < iNrFiducialInputFields; ++i)
    ffm[i]->SetCallBack((NotifyProc)&EditFiducialMarksForm::calc,this);
}

EditFiducialMarksForm::EditFiducialMarksForm(MapPaneView* wnd, CoordSystemOrthoPhoto* csortho)
: FormWithDest(wnd,SDGTitleLocateFiducialMarks,fbsSHOWALWAYS|fbsBUTTONSUNDER),
  cs(csortho), mpv(wnd)
{
		fGeoRef = false;
  colFidMarks = Color(255,0,255);  // default purple

	IlwisSettings settings("Map Window\\TiePoint Editor");
	colFidMarks = settings.clrValue("Fiducial Marks Color", colFidMarks);

  clrNormal = SysColor(COLOR_WINDOWTEXT);
  clrActive = SysColor(COLOR_HIGHLIGHTTEXT);
  clrBackNormal = SysColor(COLOR_WINDOW);
  brNormal = new CBrush(clrBackNormal);
  clrBackActive = SysColor(COLOR_HIGHLIGHT);
  brActive = new CBrush(clrBackActive);

  rPrincDist = 150;//1000 * cs->rPrincDistance; // change from m to mm.
  FieldReal* fr = new FieldReal(root, SGRUiPrincDist, &rPrincDist);
  fr->SetIndependentPos();
  new FieldBlank(root, 1.1);
  int i;
	iNrFiducialInputFields = 4;
  for (i = 0; i < iNrFiducialInputFields; ++i) {
    ffm[i] = new FieldFiducialMark(root, 1+i, fGeoRef);
//    ffm[i]->cb->SetCallBack((NotifyProc)&EditFiducialMarksForm::calc,this);
  }
  for (i = 1; i < iNrFiducialInputFields; ++i) 
    ffm[i]->Align(ffm[i-1], AL_UNDER);
  for (i = 0; i < cs->iNrFiduc; ++i) {
    FieldFiducialMark* fld = ffm[i];
    fld->rDigGridX() = cs->rScannedFidRow[i];
    fld->rDigGridY() = cs->rScannedFidCol[i];
    SetDirty(fld->cDG());
    fld->crdPhoto.x = cs->cPhotoFid[i].x * 1000;
    fld->crdPhoto.y = cs->cPhotoFid[i].y * 1000;
    fld->fDigGrid = !fld->cDG().fUndef();
    fld->fPhoto = !fld->crdPhoto.fUndef();
    fld->fcDigGrid->SetVal(fld->crdDigGrid);
    fld->fcPhoto->SetVal(fld->crdPhoto);
    fld->fOk = fld->fDigGrid && fld->fPhoto;
    fld->cb->SetVal(fld->fOk);
  }
    
  FieldGroup* fg = new FieldGroup(root);
  StaticText *st;
  int iH;
  st = new StaticText(fg, SDGRemPhoto);
  iH = ffm[0]->fcPhoto->psn->iHeight + ffm[0]->fcPhoto->psn->iBndDown + 30;
  st->Align(ffm[0]->fcPhoto, AL_UNDER, -iH);
  st = new StaticText(fg, SDGRemDigGridXY);
  iH = ffm[0]->fcDigGrid->psn->iHeight + ffm[0]->fcDigGrid->psn->iBndDown + 30;
  st->Align(ffm[0]->fcDigGrid, AL_UNDER, -iH);
  st = new StaticText(fg, SDGRemDiffGridX);
  iH = ffm[0]->stDCol->psn->iHeight + ffm[0]->stDCol->psn->iBndDown + 30;
  st->Align(ffm[0]->stDRow, AL_UNDER, -iH);
  st = new StaticText(fg, SDGRemDiffGridY);
  iH = ffm[0]->stDRow->psn->iHeight + ffm[0]->stDRow->psn->iBndDown + 30;
  st->Align(ffm[0]->stDCol, AL_UNDER, -iH);

  iActDigPnt = 0;
  ffmCurr()->fAct = true;
//  new FieldBlank(root);
  FieldGroup* fgInfo = new FieldGroup(root);
  fgInfo->SetIndependentPos();
  fgInfo->Align(ffm[3], AL_UNDER);

  String sFill('X', 64);
	String sFill90('X', 90);
  stRem1 = new StaticText(fgInfo, sFill90);
	stRem1->SetIndependentPos();
  stRem2 = new StaticText(fgInfo, sFill90);
	stRem2->SetIndependentPos();
	stRem3 = new StaticText(fgInfo, sFill90);
	stRem3->SetIndependentPos();
  fgInfo->SetCallBack((NotifyProc)&EditFiducialMarksForm::initFMF);

  SetMenHelpTopic(htpGrfFiducialMarks);

  for (i = 0; i < iNrFiducialInputFields; ++i)
    ffm[i]->SetCallBack((NotifyProc)&EditFiducialMarksForm::calc,this);
}

EditFiducialMarksForm::~EditFiducialMarksForm()
{
  GetParent()->PostMessage(WM_COMMAND, ID_EDITGRFSTOPFIDMARKS, 0);
	if (fGeoRef)
		for (int i = 0; i < iNrFiducialInputFields; ++i) {
			SetDirty(ffm[i]->rc());
		}
	else
		for (int i = 0; i < iNrFiducialInputFields; ++i) {
			SetDirty(ffm[i]->cDG());
		}
  delete brNormal;
  delete brActive;
}

int EditFiducialMarksForm::ChangePDistCallBack(class Event *)
{
	frPrincDist->StoreData();
	grf->rPrincDistance = rPrincDist/1000; // change from mm to m.
	  int iNrFiduc = 0;
	calc(0);
	grf->fChanged = true;
	return 0;
}

int EditFiducialMarksForm::ChangePPointCallBack(class Event *)
{
	fcPrincPoint->StoreData();
	grf->cCameraPPoint.x = cCameraPrincPmm.x/1000;
	grf->cCameraPPoint.y = cCameraPrincPmm.y/1000;
	calc(0);
	grf->fChanged = true;
	return 0;
}

int EditFiducialMarksForm::TransformationChange(Event*)
{
	rgTransf->StoreData();
	switch (iTransformOption)
	{
		case 0:
			grf->transfFid = SolveOrthoPhoto::CONFORM;
			break;
		case 1:
			grf->transfFid = SolveOrthoPhoto::AFFINE;
			break;
		case 2:
			grf->transfFid = SolveOrthoPhoto::BILINEAR;
			break;
		case 3:
			grf->transfFid = SolveOrthoPhoto::PROJECTIVE;
			break;
		default:
			grf->transfFid = SolveOrthoPhoto::AFFINE;
	}
	calc(0);
	grf->fChanged = true;
	return 0;
}

int EditFiducialMarksForm::exec()
{
  FormWithDest::exec();
  if (fGeoRef) // fiducial marks from scanned photo (backgr raster map) 
  {
	  grf->rPrincDistance = rPrincDist / 1000; // change from mm to m.
	  int iNrFiduc = 0;
	  for (int i = 0; i < iNrFiducialInputFields; ++i) {
			if (ffm[i]->fOk) {
				grf->rScannedFidRow[iNrFiduc] = ffm[i]->rRow();
				grf->rScannedFidCol[iNrFiduc] = ffm[i]->rCol();
				grf->cPhotoFid[iNrFiduc].x  = ffm[i]->crdPhoto.x / 1000;
				grf->cPhotoFid[iNrFiduc].y  = ffm[i]->crdPhoto.y / 1000;
				iNrFiduc += 1;
			}
	  }
	  grf->iNrFiduc = iNrFiduc;
	  grf->fChanged = true;
	}
	else // fiducial marks from photo (-copy) on digitizer tablet (backgr is vector map)
  {
	  cs->rPrincDistance = rPrincDist / 1000; // change from mm to m.
	  int iNrFiduc = 0;
	  for (int i = 0; i < iNrFiducialInputFields; ++i) {
			if (ffm[i]->fOk) {
				cs->rScannedFidRow[iNrFiduc] = ffm[i]->rDigGridX();
				cs->rScannedFidCol[iNrFiduc] = ffm[i]->rDigGridY();
				cs->cPhotoFid[iNrFiduc].x  = ffm[i]->crdPhoto.x / 1000;
				cs->cPhotoFid[iNrFiduc].y  = ffm[i]->crdPhoto.y / 1000;
				iNrFiduc += 1;
			}
	  }
	  cs->iNrFiduc = iNrFiduc;
	  cs->fChanged = true;
	}
  return 0;
}

void EditFiducialMarksForm::OnLButtonDown(Coord c)
{
  FieldFiducialMark* ffm;
  ffm = ffmCurr();
  SetDirty(ffm->cDG());
  ffm->cDG() = c;
  ffm->fcDigGrid->SetVal(c);
  ffm->fDigGrid = true;
  ffm->cb->SetVal(ffm->fDigGrid);
  calc(0);
  SetDirty(ffm->cDG());
  ffmNext();
}

void EditFiducialMarksForm::OnLButtonDownRC(Coord c)
{
  FieldFiducialMark* ffm;
  ffm = ffmCurr();
  ffm->rRow() = c.x;
  ffm->rCol() = c.y;
  ffm->frc->SetVal(c);
  ffm->fRowCol = true;
  ffm->cb->SetVal(ffm->fRowCol);
  calc(0);
  SetDirty(ffm->c());
  ffmNext();
}


void EditFiducialMarksForm::SetDirty(RowCol rc)
{
//	SetDirty(mpv->pntPos(rc));
}

void EditFiducialMarksForm::SetDirty(Coord crd)
{
  SetDirty(mpv->pntPos(crd));
}

void EditFiducialMarksForm::SetDirty(zPoint p)
{
  Symbol smb;
  smb.smb = smbPlus;
  zRect rect(p,p);
  rect.top()   -= smb.iSize / 2 + 1;
  rect.left()  -= smb.iSize / 2 + 1;
  rect.bottom()+= smb.iSize / 2 + 2;
  rect.right() += smb.iSize / 2 + 2;

	CClientDC cdc(mpv);
  zPoint pntText = smb.pntText(&cdc, p);
  CSize siz = cdc.GetTextExtent("4", 1);
  pntText.x += siz.cx + 1;
  pntText.y += siz.cy + 1;
  rect.bottom() = max(rect.bottom(), pntText.y);
  rect.right() = max(rect.right(), pntText.x);
  mpv->InvalidateRect(&rect);  
}

int EditFiducialMarksForm::draw(CDC* cdc, zRect rect, Positioner* psn)
{
  Symbol smb;
  smb.smb = smbPlus;
  smb.col = colFidMarks;
	cdc->SetTextAlign(TA_LEFT|TA_TOP); //	default
  cdc->SetBkMode(TRANSPARENT);
  cdc->SetTextColor(smb.col);
  for (int i = 0; i < iNrFiducialInputFields; ++i) {
    if (!ffm[i]->fRowCol && !ffm[i]->fDigGrid)
      continue;
		zPoint pnt;
		if (ffm[i]->fRowCol) {
			double rX = ffm[i]->rRow();
			double rY = ffm[i]->rCol();
			pnt = psn->pntPos(rX-0.5,rY-0.5);
		}
		else {
			Coord c = ffm[i]->cDG();
			//Coord crdMap = cs->cConv(c);
			//Coord crdMap = cs->cConvToOther(c);
			pnt = psn->pntPos(c);
		}
    zPoint pntText = smb.pntText(cdc, pnt);
    String s("%i", i+1);
    cdc->TextOut(pnt.x,pnt.y,s.sVal());
    smb.drawSmb(cdc, 0, pnt);
  }  
  return 0;
}

int EditFiducialMarksForm::drawPrincPoint(CDC* cdc, zRect rect, Positioner* psn)
{
  Symbol smb;
  smb.smb = smbCross;
	smb.col = colFidMarks;
	cdc->SetTextAlign(TA_LEFT|TA_TOP); //	default
  cdc->SetBkMode(TRANSPARENT);
  cdc->SetTextColor(smb.col);
	zPoint pnt;
	if (fGeoRef) {
		//RowCol rcPP = grf->rcGetPrincipalPoint();
		//Coord crdPP = grf->crdGetPrincipalPoint();// this is obtained from intersection of fiducial connectors in scan geom
		//Coord crdPP = cCameraPrincP;  // this is input from user (and if not, by default 0,0 in th affine inner orientation
		Coord crdPP = grf->m_cScanPrincPoint;
		pnt = psn->pntPos(crdPP.x-0.5,crdPP.y-0.5);
	}
	else {
		Coord crdPP = cs->crdGetPrincipalPoint();
		pnt = psn->pntPos(crdPP);
	}
  zPoint pntText = smb.pntText(cdc, pnt);
  String s("PP");
  cdc->TextOut(pnt.x,pnt.y,s.sVal());
  smb.drawSmb(cdc, 0, pnt);
	return 0;
}

void EditFiducialMarksForm::SetActDigPoint(FieldFiducialMark* fld)
{
  ffm[iActDigPnt]->fAct = false;
  ffm[iActDigPnt]->cb->setDirty();
  for (int i = 0; i < iNrFiducialInputFields; ++i)
    if (ffm[i] == fld) {
      iActDigPnt = i;
      break;
    }
  ffm[iActDigPnt]->fAct = true;
  ffm[iActDigPnt]->cb->setDirty();
}

FieldFiducialMark* EditFiducialMarksForm::ffmCurr()
{
  return ffm[iActDigPnt];
}

FieldFiducialMark* EditFiducialMarksForm::ffmNext()
{ 
  ffmCurr()->fAct = false;
  ffmCurr()->cb->setDirty();
  ++iActDigPnt %= 8;
  ffmCurr()->fAct = true;	
  ffmCurr()->fcPhoto->SetFocus();
  ffmCurr()->cb->setDirty();
  return ffmCurr();
}
  
int EditFiducialMarksForm::initFMF(Event*)
{
  calc(0);
  return 0;
}

int EditFiducialMarksForm::calc(Event*)
{
  int i, j;
  double rScannedFidRow[8], rScannedFidCol[8];
  Coord cPhotoFid[8], cResiduals[8], cScanPrincPoint;
  double rPhoto2ScanScale;
  int iNrFiduc = 0;
  for (i = 0; i < iNrFiducialInputFields; ++i) {
    ffm[i]->StoreData();
    if (ffm[i]->fOk) {
			if (fGeoRef) // fiducial marks from scanned photo (backgr raster map) 
			{
				rScannedFidRow[iNrFiduc] = ffm[i]->rRow();
				rScannedFidCol[iNrFiduc] = ffm[i]->rCol();
      }
			else  // fiducial marks from photo (-copy) on digitizer tablet 
			{
				rScannedFidRow[iNrFiduc] = ffm[i]->rDigGridX();
				rScannedFidCol[iNrFiduc] = ffm[i]->rDigGridY();
      }
			cPhotoFid[iNrFiduc].x  = ffm[i]->crdPhoto.x / 1000;
      cPhotoFid[iNrFiduc].y  = ffm[i]->crdPhoto.y / 1000;
      iNrFiduc += 1;
    }  
  }
	int iErr;
	//if (grf->transf == GeoRefOrthoPhoto::CONFORM)
	if (iTransformOption == 0)
	  iErr = grf->iFindConformInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, 
		rScannedFidCol, rPhoto2ScanScale, grf->rScanPrincPointRow, grf->rScanPrincPointCol, 
									cScanPrincPoint, cResiduals);
	else if (iTransformOption > 1)
		iErr = grf->iComputeInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, 
		rScannedFidCol, rPhoto2ScanScale, grf->rScanPrincPointRow, grf->rScanPrincPointCol, 
									cScanPrincPoint, cResiduals);
	else if (iTransformOption == 1)
		iErr = grf->iFindAffineInnerOrientation(iNrFiduc, cPhotoFid, rScannedFidRow, 
		rScannedFidCol, rPhoto2ScanScale, grf->rScanPrincPointRow, grf->rScanPrincPointCol, 
									cScanPrincPoint, cResiduals);
	int iMinNrFiducials = min(iTransformOption + 2, 4);
	String sMinNrF(SDGErrLessFiducialsThanNeed_i.scVal(), iMinNrFiducials);
	switch (iErr) {
    case -11:
      stRem1->SetVal(SDGErrNoValidFiducials);
      break;
    case -12:
      stRem1->SetVal(sMinNrF);
      break;
    case -13:
      stRem1->SetVal(SDGErrIncorrectFiducialPos);
      break;
    case -14:
      stRem1->SetVal(SDGErrIncorrectFiducialOrder);
      break;
    case -15:
      stRem1->SetVal(SDGErrWrongNumber);
      break;
    default:
      stRem1->SetVal("                    ");
      break;
  }
  
	fcPrincPoint->SetVal(Coord(grf->cCameraPPoint.x*1000,grf->cCameraPPoint.y*1000));
	grf->m_cScanPrincPoint = cScanPrincPoint;
  stRem2->SetVal(" ");
	stRem3->SetVal(" ");
	double rDRowSqTotal = 0;
	double rDColSqTotal = 0;
  bool fOk = iErr == 0;
  for (i = 0, j = 0; i < iNrFiducialInputFields; ++i) {
    if (ffm[i]->fOk) {
			rDRowSqTotal += cResiduals[j].x*cResiduals[j].x;
			rDColSqTotal += cResiduals[j].y*cResiduals[j].y;
      ffm[i]->rDRow = cResiduals[j].x;
      ffm[i]->rDCol = cResiduals[j].y;
	  char txt[20];
	  sprintf(txt, "%f", cResiduals[j].x);
      ffm[i]->stDRow->SetVal(String(txt));
	  sprintf(txt, "%f", cResiduals[j].y);
	  ffm[i]->stDCol->SetVal(txt);
      j++;
    }
    ffm[i]->ShowVals();
  }
	double rSigmaDRow = sqrt(rDRowSqTotal/j);
	double rSigmaDCol = sqrt(rDColSqTotal/j);
  if (fOk)
    EnableOK();
  else
    DisableOK();
  if (fOk) {
    double rMMpix = 1000 / rPhoto2ScanScale;
		String sDPI, sDPmm, sMMpix, sPPinRowCols, sRMSrow, sRMScol;
		if (fGeoRef) {
			double rDPI = abs(0.0254 * rPhoto2ScanScale); // dots per inch
			sDPI = String("%S %.2f dpi		                                    RMS:", SDGMsgScanResolution, rDPI);
			if (rSigmaDRow <= 10)
				sRMSrow = String("%.2f rows,",rSigmaDRow);
			else
				sRMSrow = String(">10 rows !,");
			if (rSigmaDCol <= 10)
				sRMScol = String("%.2f cols",rSigmaDCol);
			else
				sRMScol = String(">10 cols !");
			sMMpix = String("%.2f mm / %S", rMMpix, SDGMsgPixel);
			sPPinRowCols = String("Principal Point in scan: ");
			if (grf->fSubPixelPrecision) {
				sPPinRowCols &= String("Row %.2f, ", grf->rScanPrincPointRow);
				sPPinRowCols &= String("Col %.2f ", grf->rScanPrincPointCol);
				sMMpix &= String(" (Fiducials and ground-control in sub-pixel precision)");
			}
			else {
				sPPinRowCols &= String("Row %.0f, ", grf->rScanPrincPointRow);
				sPPinRowCols &= String("Col %.0f ", grf->rScanPrincPointCol);
			}
		}
		else { // when digitizing from digitizer tablet
			double rDPmm = 0.001 * rPhoto2ScanScale; //dots per mm
			sDPmm = String("%S %.2f grid units per mm", SDGMsgDigGridResolution, rDPmm);
			sMMpix = String("%.2f mm / %S", rMMpix, SDGMsgDigGridUnit);
		}
    stRem1->SetVal(sDPI);
    stRem2->SetVal(sMMpix);
		stRem3->SetVal(sPPinRowCols);
		stRMSrow->SetVal(sRMSrow);
		stRMScol->SetVal(sRMScol);
  }
	mpv->Invalidate(); // Bas: (temporary try-out): redraw pane after each change in the form
  return 0;
}

HBRUSH EditFiducialMarksForm::OnCtlColor(CDC* cdc, CWnd* pWnd, UINT nCtlColor)
{
	if (nCtlColor == CTLCOLOR_BTN) {
    for (int i = 0; i < iNrFiducialInputFields; ++i) {
      if (ffm[i]->cb->handle() == pWnd->m_hWnd) {
        if (ffm[i]->fAct) {
					cdc->SetTextColor(clrActive);
					cdc->SetBkColor(clrBackActive);
					return *brActive;
        }
        else {
					cdc->SetTextColor(clrNormal);
					cdc->SetBkColor(clrBackNormal);
					return *brNormal;
        }
      }
		}
  }
	return FormBaseDialog::OnCtlColor(cdc, pWnd, nCtlColor);
}

BOOL EditFiducialMarksForm::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (EN_SETFOCUS == HIWORD(wParam)) 
	{
		int iId = LOWORD(wParam);
		int iField = 0;
		for (int i = 0; i < iNrFiducialInputFields; ++i) {
			if (iId > ffm[i]->Id()) {
				iField = i;
			}
		}
		SetActDigPoint(ffm[iField]);
		return TRUE;
	}
	else
		return FormWithDest::OnCommand(wParam, lParam);
}

