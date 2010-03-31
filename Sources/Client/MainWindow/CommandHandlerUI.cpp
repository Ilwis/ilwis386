#include "Client\Headers\AppFormsPCH.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Client\MainWindow\ACTION.H"
#include "Client\ilwis.h"
#include "Client\Forms\DatumWizard.h"
#include "Client\Forms\IMP14.H"
#include "Client\Forms\expobj.h"
#include "Client\Forms\IMPORT.H"
#include "Client\Forms\EXPORT.H"
#include "Headers\constant.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Engine\Base\System\LOGGER.H"
#include "Client\Base\BaseCommandHandlerUI.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Scripting\Script.h"
#include "Client\Forms\ScriptForm.h"
#include "Client\Forms\SimpleCalcResultsForm.h"
#include "Engine\Scripting\Calc.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Client\MainWindow\mainwind.h"
#include "Client\MainWindow\Catalog\Catalog.h"
#include "Client\MainWindow\Catalog\CatalogDocument.h"
#include "Client\Base\WinThread.h"
#include "Client\Forms\PropForm.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Client\Forms\RemoveFilesCollector.h"
#include "Client\Editors\Stereoscopy\StereoMapWindow.h"
#include "Client\Editors\Stereoscopy\StereoscopeWindow.h"
#include "Client\Mapwindow\MapListSplitterWindow.h"
#include "Client\MainWindow\open.h"
#include "Client\Forms\DEFFORM.H"
#include "Headers\messages.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Domain\dmgroup.h"
#include "Client\Forms\RemoveFilesCollector.h"
#include "Engine\DataExchange\DatabaseCollection.h"
#include "Headers\Hs\Mainwind.hs"
#include "Headers\Hs\Sample.hs"
#include "Headers\Hs\GEOREF3D.hs"
#include "Headers\Hs\Applications.hs"


void ResetDir();
LRESULT Cmdsample(CWnd *p, const String& s);
LRESULT Cmdmakestereopair(CWnd *parent, const String& sCmd);

//void OpenAnaglyph();
//String sDataBaseName(const String& sDb);

struct WinAndParm 
{ // struct for passing parms to import, export, copy and delete threads
	WinAndParm(): handle(0), s(0), sPath(0) {}
	~WinAndParm() { delete s; delete sPath; }
	HWND handle;
	String* s;
	String* sPath;
};

CommandHandlerUI::CommandHandlerUI(CFrameWnd *frm) : 
	BaseCommandHandlerUI(frm)
{
}

CommandHandlerUI::~CommandHandlerUI()
{
}

LRESULT Cmdimport(CWnd *wndOwner, const String& sPar)
{
	// call the form within the mainwindow thread
	// AskImport will start the actual import in a thread
	Importing import;
	import.AskImport(wndOwner, sPar.scVal());

	return -1;
}


int OpenMapListColorComp(CWnd * wnd)
{
	class ShowMapListForm: public FormWithDest
	{
	public:
		ShowMapListForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMapListColorComp)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpl");
			SetMenHelpTopic(htpOpenMapListColorComp);
			create();
		}
	};
	ResetDir();
	String sMap;
	ShowMapListForm frm(wnd, &sMap);
	if (frm.fOkClicked()) 
	{
		String sCmd ("colorcomp %S", sMap);
		IlwWinApp()->Execute(sCmd);
	}
	return 1;
}

LRESULT Cmdmplcolorcomp(CWnd *wnd, const String& sCmd)
{
	CDC *dc = wnd->GetDC();

	bool fPalette = (dc->GetDeviceCaps(RASTERCAPS) & RC_PALETTE) != 0;
	if (fPalette) {
		MessageBox(0,
			SMSMsgColorDepth16bitOrHigher.scVal(),
			SMSTitleShowMapListColorComp.scVal(),
			MB_OK|MB_ICONSTOP);
		return -1;
	}
	if (sCmd.size() == 0 )
		OpenMapListColorComp(wnd);
	else {
		ParmList pm (sCmd);
		String s = pm.sGet(0);
		FileName fn(s);
		if ("" == fn.sExt)
			fn.sExt = ".mpl";
		s = fn.sFullNameQuoted();
		if (pm.fExist("noask")) //  || pm.fExist("quiet")) // quiet is always supplied by script. I leave it up to the user to decide whether to get the form or not.
		{
			WinThread* thr = new WinThread(IlwWinApp()->docTemplMapWindow(), s.scVal(), IlwisDocument::otNOASK);
			if (thr) 
				thr->CreateThread(0, 0);
		}
		else
			IlwWinApp()->OpenDocumentAsMap(s.scVal());
	}

	return -1;
}

LRESULT Cmddel(CWnd *wndOwner, const String& sCmd)
{
	String sFn;
	if (sCmd == "") {
		DataObjectForm frm(wndOwner, SMSTitleDelObject.c_str(), &sFn, htpSelDelete);
		if (!frm.fOkClicked()) 
			return -1;
	}
	return (LRESULT)new String(sFn);
}

void CommandHandlerUI::CmdExport14(const String& sCmd)
{
	String s(sCmd);
	ParmList pl(s);
	String s1 = pl.sGet(0);
	String s2 = pl.sGet(1);
	String sFn;
	if (s1.length() == 0)
	{
		DataObjectForm frm(wndOwner, SMSTitleExport14, &sFn, htpSelExport);
		if (!frm.fOkClicked()) 
			return;
	}
	else
		sFn = s1;
	FileName fn(sFn);
	
	ExportObject(wndOwner,fn,s2);
}

LRESULT Cmdsmce(CWnd* parent, const String& sCmd)
{
  IlwWinApp()->OpenDocumentAsSMCE(sCmd.scVal());

  return -1;
}

LRESULT Cmddir(CWnd *wndOwner, const String& sCmd)
{
	MainWindow *wndMain = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
	if ( wndMain)
	{
		CMDIChildWnd *wndChild = wndMain->MDIGetActive();
		if ( wndChild)
		{
			CView *view = wndChild->GetActiveView();
			Catalog *cat = dynamic_cast<Catalog *>(view);
			if ( cat )
			{
				CatalogDocument *doc = dynamic_cast<CatalogDocument *> (cat->GetDocument());
				if ( doc )
				{
					doc->SetFilter(sCmd);
				}
			}
		}
		
	}

	return -1;
}

void CommandHandlerUI::CmdSetWindowRect(const String& sC)
{
	Array<String> arParts;
	ParmList p(sC);
	size_t iWhere = sC.find("-quiet");

	String sCmd = iWhere == string::npos ? p.sCmd() : sC.substr(0, iWhere - 1);

	arParts.push_back(String());
	for(unsigned int i=0; i < sCmd.size(); ++i)
	{
		if ( sCmd[i] == ',')
			arParts.push_back(String());
		else
			arParts.at(arParts.size() - 1) += sCmd[i];
	}
	if ( arParts.size() < 4)
		throw ErrorObject(SMSErrNotEnoughParameters);
	IlwWinApp()->SetNextWindowRect(CPoint(arParts[0].iVal(), arParts[1].iVal()),
		                             CSize(arParts[2].iVal(), arParts[3].iVal()));
}

void CommandHandlerUI::EditObject(const FileName& fn)
{
	IlwWinApp()->OpenDocumentFile(fn.sFullNameQuoted().scVal(), IlwisDocument::otEDIT);
}

LRESULT Cmdmkdir(CWnd *wndOwner, const String& str)
{
	class FormCreateDir: public FormWithDest
	{
	public:
		FormCreateDir(CWnd* wPar, String* sDir)
			: FormWithDest(wPar, SMSTitleCreateDirectory)
		{
			iImg = IlwWinApp()->iImage("directory");
			
			new FieldString(root, SMSUiDirName, sDir);
			create();
		}
	};
	ParmList p(str);
	String sDir = p.sGet(0);
	SetCurrentDirectory(IlwWinApp()->sGetCurDir().scVal());
	if ("" == sDir) 
	{
		FormCreateDir frm(wndOwner, &sDir);
		if (!frm.fOkClicked())
			return -1;
	}
	String  sNewDir = sDir.sUnQuote();
	Directory  dir(sNewDir);
	if (!dir.fValid())	
		return -1;	
	if (!CreateDirectory(sNewDir.c_str(), NULL))
		throw WindowsError(GetLastError());

	IlwWinApp()->DirChanged();
	IlwWinApp()->Execute("opendir " + sDir);
	//CmdOpenDir(sDir);
	MainWindow *mw = dynamic_cast<MainWindow *>( AfxGetMainWnd()) ;
	if ( mw)
		mw->RefreshNavigator();

	return -1;
	
}

LRESULT Cmdremovedir(CWnd *wndOwner, const String& str)
{
	class FormRemoveDir: public FormWithDest
	{
	public:
		FormRemoveDir(CWnd* wPar, String* sDir)
			: FormWithDest(wPar, SMSTitleRemoveDirectory)
		{
			iImg = IlwWinApp()->iImage("directory");
			new FieldString(root, SMSUiDirName, sDir);
			create();
		}
	};
	ParmList p(str);
	String sDir = p.sGet(0).sUnQuote();
	SetCurrentDirectory(IlwWinApp()->sGetCurDir().scVal());
	if ("" == sDir) 
	{
		FormRemoveDir frm(wndOwner, &sDir);
		if (!frm.fOkClicked())
			return -1;
	}
	Directory  dir(sDir);
	if ( dir.fValid())
	{
		if (_rmdir(sDir.c_str()))
			throw DosError(errno);
	}

	IlwWinApp()->DirChanged();

	return -1;
}



//void CommandHandlerUI::CmdColorComp(const String& s)
//{
//  new FormColorComp(wndOwner, s.scVal());
//}

void CommandHandlerUI::PopupHelpObject(const String& sObj)
{
	FileName fn(sObj);
	ObjectStruct* os = new ObjectStruct(fn);
	if (os->obj.fValid())
	{
		os->eRequest = ObjectStruct::eHELP; 
		AfxBeginThread(ShowPropForm, (LPVOID)os); 
	}
	else
		delete os;
}

LRESULT Cmdseg(CWnd *wndOwner, const String& s)
{
	class ShowMapForm: public FormWithDest
	{
	public:
		ShowMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mps");
			//      SetMenHelpTopic(htpOpenSegmentMap);
			create();
		}
	};
	String sMap = s;
	if ("" == sMap)
	{
		ResetDir();
		ShowMapForm frm(wndOwner, &sMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnMap(sMap);
	if ("" == fnMap.sExt)
		fnMap.sExt = ".mps";
	IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));
	return -1;
}

LRESULT Cmdpol(CWnd *wndOwner, const String& s)
{
	class ShowMapForm: public FormWithDest
	{
	public:
		ShowMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpa");
			//      SetMenHelpTopic(htpOpenPolygonMap);
			create();
		}
	};
	String sMap = s;
	if ("" == sMap)
	{
		ResetDir();
		ShowMapForm frm(wndOwner, &sMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnMap(sMap);
	if ("" == fnMap.sExt)
		fnMap.sExt = ".mpa";
	IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));

	return -1;
}

LRESULT Cmdtbl(CWnd *wndOwner, const String& s)
{
	class ShowTblForm: public FormWithDest
	{
	public:
		ShowTblForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowTable)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".tbt.ta2");
			SetMenHelpTopic(htpOpenTable);
			create();
		}
	};
	String sTbl = s;
	if ("" == sTbl)
	{
		ResetDir();
		ShowTblForm frm(wndOwner, &sTbl);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnTbl(sTbl);
	if ("" == fnTbl.sExt)
		fnTbl.sExt = ".tbt";
	IlwWinApp()->Execute(String("show %S", fnTbl.sFullNameQuoted()));

	return -1;
}

LRESULT Cmdpnt(CWnd *wndOwner,const String& s)
{
	class ShowMapForm: public FormWithDest
	{
	public:
		ShowMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpp");
			//      SetMenHelpTopic(htpOpenPointMap);
			create();
		}
	};
	String sMap = s;
	if ("" == sMap)
	{
		ResetDir();
		ShowMapForm frm(wndOwner, &sMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnMap(sMap);
	if ("" == fnMap.sExt)
		fnMap.sExt = ".mpp";
	IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));

	return -1;
}

LRESULT Cmdshowastable(CWnd *wndOwner, const String& s)
{
	class ShowTblForm: public FormWithDest
	{
	public:
		ShowTblForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowTable)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".dom.rpr.mpp.grf.tbt.ta2.his.hsa.hss.hsp.atx", new TableAsLister(""));
			SetMenHelpTopic(htpOpenAsTable);
			create();
		}
	};
	ResetDir();
	String sTbl = s;
	if ("" == sTbl) {
		ShowTblForm frm(wndOwner, &sTbl);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnTbl(sTbl);
	if ("" == fnTbl.sExt) 
		fnTbl.sExt = ".tbt";
	sTbl = fnTbl.sFullNameQuoted();
	IlwWinApp()->OpenDocumentAsTable(sTbl.scVal());

	return -1;
}

LRESULT Cmdshowasdomain(CWnd *wndOwner, const String& s)
{
	if ("" == s)
		return -1;
	FileName fn(s);
	if ("" == fn.sExt) 
		fn.sExt = ".dom";
	String str = fn.sFullNameQuoted();
	IlwWinApp()->OpenDocumentAsDomain(str.scVal());

	return -1;
}

LRESULT Cmdshowasrepresentation(CWnd *wndOwner, const String& s)
{
	if ("" == s)
		return -1;
	FileName fn(s);
	if ("" == fn.sExt) 
		fn.sExt = ".rpr";
	String str = fn.sFullNameQuoted();
	IlwWinApp()->OpenDocumentAsRpr(str.scVal());

	return -1;
}

LRESULT Cmdmap(CWnd *wndOwner, const String& s)
{
	class ShowMapForm: public FormWithDest
	{
	public:
		ShowMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpr");
			//      SetMenHelpTopic(htpOpenMap);
			create();
		}
	};
	String sMap = s;
	if ("" == sMap)
	{
		ResetDir();
		ShowMapForm frm(wndOwner, &sMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnMap(sMap);
	if ("" == fnMap.sExt)
		fnMap.sExt = ".mpr";
	IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));

	return -1;
}

LRESULT Cmdview(CWnd *wndOwner,const String& s)
{
	class ShowMapForm: public FormWithDest
	{
	public:
		ShowMapForm(CWnd* parent, String* sName)
			: FormWithDest(parent, SMSTitleShowMap)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldDataTypeLarge(root, sName, ".mpv");
			create();
		}
	};
	String sMap = s;
	if ("" == sMap)
	{
		ResetDir();
		ShowMapForm frm(wndOwner, &sMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	FileName fnMap(sMap);
	if ("" == fnMap.sExt)
		fnMap.sExt = ".mpv";
	IlwWinApp()->Execute(String("show %S", fnMap.sFullNameQuoted()));

	return -1;
}

LRESULT Cmdpixelinfo(CWnd *parent, const String& s)
{
	IlwWinApp()->OpenPixelInfo(s.scVal());
	return -1;
}

static void UpdateCatalog()
{
	AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, 0);
}

static void SortDomainError(const String& sDomain)
{
	throw ErrorSortDomain(sDomain, FileName(), 0);
}

static void NotDomainGroupError(const String& sDomain)
{
	throw ErrorObject(
		WhatError(String(SDATErrDomainGroupExpected_S.scVal(), sDomain), 0), 
		FileName());
}

void CommandHandlerUI::CmdDelFile(const String& sFiles)
{
	Array<FileName> afn;
	File::GetFileNames(sFiles, afn);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
		_unlink(afn[i].sFullName(true).scVal());
	UpdateCatalog();
}

void CommandHandlerUI::CmdDelColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	Table tbl(col->fnTbl);
	col = tbl->col(col->sName());
	if (!col->fReadOnly() && !col->fOwnedByTable()) 
	{
		tbl->LoadData();
		col->fErase = true;
		tbl->RemoveCol(col);
	}
}

void CommandHandlerUI::CmdCalcColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	col->Calc();
}

void CommandHandlerUI::CmdUpdateColumn(const String& sCol)
{
	ParmList pl(sCol);
	String sColumnName = pl.sGet(0);
	Column col = Column(sColumnName);
	col->MakeUpToDate();
}

void CommandHandlerUI::CmdBreakDepColumn(const String& sCol)
{
  Column col = Column(sCol);
  col->BreakDependency();
}

void CommandHandlerUI::CmdDomClassToID(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomClassToIDInThread, (LPVOID)(s));
}

void CommandHandlerUI::CmdDomIDToClass(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomIDToClassInThread, (LPVOID)(s));
}

void CommandHandlerUI::CmdDomPicToClass(const String& sDom)
{
	String *s = new String(sDom);
	AfxBeginThread(CmdDomPicToClassInThread, (LPVOID)(s));
}

UINT CommandHandlerUI::CmdDomClassToIDInThread(void *p)
{
	IlwWinApp()->Context()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainSort *pdsrt = dm->pdsrt();
		if (0 == pdsrt)
			return 0;

		dm.ClassToIdent();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		

	IlwWinApp()->Context()->RemoveThreadLocalVars();

	return 0;
}

UINT CommandHandlerUI::CmdDomPicToClassInThread(void *p)
{
	IlwWinApp()->Context()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainPicture *pdp = dm->pdp();
		if (0 == pdp)
			return 0;

		dm.PictureToClass();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		
	
	IlwWinApp()->Context()->RemoveThreadLocalVars();

	return 0;
}

UINT CommandHandlerUI::CmdDomIDToClassInThread(void *p)
{
	IlwWinApp()->Context()->InitThreadLocalVars();
	try
	{

		String *sDom = (String*)p;
		Domain dm(*sDom);

		delete sDom;

		DomainSort *pdsrt = dm->pdsrt();
		if (0 == pdsrt)
			return 0;

		dm.IdentToClass();
	}
	catch ( ErrorObject& err)
	{
		err.Show();
	}		

	IlwWinApp()->Context()->RemoveThreadLocalVars();

	return 0;
}

void CommandHandlerUI::CmdSetGrf(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; 
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	GeoRef gr(pm.sGet(1));
	for (unsigned int i=0; i < afn.iSize(); ++i)
	{
		Map mp(afn[i]);
		if (!mp->fReadOnly())
		{
			mp->SetGeoRef(gr);
			if (!gr->fGeoRefNone())
				mp->SetSize(gr->rcSize());
		}
	}
}

void CommandHandlerUI::CmdSetCsy(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".grf"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".mpr";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	CoordSystem csy(pm.sGet(1));
	String sMapExtensions(".mpr.mpp.mps.mpa");
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly())
				mp->SetCoordSystem(csy);
		}
		else if (fCIStrEqual(sExt, ".grf"))
		{
			GeoRef grf(fn);
			if (!grf->fReadOnly())
				grf->SetCoordSystem(csy);
		}
	}
}

void CommandHandlerUI::CmdSetAttTable(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	asExt &= ".dom";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	String sMapExtensions(".mpr.mpp.mps.mpa");
	if (pm.fExist(1))
	{
		Table tblAtt(pm.sGet(1));
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				if (!mp->fReadOnly())
					mp->SetAttributeTable(tblAtt);
			}
			else if (fCIStrEqual(sExt, ".dom")) {
				Domain dm(fn);
				if (!dm->fReadOnly())
				{
					DomainSort* pdsrt = dm->pdsrt();
					if (0 != pdsrt)
						pdsrt->SetAttributeTable(tblAtt);
				}
			}
		}
	}
	else
	{
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				if (!mp->fReadOnly())
					mp->SetNoAttributeTable();
			}
			else if (fCIStrEqual(sExt, ".dom"))
			{
				Domain dm(fn);
				if (!dm->fReadOnly()) {
					DomainSort* pdsrt = dm->pdsrt();
					if (0 != pdsrt)
						pdsrt->SetNoAttributeTable();
				}
			}
		}
	}
}

void CommandHandlerUI::CmdSetDom(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	Domain dm(pm.sGet(1));
	ValueRange vr;
	if (pm.iFixed() == 3)
		vr = ValueRange(pm.sGet(2));
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");

	String sMapExtensions(".mpr.mpp.mps.mpa");
	if (fForce)
	{
		for (unsigned int i=0; i < afn.iSize(); ++i)
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				ObjectInfo::WriteElement("BaseMap", "Domain", fn, dm->sName(true, fn.sPath()));
				if (vr.fValid())
					ObjectInfo::WriteElement("BaseMap", "ValueRange", fn, vr->sRange(true));
			}
			else if (fCIStrEqual(fn.sExt, ".tbt"))
				ObjectInfo::WriteElement("Table", "Domain", fn, dm->sName(true, fn.sPath()));
		}  
		return;
	}

	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly())
			{
				if (!mp->fDependent() || mp->fDomainChangeable() )
					mp->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
			}
		}
	}
}

void CommandHandlerUI::CmdSetValRange(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
	File::GetFileNames(pm.sGet(0), afn, &asExt);
	ValueRange vr(pm.sGet(1));

	String sMapExtensions(".mpr.mpp.mps.mpa");
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt; sExt.toLower();
		if (sMapExtensions.find(sExt) != String::npos)
		{
			BaseMap mp(fn);
			if (!mp->fReadOnly()) {
				if (!mp->fDependent() || mp->fDomainChangeable() )
					mp->SetValueRange(vr);
			}
		}
	}
}

void CommandHandlerUI::CmdChangeDom(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(1));
	ValueRange vr;
	if (pm.iFixed() == 3)
		vr = ValueRange(pm.sGet(2));
	Column colAtt;
	if (pm.fExist("colatt"))
		colAtt = Column(pm.sGet("colatt"));
	if (!pm.fExist("col"))
	{
		Array<FileName> afn;
		Array<String> asExt;
		asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps";
		File::GetFileNames(pm.sGet(0), afn, &asExt);
		String sMapExtensions(".mpr.mpp.mps.mpa");
		for (unsigned int i=0; i < afn.iSize(); ++i) 
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt; sExt.toLower();
			if (sMapExtensions.find(sExt) != String::npos)
			{
				BaseMap mp(fn);
				mp->fConvertTo(DomainValueRangeStruct(dm, vr), colAtt);
			}
		}
	}
	else {
		Column col(pm.sGet("col"));
		if (col.fValid())
			col->fConvertTo(DomainValueRangeStruct(dm, vr), colAtt);
	}  
}

void CommandHandlerUI::CmdMergeDom(const String& s)
{
	ParmList pm(s);
	try
	{
		Domain dm1(pm.sGet(0));
		if (dm1->fReadOnly())
			return; // warning ??
		Domain dm2(pm.sGet(1));
		DomainSort* pdsrt1 = dm1->pdsrt();
		DomainSort* pdsrt2 = dm2->pdsrt();
		if (0 == pdsrt1)
			SortDomainError(dm1->sName(true));
		if (0 == pdsrt2)
			SortDomainError(dm2->sName(true));
		pdsrt1->Merge(pdsrt2);
	}
	catch (const ErrorObject&) {
		return;
	}
}

void CommandHandlerUI::CmdAddItemToDomain(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(0));
	if (dm->fReadOnly())
		return; // warning ??
	DomainSort* pdsrt = dm->pdsrt();
	if (0 == pdsrt)
		SortDomainError(dm->sName(true));
	String sName = pm.sGet(1);
	// remove " or '
	if ((sName[0] == '\"') ||(sName[0] == '\''))
		sName = sName.sRight(sName.length()-1);
	if ((sName[sName.length()-1] == '\"') || (sName[sName.length()-1] == '\''))
		sName = sName.sLeft(sName.length()-1);
	String sCode;
	if (pm.fExist(2))
		sCode = pm.sGet(2);
	if ((sCode[0] == '\"') ||(sCode[0] == '\''))
		sCode = sCode.sRight(sCode.length()-1);
	if ((sCode[sCode.length()-1] == '\"') || (sCode[sCode.length()-1] == '\''))
		sCode = sCode.sLeft(sCode.length()-1);
	long iNew = pdsrt->iAdd(sName);
	if (pm.fExist(2) || (sCode.length() != 0))
		pdsrt->SetCode(iNew, sCode);
}

void CommandHandlerUI::CmdAddItemToDomainGroup(const String& s)
{
	ParmList pm(s);
	Domain dm(pm.sGet(0));
	if (dm->fReadOnly())
		return; // warning ??
	DomainGroup* pdgrp = dm->pdgrp();
	if (0 == pdgrp)
		NotDomainGroupError(dm->sName(true));
	double rLim = pm.sGet(1).rVal();
	String sName = pm.sGet(2);
	// remove " or '
	if ((sName[0] == '\"') ||(sName[0] == '\''))
		sName = sName.sRight(sName.length()-1);
	if ((sName[sName.length()-1] == '\"') || (sName[sName.length()-1] == '\''))
		sName = sName.sLeft(sName.length()-1);
	String sCode;
	if (pm.fExist(3))
		sCode = pm.sGet(3);
	if ((sCode[0] == '\"') ||(sCode[0] == '\''))
		sCode = sCode.sRight(sCode.length()-1);
	if ((sCode[sCode.length()-1] == '\"') || (sCode[sCode.length()-1] == '\''))
		sCode = sCode.sLeft(sCode.length()-1);
	long iNew = pdgrp->iAdd(sName);
	if (pm.fExist(3) || (sCode.length() != 0))
		pdgrp->SetCode(iNew, sCode);
	pdgrp->SetUpperBound(iNew, rLim);
}

void CommandHandlerUI::CmdRenameObject(const String& s)
{
	ParmList pm(s);
	if (pm.iFixed() != 2)
		return;
	FileName fnObject(pm.sGet(0));
	FileName fnNew(pm.sGet(1));
	IlwisObject obj = IlwisObject::obj(fnObject);
	obj->Rename(fnNew);
}  
  
void CommandHandlerUI::CmdCreateTable(const String& s)
{
	if ( s == "" ) return;
	ParmList pm(s);
	FileName fnTbl(pm.sGet(0), ".tbt", true);
	if (File::fExist(fnTbl))
		if (!DeleteFile(fnTbl.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnTbl.sRelative()));
	String sDom = pm.sGet(1);
	if ( sDom == "")
		return;
	long iNr = sDom.iVal();
	if (iNr > 0) {
		Domain dm("None");
		Table tbl(fnTbl, dm);
		tbl->iRecNew(iNr);
	}
	else {
		Domain dm(sDom);
		Table tbl(fnTbl, dm);
	}
}

void CommandHandlerUI::CmdCreateMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mpr", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	String sGrf = pm.sGet(1);
	String sDom = pm.sGet(2);
	GeoRef grf(sGrf);
	Domain dm(sDom);
	Map map(fnMap, grf, grf->rcSize(), dm);
	map->FillWithUndef();
}

void CommandHandlerUI::CmdCrMapList(const String& sCmd)
{
	ParmList pm(sCmd);
	FileName fnMapList(pm.sGet(0), ".mpl", true);
	if (File::fExist(fnMapList))
		return;
	Array<FileName> arfnMaps;
	for(int i=1; i < pm.iFixed(); ++i) 
	{
		arfnMaps &= FileName(pm.sGet(i));
	}		
	MapList mpl(fnMapList, arfnMaps);
}

void CommandHandlerUI::CmdCreatePointMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mpp", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	Domain dm;
	CoordSystem csy;
	
	if (pm.iFixed() < 4)
	{
		CoordSystem csy(pm.sGet(1));
		dm = Domain (pm.sGet(2));
	}
	else
	{
		String sCsy = pm.sGet(1);		
		double rXT = pm.sGet(2).rVal();
		double rYL = pm.sGet(3).rVal();
		double rXR = pm.sGet(4).rVal();
		double rYB = pm.sGet(5).rVal();		
		dm = Domain(pm.sGet(6));		
		csy = CoordSystem(sCsy);
		csy->cb = CoordBounds(Coord(rXT, rYL), Coord(rXR, rYB));
	}	
	PointMap map(fnMap, csy, csy->cb, dm);	
}

void CommandHandlerUI::CmdCreateSegMap(const String& s)
{
	ParmList pm(s);
	FileName fnMap(pm.sGet(0), ".mps", true);
	if (File::fExist(fnMap))
		if (!DeleteFile(fnMap.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnMap.sRelative()));
	Domain dm;
	CoordSystem csy;
	
	if (pm.iFixed() < 4)
	{
		CoordSystem csy(pm.sGet(1));
		dm = Domain (pm.sGet(2));
	}
	else
	{
		String sCsy = pm.sGet(1);		
		double rXT = pm.sGet(2).rVal();
		double rYL = pm.sGet(3).rVal();
		double rXR = pm.sGet(4).rVal();
		double rYB = pm.sGet(5).rVal();		
		dm = Domain(pm.sGet(6));		
		csy = CoordSystem(sCsy);
		csy->cb = CoordBounds(Coord(rXT, rYL), Coord(rXR, rYB));
	}
	SegmentMap map(fnMap, csy, csy->cb, dm);			
}

void CommandHandlerUI::CmdCreateDom(const String& s)
{
	ParmList pm(s);
	FileName fnDom(pm.sGet(0), ".dom", true);
	if (File::fExist(fnDom))
		if (!DeleteFile(fnDom.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnDom.sRelative()));
	String sType = pm.sGet("type");
	DomainType dmt = dmtNONE;
	if ((fCIStrEqual(sType , "class")) || (sType == "")) 
		dmt = dmtCLASS;
	else if (fCIStrEqual(sType , "id"))
		dmt = dmtID;
	else if (fCIStrEqual(sType , "group"))
		dmt = dmtGROUP;
	else if (fCIStrEqual(sType , "value"))
		dmt = dmtVALUE;
	switch (dmt) 
	{
		case dmtVALUE: 
		{
			double rMin = pm.sGet("min").rVal();
			double rMax = pm.sGet("max").rVal();
			double rStep;
			String s = pm.sGet("prec");
			if (s.length() == 0)
				rStep = 1;
			else
				rStep = s.rVal();
			if (rStep < 0)
				rStep = 0;
			Domain(fnDom, rMin, rMax, rStep);
		}
		break;
		case dmtCLASS: 
		case dmtID: 
		{
			String sPrefix;
			sPrefix = pm.sGet("prefix");
			if (sPrefix.length() == 0) {
				if (dmt == dmtID)
					sPrefix = "id";
				else
					sPrefix = "class";
			}
			String s = pm.sGet("items");
			long iNr = s.iVal();
			if (iNr <= 0)
				iNr = 0;
			Domain(fnDom, iNr, dmt, sPrefix);
		}
		break;
		case dmtGROUP: 
		case dmtPICTURE: 
		{
			String s = pm.sGet("items");
			long iNr = s.iVal();
			if (iNr <= 0)
				iNr = 0;
			Domain(fnDom, iNr, dmt);
		}
		break;
		default :
			return;
	}
}

void CommandHandlerUI::CmdCreateRpr(const String& s)
{
	ParmList pm(s);
	FileName fnRpr(pm.sGet(0), ".rpr", true);
	if (File::fExist(fnRpr))
		if (!DeleteFile(fnRpr.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnRpr.sRelative()));
	Domain dm(pm.sGet(1));
	Representation(fnRpr, dm);
}

void CommandHandlerUI::CmdCreateGrf(const String& s)
{
	ParmList pm(s);
	FileName fnGrf(pm.sGet(0), ".grf", true);
	if (File::fExist(fnGrf))
		if (!DeleteFile(fnGrf.sFullPath().scVal()))
			throw ErrorObject(String(SMSErrorCouldNotOverWrite_S.scVal(), fnGrf.sRelative()));
	GeoRef gr;
	RowCol rc;
	rc.Row = pm.sGet(1).iVal();
	rc.Col = pm.sGet(2).iVal();
	String sType = pm.sGet("type");
	CoordSystem cs;
	//  if (pm.fGet("crdsys"))
	String sCsys = pm.sGet("crdsys");
	if (sCsys.length() > 0)
		cs = CoordSystem(sCsys);
	if ((fCIStrEqual(sType , "corners")) || (sType == "")) {
		bool fCoC = pm.fGet("centercorners");
		double rMinX=0, rMinY=0, rMaxX=0, rMaxY=0;
		if (cs.fValid()) {
			rMinX = cs->cb.MinX();
			rMinY = cs->cb.MinY();
			rMaxX = cs->cb.MaxX();
			rMaxY = cs->cb.MaxY();
		}
		String s = pm.sGet("lowleft");
		if (s.length() > 0) {
			char* str = s.sVal();
			if (str[0] == '(')
				str++;
			if (2 != sscanf(str, "%lg,%lg", &rMinX, &rMinY))
				return;
		}
		s = pm.sGet("upright");
		if (s.length() > 0) {
			char* str = s.sVal();
			if (str[0] == '(')
				str++;
			if (2 != sscanf(str, "%lg,%lg", &rMaxX, &rMaxY))
				return;
		}
		else {
			s = pm.sGet("pixsize");
			if (s.length() > 0) {
				double rPixSize = s.rVal();
				if (rPixSize < 0)
					rPixSize = 0;
				rMaxX = rMinX + (rc.Col - (int)fCoC) * rPixSize;
				rMaxY = rMinY + (rc.Row - (int)fCoC) * rPixSize;
			}
		}
		gr.SetPointer(new GeoRefCorners(fnGrf, cs, rc, !fCoC, Coord(rMinX, rMinY), Coord(rMaxX, rMaxY)));
	}
}


void CommandHandlerUI::CmdCreate2DimTable(const String& s)
{
	ParmList pm(s);
	String sDom1 = pm.sGet(1);
	String sDom2 = pm.sGet(2);
	Domain dm1(sDom1);
	Domain dm2(sDom2);
	Domain dmVal(pm.sGet(3));
	ValueRange vr;
	if (pm.fExist(4))
		vr = ValueRange(pm.sGet(4));
	Table2Dim tbl2dim(FileName(pm.sGet(0)), dm1, dm2, DomainValueRangeStruct(dmVal, vr));
}

void CommandHandlerUI::CalcObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".his"; asExt &= ".hsp"; asExt &= ".hss"; asExt &= ".hsa";
	asExt &= ".mpl"; asExt &= ".mat"; asExt &= ".stp";
	File::GetFileNames(sFileMask, afn, &asExt);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if (ObjectInfo::fDependent(fn)) {
			String sExt = fn.sExt;
			if ((0 != trq) && (afn.iSize() > 1)) {
				String sCalc("calc %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			IlwisObject obj = IlwisObject::obj(fn);
			if (obj.fValid())
				obj->Calc();
		}
	}
}

void CommandHandlerUI::BreakDepObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".mat"; asExt &= ".mpl"; asExt &= ".stp";
	File::GetFileNames(sFileMask, afn, &asExt);
	if (fForce)
	{
		for (unsigned int i=0; i < afn.iSize(); ++i)
		{
			FileName& fn = afn[i];
			String sExt = fn.sExt;
			if ((0 != trq) && (afn.iSize() > 1))
			{
				String sCalc("breakdep %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			
			if (fCIStrEqual(fn.sExt, ".mpr"))
				ObjectInfo::WriteElement("Map", "Type", fn, "MapStore");
			else if (fCIStrEqual(fn.sExt, ".mps"))
				ObjectInfo::WriteElement("SegmentMap", "Type", fn, "SegmentMapStore");
			else if (fCIStrEqual(fn.sExt, ".mpa"))
				ObjectInfo::WriteElement("PolygonMap", "Type", fn, "PolygonMapStore");
			else if (fCIStrEqual(fn.sExt, ".mpp"))
				ObjectInfo::WriteElement("PointMap", "Type", fn, "PointMapStore");
		}
		return;
	}
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if ((0 != trq) && (afn.iSize() > 1)) {
			String sCalc("breakdep %S (%S%S)", sFileMask, fn.sFile, sExt);
			if (trq->fText(sCalc))
				return;
		}
		if (ObjectInfo::fDependent(fn)) 
		{
			IlwisObject obj = IlwisObject::obj(fn);
			if (!obj->fReadOnly())
				obj->BreakDependency();
		}
	}
}

void CommandHandlerUI::UpdateObjects(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sFileMask = pm.sGet(0);
	bool fForce = false;
	if (pm.fExist("force")) 
		fForce = pm.fGet("force");
	Array<FileName> afn;
	Array<String> asExt;
	asExt &= ".mpr"; asExt &= ".mpp"; asExt &= ".mpa"; asExt &= ".mps"; asExt &= ".tbt";
	asExt &= ".mpl"; asExt &= ".mat"; asExt &= ".stp";
	File::GetFileNames(sFileMask, afn, &asExt);
	for (unsigned int i=0; i < afn.iSize(); ++i) 
	{
		FileName& fn = afn[i];
		String sExt = fn.sExt;
		if (ObjectInfo::fDependent(fn)) 
		{
			if ((0 != trq) && (afn.iSize() > 1)) {
				String sCalc("update %S (%S%S)", sFileMask, fn.sFile, sExt);
				if (trq->fText(sCalc))
					return;
			}
			IlwisObject obj = IlwisObject::obj(fn);
			if (obj.fValid())
				obj->MakeUpToDate();
		}
	}
}


void CommandHandlerUI::CmdSetReadOnly(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	IlwisObject::GetAllObjectExtensions(asExt);
	File::GetFileNames(pm.sGet(0), afn, &asExt);  
	for (unsigned int i=0; i < afn.iSize(); ++i) {
		IlwisObject obj = IlwisObject::obj(afn[i]);
		obj->SetReadOnly(true);
	}  
}

void CommandHandlerUI::CmdSetReadWrite(const String& s)
{
	ParmList pm(s);
	Array<FileName> afn;
	Array<String> asExt;
	IlwisObject::GetAllObjectExtensions(asExt);
	File::GetFileNames(pm.sGet(0), afn, &asExt);  
	for (unsigned int i=0; i < afn.iSize(); ++i) {
		IlwisObject obj = IlwisObject::obj(afn[i]);
		obj->SetReadOnly(false);
	}  
}

void CommandHandlerUI::CmdCloseAll(const String&)
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CLOSEALL, 0); // no force
}

char* sComposed[]  = { ":=", 0};

void CommandHandlerUI::CmdTabCalc(const String& sCmd)
{
	TextInput ip(sCmd);
	TokenizerBase tkn(&ip);
	tkn.SetComposed(sComposed);
	Token tok = tkn.tokGet();
	FileName fnObject(tok.sVal());
	Table tbl(FileName(fnObject.sFullNameQuoted(true), ".tbt", false));
	tok = tkn.tokGet();
	String sCol = tok.sVal();
	tok = tkn.tokGet();
	if (tok == "[") {
		Calculator calc(tbl, String("%S[%S",sCol,tkn.sCurrentLine()));
		calc.ExecColumnIndexStore();
		return;
	}
	Domain dm;
	ValueRange vr;
	if (tok == "{")
		if (!ScriptPtr::fScanDomainValueRange(tok, tkn/**tkn.tknb*/, dm, vr))
			return;
	bool fBreakDep = tok == ":=";
	String sRestOfLine = tkn.sCurrentLine();
	tbl->LoadData();
	Column col(tbl, sCol, sRestOfLine);
	if (!col->fDependent())
		return;
	if (dm.fValid() && col->fDomainChangeable())
		col->SetDomainValueRangeStruct(DomainValueRangeStruct(dm, vr));
	else if (col->fValueRangeChangeable() && vr.fValid())
		col->SetValueRange(vr);
	col->Calc();
	col->CalcMinMax();  // on special request of Jelle in tblwind.c, so why not here?
	if (fBreakDep)
		col->BreakDependency();
}

void CommandHandlerUI::CopyObjects(const String& sCommand, Tranquilizer* trq, CWnd* wnd, bool fOnlyDirectory)
{
	ParmList pm(sCommand);
	if ( pm.iFixed() < 1 )
		return;
	bool fBreakDep = pm.fGet("breakdep");
	bool fQuiet = pm.fGet("quiet");
	String sTo = pm.sGet(pm.iFixed()-1);
	FileName fnTo = FileName(sTo);
	vector<FileName> afnFiles(pm.iFixed()-1);
	for (int iParm = 0; iParm < pm.iFixed()-1; iParm++) 
		afnFiles[iParm] = FileName(pm.sGet(iParm));
	
	if ( afnFiles.size() == 0 )
		throw ErrorObject(SMSErrorNoFilesToCopy);
	
	if ( fnTo.sExt == "" && Directory::fIsExistingDir( Directory(sTo)))
	{
		ObjectCopierUI copier( wnd, afnFiles, Directory(sTo));
		copier.Copy(fBreakDep, fQuiet);
	}
	else
	{
		if ( fnTo.sExt == "")
			fnTo.sExt = afnFiles[0].sExt;
		ObjectCopierUI copier( wnd, afnFiles[0], fnTo);
		copier.Copy(fBreakDep, fQuiet);
	}		
}

void CommandHandlerUI::CopyFiles(const String& sCommand, Tranquilizer* trq)
{
	ParmList pm(sCommand);
	String sTo = pm.sGet(1);
	FileName fnTo = FileName(sTo);
	if (fnTo.sExt == "" && Directory::fIsExistingDir( Directory(sTo)))
	{ // copy to other subdir
		String sFileMask = pm.sGet(0);
		String sDirFrom = FileName(sFileMask).sPath();
		String sDirTo = fnTo.sFullPath();
		if (sDirTo.length() == 0)
			sDirTo = fnTo.sPath();
		if (sDirTo.length() == 0)
			sDirTo = sTo;
		for (FilenameIter iter(pm.sGet(0)); iter.fValid(); ++iter)
		{
			String sFile("%S%s", sDirFrom, (*iter).c_str());
			FileName fnFrom = FileName(sFile);
			FileName fnTo = fnFrom;
			fnTo.Dir(sDirTo);
			if (!File::fCopy(fnFrom, fnTo, *trq))
				break;
		}
	}
	else  // copy in current dir
	{
		FileName fnFrom(pm.sGet(0));
		FileName fnTo = fnFrom;
		fnTo.sFile = pm.sGet(1).sUnQuote();
		File::Copy(fnFrom, fnTo);
	}
	UpdateCatalog();
}

LRESULT Cmdfinddatumparms(CWnd* parent, const String& s)
{
	DatumWizard frm(parent, s.scVal());
	frm.DoModal();

	return -1;
}

LRESULT fCmdSimpleCalc(CWnd *wnd, const String& sCmd)
{
	String sRes,sMsg;
	if ( sCmd.sHead(" ") == "Text") {
		sMsg = sCmd.sTail(" ");
		SimpleCalcText *frm = new SimpleCalcText(SimpleCalcText(wnd,&sMsg));
		
	} else {
		sRes = Calculator::sSimpleCalc(sCmd.sTrimSpaces());
		sMsg = String(" %S = %S", sRes, sCmd);
		MainWindow *mw = dynamic_cast<MainWindow *>( AfxGetMainWnd()) ;
		//MainWindow *mw = dynamic_cast<MainWindow *>(IlwWinApp()->GetMainWnd());
		if ( mw)
		{
			mw->frmSimpleCalcResults()->ShowWindow(SW_SHOW);
			mw->frmSimpleCalcResults()->AddString( sMsg);
		}
		else
		{
			MessageBox(0, sMsg.scVal(), SMSTitleSimpleCalc.scVal(), MB_OK);
		}
	}

	return -1;
		
}


void CommandHandlerUI::CmdMplSplitter(const String& sCmd)
{
	WinThread* thr = new WinThread(RUNTIME_CLASS(MapListSplitterWindow), sCmd);
	if (thr) 
		thr->CreateThread(0, 0);
}

void CommandHandlerUI::CmdTestingDBConnection(const String& sCmd)
{
	IlwWinApp()->GetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_IMPORT_CONNECT_DATABASE);

}

LRESULT Cmdsample(CWnd *parent, const String& s)
{
	class SampleMapForm: public FormWithDest
	{
	public:
		SampleMapForm(CWnd* parent, String* sSample)
			: FormWithDest(parent, SSSTitleSampling)
		{
			iImg = IlwWinApp()->iImage("DspMap16Ico");
			new FieldSampleSetC(root, SSSUiSampleSet, sSample);
			SetMenHelpTopic(htpEditMapSample);
			create();
		}
	};
	String sSampleMap;
	if ("" != s) 
	{
		TextInput inp(s);
		TokenizerBase tokenizer(&inp);
		Token tok;
		tok = tokenizer.tokGet();
		sSampleMap = tok.sVal();
	}  
	if ("" == sSampleMap) 
	{
		//ResetDir();
		SampleMapForm frm(parent, &sSampleMap);
		if (!frm.fOkClicked()) 
			return -1;
	}
	String sCmd = "open ";
	sCmd &= sSampleMap;
	IlwWinApp()->Execute(sCmd);

	return -1;
}

LRESULT CmdeditDefinition(const String& sObj)
{
	ParmList p(sObj);
	String sCmd = p.sGet(0);
	FileName fn(sCmd);
	ObjectStruct* os = new ObjectStruct(fn);
	if (os->obj.fValid()) {
		os->eRequest = ObjectStruct::ePROP; 
		AfxBeginThread(ShowDefForm, (LPVOID)os);
	}
	else
		delete os;

	return -1;
}

static UINT ExportInThread(LPVOID p)
{
	WinAndParm *wp = (WinAndParm*)(p);
	Exporting exprt;
	IlwWinApp()->Context()->InitThreadLocalVars();
	IlwWinApp()->SetCurDir(*(wp->sPath));
	CWnd* wnd = CWnd::FromHandle(wp->handle);
	
	try
	{
		exprt.Export(wnd, wp->s->scVal());
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	
	delete wp;
	
	IlwWinApp()->Context()->RemoveThreadLocalVars();
	return FALSE;
}

LRESULT Cmdexport(CWnd *wndOwner, const String& sPar)
{
	WinAndParm *wp = new WinAndParm;
	wp->handle = wndOwner->m_hWnd;
	wp->s = new String(sPar);
	wp->sPath = new String(IlwWinApp()->sGetCurDir());
	AfxBeginThread(ExportInThread, (LPVOID)(wp)); 

	return -1;
}

UINT CommandHandlerUI::CmdCopyFileInThread(void *p)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	WinAndParm* wp = (WinAndParm*)(p);
	IlwWinApp()->Context()->InitThreadLocalVars();
	IlwWinApp()->SetCurDir(*(wp->sPath));
	String s(wp->s->scVal());

	CWnd *wnd= CWnd::FromHandle(wp->handle);

	// wp made outside this function
	delete wp;

	try
	{
    Tranquilizer trq;
    // trq.SetTitle(String(SMAPTextImportRasterFrom14.scVal(), fn.sFile));
    trq.Start();
		CopyFiles(s, &trq);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	IlwWinApp()->Context()->RemoveThreadLocalVars();
	return 1;
}

LRESULT Cmdcopy(CWnd *wndOwner, const String& sPar)
{
	WinAndParm* wp = new WinAndParm;
	wp->handle = wndOwner->m_hWnd;
	wp->s = new String(sPar);
	wp->sPath = new String(IlwWinApp()->sGetCurDir());
	AfxBeginThread(CommandHandlerUI::CmdCopyInThread, (LPVOID)(wp));

	return -1;
}

UINT CommandHandlerUI::CmdCopyInThread(void *p)
{
	// make background jobs easier interruptable
	AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	WinAndParm* wp = (WinAndParm*)(p);
	IlwWinApp()->Context()->InitThreadLocalVars();
	IlwWinApp()->SetCurDir(*(wp->sPath));
	String s(wp->s->scVal());

	CWnd *wnd= CWnd::FromHandle(wp->handle);

	// wp made outside this function
	delete wp;

	try
	{
		CopyObjects(s, 0, wnd);
	}
	catch(ErrorObject& err)
	{
		err.Show();
	}
	IlwWinApp()->Context()->RemoveThreadLocalVars();
	return 1;
}

void ResetDir()
{
	String sDir = IlwWinApp()->sGetCurDir();
	SetCurrentDirectory(sDir.c_str());
}

LRESULT Cmdshowdefform(CWnd *parent, const String& sCmd) {
	Map map(sCmd.sHead(","));
	bool fShow = sCmd.sTail(",").sHead(",") == "1";
	bool fFreeze = sCmd.sTail(",") == "1";
	MapDefForm frm(parent, map, !fShow, fFreeze);

	return frm.fOkClicked();

}

class CreateViewForm : public FormWithDest
{
public:
	  CreateViewForm(CWnd *par, String& _sQuery, String& view) :
	  FormWithDest(par, "Define View"),
	  sQuery(_sQuery)
	  {
		getParts(sQuery, selectp, fromp, wherep);
	    new FieldString(root,"View Name", &view);
		new StaticText(root,"Query",true);
		FieldStringMulti *fsSelect = new FieldStringMulti(root, "Select", &selectp);
		zDimension dim = fsSelect->Dim("gk");	
		fsSelect->SetWidth((short)(15 * dim.width()));
		fsSelect->SetHeight((short)(2 * dim.height()));
		FieldStringMulti *fsFrom = new FieldStringMulti(root, "From", &fromp);
		fsFrom->SetWidth((short)(15 * dim.width()));
		fsFrom->SetHeight((short)(1 * dim.height()));
		FieldStringMulti *fsWhere = new FieldStringMulti(root, "Where", &wherep);
		fsWhere->SetWidth((short)(15 * dim.width()));
		fsWhere->SetHeight((short)(4 * dim.height()));
		create();

	  }

	  void getParts(const String& query, String &sp, String& fp, String& wp) {
		  if ( query == "")
			  return;
		  String sQ = query;
		  sQ.toLower();
		  size_t selectIndex = sQ.find("select");
		  size_t fromIndex = sQ.find("from");
		  size_t whereIndex = sQ.find("where");
		  if ( selectIndex < fromIndex && fromIndex < whereIndex) {
			  sp = query.substr(selectIndex + 7, fromIndex  - selectIndex - 7);
			  fp = query.substr(fromIndex + 5, whereIndex == string::npos ? query.size() - fromIndex - 5 : whereIndex);
			  if ( whereIndex != string::npos)
				wp = query.substr(whereIndex + 6, query.size() - whereIndex - 6);

		  }

	  }

	  int exec() {
		  FormWithDest::exec();
		  if ( selectp != "" && fromp != "") {
			  sQuery = String("SELECT %S FROM %S", selectp.sTrimSpaces(), fromp.sTrimSpaces());
			if ( wherep != "") {
				sQuery += " WHERE "+ wherep.sTrimSpaces();
			}
		  }
		  return 1;
	  }
private:
    String selectp,fromp,wherep;
	String& sQuery;

};

LRESULT createView(CWnd *parent, const String& sCmd) {
	String query, view = FileName(String("%S_view",sCmd.sHead("."))).sRelative();
	ObjectInfo::ReadElement("ForeignFormat","Query",FileName(sCmd),query);
	CreateViewForm frm(parent,query,view);
	if (frm.fOkClicked()) {
		FileName fnTable(sCmd);
		FileName fnView(view,".tbt");
		String collection;
		ObjectInfo::ReadElement("Collection","Item0",fnTable,collection);
		DataBaseCollection dbCol; 
		dbCol = DataBaseCollection(FileName(collection));
		dbCol->Add(fnView);
		ParmList pm;
		pm.Add(new Parm("table", fnTable.sFile));
		pm.Add(new Parm("query", query));
		dbCol->CreateImpliciteObject(fnView,pm);
		
	}

	return -1;
}

LRESULT Cmdrun(CWnd *parent, const String& sCmd) {
	new ScriptForm(Script(FileName(sCmd)));
	return -1;
}

LRESULT Cmdvisible(CWnd *parent, const String& sCmd) {
	parent->ShowWindow(SW_SHOW);
	return -1;
}

LRESULT Cmdinvisible(CWnd *parent, const String& sCmd) {
	parent->ShowWindow(SW_HIDE);
	return -1;
}

LRESULT CmdSetCommandLine(CWnd *parent,const String& sPar)
{
	String *s = new String(sPar);
	parent->PostMessage(ILWM_SETCOMMANDLINE, (WPARAM)s, 0); 
	return -1;
}

LRESULT CmdDirChanged(CWnd *parent,const String& sPar)
{
	IlwWinApp()->DirChanged();
	return -1;
}

void CommandHandlerUI::addExtraCommands() {
	addCommand(CommandHandlerUI::createApplicationInfo("simplecalc","",fCmdSimpleCalc,"","","","",0,"run simple calc"));
	addCommand(CommandHandlerUI::createApplicationInfo("dirchanged","",CmdDirChanged,"","","","",0,""));
	addCommand(CommandHandlerUI::createApplicationInfo("run","",Cmdrun,"","","","",0,"Run a script"));
	addCommand(CommandHandlerUI::createApplicationInfo("commandline","",CmdSetCommandLine,"","","","",0,"Sets the commandline"));
	addCommand(CommandHandlerUI::createApplicationInfo("visible","",Cmdvisible,"","","","",0,"Make Ilwis visible", false));	
	addCommand(CommandHandlerUI::createApplicationInfo("invisible","",Cmdinvisible,"","","","",0,"Make Ilwis invisible", false));	
	addCommand(CommandHandlerUI::createApplicationInfo("rmdir","",Cmddel,"","Rmdir","","",0,"Remove directory", false));	
	addCommand(CommandHandlerUI::createApplicationInfo("del","delete",Cmddel,"","Del","","",0,"Delete file(s)", false));
	addCommand(CommandHandlerUI::createApplicationInfo("copy","",Cmdcopy,"","Copy","","",0,"Celete file(s)", false));
	addCommand(CommandHandlerUI::createApplicationInfo("run","",Cmdrun,"","Run","",".isl",0,"Run", false));
	addCommand(CommandHandlerUI::createApplicationInfo("dir","",Cmddir,"","Dir","",".isl",0,"Change directory", false));
	addCommand(CommandHandlerUI::createApplicationInfo("mkdir","makedir",Cmdmkdir,"","MakeDir","","",0,"Create directory", false));
	addCommand(CommandHandlerUI::createApplicationInfo("seg","",Cmdseg,"","Open Segmentmap","",".mps",0,"Show a segmentmap", false));
	addCommand(CommandHandlerUI::createApplicationInfo("pol","",Cmdpol,"","Open Polygonmap","",".mpa",0,"Show a polygonmap", false));
	addCommand(CommandHandlerUI::createApplicationInfo("tbl","table",Cmdtbl,"","Open Table","",".tbt",0,"Opens a Table", false));
	addCommand(CommandHandlerUI::createApplicationInfo("map","",Cmdmap,"","Open Map","",".mpr",0,"Opens a raster map", false));
	addCommand(CommandHandlerUI::createApplicationInfo("pnt","",Cmdpnt,"","Open Pointmap","",".mpp",0,"Opens a Pointmap", false));
	addCommand(CommandHandlerUI::createApplicationInfo("prop","properties",Cmdpropobject,"","Object Properties","","",0,"Shows the properties of an object",false));
    addCommand(CommandHandlerUI::createApplicationInfo("pixelinfo","",Cmdpixelinfo,"..","Pixel Info","info","",7004,"Open or activate the pixel information window"));
	addCommand(CommandHandlerUI::createApplicationInfo("showdefform","", Cmdshowdefform,"..","","","",-1,"", false));
	addCommand(CommandHandlerUI::createApplicationInfo("deleteobjects","",Cmddeleteobjects,"","",".",".",0000,"", false));

	addCommand(CommandHandlerUI::createApplicationInfo("open","show",Cmdopen,SMENUopen,SMENUSopen,"DspMap16Ico","",7010,SMENUDopen));
	addCommand(CommandHandlerUI::createApplicationInfo("slideshow","",Cmdmplslideshow,SMENUslideshow,SMENUSslideshow,"DspMap16Ico",".mpl",7013,SMENUDslideshow));
	addCommand(CommandHandlerUI::createApplicationInfo("showastbl","showastable",Cmdshowastable,SMENUshowastable,SMENUSshowastable,".tbt",".tbt",0,SMENUDshowastable));
	addCommand(CommandHandlerUI::createApplicationInfo("display3d","",Cmddisplay3d,SMENUdisplay3d,SMENUSdisplay3d,"DspMap16Ico","",7002,SMENUDdisplay3d));
	addCommand(CommandHandlerUI::createApplicationInfo("edit","",Cmdedit,SMENUedit,SMENUSedit,"DspMap16Ico","",7001,SMENUDedit));
    addCommand(CommandHandlerUI::createApplicationInfo("pixelinfo","",Cmdpixelinfo,SMENUpixelinfo,SMENUSpixelinfo,"info","",7004,SMENUDpixelinfo));
	addCommand(CommandHandlerUI::createApplicationInfo("makestereopair","",Cmdmakestereopair,SMENUmakestereopair,SMENUSmakestereopair,".stp","",7015,SMENUDmakestereopair));
	addCommand(CommandHandlerUI::createApplicationInfo("sample","",Cmdsample,SMENUsample,SMENUSsample,"DspMap16Ico","",7003,SMENUDsample));
	addCommand(CommandHandlerUI::createApplicationInfo("anaglyph","",Cmdanaglyph,SMENUanaglyph,SMENUSanaglyph,".stp",".stp",7017,SMENUDanaglyph));
	addCommand(CommandHandlerUI::createApplicationInfo("stereoscope","",Cmdstereoscope,SMENUstereoscope,SMENUSstereoscope,".stp",".stp",7016,SMENUDstereoscope));
	addCommand(CommandHandlerUI::createApplicationInfo("finddatumparms","",Cmdfinddatumparms,SMENUfinddatumparms,SMENUSfinddatumparms,".csy",".mpp",4778,SMENUDfinddatumparms));
	addCommand(CommandHandlerUI::createApplicationInfo("smce","",Cmdsmce,SMENUsmce,SMENUSsmce,".smc","",8110,SMENUDsmce));
	addCommand(CommandHandlerUI::createApplicationInfo("createmap","",Cmdcreatemap,SMENUcreatemap,SMENUScreatemap,".mpr",".dom.grf",1000,SMENUDcreatemap));
	addCommand(CommandHandlerUI::createApplicationInfo("createpol","",Cmdcreatepol,SMENUcreatepol,SMENUScreatepol,".mpa",".dom.csy",1001,SMENUDcreatepol));
	addCommand(CommandHandlerUI::createApplicationInfo("createseg","",Cmdcreateseg,SMENUcreateseg,SMENUScreateseg,".mps",".dom.csy",1002,SMENUDcreateseg));
	addCommand(CommandHandlerUI::createApplicationInfo("createpnt","",Cmdcreatepnt,SMENUcreatepnt,SMENUScreatepnt,".mpp",".dom.csy",1003,SMENUDcreatepnt));
	addCommand(CommandHandlerUI::createApplicationInfo("layout","",Cmdlayout,SMENUlayout,SMENUSlayout,".ilo",".mpv",1000,SMENUDlayout));
	addCommand(CommandHandlerUI::createApplicationInfo("createtbl","",Cmdcreatetbl,SMENUcreatetbl,SMENUScreatetbl,".tbt",".dom",1005,SMENUDcreatetbl));
	addCommand(CommandHandlerUI::createApplicationInfo("createtb2","",Cmdcreatetb2,SMENUcreatetb2,SMENUScreatetb2,".ta2",".dom",1027,SMENUDcreatetb2));
	addCommand(CommandHandlerUI::createApplicationInfo("graph","",Cmdgraph,SMENUgraph,SMENUSgraph,".grh",".tbt",1000,SMENUDgraph));
	addCommand(CommandHandlerUI::createApplicationInfo("rosediagram","",Cmdrosediagram,SMENUrosediagram,SMENUSrosediagram,".grh",".tbt",1000,SMENUDrosediagram));
	addCommand(CommandHandlerUI::createApplicationInfo("createdom","",Cmdcreatedom,SMENUcreatedom,SMENUScreatedom,".dom","",1010,SMENUDcreatedom));
	addCommand(CommandHandlerUI::createApplicationInfo("createrpr","",Cmdcreaterpr,SMENUcreaterpr,SMENUScreaterpr,".rpr",".dom",1011,SMENUDcreaterpr));
	addCommand(CommandHandlerUI::createApplicationInfo("creategrf","",Cmdcreategrf,SMENUcreategrf,SMENUScreategrf,".grf",".csy",1012,SMENUDcreategrf));
	addCommand(CommandHandlerUI::createApplicationInfo("createcsy","",Cmdcreatecsy,SMENUcreatecsy,SMENUScreatecsy,".csy","",1013,SMENUDcreatecsy));
	addCommand(CommandHandlerUI::createApplicationInfo("creatempl","",Cmdcreatempl,SMENUcreatempl,SMENUScreatempl,".mpl","",1004,SMENUDcreatempl));
	addCommand(CommandHandlerUI::createApplicationInfo("createsms","",Cmdcreatesms,SMENUcreatesms,SMENUScreatesms,".sms",".mpl",1022,SMENUDcreatesms));
	addCommand(CommandHandlerUI::createApplicationInfo("createfil","",Cmdcreatefil,SMENUcreatefil,SMENUScreatefil,".fil","",1024,SMENUDcreatefil));
	addCommand(CommandHandlerUI::createApplicationInfo("createfun","",Cmdcreatefun,SMENUcreatefun,SMENUScreatefun,".fun","",1025,SMENUDcreatefun));
	addCommand(CommandHandlerUI::createApplicationInfo("createisl","",Cmdcreateisl,SMENUcreateisl,SMENUScreateisl,".isl","",1026,SMENUDcreateisl));
	addCommand(CommandHandlerUI::createApplicationInfo("createioc","",Cmdcreateioc,SMENUcreateioc,SMENUScreateioc,".ioc","",1026,SMENUDcreateioc));
	addCommand(CommandHandlerUI::createApplicationInfo("import","",Cmdimport,SMENUimport,SMENUSimport,"ExeIcoL","",7006,SMENUDimport));
	addCommand(CommandHandlerUI::createApplicationInfo("import -tablewizard","",Cmdimport,SMENUimportTable,SMENUSimportTable,".tbt","",7006,SMENUDimportTable));
	addCommand(CommandHandlerUI::createApplicationInfo("import -gdal","",Cmdimport,SMENUimportGdal,SMENUSimportGdal,".mpr","",7006,SMENUDimportGdal));
	addCommand(CommandHandlerUI::createApplicationInfo("import -generalraster","",Cmdimport,SMENUimportGR,SMENUSimportGR,".mpr","",7006,SMENUDimportGR));
	addCommand(CommandHandlerUI::createApplicationInfo("export","",Cmdexport,SMENUexport,SMENUSexport,"ExeIcoL",".mpr.mpa.mps.mpp.tbt.mpl.his.hss.hsa.hsp",7008,SMENUDexport));
	addCommand(CommandHandlerUI::createApplicationInfo("colorcomp","mpl",Cmdmplcolorcomp,SMENUcolorcomp,SMENUScolorcomp,"DspMap16Ico","",7014,SMENUDcolorcomp));
	addCommand(CommandHandlerUI::createApplicationInfo("colorcomp","mpl",Cmdmplcolorcomp,SMENUcolorcomp,SMENUScolorcomp,"DspMap16Ico","",7014,SMENUDcolorcomp));
	addCommand(CommandHandlerUI::createApplicationInfo("createView","",createView,"","","","",0,""));
}

vector<Action *> CommandHandlerUI::CreateActions() {
	vector<Action *> actions;
	vector<CString> sortedActions;
	map<CString, ApplicationInfoUI *> alternateOrdering;
	for(CommandIterUI iter = commands.begin(); iter != commands.end(); ++iter) {
		ApplicationInfoUI *ai = (*iter).second;
		CString temp(ai->menuString.scVal());
		temp.Remove('&');
		CString temp2;
		//temp2.Format("%s%d", temp, rootCounter++);
		temp2.Format("%s|%s",temp, ai->command.scVal());
			 
		sortedActions.push_back(temp2);
		alternateOrdering[temp2]=ai;

	}
	IlwisSettings settings("DefaultSettings");
	FileName fn(String("%S\\Resources\\Def\\OperationsLayout.def", getEngine()->getContext()->sIlwDir()));
	if ( settings.fValue("OperationsMenu") && fn.fExist()) {
		File file(fn,facRO);
		sortedActions.clear();
		String line;
		while(file.ReadLnAscii(line) != 0) {
			sortedActions.push_back(CString(line.scVal()));
		}
	} else
		sort(sortedActions.begin(), sortedActions.end());

	for(vector<CString>::iterator iter2 = sortedActions.begin(); iter2 != sortedActions.end(); ++iter2) {
		CString v(*iter2);
		ApplicationInfoUI *ai = alternateOrdering[v];
		if ( ai->visible) {
			Action *act = new Action(ai);
			actions.push_back(act);
		}
	}
	return actions;
}

void CommandHandlerUI::saveOperationsDefinitions(const FileName& fn) {
	vector<Action *> actions;
	vector<CString> sortedActions;
	int rootCounter = 0;
	for(CommandIterUI iter = commands.begin(); iter != commands.end(); ++iter) {
		ApplicationInfoUI *ai = (*iter).second;
		CString temp(ai->menuString.scVal());
		temp.Remove('&');
		CString temp2;
		temp2.Format("%s|%s",temp, ai->command.scVal());
			 
		sortedActions.push_back(temp2);

	}
	sort(sortedActions.begin(), sortedActions.end());
	File file(fn,facCRT);
	for(vector<CString>::iterator cur = sortedActions.begin(); cur != sortedActions.end(); ++cur) {
		String s((*cur));
		if ( s == "" || s == "..")
			continue;
		file.WriteLnAscii(s);
	}
	file.Close();
}

