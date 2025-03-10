/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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

#include "Client\Headers\AppFormsPCH.h"
#include "PolygonApplicationsUI\PolygonMapBuffer.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT Cmdpolygonmapbuffer(CWnd *wnd, const String& s)
{
	new FormPolygonMapBuffer(wnd, s.c_str());
	return -1;
}

FormPolygonMapBuffer::FormPolygonMapBuffer(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, TR("Create a Buffer Map")), distance(0), endStyle(0)
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
			if (sInMap == "")
				sInMap = fn.sFullNameQuoted(true);
			else  
				sOutMap = fn.sFullName(true);
		}
	}
	styles.push_back(TR("Round") + ".PBRound");
	styles.push_back(TR("Square") + ".PBSquare");
	styles.push_back(TR("Butt") + ".PBButt");
	new FieldDataType(root, TR("&Input Map"), &sInMap, ".mpp.mps.mpa", true);
	
	frDistance = new FieldReal(root, TR("&Buffer distance"), &distance, ValueRange(-1e9,1e9,0.0));
	new FieldOneSelectString(root, TR("&End cap style"), &endStyle, styles);
	new FieldBlank(root);
			
    initPolygonMapOut(false);
	String sFill('x', 60);
	//stRemark = new StaticText(root, sFill);
	//stRemark->SetIndependentPos();
	
	//SetAppHelpTopic(htpGenMapPolygonMapBuffer);
	create();
}

FormEntry *FormPolygonMapBuffer::CheckData() {
	frDistance->StoreData();
	if ( distance == 0)
		return frDistance;
	return NULL;
}

int FormPolygonMapBuffer::exec()
{
	FormPolygonMapCreate::exec();
	String type;
	switch(endStyle) {
		case 0:
			type="Round"; break;
		case 1:
			type="Square"; break;
		case 2:
			type="Butt"; break;
	};
	FileName fnMap(sInMap);
	String sExpr = String("PolygonMapBuffer(%S,%f,%S)", fnMap.sRelativeQuoted(true), distance, type);


	execPolygonMapOut(sExpr);

	return 0;
}

