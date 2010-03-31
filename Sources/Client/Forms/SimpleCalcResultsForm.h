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
#include "Client\FormElements\selector.h"
#include "Headers\Hs\Mainwind.hs"

class SimpleCalcResultForm : public FormBaseDialog
{
public:
	SimpleCalcResultForm(CWnd *wPar) :
		FormBaseDialog(wPar, SMSTitleSimpleCalc, (fbsSHOWALWAYS | fbsNOCANCELBUTTON | fbsHIDEONCLOSE))
	{
		fs = new StringArrayLister(root, dummy);
		zDimension dim = fs->Dim("gk");
		fs->SetWidth((short)(20 * dim.width()));
		fs->SetHeight((short)(7 * dim.height()));
		CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
		//fs->Font(fnt)
		create();
	}
	int exec() { return iUNDEF; }
	void AddString(const String& s)
	{
		fs->AddString(s, 0);
	}				
private:
	Array<String> dummy;
	StringArrayLister *fs;
};

class SimpleCalcText : public FormWithDest
{
public:
	SimpleCalcText(CWnd *wPar, String *txt) :
		FormWithDest(wPar, SMSTitleSimpleCalc,false)
	{
		fs = new FieldStringMulti(root, txt, true);
		zDimension dim = fs->Dim("gk");
		fs->SetWidth((short)(25 * dim.width()));
		fs->SetHeight((short)(20 * dim.height()));
		CFont *fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
		//fs->Font(fnt)
		create();
	}
	int exec() { return iUNDEF; }

private:
	FieldStringMulti *fs;
};	
