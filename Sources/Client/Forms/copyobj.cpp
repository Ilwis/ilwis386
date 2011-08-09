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
/* CopyObject
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK    7 Aug 98    6:13 pm
*/

#include "Client\Headers\AppFormsPCH.h"
#include "Client\ilwis.h"
#include "Engine\Base\File\COPIER.H"
#include "Headers\hourglss.h"
#include "Headers\Hs\Mainwind.hs"
#include "Client\Forms\copyobj.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"

String CopyObjectForm::sLastDir = "";

CopyObjectForm::CopyObjectForm(CWnd* wPar, list<FileName>& lfnFiles, String& _sTo, bool fOnlyDirectory)
: FormWithDest(wPar, TR("Copy Object To")), m_fsNewName(0), sTo(_sTo)
{
	FileName fn((*(lfnFiles.begin())));
	fnOld = fn;
	IlwisObject obj = IlwisObject::obj(fn);
	String sTypeName = obj.fValid() ? obj->sTypeName() : fn.sRelative();
	String str;
	if ( lfnFiles.size() > 1)
		str = TR("Copy selected objects to:");
	else
		str = String(TR("Copy %S").c_str(), sTypeName);

	StaticText* st;
	FormEntry *fb = 0;
	st = new StaticText(root, str);
	st->SetIndependentPos();
	int iNameWidth = max(st->psn->iMinWidth, 500);
	if ( obj.fValid() && obj->sDescription[0] && obj->sDescription != sTypeName && lfnFiles.size() == 1) 
	{
		st->psn->SetBound(0,0,0,0);
		String s = obj->sDescription;
		st = new StaticText(root, s);
		st->SetIndependentPos();
		zDimension dim = st->Dim(s);
		dim.width() = min(dim.width(), iNameWidth);
		st->SetFieldWidth((short)dim.width());

	}
	DWORD iFileAtt=GetFileAttributes(sTo.c_str());
	if ((-1 != iFileAtt) && (iFileAtt & FILE_ATTRIBUTE_DIRECTORY))
	{
		iOption = 1;
		sDirectory = sTo;
	}
	else
	{
		iOption = 0;
		sNewName = sTo;
	}
	if ( sDirectory == "")
		sDirectory = sLastDir == ""  ? IlwWinApp()->sGetCurDir() : sLastDir;
	
	if ( !fOnlyDirectory )
	{
		RadioGroup* rg = new RadioGroup(root, "", &iOption);
		RadioButton* rb = new RadioButton(rg, TR("New &Name"));
		m_fsNewName = new FieldString(rb, "", &sNewName, Domain(), false);
		m_fsNewName->SetWidth(120);

		rb = new RadioButton(rg, TR("New &Directory"));
		FieldGroup* fg = new FieldGroup(rb);
		String strCopyTo(TR("Copy %S to").c_str(), sTypeName);
		FormEntry *fb = new FieldBrowseDir(fg, "", strCopyTo, &sDirectory);
		fb->SetIndependentPos();
		fb->SetWidth(120);
		
	}
	else
	{
		String strCopyTo(TR("Copy %S to").c_str(), sTypeName);		
		FormEntry *fb = new FieldBrowseDir(root, "", strCopyTo, &sDirectory);		
		fb->SetWidth(120);
		fb->SetIndependentPos();
	}
	fBreakDep = false;
	if (fb && ".mpv" != fn.sExt && ObjectInfo::fDependenciesForCopy(fn))
	{
		CheckBox* cb = new CheckBox(root, TR("&Break Dependency Link"), &fBreakDep);
		cb->Align(fb, AL_UNDER);
		cb->SetIndependentPos();
	}  
	SetMenHelpTopic("ilwismen\\copy_object_to.htm");      
	create();
};

CopyObjectForm::~CopyObjectForm()  
{
	sLastDir = sDirectory;
}

FormEntry* CopyObjectForm::feDefaultFocus()
{
	if (m_fsNewName)
		m_fsNewName->SelectAll();
	
	return m_fsNewName;
}

class BrowseForm: public FormWithDest
{
public:
  BrowseForm(CWnd* wPar)
  : FormWithDest(wPar, TR("Browse Directory"))
  {
    new FieldDataTypeLarge(root, &sName, "");
    create();
  }
  String sName;
};

int CopyObjectForm::Browse(Event*)
{
	return 1;
}

int CopyObjectForm::exec()
{
	root->StoreData();
	if ( sNewName != "" )
	{
		FileName fnNew(sNewName);
		if ( fnNew.sExt == "")
			fnNew.sExt = fnOld.sExt;
		
		sTo = fnNew.sFullPathQuoted();
	}	
	else if ( sDirectory != "" )
	{
		if ( sDirectory[sDirectory.size() - 1] != '\\' )
			sDirectory &= "\\";
		Directory dirNew(sDirectory);
		sTo = dirNew.sFullPathQuoted();
	}		
	return 1;	
}
