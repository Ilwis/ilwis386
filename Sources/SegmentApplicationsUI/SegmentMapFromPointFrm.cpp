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
*/
#include "Client\Headers\AppFormsPCH.h"
#include "SegmentMapFromPointFrm.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT CmdSegmentMapFromPointFrm(CWnd *wnd, const String& s)
{
	new FormSegmentMapFromPointFrm(wnd, s.c_str());
	return -1;
}

FormSegmentMapFromPointFrm::FormSegmentMapFromPointFrm(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, TR("Create a Segment map of a pointmap"))
{
	hasIdentityColumn = hasOrderColumn = false;
	if (sPar)
	{
		TextInput ip(sPar);
		TokenizerBase tokenizer(&ip);
		String sVal;
		for (;;) {
			Token tok = tokenizer.tokGet();
			sVal = tok.sVal();
			if (sVal == "")
				break;
			FileName fn(sVal);
			if (sInMap1 == "")
				sInMap1 = fn.sFullNameQuoted(true);
			else  
				sOutMap = fn.sFullName(true);
		}
	}
	fdMap = new FieldDataType(root, TR("Point map"), &sInMap1, ".mpp", true);
	fdMap->SetCallBack((NotifyProc)&FormSegmentMapFromPointFrm::changeColumns);
	cbIdent = new CheckBox(root,TR("Use identity column"), &hasIdentityColumn);
	fcIdent = new FieldColumn(cbIdent,"",attTable,&identCol, dmCLASS | dmIDENT | dmVALUE | dmIMAGE | dmBOOL | dmUNIQUEID | dmTIME);

	cbOrder = new CheckBox(root,TR("Use order column"), &hasOrderColumn);
	fcOrder = new FieldColumn(cbOrder,"",attTable,&orderCol, dmVALUE);
	cbOrder->Align(cbIdent, AL_UNDER);
	
	FieldBlank *fb = new FieldBlank(root);
	fb->Align(cbOrder, AL_UNDER);
			
    initSegmentMapOut(false);
	String sFill('x', 60);
	create();
}

int FormSegmentMapFromPointFrm::changeColumns(Event *ev) {
	fdMap->StoreData();
	PointMap pm(sInMap1);
	if ( pm.fValid()) {
		if ( pm->fTblAtt()) {
			attTable = pm->tblAtt();
			fcIdent->FillWithColumns(&attTable);
			fcOrder->FillWithColumns(&attTable);

		}
	}
	return 1;
}

FormEntry *FormSegmentMapFromPointFrm::CheckData() {
	return NULL;
}

int FormSegmentMapFromPointFrm::exec()
{
	FormSegmentMapCreate::exec();
	FileName fnMap1(sInMap1);
	String sExpr;
	if ( identCol != "" && orderCol != "")
		sExpr = String("SegmentMapFromPoints(%S,%S,%S)", fnMap1.sRelative(), identCol, orderCol);
	else if (identCol != "" && orderCol == "")
		sExpr = String("SegmentMapFromPoints(%S,%S)", fnMap1.sRelative(), identCol);
	else if (identCol == "" && orderCol != "")
		sExpr = String("SegmentMapFromPoints(%S,,%S)", fnMap1.sRelative(), orderCol);
	else 
		sExpr = String("SegmentMapFromPoints(%S)", fnMap1.sRelative());


	execSegmentMapOut(sExpr);

	return 0;
}

