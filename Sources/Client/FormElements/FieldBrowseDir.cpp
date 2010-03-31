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
#include "Client\Editors\Utils\BrowseForFolder.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\Editors\Utils\SBDestination.h"

BrowseDirButton::BrowseDirButton(FormEntry *fr) :
	PushButtonSimple(fr, "...", (NotifyProc)&FieldBrowseDir::ShowBrowseForm, true, false)
{
  zDimension dim = Dim("gh");
  dim.width() = 18;
  dim.height() = dim.height() + 6;

  psn->iMinWidth = dim.width();
  psn->iMinHeight = psn->iHeight = dim.height();
}

//--------------------------------------------------------------------------------
FieldBrowseDir::FieldBrowseDir(FormEntry *fr, const String& sQuestion, const String& sRem, String *sVal) 
: FieldString(fr, sQuestion, sVal),
	sRemark(sRem)
{
  //psn->SetBound(0,-1,0,0);
  bb = new BrowseDirButton(this);
  bb->Align(this, AL_AFTER);
  bb->SetIndependentPos();
}

void FieldBrowseDir::create() 
{ 
  bb->psn->iPosX = fld->psn->iPosX+fld->psn->iMinWidth;
  CreateChildren();
}

int FieldBrowseDir::ShowBrowseForm(Event *ev)
{
	CSBDestination sb(0, sRemark.scVal());
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	String sV = sVal();
	sb.SetInitialSelection(sV.scVal());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		SetVal(String(sBrowse));
	}
	return 1;
}
