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
// $Log: /ILWIS 3.0/ApplicationForms/frmSegmentMapRelate.cpp $
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
 * Always a DomainSegmentMapRelate will be used
 * 
 * 5     11/11/99 15:37 Willem
 * Added a callback in the from to disallow ':' (colon) as legal character
 * in the domain prefix
 * 
 * 4     4/11/99 17:01 Willem
 * Added a field to enter the domain prefix for SegmentMapRelate operations for
 * Segmentmap, segmentmap and Segmentmap
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
// Added sNewDom with AreaNumbering and SegmentMapRelate
//
/* FormGeneralMapApplications
   Copyright Ilwis System Development ITC
   march 1997, by Wim Koolhoven
	Last change:  WK   11 Sep 97   11:29 am
*/
#include "Client\Headers\AppFormsPCH.h"
#include "SegmentApplicationsUI\frmSegrelate.h"
#include "Client\FormElements\objlist.h"
#include "Headers\Hs\DOMAIN.hs"

LRESULT Cmdsegmentmaprelate(CWnd *wnd, const String& s)
{
	new FormSegmentMapRelate(wnd, s.c_str());
	return -1;
}

FormSegmentMapRelate::FormSegmentMapRelate(CWnd* mw, const char* sPar)
: FormSegmentMapCreate(mw, TR("Relation to other Features")),negation(false),fsMatrix(NULL),relation(0),inCallBack(false)
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

	//relations.push_back(TR("Contains") + ".Contains");
	relations.push_back(TR("CoveredBy") + ".CoveredBy");
	relations.push_back(TR("Covers") + ".Covers");
	relations.push_back(TR("Crosses") + ".Crosses");
	relations.push_back(TR("Disjoint") + ".DisJoint");
	relations.push_back(TR("Equals") + ".Equals");
	relations.push_back(TR("Intersects") + ".Intersect");
	relations.push_back(TR("DE9IM Matrix") + ".MAT16ICO");
	relations.push_back(TR("Overlaps") + ".Overlaps");
	relations.push_back(TR("Touches") + ".Touches");
	relations.push_back(TR("Within") + ".Within");

	fInput1 = new FieldDataType(root, TR("First Input map"), &sInMap1, ".mps", true);
	//fInput1->SetCallBack((NotifyProc)&FormSegmentMapRelate::SetExtension);
	StaticText *txt = new StaticText(root,TR("Relation"));
	FieldGroup *fg = new FieldGroup(root);
	CheckBox *cb = new CheckBox(fg,TR("Not"),&negation);
	cb->Align(txt, AL_AFTER);
	fsRelation = new FieldOneSelectString(fg, "", &relation, relations);
	fsRelation->Align(cb,AL_AFTER);
	fsRelation->SetComboWidth(60);
	fsRelation->SetCallBack((NotifyProc)&FormSegmentMapRelate::SetPattern);
	fg->SetIndependentPos();
	fsMatrix = new FieldString(root,TR("DE9IM Patterm"),&pattern);
	fsMatrix->Hide();
	fsMatrix->Align(txt,AL_UNDER);
	FieldBlank *fb  = new FieldBlank(root,0.1);
	//fb->Align(fsMatrix,AL_UNDER);
	fInput2 = new FieldDataType(root, TR("Second Input map"), &sInMap2, ".mps.mpa.mpp", true);
	//fInput2->SetCallBack((NotifyProc)&FormSegmentMapRelate::SetExtension);
		
	new FieldBlank(root);
			
    initSegmentMapOut(false);
	String sFill('x', 60);
	//stRemark = new StaticText(root, sFill);
	//stRemark->SetIndependentPos();
	
	//SetAppHelpTopic(htpGenMapSegmentMapRelate);
	create();
}

int FormSegmentMapRelate::SetExtension(Event *ev) {
	if ( inCallBack)
		return 1;
	inCallBack = true;
	fInput1->StoreData();
	fInput2->StoreData();
	FileName fnMap1(sInMap1);
	FileName fnMap2(sInMap2);
	if ( fnMap1.sExt == "" && fnMap2.sExt == "") { // start situation; ignore
		inCallBack = false;
		return 1;
	}

	if ( fnMap1.sExt == ".mps") {
		ObjectExtensionLister *ol = new ObjectExtensionLister(0, ".mps.mpa.mpp");
		fInput2->SetObjLister(ol);
	}
	if ( fnMap2.sExt == ".mps") {
		ObjectExtensionLister *ol = new ObjectExtensionLister(0, ".mps.mpa.mpp");
		fInput1->SetObjLister(ol);
	}
	if ( fnMap1.sExt != ".mps" && fnMap1.sExt != "") {
		ObjectExtensionLister *ol = new ObjectExtensionLister(0, ".mps");
		fInput2->SetObjLister(ol);
	}
	if ( fnMap2.sExt != ".mps" && fnMap2.sExt != "") {
		ObjectExtensionLister *ol = new ObjectExtensionLister(0, ".mps");
		fInput1->SetObjLister(ol);
	}
	inCallBack = false;

	return 1;
}
FormEntry *FormSegmentMapRelate::CheckData() {
	fInput1->StoreData();
	fInput2->StoreData();
	if ( relation == 6) {
		if ( pattern.size() != 9)
			return fsMatrix;
		pattern.toUpper();
		if ( pattern.find_first_not_of("*TF") != string::npos)
			return fsMatrix;
	}
	FileName fnMap1(sInMap1);
	FileName fnMap2(sInMap2);
	if ( fnMap1.sExt != ".mps" && fnMap2.sExt != ".mps")
		return fInput1;
	return NULL;
}

int FormSegmentMapRelate::SetPattern(Event *ev) {
	fsRelation->StoreData();
	if ( relations.size() > 0 && fsMatrix ) {
		if (relations[relation] == TR("DE9IM Matrix") + ".MAT16ICO") {
			fsMatrix->Show();
		} else {
			fsMatrix->Hide();
		}
	}
	return 1;

}

int FormSegmentMapRelate::exec()
{
	FormSegmentMapCreate::exec();
	FileName fnMap1(sInMap1);
	String rel;
	FileName fnMap2(sInMap2);
	switch(relation) {
		case 0:
			rel = "CoveredBy"; break;
		case 1:
			rel = "Covers"; break;
		case 2:
			rel = "Crosses"; break;
		case 3:
			rel = "Disjoint"; break;
		case 4:
			rel = "Equals"; break;
		case 5:
			rel = "Intersects"; break;
		case 6:
			rel = "DE-9IM[" + pattern + "]"; break;
		case 7:
			rel = "Overlap"; break;
		case 8:
			rel = "Touches";break;
		case 9:
			rel = "Within";break;
	};
	String negate = negation ? "true" : "false";
	String sExpr = String("SegmentMapRelate(%S,%S,%S,%S)", fnMap1.sRelative(), fnMap2.sRelative(),rel,negate);


	execSegmentMapOut(sExpr);

	return 0;
}

