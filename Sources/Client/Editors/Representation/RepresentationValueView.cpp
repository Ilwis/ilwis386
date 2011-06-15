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
#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Headers\Hs\Represen.hs"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\FormElements\fldcolor.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Client\ilwis.h"
#include "Client\Base\BaseView.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\Domain\dm.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Representation\Rprgrad.h"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Utils\GradientBand.h"
#include "Client\Base\ButtonBar.h"
#include "Client\Base\Framewin.h"
#include "Client\Base\BaseView2.h"
#include "Client\Editors\Representation\RepresentationDoc.h"
#include "Client\Editors\Representation\RepresentationView.h"
#include "Client\Editors\Representation\RepresentationValueView.h"
#include "Headers\Htp\Ilwismen.htp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const static int ID_EDITOR_LISTBOX = 200;
const static int ID_VALUE_EDIT = 201;
const static int ID_COLORMETHOD_CB = 202;
const static int iLEFTEDGE = 65;

//-- [ InPlaceColorMethodCB ] --------------------------------------------------------------------
BEGIN_MESSAGE_MAP(InPlaceColorMethodCB, CComboBox)
	//ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

InPlaceColorMethodCB::InPlaceColorMethodCB()
{}

InPlaceColorMethodCB::~InPlaceColorMethodCB()
{
}

void InPlaceColorMethodCB::OnKillFocus( CWnd* pNewWnd )
{
	CComboBox::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);
}

//-- [ InPlaceValueEdit ] -----------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(InPlaceValueEdit, CEdit)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
END_MESSAGE_MAP()

InPlaceValueEdit::InPlaceValueEdit() : iIndex(iUNDEF), rMax(rUNDEF), rMin(rUNDEF), fIgnoreReturn(false)
{}

InPlaceValueEdit::~InPlaceValueEdit()
{}

BOOL InPlaceValueEdit::Create(RepresentationValueDoc *doc, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID )
{
	rpvDoc = doc;
	return CEdit::Create(dwStyle, rect, pParentWnd, nID);
}

void InPlaceValueEdit::SetEditId(int id)
{
	iIndex=id;
}

double InPlaceValueEdit::rValidValue(double rVal)
{
	ISTRUE(fINotEqual, rMax, rUNDEF);
	ISTRUE(fINotEqual, rMin, rUNDEF);
	String sName;
	ObjectInfo::ReadElement("Representation", "domain", rpvDoc->rpr()->fnObj, sName);	
	bool fUseSystemDomain = fCIStrEqual(sName, "value.dom");
	if ( !rpvDoc->fUsesGradual() && fUseSystemDomain)
	{
	  if ( iIndex == 0 && rVal > rMax)
		{
			MessageBeep(MB_ICONASTERISK);
			return rMax;
		}
		else if ( rpvDoc->prg()->iLimits() - 1 == iIndex && rVal < rMin )
		{
			MessageBeep(MB_ICONASTERISK);
			return rMin;
		}

		return rVal;
	}		


	rVal /= (rpvDoc->fRepresentationValue() ? 1.0 : 100.0);
	if ( rVal > rMax )
	{
		MessageBeep(MB_ICONASTERISK);
		return rMax;
	}
	if ( rVal < rMin )
  {
		MessageBeep(MB_ICONASTERISK);
		return rMin;
	}

	return rVal;
}

void InPlaceValueEdit::SetRange ( double rLMin, double rLMax )
{
	rMin = rLMin;
	rMax = rLMax;
}

void InPlaceValueEdit::OnKillFocus( CWnd* pNewWnd )
{
	CEdit::OnKillFocus(pNewWnd);
	OnKeyDown(VK_RETURN, 1, 0);
}

void InPlaceValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch ( nChar )
	{
		case VK_DOWN:
			{
				OnChar(VK_RETURN, 1, 0);
			}
			break;
		case VK_UP:
			{
				OnChar(VK_RETURN, 1, 0);
			}
			break;
		default:
			CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void InPlaceValueEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CString str;
	switch (nChar)
	{
		case VK_RETURN:
		{
			ISTRUE(fINotEqual, (long)iIndex, iUNDEF);
			GetWindowText(str);
			if (str != "")
      {
				double rVal = String(str).rVal();
				rVal= rValidValue(rVal);
				Color col = rpvDoc->GetColor(iIndex);
				rpvDoc->Edit(iIndex, rVal, col);
				RepresentationValueView *view = dynamic_cast<RepresentationValueView*>(rpvDoc->wndGetActiveView());
				view->rprBar.Invalidate();
				fIgnoreReturn = true;
			}
    }
		//fall through , escape does things return also needs
		case VK_ESCAPE:
			SetWindowText("");
			ShowWindow(SW_HIDE);
			iIndex=iUNDEF;
			break;
    default:
			CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

//-----------------------------------------------------------------------------------------------
// purpose : Catches alphabetic characters. They must be processed before MFC self processes them
// parameters : MSG *pMsg the messages that needs processing
// returns : FALSE if no further processing is wanted
//------------------------------------------------------------------------------------------------- 
BOOL InPlaceValueEdit::PreTranslateMessage(MSG* pMsg)
{
	CString str;
  if (pMsg->message == WM_KEYDOWN )
	{
		if ( pMsg->wParam == VK_DELETE )
		{
			SendMessage(WM_KEYDOWN, pMsg->wParam, pMsg->lParam);
			return FALSE;
		}
		else if ( isalpha(pMsg->wParam) )  
			return TRUE;
	}
  return CEdit::PreTranslateMessage(pMsg);
}

//-- [ RepresentationBar ] --------------------------------------------------------------
BEGIN_MESSAGE_MAP(RepresentationBar, CSizingControlBar)
	ON_WM_PAINT()
	ON_WM_SIZE()
END_MESSAGE_MAP()

RepresentationBar::RepresentationBar() : CSizingControlBar(), band(NULL), fFloating(false)
{
}

RepresentationBar::~RepresentationBar()
{
	delete band;
}

//-----------------------------------------------------------------------------------------------
// purpose : Initializes and creates the bar with the gradientBand. 
// parameters : CWnd *pParent	Parent of the bar
//              CView *v View that uses the bar
//              RepresentationValueDoc *doc document that uses the bar
// returns : the return of the creation of the CToolBar CreateEx
//------------------------------------------------------------------------------------------------- 
BOOL RepresentationBar::Create(CWnd* pParent, CView *v, RepresentationValueDoc *doc)
{
	ASSERT_VALID(pParent);	// must have a parent
	rprValDoc=doc;
	view=v;
	CRect rct;
	pParent->GetClientRect(&rct);
	unsigned int dwBarStyle = CBRS_SIZE_DYNAMIC | TBSTYLE_FLAT; 
	int iRet = CSizingControlBar::Create("Representation Value", pParent, CSize(120,80), TRUE, ID_REPRESENTATIONBAR);
	EnableDocking(CBRS_ALIGN_ANY);
	SetBarStyle(GetBarStyle() | dwBarStyle );
	ASSERT_VALID(this);
	parent = pParent;
	rct.DeflateRect(5,5);
	band = new GradientBand;
	band->Create(this, doc->rpr(), rct);
	CFont* fnt = const_cast<CFont*>(IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	band->SetFont(fnt);
	return iRet;
}

void RepresentationBar::OnSize(UINT nType, int cx, int cy)
{
	CSizingControlBar::OnSize(nType, cx, cy);
	Invalidate();
}

//-----------------------------------------------------------------------------------------------
// purpose : Calculates the size of the Bar. 
// parameters : int nLength sugested length by the system
//              DWORD dwMode current mode of the bar. Used for determining which layout has to be followed.
// returns : the sugested size of the ControlBar.
//------------------------------------------------------------------------------------------------- 
CSize RepresentationBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	if ( IsFloating() != (int)fFloating )
	{
		fFloating = IsFloating() != 0;
		if ( IsFloating() )
		{
			m_szFloat = band->GetSizeNeeded();
		}
	}
	return CSizingControlBar::CalcDynamicLayout(nLength, dwMode); 
}

//-----------------------------------------------------------------------------------------------
// purpose : Paints the RepresentationBar with the gradientBand.
// parameters : -
// returns : -
//------------------------------------------------------------------------------------------------- 
void RepresentationBar::OnPaint()
{
	CPaintDC cdc(this);
	CRect rct;
	GetClientRect(&rct);

	if (IsHorzDocked())
		band->SetHorizontal(true);
	else if (IsVertDocked())
		band->SetHorizontal(false);
				
	if (band->fHorizontal())
		rct.top += 5;
	else
		rct.left += 5;
	band->SetOuterRect(rct);
	Color clr = GetSysColor(COLOR_3DFACE);
	cdc.FillRect(&rct, &CBrush(clr));
	band->Draw(&cdc);	
}

//-- [ LimitForm ]-----------------------------------------------------------------------
class LimitForm: public FormWithDest
{
public:
  LimitForm(CWnd* wPar, const String& sTitle, 
    double* rVal, const Domain& dm, Color* col, bool fUsesGrad, const String& htp)
  : FormWithDest(wPar, sTitle)
  {
    if ("value" == dm->fnObj.sFile && fUsesGrad )
      fr = new FieldReal(root, SRPUiValue, rVal, ValueRange(0,100,0.1));
    else
      fr = new FieldReal(root, SRPUiValue, rVal, dm);
    new FieldColor(root, SRPUiColor, col);
    SetMenHelpTopic(htp);
    create();
  }
  FormEntry* feDefaultFocus()
    { return fr; }
private:
  FieldReal* fr;  
};
//-- [ MultipleLimitsForm ]-----------------------------------------------------------------

struct BandInfo
{
	BandInfo() { rMax = rUNDEF; }
	double rMax;
	Color clrMax;
};

class LimitRow : public FieldGroup
{
	public:
		LimitRow(FormEntry *par, BandInfo& inf, int iRow) :
				FieldGroup(par)
		{
			FormEntry *fe1, *fe2;
			FormEntry *fs1, *fs2;

			if ( iRow == 0 )
			{
				fs1 = new StaticText(this, SRPUiValue);
				fs2 = new StaticText(this, SRPUiColor);
				fs2->Align(fs1, AL_AFTER);
				fs1->psn->iBndDown = 0;
			}
 			fe1 = new FieldReal(this,"", &inf.rMax);
			if (iRow == 0)	fe1->Align(fs1, AL_UNDER);
			fe2 = new FieldColor(this, "", &inf.clrMax);
			fe2->Align(fe1, AL_AFTER);
		}
};

class MultiLimitForm : public FormWithDest
{
public:
	MultiLimitForm(CWnd* par, vector<BandInfo> &info) 
	:	FormWithDest(par, SRPTitleMultiLimits),
    iLimits(2),
		arBInf(info)
	{
    FormEntry *fe, *fe1;
		fes.resize(6);
		arBInfTemp.resize(6);
	  fe = fi = new FieldInt(root, SRPUiNrLimits, &iLimits, ValueRange(1,6), true);
		fi->SetCallBack((NotifyProc)&MultiLimitForm::ChangeNoOfLimits);
		for(int i=0; i < 6; ++i)
		{
 			fes[i] = fe1 = new LimitRow(root, arBInfTemp[i], i);
			fe1->Align(fe, AL_UNDER);
      fe = fe1;
		}
		SetMenHelpTopic("");		
 		create();
	}

	int ChangeNoOfLimits(Event *)
	{
		//if ( fi->iVal() == iLimits)
		//	return 0;

		fi->StoreData();

		for(int i=0; i<6; ++i)
		{
			if ( i >= iLimits)
				fes[i]->Hide();
			else
				fes[i]->Show();
		}
  	return 1;
	}

	FormEntry* feDefaultFocus()
    { return fi; }

	int exec()
	{
		FormWithDest::exec();
		iLimits = 0;
		for(unsigned int i = 0; i < arBInf.size(); ++i)
		{
			if (arBInfTemp[i].rMax == rUNDEF )
				continue;
			if (i > 0 && arBInfTemp[i-1].rMax >= arBInfTemp[i].rMax)
			{
				throw ErrorObject(SPRErrMultLimitOrder);
			}
			arBInf[i] = arBInfTemp[i];
			iLimits++;
		}
		arBInf.resize(iLimits);
		return 1;
	}
private:
	FieldInt *fi;
	int iLimits;
	vector<BandInfo> arBInfTemp;
	vector<BandInfo> &arBInf;
	vector<FormEntry *> fes;
};
//-- [ RepresentationValueLB ]--------------------------------------------------------------
BEGIN_MESSAGE_MAP(RepresentationValueLB, CListBox)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONUP()
	ON_CBN_SELCHANGE( ID_COLORMETHOD_CB , InPlaceCBChanged)
	ON_WM_KEYUP()
	ON_COMMAND(ID_RPREDITITEMS, OnEdit)
	ON_COMMAND(ID_RPRLOWER, OnLower)
	ON_COMMAND(ID_RPRUPPER, OnUpper)
	ON_COMMAND(ID_RPRSTRETCH, OnStretch)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

RepresentationValueLB::RepresentationValueLB() : info(NULL)
{}

RepresentationValueLB::~RepresentationValueLB()
{
	delete info;
}

//-----------------------------------------------------------------------------------------------
// purpose : Creates the Listbox that holds the main part of the editor. It initializes the in-place 
//           controls and sets the font of the listbox
// parameters : RepresentationValueView *rv. The view needed to retrieve information
// returns : succes of the creation of the listbox
//------------------------------------------------------------------------------------------------- 
BOOL RepresentationValueLB::Create(RepresentationValueView *rv)
{
	CFrameWnd *pfr = rv->GetParentFrame();
	CRect clientRct;
	pfr->GetClientRect(&clientRct);

	int iRet=CListBox::Create(WS_VISIBLE | WS_CHILD | WS_VSCROLL|LBS_NOINTEGRALHEIGHT|
	                          LBS_OWNERDRAWVARIABLE| LBS_NOTIFY|LBS_HASSTRINGS|
	                          LBS_WANTKEYBOARDINPUT|WS_BORDER,
	                          clientRct,
	                          rv,
	                          ID_EDITOR_LISTBOX );
	
	view = rv;
	CFont *fnt = const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	SetFont(fnt);
	info = new zFontInfo(&zDisplay(this));
	RepresentationValueDoc *rgDoc = view->GetDocument();
	value.Create(rgDoc, WS_CHILD | WS_BORDER, CRect(0,0,0,0), this, ID_VALUE_EDIT);
	clrMethod.Create(WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST ,  CRect(0,0,100,100), this, ID_COLORMETHOD_CB);
	value.SetFont(fnt);
	clrMethod.SetFont(fnt);
	clrMethod.AddString(SRPUiUpper.c_str());
	clrMethod.AddString(SRPUiLower.c_str());
	clrMethod.AddString(SRPUiStretch.c_str());	

	return iRet;
}

void RepresentationValueLB::InPlaceCBChanged()
{
	int iLBIndex = GetCurSel();
	int iCBIndex = clrMethod.GetCurSel();
	if (iLBIndex == CB_ERR  || iCBIndex == CB_ERR)
		return;

	view->SetColorMethod(iLBIndex, ( RepresentationGradual::ColorRange) iCBIndex);
	clrMethod.ShowWindow(SW_HIDE);
}

void RepresentationValueLB::OnKillFocus( CWnd* pNewWnd )
{
	CListBox::OnKillFocus(pNewWnd);
	value.ShowWindow(SW_HIDE);
}

void RepresentationValueLB::DrawItem( LPDRAWITEMSTRUCT dis )
{
	RepresentationValueDoc *rgDoc = view->GetDocument();
	
	if (!(dis->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
		return;

	int id = dis->itemID;
	CDC dc;
	dc.Attach(dis->hDC);
	CBrush br((COLORREF)(GetSysColor(COLOR_WINDOW)));
	CPen pen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));

	HANDLE hBrush = dc.SelectObject(br);
	HANDLE hPen = dc.SelectObject(pen);
	dc.Rectangle(dis->rcItem.left, dis->rcItem.top, dis->rcItem.right, dis->rcItem.bottom);
	dc.SelectObject( hBrush);
	dc.SelectObject( hPen);

	CRgn rr;
	rr.CreateRectRgnIndirect(&(dis->rcItem));
	dc.SelectClipRgn(&rr);

	int iSize = rgDoc->iNoColors() - 1;  
	int i = iSize - id / 2;

	String str;
	if (id % 2) 
	{
		switch (rgDoc->GetColorMethod(i-1))
		{
			case RepresentationGradual::crUPPER:
				str = SRPRemUpper;
				break;
			case RepresentationGradual::crLOWER:
				str = SRPRemLower;
				break;
			case RepresentationGradual::crSTRETCH:
				str = SRPRemStretch;
				break;
		}    
	}
	else 
	{
		if (rgDoc->fRepresentationValue())
			str = rgDoc->sValue(rgDoc->rGetLimitValue(i));
		else        
			str = String("%5.1f %%", 100 * rgDoc->rGetLimitValue(i));
	}
	{
		Color col;  
		if (dis->itemState & ODS_SELECTED)
			col = GetSysColor(COLOR_HIGHLIGHTTEXT);
		else  
			col = GetSysColor(COLOR_WINDOWTEXT);

		dc.SetTextColor( col);  
		dc.SetBkMode( OPAQUE);
		if (dis->itemState & ODS_SELECTED) 
			col = GetSysColor(COLOR_HIGHLIGHT);
		else
			col = GetSysColor(COLOR_WINDOW);
		dc.SetBkColor( col);  
		int iH; 
		iH = (dis->rcItem.bottom - dis->rcItem.top - info->height()) / 2;
		dc.TextOut( dis->rcItem.left + 5, dis->rcItem.top + iH, str.c_str(), str.size());
	}
	
	CRect rct(dis->rcItem.left + iLEFTEDGE, dis->rcItem.top, dis->rcItem.right - 5, dis->rcItem.bottom-2); 
	if ( id % 2 )
	{
		switch (rgDoc->GetColorMethod(i-1)) 
		{
			case RepresentationGradual::crUPPER:
			{
				Color clr = rgDoc->GetColor( i );
				CBrush brush(clr);
				dc.FillRect(rct, &brush);					
			}	break;			
			case RepresentationGradual::crLOWER:
			{
				Color clr = rgDoc->GetColor( i-1 );
				CBrush brush(clr);
				dc.FillRect(rct, &brush);				
				
			} break;
			case RepresentationGradual::crSTRETCH:
			{
				int iSteps = rgDoc->iGetStretchSteps();
				int iXShift = (rct.right - rct.left) / iSteps;
				int iOffset = iXShift / 2;
				int iRest = (rct.right - rct.left) % iSteps - 1;
				CRect rctBlock(rct.left, rct.top, rct.left + iOffset, rct.bottom);
				for(int iBlock = 0; iBlock < iSteps + 1; ++iBlock)
				{
					int iIndex = (i - 1) * iSteps + iBlock + 1;
					Color clrDraw = rgDoc->rpr()->clrRaw(iIndex);
					CBrush brush(clrDraw);
					dc.FillRect(rctBlock, &brush);
					if ( iBlock != iSteps - 1 )
						rctBlock = CRect(rctBlock.right, rctBlock.top, rctBlock.right + iXShift + (iRest-- != 0 ? 1 : 0), rctBlock.bottom);
					else
						rctBlock = CRect(rctBlock.right, rctBlock.top, rct.right, rctBlock.bottom);
				}					
				
			}
		}			
	}
	else
	{
		Color clr = rgDoc->GetColor( i );
		CBrush brush(clr);
		dc.FillRect(rct, &brush);		
		
	}		
	
	value.Invalidate();
	clrMethod.Invalidate();
	dc.Detach();
}

void RepresentationValueLB::MeasureItem(LPMEASUREITEMSTRUCT mi )
{
	if (mi->itemID % 2)
     mi->itemHeight = 3 * info->height();
	else
     mi->itemHeight = (UINT)(1.5 * info->height());
}

void RepresentationValueLB::OnLButtonDblClk(UINT a, CPoint b)
{
	CListBox::OnLButtonDblClk(a, b);
	value.ShowWindow(SW_HIDE);
	clrMethod.ShowWindow(SW_HIDE);
	view->OnEdit();
}

void RepresentationValueLB::OnContextMenu( CWnd* pWnd, CPoint point )
{
	CMenu menu;
	menu.CreatePopupMenu();
	CRect rct1, rct2;
	GetItemRect(0, &rct1);
	GetItemRect(1, &rct2);
	ScreenToClient(&point);
	double rFrac = (double)rct1.Height() / (rct1.Height() + rct2.Height());
  double rIndex = (double)point.y / (rct1.Height() + rct2.Height());
	int iIndex = (rIndex - (int)rIndex > rFrac) ? 2*(int)rIndex + 1  : 2*(int)rIndex ;
	ClientToScreen(&point);

	unsigned int uFlagsRO = 0;
	if (view->GetDocument()->fReadOnly())
		uFlagsRO = MF_GRAYED;
	if ( iIndex % 2 == 0)
	{
		
		String sText = String(ILWSF("men", ID_RPREDITITEMS));
		menu.AppendMenu(MF_STRING | uFlagsRO , ID_RPREDITITEMS, sText.c_str());
		SetCurSel(iIndex);
	}
	else
	{
		SetCurSel(iIndex);
		unsigned int uiCheck1 = MF_UNCHECKED ,uiCheck2 = MF_UNCHECKED , uiCheck3 = MF_UNCHECKED ;

		menu.AppendMenu(MF_STRING | uFlagsRO | uiCheck1, ID_RPRUPPER, SRPUiUpper.c_str());
		menu.AppendMenu(MF_STRING | uFlagsRO | uiCheck3, ID_RPRSTRETCH, SRPUiStretch.c_str());
		menu.AppendMenu(MF_STRING | uFlagsRO | uiCheck2, ID_RPRLOWER, SRPUiLower.c_str());
	}

	menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.Detach();
}

void RepresentationValueLB::OnUpper()
{
	int iIndex = GetCurSel();
	view->SetColorMethod(iIndex, RepresentationGradual::crUPPER);
}

void RepresentationValueLB::OnLower()
{
	int iIndex = GetCurSel();
	view->SetColorMethod(iIndex, RepresentationGradual::crLOWER);
}

void RepresentationValueLB::OnStretch()
{
	int iIndex = GetCurSel();
	view->SetColorMethod(iIndex, RepresentationGradual::crSTRETCH);
}

void RepresentationValueLB::OnEdit()
{
	view->OnEdit();
}

void RepresentationValueLB::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	switch ( nChar)
	{
		case VK_DELETE:
				view->OnDelete();
				break;
		case VK_INSERT:
			view->OnInsert();
			break;
		case VK_RETURN:
			if ( value.fIgnoreReturn)
      {
				// hack to prevent returns from the value edit to go to the LB.
				value.fIgnoreReturn=false;
				break;
			}
			// fall through F2 == RETURN
   	case VK_F2:
		   view->OnEdit();
			break;
	}
		
}

void RepresentationValueLB::OnLButtonUp(UINT nFlags, CPoint point )
{
	CListBox::OnLButtonUp(nFlags, point);
	value.ShowWindow(SW_HIDE); // hide we will see if has to be shown again
	clrMethod.ShowWindow(SW_HIDE);
	int iSel = GetCurSel();
	if ( iSel == -1 ) return;
	int iN = view->GetDocument()->iNoColors();
	int id = iN - iSel/2 - 1;
	CRect itemRect;
	GetItemRect(iSel, itemRect);
	if ( point.x < iLEFTEDGE )
	{

		value.SetEditId(id);
		if ( iSel % 2 == 0)
		{
			double rLower = view->GetDocument()->rGetLimitValue(max(0, id-1));
			double rUpper =  view->GetDocument()->rGetLimitValue(min(id + 1, iN-1));
			String str;
			if (view->GetDocument()->fRepresentationValue())
				str = view->GetDocument()->sValue(view->GetDocument()->rGetLimitValue(id));
			else        
				str = String("%5.1f %%", 100 * view->GetDocument()->rGetLimitValue(id));
			value.SetRange(rLower, rUpper);
			value.SetWindowText(str.c_str());
			value.SetSel(0,-1);
			value.ShowWindow(SW_SHOW);
			value.MoveWindow(itemRect.left + 3, itemRect.top, iLEFTEDGE - 5, info->height(), TRUE);
			value.SetFocus();
		}
		else
		{
			clrMethod.ShowWindow(SW_SHOW);
			int iY = itemRect.top + (itemRect.bottom - itemRect.top -  info->height()) / 2 ;
			clrMethod.MoveWindow(itemRect.left + 3, iY, iLEFTEDGE - 5, info->height() * 10, TRUE);
			int iId = view->GetDocument()->GetColorMethod(id - 1);
			clrMethod.SetCurSel(iId);			
			clrMethod.SetFocus();
		}
	}
}

//-- [ RepresentationValueView ]------------------------------------------------------------ 
IMPLEMENT_DYNCREATE(RepresentationValueView, RepresentationView)

BEGIN_MESSAGE_MAP(RepresentationValueView, RepresentationView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	//ON_WM_CLOSE()
	ON_WM_KEYUP()
	ON_COMMAND(ID_RPREDITLIMIT, OnEdit)
	ON_COMMAND(ID_RPRINSERT, OnInsert)
	ON_COMMAND(ID_RPRDELETE, OnDelete)
	ON_COMMAND(ID_RPRSTEPS, OnStretchSteps)
	ON_COMMAND(ID_RPRMULTSTEPS, OnMultSteps)
	ON_COMMAND(ID_REPRESENTATIONBAR, OnRepresentationBar)
	ON_COMMAND(ID_RPRVALUEBUTTONBAR, OnRprButtonBar)		
	ON_UPDATE_COMMAND_UI(ID_RPRVALUEBUTTONBAR, OnUpdateRprButtonBar)	
	ON_UPDATE_COMMAND_UI(ID_REPRESENTATIONBAR, OnUpdateRepresentationBar)
	ON_UPDATE_COMMAND_UI(ID_RPREDITLIMIT, OnUpdateEditItem)
	ON_UPDATE_COMMAND_UI(ID_RPRINSERT, OnUpdateEditItem)
	ON_UPDATE_COMMAND_UI(ID_RPRDELETE, OnUpdateEditItem)
	ON_UPDATE_COMMAND_UI(ID_RPRSTEPS, OnUpdateEditItem)
END_MESSAGE_MAP()

RepresentationValueView::RepresentationValueView() 
{
}

RepresentationValueView::~RepresentationValueView()
{
}

BOOL RepresentationValueView::PreCreateWindow(CREATESTRUCT& cs)
{

	return RepresentationView::PreCreateWindow(cs);
}

void RepresentationValueView::OnMultSteps()
{
	RepresentationValueDoc *rgDoc = GetDocument();
	vector<BandInfo> vc(6);
	MultiLimitForm frm(this, vc);
  if (frm.fOkClicked()) 
	{
		for(unsigned int i=0; i< vc.size(); ++i)
		{
			double rVal = vc[i].rMax;
			Color col = vc[i].clrMax;
			if (! rgDoc->fRepresentationValue())
				rVal /= 100;
			rgDoc->Insert(rVal, col);
		}
		init();
    rprBar.Invalidate();
  }
}

void RepresentationValueView::OnInitialUpdate() 
{
	RepresentationView::OnInitialUpdate();
	RepresentationValueDoc *scrdoc=GetDocument();
	ISTRUE(fINotEqual, scrdoc, (RepresentationValueDoc *)NULL);
	CFrameWnd *frm = GetParentFrame();
	lb.Create(this);
	init();
	
	BOOL fRet = rprBar.Create(frm, this, scrdoc);
	ASSERT_VALID(&rprBar);
	frm->EnableDocking(CBRS_ALIGN_ANY);
	frm->DockControlBar(&rprBar, AFX_IDW_DOCKBAR_BOTTOM);
	CFont *fnt=const_cast<CFont *> ( IlwWinApp()->GetFont(IlwisWinApp::sfWindowMedium));
	rprBar.SetFont(fnt);

	AddToFileMenu(ID_FILE_SAVE_COPY_AS);
	AddToFileMenu(ID_RPRDOM);
	AddToFileMenu(ID_PROP);
	AddToFileMenu(DONT_CARE, MF_SEPARATOR);
	AddToFileMenu(ID_FILE_CLOSE);

	AddToEditMenu(ID_RPRINSERT);
	AddToEditMenu(ID_RPREDITLIMIT);
	AddToEditMenu(ID_RPRDELETE);
	AddToEditMenu(ID_RPRSTEPS);
	AddToEditMenu(DONT_CARE, MF_SEPARATOR);
	AddToEditMenu(ID_RPRMULTSTEPS);

	AddToViewMenu(ID_DESCRIPTIONBAR);
	AddToViewMenu(ID_REPRESENTATIONBAR);
	AddToViewMenu(ID_STATUSLINE);
	AddToViewMenu(ID_RPRVALUEBUTTONBAR);

  AddToHelpMenu(ID_HLPKEY);			// F1: Context help
  AddToHelpMenu(ID_HLPCONTENTS);	// F11: Contents
	AddToHelpMenu(DONT_CARE, MF_SEPARATOR);
	AddToHelpMenu(ID_APP_ABOUT);  //, "&About ILWIS...");

	FrameWindow* frmw = static_cast<FrameWindow*>(frm);
	frmw->SetAcceleratorTable();
	frmw->SetWindowName("RepresentationValueEditor");

	CDC *dc = GetDC();
	int iW=dc->GetDeviceCaps(HORZRES);
	int iH=dc->GetDeviceCaps(VERTRES);

	int iMaxH = (int)(iH*0.5);
	int iMaxL = (int)(iW*0.5);

	m_bbTools.Create(frm, "rprvalue.but", "Representation editor", ID_RPRVALUEBUTTONBAR);	

	frm->MoveWindow((iW - iMaxL)/2, (iH - iMaxH)/2, iMaxL, iMaxH);
	frm->EnableDocking(CBRS_ALIGN_ANY);	
	frm->DockControlBar(&m_bbTools, AFX_IDW_DOCKBAR_TOP);	
}

void RepresentationValueView::OnDraw(CDC* pDC)
{
}

#ifdef _DEBUG
void RepresentationValueView::AssertValid() const
{
	RepresentationView::AssertValid();
}

void RepresentationValueView::Dump(CDumpContext& dc) const
{
	RepresentationView::Dump(dc);
}
#endif //_DEBUG

RepresentationValueDoc* RepresentationValueView::GetDocument() 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(RepresentationValueDoc)));
	return dynamic_cast<RepresentationValueDoc *>(m_pDocument);
}


void RepresentationValueView::OnFileSave()
{
}

void RepresentationValueView::OnInsert()
{
	RepresentationValueDoc *rgDoc = GetDocument();
	ISTRUE(fINotEqual, rgDoc, (RepresentationValueDoc *) 0);

  if (rgDoc->fReadOnly()) 
	{
    MessageBeep(MB_ICONASTERISK);
    return ;
  }
  double rVal = rUNDEF;
	int iIndex = rgDoc->iNoColors() - max(lb.GetCurSel(), 0)/2 - 1;
	Color col = GetDocument()->GetColor(iIndex);
  //Color col(128,128,128);
  LimitForm frm(this, SRPTitleInsertLimit, 
    &rVal, rgDoc->dm(), &col, rgDoc->fUsesGradual(), "ilwismen\\representation_value_gradual_editor_insert_limit.htm");
  if (frm.fOkClicked()) {
    if (! rgDoc->fRepresentationValue())
      rVal /= 100;
    rgDoc->Insert(rVal, col);
    init();
    rprBar.Invalidate();
  }
}

void RepresentationValueView::SetColorMethod(int iLBIndex, RepresentationGradual::ColorRange method)
{
	int id = 	GetDocument()->iNoColors() - iLBIndex/2 - 2;
	CRect rct;
	GetDocument()->Edit(id, method);
	lb.GetItemRect(iLBIndex, rct);
	lb.InvalidateRect(&rct);
	rprBar.Invalidate();
}

void RepresentationValueView::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if ( nChar == VK_INSERT )
	{
		OnInsert();
	}
}

void RepresentationValueView::OnUpdateEditItem(CCmdUI *cmd)
{
	if ( GetDocument()->fReadOnly())
		cmd->Enable(FALSE);
}

void RepresentationValueView::OnStretchSteps()
{
	RepresentationValueDoc *rgDoc = GetDocument();

  class StretchStepsForm: public FormWithDest
  {
  public:
    StretchStepsForm(CWnd* wPar, int* iStretchSteps)
    : FormWithDest(wPar, SRPTitleStretchSteps)
    {
      fi = new FieldInt(root, SRPUiStretchSteps, iStretchSteps, ValueRange(2,30), true);
      SetMenHelpTopic("ilwismen\\representation_value_gradual_editor_stretch_steps.htm");
      create();
    }
    FormEntry* feDefaultFocus()
      { return fi; }
  private:
    FieldInt* fi;    
  };
  if (rgDoc->fReadOnly()) {
    MessageBeep(MB_ICONASTERISK);
    return;
  }
	int iStretchSteps = rgDoc->iGetStretchSteps();
  StretchStepsForm frm(this, &iStretchSteps);
  if (frm.fOkClicked()) 
	{
		rgDoc->SetStretchSteps(iStretchSteps);
    init();
    rprBar.Invalidate();
  }
}

void RepresentationValueView::OnDelete()
{
	RepresentationValueDoc *rgDoc = GetDocument();
	ISTRUE(fINotEqual, rgDoc, (RepresentationValueDoc *) 0);

  if (rgDoc->fReadOnly()) {
    MessageBeep(MB_ICONASTERISK);
    return ;
  }
  int idSel = lb.GetCurSel();
  int id = idSel;
  if (id % 2 == 0) 
  {
    id /= 2;
    id = rgDoc->iNoColors() - id - 1;
    rgDoc->Remove(id);
    init();
    lb.SetSel(idSel);
    rprBar.Invalidate();
  }
}

void RepresentationValueView::OnEdit()
{
	RepresentationValueDoc *rgDoc = GetDocument();
	ISTRUE(fINotEqual, rgDoc, (RepresentationValueDoc *) 0);

 	int iNoLimits= rgDoc->iNoColors();
  int idSel = lb.GetCurSel();
  int id = idSel;

	if (rgDoc->fReadOnly()) 
	{
		MessageBeep(MB_ICONASTERISK);
		return ;
  }
 
	if (id % 2) 
	{
    id /= 2;
    id = iNoLimits - id - 2;
		int iOldMethod = (int) rgDoc->GetColorMethod(id);
		SetColorMethod(idSel, (RepresentationGradual::ColorRange) (iOldMethod < 2 ? iOldMethod + 1 : 0)); 
  }
  else 
	{
    id = iNoLimits - id/2 - 1;
		bool fValChange = !(idSel == 0 || idSel/2 == iNoLimits - 1);
    double rVal = rgDoc->rGetLimitValue(id);
    if ( ! rgDoc->fRepresentationValue())
      rVal *= 100;
    Color col = rgDoc->GetColor(id);
		
    LimitForm frm(this, SRPTitleEditLimit, &rVal, rgDoc->dm(), &col, rgDoc->fUsesGradual(), "ilwismen\\representation_value_gradual_editor_edit_limit.htm");
    if (frm.fOkClicked()) 
		{
      if ( ! rgDoc->fRepresentationValue())
        rVal /= 100;

			rgDoc->Edit(id, fValChange ? rVal : rgDoc->rGetLimitValue(id), col);
    }
  }
	init();
	CRect rct;
	lb.GetItemRect(idSel, rct);
	lb.InvalidateRect(&rct);
	rprBar.Invalidate();
}

void RepresentationValueView::OnRepresentationBar()
{

	if ( rprBar.GetSafeHwnd())
	{
		if (rprBar.IsWindowVisible())
		{
			SetMenuState(ID_REPRESENTATIONBAR, MF_CHECKED);
			GetParentFrame()->OnBarCheck(ID_REPRESENTATIONBAR);
		}
		else
		{
			SetMenuState(ID_REPRESENTATIONBAR, MF_UNCHECKED);
			GetParentFrame()->OnBarCheck(ID_REPRESENTATIONBAR);
		}
	}
}

void	RepresentationValueView::OnUpdateRepresentationBar(CCmdUI *cmd)
{
	if (rprBar.IsWindowVisible())
		cmd->SetCheck(1);
	else
		cmd->SetCheck(0);
}


void RepresentationValueView::init()
{
	RepresentationValueDoc *rgDoc = GetDocument();
	int iSize = rgDoc->iNoColors();  
	lb.ResetContent();
	for (int i = iSize; i > 0; --i) 
	{
		if (i < iSize)
		lb.AddString("");
		String s;
		if (rgDoc->fRepresentationValue())
			s = rgDoc->sValue(rgDoc->rGetLimitValue(i-1));
		else        
			s = String("%5.1f %%", 100 * rgDoc->rGetLimitValue(i-1));
		lb.AddString(s.scVal());
	}
	lb.SetCurSel(0);
	ColorBuf clrBuf;
	rgDoc->prg()->GetColorLut(clrBuf);
}

void RepresentationValueView::LoadState(IlwisSettings& settings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", settings.sIlwisSubKey());
	rprBar.LoadState(sBarKey.c_str());
}

void RepresentationValueView::SaveState(IlwisSettings& settings)
{
	String sBarKey = String("%S\\ToolBars\\Bars", settings.sIlwisSubKey());
	rprBar.SaveState(sBarKey.c_str());
}

void RepresentationValueView::OnSize( UINT nType, int cx, int cy )
{
	if ( lb.GetSafeHwnd() != NULL)
		lb.MoveWindow(0,0,cx,cy);
}

void	RepresentationValueView::OnUpdateRprButtonBar(CCmdUI *cmd)
{
	GetParentFrame()->OnUpdateControlBarMenu(cmd);
}

void RepresentationValueView::OnRprButtonBar()
{
	GetParentFrame()->OnBarCheck(ID_RPRVALUEBUTTONBAR);
}
