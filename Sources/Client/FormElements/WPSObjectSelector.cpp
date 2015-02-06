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
#include "Client\ilwis.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\objlist.h"
#include "Client\FormElements\WPSObjectSelector.h"



WPSObjectSelector::WPSObjectSelector(FormEntry* parent, const String& sQuestion, String *txt, ObjectLister *obl) : 
FieldOneSelectString(parent, sQuestion, &(dummyvalue=0), files, false),lister(0)
{
	if ( obl) {
		obl->AddObjects();
		vector<String> names = obl->GetNames();
		files.resize(names.size());
		copy(names.begin(), names.end(), files.begin());
		resetContent(files);
		*txt = files[1];
		lister = obl;
	}
	text = txt;
}

WPSObjectSelector::~WPSObjectSelector() {
	if (lister)
		delete lister;
}

void WPSObjectSelector::setRemoteFiles(const vector<String>& remfiles){
	remotefiles = remfiles;
	changeContent();
	delete lister;
	lister = 0;
}

void WPSObjectSelector::StoreData(){
	FieldOneSelectString::StoreData();
	if ( dummyvalue != -1 && dummyvalue < files.size()) {
		*text = files[dummyvalue];
	}
}

void WPSObjectSelector::changeContent() {
	if ( lister) {
		lister->AddObjects();
		vector<String> names = lister->GetNames();
		for(int i=0; i < names.size(); ++i) {
			FileName fn(names[i]);
			names[i] = fn.sFile + fn.sExt;
		}
		files.clear();
		files.resize(names.size());

		copy(names.begin(), names.end(), files.begin());
	}
	if ( remotefiles.size() > 0) {
		files.clear();
		files.resize(remotefiles.size());
		copy(remotefiles.begin(), remotefiles.end(), files.begin());
	}
	resetContent(files);
}
void WPSObjectSelector::setLister(ObjectLister *obl){
	if ( lister)
		delete lister;
	lister = obl;
	remotefiles.clear();
	changeContent();
}

