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
// RecordView.cpp: implementation of the RecordView class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\FormElements\syscolor.h"
#include "Headers\constant.h"
#include "Headers\Hs\Table.hs"
#include "Client\TableWindow\BaseTblField.h"
#include "Client\TableWindow\TableDoc.h"
#include "Engine\Table\Rec.h"
#include "Client\TableWindow\RecordView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class RecordEditField: public BaseTblField
{
public:
  RecordEditField(RecordView*, Ilwis::Record&, long iRow, TableView* tvw);
  ~RecordEditField();
private:
  Ilwis::Record& rec;
  TableView* tvw;
};

class RecordReadOnlyField: public BaseTblField
{
public:
  RecordReadOnlyField(RecordView*, Ilwis::Record&, long iRow);
  ~RecordReadOnlyField();
};

RecordEditField::RecordEditField(RecordView* view, Ilwis::Record& rc, long row, TableView* tv)
  : BaseTblField(view,0,row,rc.dm(row-1)), rec(rc), tvw(tv)
{
  if (iRow <= 0 || iRow > rec.iFields()+1) return;
  if (!rec.fEditable((int)iRow-1))
    return;
	String str = rec.sValue((int)iRow-1,0);
  init(str);
}

RecordEditField::~RecordEditField()
{
  if (fHasChanged()) 
  {
    String s = sText();
    int iCol = iRow - 1;
    Domain dm = rec.dm(iCol);
    DomainSort* ds = dm->pdsrt();
    if (0 != ds && !dm->fValid(s))
    {
      String sMsg(TR("%S is not in the domain %S\nAdd this item to the domain?").c_str(), s, ds->sName());
      int iRet = tbpn->MessageBox(sMsg.sVal(), TR("Invalid Value").c_str(),
                   MB_YESNO|MB_ICONEXCLAMATION);
      if (IDYES == iRet)
        ds->iAdd(s);
    }
    rec.PutVal(iCol, s);
		if (tvw)
			tvw->Updated();
		CDocument* doc = tbpn->GetDocument();
		if (doc)
			doc->UpdateAllViews(0);
  }
}

RecordReadOnlyField::RecordReadOnlyField(RecordView* view, Ilwis::Record& rec, long row)
: BaseTblField(view,0,row,true)
{
  String str = rec.sValue((int)iRow-1);
  init(str);
}

RecordReadOnlyField::~RecordReadOnlyField()
{
}

BEGIN_MESSAGE_MAP(RecordView, BaseTablePaneView)
	//{{AFX_MSG_MAP(RecordView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
  
RecordView::RecordView()
{
  iColWidth = new int[10];
  iColPix = new int[10];
  iButtonWidth = 10;
}

RecordView::~RecordView()
{
}

void RecordView::Create(CWnd* wPar)
{
	CView::Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
			CRect(0,0,0,0), wPar, 100, 0);
}


void RecordView::SetRecord(Ilwis::Record record, TableView* t)
{
  if (tField) delete tField;
  tField = 0;
  rec = record;
	Invalidate();
	tvw = t;
}

void RecordView::InitColPix(CDC* cdc)
{
	if (0 == iColPix || iCharWidth == 0)
		return;
  iColPix[0] = iButtonWidth * iCharWidth + 6;
	CRect rect;
	GetClientRect(rect);
  iColPix[1] = rect.Width();
	int iWidth = iColPix[1] - iColPix[0];
  iColWidth[0] = iWidth / iCharWidth;
}

int RecordView::iCols() const
{
	return 1;
}

long RecordView::iRows() const
{
	if (rec.fValid())
		return rec.iFields();
	else
		return 1;
}

void RecordView::OnFieldPressed(int iCol, long iRow, bool fLeft)
{
  if (tField) delete tField;
  tField = 0;
  if (iCol == 0 && iRow >= 1 && iRow <= iRows())
    if (fLeft)
      if (rec.fEditable((int)iRow-1))
        tField = new RecordEditField(this,rec,iRow, tvw);
	    else 
		    tField = new RecordReadOnlyField(this,rec,iRow);  
}

String RecordView::sColButton(int iCol) const
{
	if (!rec.fValid())
		return "";
  return rec.sKey();
}

String RecordView::sRowButton(long iRow) const
{
	if (!rec.fValid())
		return "";
  String s = rec.sCol((int)iRow-1);
	for (int i = s.length(); i < iButWidth(); i++)
		s &= ' ';
	return s;
}

String RecordView::sField(int iCol, long iRow) const
{
	if (!rec.fValid())
		return "";
  String sRes;
  iRow -= 1;
  sRes = rec.sValue((int)iRow);
  if (rec.dm(iRow)->pdv() && (0 == rec.dm(iRow)->pdbool())) {
    String s = sRes;
    int iAdd = 9;
    char* sStr = s.sVal();
    char* sDot = strchr(sStr,'.');
    if (0 == sDot) 
      iAdd -= s.length();
    else
      iAdd -= sDot - sStr;
    if (iAdd > 0) {  
      sRes = String(' ', iAdd);    
      sRes &= s;
    }
  }
  return sRes;
}

String RecordView::sDescrField(int iCol, long iRow) const
{
	if (!rec.fValid())
		return "";
  return String(TR("Edit field %S").c_str(), sRowButton(iRow));
}


bool RecordView::fAllowPaste() const
{
	if (0 == tvw)
		return false;
  return !tvw->fReadOnly() && !mmSelect.fUndef() && fValidSelection();
}
#define sMen(ID) ILWSF("men",ID).c_str()
#define add(ID) men.AppendMenu(MF_STRING, ID, sMen(ID)); 

void RecordView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
  CMenu men;
	men.CreatePopupMenu();
  add(ID_EDIT_COPY );
	men.EnableMenuItem(ID_EDIT_COPY, fAllowCopy() ? MF_ENABLED : MF_GRAYED);
/*  
  -- excluded because these options do not work - Wim 22/10/01
  add(ID_EDIT_PASTE);
	men.EnableMenuItem(ID_EDIT_PASTE, fAllowPaste() ? MF_ENABLED : MF_GRAYED);
  add(ID_CLEAR);
	men.EnableMenuItem(ID_CLEAR, fAllowClear() ? MF_ENABLED : MF_GRAYED);
  add(ID_EDIT);
  int iMinRow = mmSelect.MinRow();
  int iMaxRow = mmSelect.MaxRow();
	men.EnableMenuItem(ID_EDIT, (iMaxRow >= iMinRow) && (0 != tvw) && !tvw->fReadOnly()? MF_ENABLED : MF_GRAYED);
*/
  men.TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x, point.y, pWnd);
}

TableDoc* RecordView::GetDocument()
{
  return (TableDoc*)m_pDocument;
}

const TableDoc* RecordView::GetDocument() const
{
  return (const TableDoc*)m_pDocument;
}
