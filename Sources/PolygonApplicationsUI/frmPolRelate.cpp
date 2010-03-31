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
/*
// $Log: /ILWIS 3.0/ApplicationForms/frmPolygonMapRelate.cpp $
 * 
 * 8     16/02/01 17:23 Willem
 * - Removed a lot of old commented code
 * - Added quoting to the input filename if needed
 * 
 * 7     8-11-00 17:25 Koolhoven
 * use sFullNameQuoted() on input data to ensure selection in forms
 * 
 * 6     16-06-00 12:11 Koolhoven
 * No longer ask for new domain..
 * Always a DomainPolygonMapRelate will be used
 * 
 * 5     11/11/99 15:37 Willem
 * Added a callback in the from to disallow ':' (colon) as legal character
 * in the domain prefix
 * 
 * 4     4/11/99 17:01 Willem
 * Added a field to enter the domain prefix for PolygonMapRelate operations for
 * pointmap, segmentmap and polygonmap
 * 
 * 3     9/13/99 1:05p Wind
 * comments
 * 
 * 2     9/13/99 12:36p Wind
 * adapted :exec functions to use of quoted file names when building an
 * expression
*/
// Revision 1.4  1998/09/16 17:33:54  Wim
// 22beta2
//
// Revision 1.3  1997/09/11 09:32:16  Wim
// Strip new domain name of the path.
//
// Revision 1.2  1997-08-26 14:55:00+02  Wim
// Added sNewDom with AreaNumbering and PolygonMapRelate
//
/* FormGeneralMapApplications
   Copyright Ilwis System Development ITC
   march 1997, by Wim Koolhoven
	Last change:  WK   11 Sep 97   11:29 am
*/
#include "Client\Headers\AppFormsPCH.h"
#include "PolygonApplicationsUI\frmpolrelate.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT Cmdpolygonmaprelate(CWnd *wnd, const String& s)
{
	new FormPolygonMapRelate(wnd, s.scVal());
	return -1;
}

FormPolygonMapRelate::FormPolygonMapRelate(CWnd* mw, const char* sPar)
: FormPolygonMapCreate(mw, SAFTilteMaprelate),negation(false),fsMatrix(NULL),relation(0)
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

	relations.push_back(SAFUIContains + ".Contains");
	relations.push_back(SAFUICoveredBy + ".CoveredBy");
	relations.push_back(SAFUICovers + ".Covers");
	relations.push_back(SAFUICrosses + ".Crosses");
	relations.push_back(SAFUIDisJoint + ".DisJoint");
	relations.push_back(SAFUIEquals + ".Equals");
	relations.push_back(SAFUiIntersect + ".Intersect");
	relations.push_back(SAFUIDE9IM + ".MAT16ICO");
	relations.push_back(SAFUIOverlaps + ".Overlaps");
	relations.push_back(SAFUITouches + ".Touches");
	relations.push_back(SAFUIWithin + ".Within");

	FieldDataType *fd = new FieldDataType(root, SAFUiFirstInput, &sInMap1, ".mpa", true);
	StaticText *txt = new StaticText(root,SAFUIRelations);
	FieldGroup *fg = new FieldGroup(root);
	CheckBox *cb = new CheckBox(fg,SAFUiNot,&negation);
	cb->Align(txt, AL_AFTER);
	fsRelation = new FieldOneSelectString(fg, "", &relation, relations);
	fsRelation->Align(cb,AL_AFTER);
	fsRelation->SetComboWidth(60);
	fsRelation->SetCallBack((NotifyProc)&FormPolygonMapRelate::SetPattern);
	fg->SetIndependentPos();
	fsMatrix = new FieldString(root,SAFUIDE9IMPattern,&pattern);
	fsMatrix->Hide();
	fsMatrix->Align(txt,AL_UNDER);
	FieldBlank *fb  = new FieldBlank(root,0.1);
	//fb->Align(fsMatrix,AL_UNDER);
	fd = new FieldDataType(root, SAFUiSecondInput, &sInMap2, ".mpa.mps.mpp", true);
		
	new FieldBlank(root);
			
    initPolygonMapOut(false);
	String sFill('x', 60);
	//stRemark = new StaticText(root, sFill);
	//stRemark->SetIndependentPos();
	
	//SetAppHelpTopic(htpGenMapPolygonMapRelate);
	create();
}

FormEntry *FormPolygonMapRelate::CheckData() {
	if ( relation == 7) {
		if ( pattern.size() != 9)
			return fsMatrix;
		pattern.toUpper();
		if ( pattern.find_first_not_of("*TF") != string::npos)
			return fsMatrix;
		}
	return NULL;
}

int FormPolygonMapRelate::SetPattern(Event *ev) {
	fsRelation->StoreData();
	if ( relations.size() > 0 && fsMatrix ) {
		if (relations[relation] == SAFUIDE9IM + ".MAT16ICO") {
			fsMatrix->Show();
		} else {
			fsMatrix->Hide();
		}
	}
	return 1;

}
int FormPolygonMapRelate::exec()
{
	FormPolygonMapCreate::exec();
	FileName fnMap1(sInMap1);
	String rel;
	FileName fnMap2(sInMap2);
	switch(relation) {
		case 0:
			rel = "Contains"; break;
		case 1:
			rel = "CoveredBy"; break;
		case 2:
			rel = "Covers"; break;
		case 3:
			rel = "Crosses"; break;
		case 4:
			rel = "Disjoint"; break;
		case 5:
			rel = "Equals"; break;
		case 6:
			rel = "Intersects"; break;
		case 7:
			rel = "DE-9IM[" + pattern + "]"; break;
		case 8:
			rel = "Overlap"; break;
		case 9:
			rel = "Touches";break;
		case 10:
			rel = "Within";break;
	};
	String negate = negation ? "true" : "false";
	String sExpr = String("PolygonMapRelate(%S,%S,%S,%S)", fnMap1.sRelative(), fnMap2.sRelative(),rel,negate);


	execPolygonMapOut(sExpr);

	return 0;
}

