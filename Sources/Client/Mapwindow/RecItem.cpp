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
/* RecItem, RecItemMap, RecItemPolMap, 
RecItemTable, RecItemColumn,
RecItemInt, RecItemCoord
by Wim Koolhoven
(c) Ilwis System Development ITC
Last change:  WK   24 Jul 97    1:03 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#include "Client\Mapwindow\RECITEM.H"
#include "Engine\Map\Feature.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\ilwis.h"
#include "Client\Editors\Map\FeatureSetEditor.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include <stdlib.h>
#include "Headers\Hs\PIXINFO.hs"
#include "Engine\Domain\Dmvalue.h"

using namespace ILWIS;

RecItem::RecItem(RecItem* parent)
: children(true), cwcs(crdUNDEF)
{
	riParent = parent;
	fShow = true;
	fShowChildren = true;
	fValid = false;
	fResValid = false;
	fStrValid = false;
	fEditable = false;
	cwcs = crdUNDEF;
	associatedDrawer = 0;
}

RecItem::~RecItem()
{
	// members of children will be deleted automatically
}

void RecItem::setAssociatedDrawer(NewDrawer *drw) {
	associatedDrawer = drw;
	for (SLIterP<RecItem> iter(&children); iter.fValid(); ++iter) {
		iter()->setAssociatedDrawer(drw);
	}

}

bool RecItem::fAllowEdit() {
	if ( associatedDrawer) {
		ILWIS::BaseMapEditor* edit = associatedDrawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->editGet();

		return associatedDrawer->inEditMode() && edit->hasSelection();
	}
	return fEditable; 
}

void RecItem::AllowEdit(bool fEdit) {
	fEditable = fEdit;
}

int RecItem::AddSelfToArray(RecItemArray& ria)
{
	if (fShow)
		ria &= this;
	if (fShowChildren)
		for (SLIterP<RecItem> iter(&children); iter.fValid(); ++iter)
			iter()->AddSelfToArray(ria);
	return 0;
}

int RecItem::AddSelfAlwaysToArray(RecItemArray& ria)
{
	ria &= this;
	if (fShowChildren)
		for (SLIterP<RecItem> iter(&children); iter.fValid(); ++iter)
			iter()->AddSelfAlwaysToArray(ria);
	return 0;
}

RecItemMap* RecItem::AddMap(BaseMap map, ILWIS::NewDrawer *drw)
{
	map->MakeUsable();
	RecItemMap* ri = new RecItemMap(this,map, drw);
	children.append(ri);
	return ri;
}

RecItemCoordSystem* RecItem::AddCoordSystem(const CoordSystem& csy)
{
	RecItemCoordSystem* ri = new RecItemCoordSystem(this,csy);
	children.append(ri);
	return ri;
}

RecItemGeoRef* RecItem::AddGeoRef(const GeoRef& grf)
{
	RecItemGeoRef* ri = new RecItemGeoRef(this, grf);
	children.append(ri);
	return ri;
}

RecItemTable* RecItem::AddTable(Table tbl, ILWIS::NewDrawer *drw) {
	tbl->MakeUsable();
	RecItemTable* ri = new RecItemTable(this,tbl);
	for (int i = 0; i < tbl->iCols(); ++i)
		ri->AddColumn(tbl->col(i), drw);
	children.append(ri);
	return ri;
}

RecItemTable* RecItem::AddTable(Table tbl, bool fAllowEdit)
{
	tbl->MakeUsable();
	RecItemTable* ri = new RecItemTable(this,tbl);
	for (int i = 0; i < tbl->iCols(); ++i)
		ri->AddColumn(tbl->col(i), fAllowEdit);
	ri->AllowEdit(fAllowEdit);
	children.append(ri);
	return ri;
}

/*
RecItemTable* RecItem::AddTable(TableView* view, bool fAllowEdit)
{
RecItemTable* ri = new RecItemTable(this,view);
for (int i = 0; i < view->iCols(); ++i)
ri->AddColumn(view->Col(i), fAllowEdit);
ri->AllowEdit(fAllowEdit);
children.append(ri);
return ri;
}
*/

int RecItem::Changed()
{
	if (fValid) {
		fValid = false;
		for (SLIterP<RecItem> iter(&children); iter.fValid(); ++iter)
			iter()->Changed();
	}
	fResValid = false;
	fStrValid = false;
	return 0;
}

void RecItem::updateView(const IlwisObject& obj) {
	list<CDocument *> docs;
	IlwWinApp()->getDocumentList(docs);
	for(list<CDocument *>::iterator cur = docs.begin(); cur != docs.end(); ++cur) {
		IlwisDocument *idoc = dynamic_cast<IlwisDocument *>(*cur);
		if ( idoc) {
			if(idoc->usesObject(obj)) {
				// for the moment not, this is a far to expensive operation for the updayte of a cell of the pixview
				/*MapCompositionDoc *mdoc = dynamic_cast<MapCompositionDoc *>(idoc);
				if ( mdoc) {
					NewDrawer *drw = mdoc->getDrawerFor(obj);
					ILWIS::PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
					drw->prepare(&pp);
				}*/
				idoc->UpdateAllViews(0,2);
			}
		}
	}
}

String RecItem::sName() const
{
	return "";
}

FileName RecItem::fnObj() const
{
	return FileName();
}

long RecItem::iValue()
{
	return iUNDEF;
}

const String& RecItem::sValue(int iWidth)  // text to display
{
	sVal = "?";
	if (iWidth > 1)
		sVal &= String(' ', iWidth - 1);
	return sVal;
}

const CoordWithCoordSystem& RecItem::crdValue()
{
	return cwcs;
}

void RecItem::SetValue(const char*)
{
}

int RecItem::Configure(CWnd* win)
{
	MessageBeep(MB_ICONASTERISK);
	return 0;
	/*  
	String sTitle = "RecItem ";
	sTitle &= sName();
	sTitle &= " Error";
	zMessage msg(win, SDNoCnfFrm, sTitle,
	MB_OK | MB_ICONEXCLAMATION);
	return 0;
	*/  
}

RecItemMap::RecItemMap(RecItem* parent, BaseMap map, ILWIS::NewDrawer *drw)
: RecItem(parent), _map(map)
{
	associatedDrawer = drw;
	Table tbl = _map->tblAtt();
	if (tbl.fValid()) {
		RecItemTable* rit = AddTable(tbl,drw);
		rit->ShowRec(false);
	} 
	cwcs = _map->cs();
}

String RecItemMap::sName() const
{
	FileName fn = map()->fnObj;
	String s = fn.sFile;
	s &= fn.sExt;
	return s;
}

FileName RecItemMap::fnObj() const
{
	return map()->fnObj;
}

long RecItemMap::iValue()   // pixel value
{
	if (!fValid) {
		fValid = true;
		CoordWithCoordSystem cPar = riParent->crdValue();
		cwcs = cwcs->cConv(cPar, cPar);
		if (abs(cwcs.c().x) > 1e10 || abs(cwcs.c().y) > 1e10) {
			fResValid = true;
			iVal = iUNDEF;
		}
		else
			fResValid = false;
	}
	if (!fResValid) {
		fResValid = true;
		iVal = map()->iRaw(cwcs);
	}
	return iVal;
}

void RecItemMap::PutVal(const String& s) {
	//if ( !fAllowEdit())
	//	return;
	IObjectType type = IOTYPE(fnObj());
	if ( type == IlwisObject::iotRASMAP) {
		Map bmp(fnObj());
		Coord c = crdValue();
		bmp->PutVal(c,s);
		iVal = bmp->dvrs().iRaw(s);
		sVal = s;
		updateView(bmp);
	} else if ( type == IlwisObject::iotSEGMENTMAP || 
		type == IlwisObject::iotPOINTMAP || 
		type == IlwisObject::iotPOLYGONMAP) {
			BaseMap bmp(fnObj());
			vector<Geometry *> features = bmp->getFeatures(crdValue());
			for(int i=0; i < features.size(); ++i) {
				Feature *f = CFEATURE(features[i]);
				f->PutVal(bmp->dvrs(), s);
			}
			iVal = bmp->dvrs().iRaw(s);
			if ( iVal != iUNDEF){
				sVal = s;
				bmp->fChanged = true;
				updateView(bmp);
			}
	}
}

const String& RecItemMap::sValue(int iWidth)   // meaning
{
	if (!fValid) {
		fValid = true;
		CoordWithCoordSystem cPar = riParent->crdValue();
		if (cPar.c().fUndef())
		{
			fResValid = true;
			fStrValid = true;
			iVal = iUNDEF;
			sVal = String("%-*S", 45, SPIErrNoCoordinate);
		}
		else {
			cwcs = cwcs->cConv(cPar, cPar);
			if (cwcs.c().fUndef()) {
				fResValid = true;
				fStrValid = true;
				iVal = iUNDEF;
				sVal = String("%-*S", 45, SPIErrImpossibleCoord);
			}
			else if (!map()->cb().fContains(cwcs.c())) {
				fResValid = true;
				fStrValid = true;
				iVal = iUNDEF;
				sVal = String("%-*S", 45, SPIErrOutsideMap);
			}
			else
				fStrValid = false;
		}
	}
	if (!fStrValid) {
		fStrValid = true;
		sVal = map()->sValue(cwcs,iWidth);
		DomainValue* dv = map()->dm()->pdv();
		if (dv && dv->fUnit())
			sVal = String("%S %S", sVal, dv->sUnit());
	}
	return sVal;
}

int RecItemMap::Configure(CWnd* win)
{
	return RecItem::Configure(win);
}

RecItemTable::RecItemTable(RecItem* parent, Table tbl)
: RecItem(parent)
{
	_tbl = tbl;
}

/*
RecItemTable::RecItemTable(RecItem* parent, TableView* view)
: RecItem(parent,view,"tbl")
{
}
*/

int RecItemTable::Configure(CWnd* win)
{
	return RecItem::Configure(win);
}

String RecItemTable::sName() const
{
	FileName fn = tbl()->fnObj;
	String s = fn.sFile;
	s &= fn.sExt;
	return s;
}

FileName RecItemTable::fnObj() const
{
	return tbl()->fnObj;
}

long RecItemTable::iValue()   // record number
{
	if (!fValid || !fResValid) {
		fValid = true;
		fResValid = true;
		iRec = riParent->iValue();
		if (iRec < 0 || iRec > tbl()->iRecs())
			iRec = iUNDEF;
	}
	return iRec;
}

const String& RecItemTable::sValue(int iWidth)	// key value
{
	if(!fValid || !fStrValid) {
		fStrValid = true;
		if (iValue() == iUNDEF) {
			sVal = "?";
			if (iWidth > 1)
				sVal &= String(' ', iWidth - 1);
		}
		else
			sVal = tbl()->dm()->sValueByRaw(iValue(), iWidth);
		//  sVal = String("%3li               ", iValue());
	}
	return sVal;
}

void RecItemTable::SetValue(const char* s)
{
	riParent->SetValue(s);
}

RecItemColumn* RecItemTable::AddColumn(Column col, bool fAllowEdit)
{
	RecItemColumn* ri = new RecItemColumn(this,col);
	ri->AllowEdit(fAllowEdit);
	children.append(ri);
	return ri;
}

RecItemColumn* RecItemTable::AddColumn(Column col, ILWIS::NewDrawer *drw)
{
	RecItemColumn* ri = new RecItemColumn(this,col, drw);
	children.append(ri);
	return ri;
}

RecItemColumn::RecItemColumn(RecItem* parent, Column col)
: RecItem(parent)
{
	_col = col;
}

RecItemColumn::RecItemColumn(RecItem* parent, Column col, ILWIS::NewDrawer *drw)
: RecItem(parent)
{
	associatedDrawer = drw;
	_col = col;
}

int RecItemColumn::Configure(CWnd* win)
{
	return RecItem::Configure(win);
}

String RecItemColumn::sName() const
{
	String s = col()->sName();
	s &= ".clm";
	return s;
}

FileName RecItemColumn::fnObj() const {
	FileName fn(col()->fnTbl);
	fn.sCol = col()->sName();
	return fn;
}

void RecItemColumn::PutVal(const String& s) {
	//if ( !fAllowEdit())
	//	return;
	_col->PutVal(iRec, s);
	iVal = _col->dvrs().iRaw(s);
	sVal = s;
	Table tbl(_col->fnTbl);
	updateView(tbl);
}

long RecItemColumn::iValue()
{
	if (!fValid) {
		fValid = true;
		long iTmp = riParent->iValue();
		if (iTmp != iRec) {
			if (iTmp < 0) {
				iRec = iUNDEF;
				fResValid = true;
				iVal = iUNDEF;
			}
			else {
				iRec = iTmp;
				fResValid = false;
			}
		}
	}
	if (!fResValid) {
		fResValid = true;
		iVal = col()->iRaw(iRec);
	}
	return iVal;
}

const String& RecItemColumn::sValue(int iWidth)
{
	if(!fValid || !fStrValid) {
		fStrValid = true;
		iValue();
		sVal = col()->sValue(iRec,iWidth);  // width ????
		DomainValue* dv = col()->dm()->pdv();
		if (dv && dv->fUnit())
			sVal = String("%S %S", sVal, dv->sUnit());
	}
	return sVal;
}

const CoordWithCoordSystem& RecItemColumn::crdValue()
{
	return cwcs;
}

void RecItemColumn::SetValue(const char* s)
{
	//  String str = s;
	//  col()->SetWrite(true);
	//  col()->PutVal(iRec,str);
	//  Changed();
}


RecItemInt::RecItemInt(long i)
: RecItem(NULL)
{
	iVal = i;
	sNam = ""; //SDRecord;
	sValue(0);
	fShow = false;
}

int RecItemInt::Configure(CWnd* win)
{
	return RecItem::Configure(win);
}

String RecItemInt::sName() const
{
	return sNam;
}

const String& RecItemInt::sValue(int iWidth)
{
	if (!fValid || !fStrValid) {
		fStrValid = true;
		sVal = String("%-*li", 23, iValue());
	}
	return sVal;
}

long RecItemInt::iValue()
{
	fValid = true;
	fResValid = true;
	return iVal;
}

void RecItemInt::SetValue(long i)
{
	iVal = i;
	RecItem::Changed();
}

void RecItemInt::SetValue(const char* s)
{
	long iNew = atol(s);
	SetValue(iNew);
}


RecItemCoord::RecItemCoord(RecItem* parent, const CoordWithCoordSystem& c)
: RecItem(parent)
{
	cwcs = c;
	sNam = String("%S.crd", SPIRemCoordinate);
	sValue(0);
}

int RecItemCoord::Configure(CWnd* win)
{
	return RecItem::Configure(win);
	/*
	class Form: public FormWithDest
	{
	public:
	Form(CWnd* win, bool* fContinuous)
	: FormWithDest(win, SDUpdCoord)
	{
	new CheckBox (root, SDUiContUpd, fContinuous);
	SetMenHelpTopic(htpCrdCnf);
	create();
	}
	};
	bool fCont = fContinuous;
	Form frm(win,&fCont);
	fContinuous = fCont;
	return frm.fOkClicked();
	*/
}

String RecItemCoord::sName() const
{
	return sNam;
}

const String& RecItemCoord::sValue(int iWidth)
{
	if (!fValid || !fStrValid) {
		fStrValid = true;
		Coord c = crdValue();
		if (c.fUndef())
			sVal = String("%-*S", 45, SPIErrNoCoordinate);
		else
			sVal = cwcs->sValue(c, iWidth);
		/*    
		if (abs(c.x) > 1e10 || abs(c.y) > 1e10)
		sVal = String("(??????????,??????????)    ");
		else
		sVal = String("(%10.1f,%10.1f)    ", c.x, c.y);
		*/      
	}
	return sVal;
}

const CoordWithCoordSystem& RecItemCoord::crdValue()
{
	fValid = true;
	fResValid = true;
	return cwcs;
}

void RecItemCoord::SetValue(const char*)
{
}

void RecItemCoord::SetValue(const CoordWithCoordSystem& c)
{
	cwcs = c;
	RecItem::Changed();
}

RecItemCoordSystem::RecItemCoordSystem(RecItem* parent, const CoordSystem& cs)
: RecItemCoord(parent, cs)
{
	sNam = cs->sName(true);
}

const CoordWithCoordSystem& RecItemCoordSystem::crdValue()
{
	CoordWithCoordSystem cPar = riParent->crdValue();
	cwcs = cwcs->cConv(cPar, cPar);
	return cwcs;
}

FileName RecItemCoordSystem::fnObj() const
{
	return cwcs->fnObj;
}

RecItemGeoRef::RecItemGeoRef(RecItem* parent, const GeoRef& georef)
: RecItem(parent)
, grf(georef)
{
	cwcs = grf->cs();
}

String RecItemGeoRef::sName() const
{
	FileName fn = grf->fnObj;
	String s = fn.sFile;
	s &= fn.sExt;
	return s;
}

FileName RecItemGeoRef::fnObj() const
{
	return grf->fnObj;
}

const String& RecItemGeoRef::sValue(int iWidth)
{
	if(!fStrValid) 
	{
		RowCol rc = rcValue();
		if (rc.fUndef()) 
		{
			sVal = "?,?";
			if (iWidth > 3)
				sVal &= String(' ', iWidth - 3);
		}
		else 
		{
			rc.Row += 1;
			rc.Col += 1;
			if (iWidth > 0)
			{
				iWidth -= 1; // the comma
				int iW1 = iWidth / 2;
				int iW2 = iWidth - iW1;
				sVal = String("%li:*,%li:*", rc.Row, iW1, rc.Col, iW2);
			}
			else
				sVal = String("%li,%li", rc.Row, rc.Col);
		}
		fStrValid = true;
	}
	return sVal;
}

RowCol RecItemGeoRef::rcValue()
{
	CoordWithCoordSystem cPar = riParent->crdValue();
	cwcs = cwcs->cConv(cPar, cPar);
	RowCol rcSize = grf->rcSize();
	RowCol rc = grf->rcConv(cwcs);
	if (rc.Row < 0 || rc.Row >= rcSize.Row ||
		rc.Col < 0 || rc.Col >= rcSize.Col)
		return RowCol();
	return rc;
}
