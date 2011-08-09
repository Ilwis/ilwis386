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
#include "PointApplicationsUI\PointMapDifference.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT Cmdpointmapdifference(CWnd *wnd, const String& s)
{
	new FormPointMapDifference(wnd, s.c_str());
	return -1;
}

FormPointMapDifference::FormPointMapDifference(CWnd* mw, const char* sPar)
: FormPointMapCreate(mw, TR("Create a Intersection map of Points"))
{
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
	new FieldDataType(root, TR("First Input map"), &sInMap1, ".mpp", true);
	new FieldDataType(root, TR("Second Input map"), &sInMap2, ".mpp", true);
	
	new FieldBlank(root);
			
    initPointMapOut(false);
	String sFill('x', 60);
	//stRemark = new StaticText(root, sFill);
	//stRemark->SetIndependentPos();
	
	//SetAppHelpTopic(htpGenMapPointMapDifference);
	create();
}

FormEntry *FormPointMapDifference::CheckData() {
	return NULL;
}

int FormPointMapDifference::exec()
{
	FormPointMapCreate::exec();
	FileName fnMap1(sInMap1);
	FileName fnMap2(sInMap2);
	String sExpr = String("PointMapDifference(%S,%S)", fnMap1.sRelative(), fnMap2.sRelative());


	execPointMapOut(sExpr);

	return 0;
}

