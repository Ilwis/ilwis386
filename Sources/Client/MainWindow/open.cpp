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

#include "Client\Headers\AppFormsPCH.h"
#include "Client\MainWindow\ACTION.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\WinThread.h"
#include "Client\Editors\Stereoscopy\StereoscopeWindow.h"
#include "Client\Editors\Stereoscopy\StereoMapWindow.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\File\Directory.h"
#include "Client\Forms\PropForm.h"
#include "Client\MainWindow\open.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\GEOREF3D.hs"

String sRootName(const String& sDb)
{
	size_t iWhere = sDb.rfind('\\');
	if ( iWhere != string::npos)
	{
		size_t iWhere2 = sDb.rfind('\\', iWhere - 1);
		if ( iWhere2 == string::npos ) iWhere2 = 0;
		String sDbName = sDb.substr(iWhere2, iWhere - iWhere2 );
		return sDbName.sUnQuote();
	}
	return "";
}
LRESULT OpenImplicitObject(ParmList& pm) {

	FileName fnRoot(sRootName(pm.sGet(0))); // parse the name te retrieve the name of the container
	if ( pm.fExist("quiet")){
		pm.Remove("quiet");
		pm.Add(new Parm("nothreads", true));
	}
	if ( fnRoot.fExist()) {// it must exist, else we can not use it to get extra information from it
		FileName fnTemp(pm.sGet(0));
		if ( fnTemp.sFile != "")
		{
			FileName fnTable(fnTemp.sFile, fnTemp.sExt); // name of the file to be produced
			if ( IlwisObject::iotObjectType(fnRoot) != IlwisObject::iotANY) //  the container was an ILWIS container
				pm.Add(new Parm("collection", fnRoot.sRelativeQuoted()));
			IlwWinApp()->OpenDocumentFile(fnTable.sFullPath().c_str(), pm);
		}
	}
	return 1;
}

LRESULT OpenForeignLocation(ParmList& pm) {
	
	FileName fnRoot(sRootName(pm.sGet(0))); // parse the name te retrieve the name of the container
	if ( !pm.fExist("method")){
		String sMethod = ForeignFormat::sFindDefaultMethod(fnRoot);
		pm.Add(new Parm("method", sMethod));
	}
	if ( pm.fExist("quiet")){
		pm.Remove("quiet");
		pm.Add(new Parm("nothreads", true));
	}

	String sLocation = pm.sGet(0);
	bool fUrl = URL::isUrl(sLocation);
	if ( fUrl)
		IlwWinApp()->OpenDocumentFile(URL(sLocation), pm);

	return 1;
}

LRESULT OpenForeignDataFile(ParmList& pm) {
	FileName fnFile(pm.sGet(0));
	if ( !pm.fExist("method")){
		String sMethod = ForeignFormat::sFindDefaultMethod(fnFile);
		pm.Add(new Parm("method", sMethod));
	}
	if ( pm.fExist("quiet")){
		pm.Remove("quiet");
		pm.Add(new Parm("nothreads", true));
	}
	pm.Add(new Parm("table", fnFile.sRelativeQuoted()));

	IlwWinApp()->OpenDocumentFile(fnFile.sFullPath(), pm);
	return 1;
}

LRESULT OpenIlwisMaps(CWnd *parent, const String& sCmd) {

	ParmList pm(sCmd);
	FileName fnFile(pm.sGet(0));
	if (fnFile.sExt == ".stp")
	{
		IlwisSettings settings("DefaultSettings");
		bool fShowInStereoscope = ("Stereoscope" == settings.sValue("StereoPairDblClkAction", "Stereoscope"));
		if (fShowInStereoscope)
			Cmdstereoscope(parent, sCmd);
		else
			Cmdanaglyph(parent, sCmd);
	}
	else if (pm.fExist("noask"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otNOASK);
	else if (pm.fExist("mpr"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otMPR);
	else if (pm.fExist("mpa"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otMPA);
	else if (pm.fExist("mps"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otMPS);
	else if (pm.fExist("mpp"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otMPP);
	else if (fnFile.sExt == ".mpl" && pm.fExist("animation"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otANIMATION);
	else if (fnFile.sExt == ".ioc" && pm.fExist("animation"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otANIMATION);
	else if (fnFile.sExt == ".mpl" && pm.fExist("colorcomposite"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otCOLORCOMP);
	else if (fnFile.sExt == ".ioc" && pm.fExist("layer"))
		IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(), IlwisDocument::otCOLLECTION);
	else {
		int sz = pm.iFlags();
		if ( sz == 0)
			IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str());
		else {
			IlwWinApp()->OpenDocumentFile(fnFile.sFullNameQuoted().c_str(),pm);
		}
	}

	return 1;
}

bool isCommand(const String& sC) {
	for(int i=0; i< sC.size(); ++i) {
		char c = sC.at(i);
		if ( i == 0 && !isalpha(c))
			return false;
		if ( !isalnum(c))
			return false;
	}
	return true;
}

LRESULT Cmdopen(CWnd *parent, const String& sCmd){
	if (sCmd == "") {
		OpenBaseMap(parent);
		return 1;
	}
	String sCommand = sCmd.sHead("(");
	if ( isCommand(sCommand)) {
		IlwWinApp()->OpenDocumentFile(sCmd.c_str());
		return 1;
	}
	ParmList pm(sCmd);
	FileName fnFile(pm.sGet(0));
	if ( File::fExist(fnFile) && IlwisObject::iotObjectType(fnFile) != IlwisObject::iotANY) {
		return OpenIlwisMaps(parent, sCmd);
	}
	if ( File::fExist(fnFile) && IlwisObject::iotObjectType(fnFile) == IlwisObject::iotANY) {
		return OpenForeignDataFile(pm);
	}
	if ( !File::fExist(fnFile) && IlwisObject::iotObjectType(fnFile) != IlwisObject::iotANY) {
		return OpenImplicitObject(pm);
	}
	if ( !File::fExist(fnFile) && IlwisObject::iotObjectType(fnFile) == IlwisObject::iotANY) {
		return OpenForeignLocation(pm);
	}
	
	return 1;
}



LRESULT Cmdstereoscope(CWnd *parent, const String& sCmd)
{
	WinThread* thr = new WinThread(RUNTIME_CLASS(StereoscopeWindow), sCmd);
	if (thr) 
		thr->CreateThread(0, 0);

	return -1;
}

LRESULT Cmdanaglyph(CWnd* parent, const String& sCmd)
{
	CDC *dc = parent->GetDC();
	ParmList pm(sCmd);
	bool fPalette = (dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
	if (fPalette) {
		MessageBox(0,
			TR("To display a map list as color composite,\nthe display settings in the Control Panel\nneed to be set on more than 256 colors").c_str(),
			TR("Show Stereo Pair as Anaglyph").c_str(),
			MB_OK|MB_ICONSTOP);
		return -1;
	}
	if (sCmd.size() == 0 )
		OpenAnaglyph(parent);
	else {
		String s = sCmd;
		FileName fn(s);
		if ("" == fn.sExt)
			fn.sExt = ".stp";
		s = fn.sFullNameQuoted();
		if (pm.fExist("noask") || pm.fExist("quiet")) {
			WinThread* thr = new WinThread(IlwWinApp()->docTemplMapWindow(), s.c_str(), IlwisDocument::otNOASK);
			if (thr) 
				thr->CreateThread(0, 0);
		}
		else
			IlwWinApp()->OpenDocumentAsMap(s.c_str());
	}
	return -1;
}

void OpenAnaglyph(CWnd *parent)
{
	class ShowAnaglyphForm: public FormWithDest
	{
	public:
		ShowAnaglyphForm(CWnd* parent, String* sName)
			: FormWithDest(parent, TR("Show Stereo Pair as Anaglyph"))
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".stp");
			SetMenHelpTopic("ilwismen\\open_stereopair_as_anaglyph.htm");
			create();
		}
	};
	ResetDir();
	String sMap;
	ShowAnaglyphForm frm(parent, &sMap);
	if (frm.fOkClicked()) 
	{
		String sCmd ("anaglyph %S", sMap);
		IlwWinApp()->Execute(sCmd);
	}
}

void OpenBaseMap(CWnd *parent)
{
	class ShowBaseMapForm: public FormWithDest
	{
	public:
		ShowBaseMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, TR("Open Object"))
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			
			new FieldDataObject(root, sName);
			SetMenHelpTopic("ilwismen\\open_show_map_or_other_object.htm");
			create();
		}
	};
	ResetDir();
	String sMap;
	ShowBaseMapForm frm(parent, &sMap);
	if (frm.fOkClicked()) 
	{
		FileName fnMap(sMap);
		IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));
	}
}

LRESULT Cmdedit(CWnd *parent, const String& sCmd)
{
	if (sCmd == "")
		EditBaseMap(parent);
	else
	{
		FileName fn(sCmd);
		if (".stp" == fn.sExt) 
			Cmdmakestereopair(parent, sCmd);
		else
			IlwWinApp()->OpenDocumentFile(fn.sFullNameQuoted().c_str(), IlwisDocument::otEDIT);
	}
	return -1;
}

LRESULT Cmdmakestereopair(CWnd *parent, const String& sCmd)
{
  WinThread* thr = new WinThread(RUNTIME_CLASS(StereoMapWindow), sCmd);
	if (thr) 
		thr->CreateThread(0, 0);

	return -1;
}

int OpenMapListSlideShow(CWnd *parent)
{
	class ShowMapListForm: public FormWithDest
	{
	public:
		ShowMapListForm(CWnd* parent, String* sName) : FormWithDest(parent, TR("Show Map List as Slide Show"))
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpl");
			SetMenHelpTopic("ilwismen\\open_show_map_list_as_slide_show.htm");
			create();
		}
	};
	ResetDir();
	String sMap;
	ShowMapListForm frm(parent, &sMap);
	if (frm.fOkClicked()) 
	{
		FileName fnMap(sMap);
		String sCmd = "film ";
		sCmd &= fnMap.sFullNameQuoted();
		IlwWinApp()->Execute(sCmd);
	}
	return 1;
}

LRESULT Cmdmplslideshow(CWnd *parent, const String& sCmd)
{
	if (sCmd == "")
		OpenMapListSlideShow(parent);
	else
	{
		ParmList p(sCmd);
		String sC = p.sGet(0);		
		IlwWinApp()->OpenDocumentAsSlideShow(sC.c_str());
	}
	return -1;
}

void EditBaseMap(CWnd *parent)
{
	class EditBaseMapForm: public FormWithDest
	{
	public:
		EditBaseMapForm(CWnd *p, String* sName) : FormWithDest(p, TR("Edit Object"))
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			FieldDataObject* fdo = new FieldDataObject(root, sName);
			fdo->SetOnlyEditable();
			SetMenHelpTopic("ilwismen\\edit_object_select_object.htm");
			create();
		}
	};
	ResetDir();
	String sMap;
	EditBaseMapForm frm(parent, &sMap);
	if (frm.fOkClicked()) 
	{
		String sCmd = "edit ";
		sCmd &= sMap;
		IlwWinApp()->Execute(sCmd);
	}
}

LRESULT Cmdlayout(CWnd *parent, const String& sCmd)
{
	IlwWinApp()->OpenDocumentAsLayout(sCmd.c_str());
	return -1;
}

LRESULT Cmdgraph(CWnd *parent, const String& sCmd)
{
	IlwWinApp()->OpenDocumentAsGraph(sCmd.c_str());

  return -1;
}

LRESULT Cmdrosediagram(CWnd *parent, const String& sCmd)
{
  IlwWinApp()->OpenDocumentAsRoseDiagram(sCmd.c_str());

  return -1;
}

LRESULT Cmddisplay3d(CWnd *wndOwner, const String& sCmd)
{
	class Display3DForm: public FormWithDest
	{
	public:
		Display3DForm(CWnd* parent, String* sGrf)
			: FormWithDest(parent, TR("Display 3D"))
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldGeoRef3DC(root, TR("&GeoReference"), sGrf);
			SetMenHelpTopic("ilwismen\\display_3d.htm");
			create();
		}
	};
	String sGrf;
	if (sCmd.size() != 0) 
	{
		TextInput ip(sCmd.c_str());
		TokenizerBase tokenizer(&ip);
		Token tok;
		tok = tokenizer.tokGet();
		sGrf = tok.sVal();
	}
	if ("" == sGrf) 
	{
		ResetDir();
		Display3DForm frm(wndOwner, &sGrf);
		if (!frm.fOkClicked()) 
			return -1;
	}
	IlwWinApp()->Execute(String("open %S", sGrf));

	return -1;
}

LRESULT Cmdpropobject(CWnd *wndOwner, const String& str)
{
	ParmList pl(str);
	String sObj = pl.sGet(0);
	if (sObj.length() == 0)
	{
		DataObjectForm frm(wndOwner, TR("View Properties of..."), &sObj, "ilwismen\\view_edit_properties_of.htm");
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fn(sObj,true);

	ObjectStruct* os = new ObjectStruct(fn);
	if (os->obj.fValid())
	{
		os->eRequest = ObjectStruct::ePROP; 
		AfxBeginThread(ShowPropForm, (LPVOID)os); 
	}
	else
		delete os;

	return -1;
}
