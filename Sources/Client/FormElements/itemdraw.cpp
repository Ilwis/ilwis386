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
/* ItemDrawer
// by Wim Koolhoven
// (c) Computer Department ITC
// moved to directory ui by Jelle (nov. 93)
	Last change:  WK    9 Mar 98    5:32 pm
*/
#include "Client\Headers\formelementspch.h"
#include "Client\Base\ZappTools.h"
#include "Engine\Base\DataObjects\Color.h"

static int iDown = 0; //-32767;

ItemDrawer::ItemDrawer(BOOL fExist_)
{
  fExist = fExist_;
  fFocus = FALSE;
  if (iDown == -32767) {
    zDimension dim = (zDimension) GetMenuCheckMarkDimensions();
    iDown = (dim.height() - 14)/3;
  }
}

int ItemDrawer::DrawGen(DRAWITEMSTRUCT* dis, BOOL& fSelected, BOOL fFull, BOOL fUpNgbSel)
{
	CRgn rgn;
	rgn.CreateRectRgnIndirect(CRect(dis->rcItem));
	SelectClipRgn(dis->hDC, rgn);
	
	SetBkMode(dis->hDC, TRANSPARENT);
	if (dis->itemAction & (ODA_FOCUS | ODA_DRAWENTIRE))
	{
		if (dis->itemState & ODS_FOCUS)
			fFocus = FALSE;
		else
			fFocus = TRUE;
		if (!(dis->itemAction & ODA_DRAWENTIRE)) 
			return 1;
	}
	
	if (dis->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))
		if (dis->itemState & ODS_SELECTED)
			fSelected = TRUE;
		else
		{
			fSelected = FALSE;
			HANDLE hBrush;
			HANDLE hPen;
			CBrush* br;
			CPen* pen;
			br = new CBrush(GetSysColor(COLOR_WINDOW));
			hBrush = SelectObject(dis->hDC, HBRUSH(*br));
			pen = new CPen(PS_SOLID, 1, GetSysColor(COLOR_WINDOW));
			hPen = SelectObject(dis->hDC, HPEN(*pen));
			if (fUpNgbSel)
				Rectangle(dis->hDC, dis->rcItem.left , dis->rcItem.top - 1,
				dis->rcItem.right, dis->rcItem.bottom);
			else
				Rectangle(dis->hDC, dis->rcItem.left , dis->rcItem.top,
				dis->rcItem.right, dis->rcItem.bottom);
			SelectObject(dis->hDC, hBrush);
			SelectObject(dis->hDC, hPen);
			delete br;
			delete pen;
		}

		if (fSelected && (fFocus || !fExist))
		{
			HANDLE hBrush;
			HANDLE hPen;
			CBrush* br;
			CPen* pen;
			if (fFull)
				br = new CBrush(GetSysColor(COLOR_HIGHLIGHT));
			else
				br = new CBrush(GetSysColor(COLOR_WINDOW));
			pen = new CPen(PS_SOLID, 1, GetSysColor(COLOR_HIGHLIGHT));
			hBrush = SelectObject(dis->hDC, HBRUSH(*br));
			hPen = SelectObject(dis->hDC, HPEN(*pen));
			if (fUpNgbSel && !fFull)
				Rectangle(dis->hDC, dis->rcItem.left , dis->rcItem.top - 1,
				dis->rcItem.right, dis->rcItem.bottom);
			else
				Rectangle(dis->hDC, dis->rcItem.left , dis->rcItem.top,
				dis->rcItem.right, dis->rcItem.bottom);
			SelectObject(dis->hDC, hPen);
			SelectObject(dis->hDC, hBrush);
			delete br;
			delete pen;
		}
		return 0;
}

int ItemDrawer::DrawItem(DRAWITEMSTRUCT* dis, const String& str,
                     BOOL fSelected, BOOL fFull, BOOL fUpNgbSel)
{
	String sName = str;

	if (DrawGen(dis,fSelected,fFull,fUpNgbSel)) return 1;
	if (str.length() == 0) return 1;
	if ( str[0] != '[' )
	{
		String sDelim, sIcon;
		int dot = str.rfind('.');
  		sIcon = str.substr(dot+1);
		sName = str.substr(0, dot);
		if ( sIcon == "drive" )
			sIcon = "Drv";
		else if ( sIcon == "directory" )
			sIcon = "Dir";

		if ( sIcon != "" ) 
		{
			zIcon icon;
			if (icon.fLoad(String("%S16Ico", sIcon)))
			{
				int iTop = dis->rcItem.top + dis->rcItem.bottom/2 - dis->rcItem.top/2 - 8 + iDown/2;
				DrawIcon(dis->hDC, dis->rcItem.left + 2, iTop, icon);
			} if ( icon.fLoad(sIcon)) {
			 	int iTop = dis->rcItem.top + dis->rcItem.bottom/2 - dis->rcItem.top/2 - 8 + iDown/2;
				DrawIcon(dis->hDC, dis->rcItem.left + 2, iTop, icon);
			}
		} 
		
	}
	else if (str[1] == '-')
	{
		String sDrive = str[2];
		sDrive += ":";
		String sIcon;				
		switch(GetDriveType(sDrive.c_str()) )
		{

			case DRIVE_CDROM:
				sIcon = "CDDRIVEICON"; break;
			case DRIVE_REMOVABLE:
				sIcon =  "FLOPPYICON"; break;
			case DRIVE_REMOTE:
				sIcon = "NETDRIVEICON"; break;

			default:
				sIcon ="Drv16Ico"; 
		}		
		zIcon icon(sIcon.c_str());
		int iTop = dis->rcItem.top + dis->rcItem.bottom/2 - dis->rcItem.top/2 - 8 + iDown/2;
		DrawIcon(dis->hDC, dis->rcItem.left + 2, iTop, icon);
		sName = String("%c:", str[2]);
	}
	else
	{
		zIcon icon("Dir16Ico");
		int iTop = dis->rcItem.top + dis->rcItem.bottom/2 - dis->rcItem.top/2 - 8 + iDown/2;
		DrawIcon(dis->hDC, dis->rcItem.left + 2, iTop, icon);
		sName = str.substr(1, str.size() - 2);
	}

	Color col;
	if (fSelected && fFull && (fFocus || !fExist))
		col = GetSysColor(COLOR_HIGHLIGHTTEXT);
	else  
		col = GetSysColor(COLOR_WINDOWTEXT);
	SetTextColor(dis->hDC, col);  
	SetTextAlign(dis->hDC, TA_BASELINE);
	int iV = dis->rcItem.top / 2 + dis->rcItem.bottom / 2 + 5;
	TextOut(dis->hDC, dis->rcItem.left + 20, iV, sName.c_str(), sName.size());

	return 1;
}

//int ItemDrawer::DrawItem(DRAWITEMSTRUCT* dis, zBitmapDisplay& bm, zString& str,
//                     BOOL fSelected, BOOL fFull, BOOL fUpNgbSel)
int ItemDrawer::DrawItem(DRAWITEMSTRUCT* dis, int iOffset, String& str,
                     BOOL fSelected, BOOL fFull, BOOL fUpNgbSel)
{
  if (str.length() == 0) return 1;
  char* s = str.sVal();
  if (DrawGen(dis,fSelected,fFull,fUpNgbSel)) return 1;
  //BitBlt(dis->hDC, dis->rcItem.left + 2, dis->rcItem.top+iDown,
  //	 16,16,bm,0,0,SRCCOPY);
//  CPen* pen; - not used
  Color col;
  if (fSelected && fFull && (fFocus || !fExist))
    col = GetSysColor(COLOR_HIGHLIGHTTEXT);
  else  
    col = GetSysColor(COLOR_WINDOWTEXT);
  SetTextColor(dis->hDC, col);  
  SetTextAlign(dis->hDC, TA_BASELINE);
  int iV = dis->rcItem.top / 2 + dis->rcItem.bottom / 2 + 5;
  TextOut(dis->hDC, dis->rcItem.left + iOffset, iV, s, strlen(s));
  return 1;
}

int ItemDrawer::DrawItem(DRAWITEMSTRUCT* dis, const zIcon& ic, String& str,
		     BOOL fSelected, BOOL fFull, BOOL fUpNgbSel)
{
	if (str.length() == 0) 
		return 1;

	char* s = str.sVal();
	if (DrawGen(dis,fSelected,fFull,fUpNgbSel))
		return 1;

	int iTop = dis->rcItem.top + dis->rcItem.bottom/2 - dis->rcItem.top/2 - 8 + iDown/2;
	DrawIcon(dis->hDC, dis->rcItem.left + 2, iTop, (HICON )(const_cast<zIcon&>(ic)));
	Color col;
	if (fSelected && fFull && (fFocus || !fExist))
		col = GetSysColor(COLOR_HIGHLIGHTTEXT);
	else  
		col = GetSysColor(COLOR_WINDOWTEXT);
	SetTextColor(dis->hDC, col);  
	SetTextAlign(dis->hDC, TA_BASELINE);
	int iV = dis->rcItem.top / 2 + dis->rcItem.bottom / 2 + 5;
	TextOut(dis->hDC, dis->rcItem.left + 20, iV, s, strlen(s));
	return 1;
}




