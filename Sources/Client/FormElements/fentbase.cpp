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
/* $Log: /ILWIS 3.0/FormElements/fentbase.cpp $
 * 
 * 19    20-04-01 15:17 Koolhoven
 * protected FormEntry::DoCallBack() against thrown exceptions
 * 
 * 18    24/08/00 18:15 Willem
 * The calculation of the text length now also considers '&' characters,
 * because StaticText and FlatIconButton now are able to display '&'
 * characters as-is
 * 
 * 17    11-07-00 11:22a Martin
 * changes to make FieldLine and the bevel work correctly
 * 
 * 16    10-07-00 9:11a Martin
 * added bevel code. Can switch on a bevel around a formentry
 * 
 * 15    21-01-00 12:39 Koolhoven
 * Only call callback function when formentry is shown
 * 
 * 14    8-12-99 13:56 Wind
 * changed use of CPaintDC by CDC with CreateComptableCD(0) in ::Dim()
 * 
 * 13    1-12-99 12:52p Martin
 * default positionering of controls is corrected (length)
 * 
 * 12    21-10-99 10:17a Martin
 * font sizes are now correctly calculated
 * 
 * 11    13-10-99 4:14p Martin
 * Added function to call a callbakc froma formentry
 * 
 * 10    20-09-99 12:49 Koolhoven
 * Let SetCallBack() also set the np of the formentry itself (to be sure
 * that it will not be overwritten by a call to SetCallBackForAll()
 * 
 * 9     3-09-99 11:20a Martin
 * Changed Dim function, text length calculation
 * 
 * 8     1-09-99 4:18p Martin
 * corrected text modifier
 * 
 * 7     27-08-99 10:26a Martin
 * Changed static text length computation
 * 
 * 6     25-08-99 12:22p Martin
 * dummy CStatic in Dim() did not work always. Parent is now mainwindow
 * 
 * 5     25/08/99 10:36 Willem
 * Adjusted comments
 * 
 * 4     25-08-99 9:15a Martin
 * Static text are now correct sized
*/
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/02 14:14:48  Wim
// Added log line
//
// fentbase.c
/* Base Form entry
// by Wim Koolhoven, aug. 1993
// changed by Jelle Wind, dec. 1993
	Last change:  WK    2 Sep 97    4:06 pm
*/
#define FENTBASE_C

#include "Client\Headers\formelementspch.h"
#include "Client\ilwis.h"

static int _id = 100;


FormEntry::FormEntry(FormBase* _frm_) :
        id(0),
        _fShow(false),
        _frm(_frm_),
        _par(NULL),
        _prm(NULL),
        _npChanged(0),
        _cb(0),
				bsStyle(bsNONE),
				_enabled(true)

{
    _id = 100;
    psn = new FormEntryPositioner();
    psn->SetBound(12, 25, 25, 25);
}

FormEntry::FormEntry(FormEntry* p, Parm *prm, bool fAutoAlign) :
        id(_id++),
        _frm(p->_frm),
        _par(p),
        _fShow(false),
		_enabled(true),
		bsStyle(bsNONE)
{
  FormEntry* feParPos = _par;
  while (feParPos->fRadioButton() || feParPos->fCheckBox())
      feParPos = feParPos->_par;

  psn = new FormEntryPositioner(feParPos->psn, 0, 0, 0, 0);
  if (fAutoAlign) 
  {
    if (!(_par->fGroup() && (_par->children.iSize() == 0)))  // align entry
    {
      if (_frm->feLastInserted &&(!_frm->feLastInserted->fGroup() || (_frm->feLastInserted != _par))) 
      {
        if (_par->fRadioButton() || _par->fCheckBox())
          Align(_par, AL_AFTER, 0);
        else
        {
          if (_frm->feLastInserted->fRadioButton())
            Align(_frm->feLastInserted->_par, AL_UNDER, 0);
          else
            Align(_frm->feLastInserted, AL_UNDER, 0);
        }
      }
    }
    _frm->feLastInserted = this;
    psn->SetBound(0, 10, 0, 5);
  }
  psn->iMinHeight = psn->iHeight = YDIALOG(_frm->iDefFldHeight());

  _par->children &= this;
  _prm = prm;
  if (_prm)
    _frm->Add(_prm);
  _npChanged = 0;
  _cb = 0;
}


FormEntry::~FormEntry()
{
    for (unsigned int i=0; i<children.iSize(); i++)
        delete children[i];
    delete psn;
}

void FormEntry::create()
{
  if (_par==0) // root object
        CreateChildren();

}

void FormEntry::SetBevelStyle(FormEntry::BevelStyle _bsStyle)
{
	bsStyle = _bsStyle;
}

void FormEntry::SetFocus()
{
}

void FormEntry::CreateChildren() 
{
  for (unsigned int i=0; i<children.iSize(); i++)
       children[i]->create();

	if ( bsStyle != FormEntry::bsNONE && bevel.GetSafeHwnd() == 0)
	{
		int iExtra = psn->iHeight == 1 ? 1 : 4; // iHeight == 1, a line is meant, not a box
		if ( bsStyle == bsRAISED )
			bevel.Create(frm()->wnd(), CRect(psn->iPosX - iExtra, psn->iPosY -iExtra, psn->iPosX + psn->iWidth + 2, psn->iPosY + psn->iHeight), true);  
		else
			bevel.Create(frm()->wnd(), CRect(psn->iPosX - iExtra, psn->iPosY -iExtra, psn->iPosX + psn->iWidth + 2, psn->iPosY + psn->iHeight), false);  
	}
};

bool FormEntry::fRadioButton() const 
{ return false; }

bool FormEntry::fCheckBox() const 
{ return false; }

bool FormEntry::fGroup() const 
{ return _par == 0; }

void FormEntry::setHelpItem(const HelpTopic& htp)
{ _htp = htp; }

void FormEntry::SetCallBack(NotifyProc np) 
{ 
	SetCallBack(np, (CallBackHandler*)frm()->wnd()); 
}

void FormEntry::SetCallBack(NotifyProc np, CallBackHandler* cb) 
{ 
	_npChanged = np; 
	_cb = cb; 
}

FormEntry* FormEntry::CheckData()
{ 
    FormEntry *pc;
    for (unsigned int i=0; i<children.iSize(); i++) 
    {
        if (!children[i]->fShow())
            continue;
        pc = children[i]->CheckData();
        if (pc != 0)
            return pc;
    }
    return 0;
}

void FormEntry::StoreData()
{
    for (unsigned int i=0; i<children.iSize(); i++) 
    {
        if (children[i]->fShow())
            children[i]->StoreData();
    }  
}

void FormEntry::ClearData()
{
    for (unsigned int i=0; i<children.iSize(); i++) 
    {
	    children[i]->ClearData();
    }  
}

void FormEntry::CheckDrawItem(DRAWITEMSTRUCT* dis)
{
  for (unsigned int i=0; i<children.iSize(); i++)
  {
    FormEntry *child  = children[i];
    if (!child)
      throw ErrorObject("Trying to access unitialized child control");
    if (child->Id() == dis->CtlID)
      child->DrawItem(&DrawItemEvent(dis->CtlID, dis));
    else
      child->CheckDrawItem(dis);
  }
}

void FormEntry::DrawItem(Event*)
{
}

void FormEntry::show(int)            // set show or hide
{
}

void FormEntry::Show()
{
  bool fWasHidden = !fShow();
  _fShow = true;
  ShowChildren();
  show(SW_SHOW);
  if (fWasHidden)
  {
    if ((_cb!=NULL) && (_npChanged!=NULL))
      (_cb->*_npChanged)(0);
  }
	if ( bevel.GetSafeHwnd() != 0)
		bevel.ShowWindow(SW_SHOW);
}


void FormEntry::Hide()
{
    _fShow = false;
    HideChildren();
    show(SW_HIDE);
		if ( bevel.GetSafeHwnd() != 0)
			bevel.ShowWindow(SW_HIDE);
}

void FormEntry::SetFieldWidth(short iWid)
{
    psn->iMinWidth = iWid;
}

void FormEntry::SetWidth(short iWid)
{
    psn->iMinWidth = XDIALOG(iWid);
}

void FormEntry::SetHeight(short iHeight)
{
    psn->iMinHeight = psn->iHeight = YDIALOG(iHeight);
}

void FormEntry::ShowChildren()
{
    for (unsigned int i=0; i<children.iSize(); i++)
        children[i]->Show();
}

void FormEntry::HideChildren()
{
    for (unsigned int i=0; i<children.iSize(); i++)
        children[i]->Hide();
}

void FormEntry::Align(FormEntry* c, enumAlign a, int iOffSet)
{
  switch (a) 
  {
    case AL_UNDER:
      if (_par->fRadioButton())
      {
        if (_par == c) // change _parent of positioner to radiogroup
          psn->ChangeParent(_par->_par->psn);
        else // change _parent of positioner to _parent of radiogroup
          psn->ChangeParent(_par->_par->_par->psn);
      }
      psn->AlignUp(c->psn, iOffSet);
      break;
    case AL_AFTER:
      if (_par->fRadioButton())
      {
        if (_par == c) // change _parent of positioner to radiogroup
          psn->ChangeParent(_par->_par->psn);
        else // change _parent of positioner to _parent of radiogroup
          psn->ChangeParent(_par->_par->_par->psn);
      }
      psn->AlignLeft(c->psn, iOffSet);
      break;
    default:
        break;
  }
}

String FormEntry::htp()
{
    return _frm->htp();
}

void FormEntry::MaxX(int *iMaxX, int iMaxHeight)
{
  if (psn->iPosY > iMaxHeight) return;
  if (children.iSize() == 0) 
  {
    if (psn->iPosX + psn->iWidth > *iMaxX)
      *iMaxX = psn->iPosX + psn->iWidth;
  }
  else
    for (unsigned int i=0; i<children.iSize(); i++)
      children[i]->MaxX(iMaxX, iMaxHeight);
}


zDimension FormEntry::Dim(const String& s, CFont* fnt)
{
		CFont *newfnt = fnt != 0 ? fnt : _frm->fnt;

    CString sx;
    for (unsigned int i=0; i< s.length(); i++)
//      if (s[i] != '&')    // Removed because static text now CAN display '&' if SS_NOPREFIX is specified
        sx += s[i];

    CDC pdc; 
    pdc.CreateCompatibleDC(0);

		CFont *oldFont = pdc.SelectObject(newfnt);
		CSize szTxt = pdc.GetTextExtent(sx);
		pdc.SelectObject(oldFont);
		return szTxt;
}

///*
void FormEntry::DoCallBack()
{
	if (!fShow())
		return;
	if ((_cb!=NULL) && (_npChanged!=NULL))
	{
		try {
			(_cb->*_npChanged)(0);
		}
		catch (ErrorObject& err)
		{
			err.Show();
		}
		catch (...)
		{
			TRACE("Unknown error in FormEntry::DoCallBack()");
		}
	}
}

void FormEntry::CallCallBacks()
{
	DoCallBack();

  for (unsigned int i=0; i<children.iSize(); i++)	
    children[i]->CallCallBacks();
}
//*/
void FormEntry::SetCallBackForAll(NotifyProc np)
{
    if (npChanged() == 0)
        SetCallBack(np);
    for (unsigned int i=0; i<children.iSize(); i++)
        children[i]->SetCallBackForAll(np);
}

void FormEntry::Enable()
{
    _frm->wnd()->MessageBox(SUIErrEnableNotImplem.sVal(), SUIErrProgrammingError.sVal(), MB_ICONSTOP|MB_OK);
}

void FormEntry::Disable()
{
    _frm->wnd()->MessageBox(SUIErrDisableNotImplem.sVal(), SUIErrProgrammingError.sVal(), MB_ICONSTOP|MB_OK);
}

void FormEntry::EnableChildren()
{
    for (unsigned int i = 0; i < childlist().iSize(); i++)
        childlist()[i]->Enable();
}

void FormEntry::DisableChildren()
{
    for (unsigned int i = 0; i < childlist().iSize(); i++)
        childlist()[i]->Disable();
}

String FormEntry::sChildrensText()
{
    if ( !fShow() ) return "";
    String s;
    for (unsigned int i = 0; i < childlist().iSize(); i++)
    {
        FormEntry *child=childlist()[i];
        if (!child->fShow())
          continue;
        s &= child->sGetText();
        s &= "\r\n";
    }
    return s;
}

String FormEntry::sGetText()
{
    return "";
}








