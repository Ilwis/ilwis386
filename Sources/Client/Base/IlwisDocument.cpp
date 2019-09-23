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
// IlwisDocument.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Headers\constant.h"
#include "Headers\messages.h"
#include "Client\ilwis.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Utils\GeneralBar.h"
#include "Client\MainWindow\AddInfoView.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\File\BaseCopier.h"
#include "Client\Forms\ObjectCopierUI.h"
#include "Client\MainWindow\ACTION.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// copied from afximpl.h
UINT AFXAPI AfxGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);
/////////////////////////////////////////////////////////////////////////////
// IlwisDocument

IMPLEMENT_DYNCREATE(IlwisDocument, COleServerDoc)

BEGIN_MESSAGE_MAP(IlwisDocument, COleServerDoc)
	//{{AFX_MSG_MAP(IlwisDocument)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_COMMAND(ID_PROP, OnProperties)
	ON_COMMAND(ID_SHOWADDINFO, OnShowAddInfo)
	ON_UPDATE_COMMAND_UI(ID_SHOWADDINFO, OnUpdateShowAddInfo)
	ON_UPDATE_COMMAND_UI(ID_PROP,		OnUpdateProperties)
	ON_COMMAND_RANGE(ID_OPERATION1, ID_OPERATION1 + 499, OnOperationCmd)
	ON_COMMAND(ID_FILE_SAVE_COPY_AS, OnSaveCopyAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()



IlwisDocument::IlwisDocument() :
ilwisobj(NULL), gbAddInfo(0), fDelaySave(false)
{
	m_dwCreationThreadID = ::GetCurrentThreadId();
	fUseSerialize = false;
}

BOOL IlwisDocument::OnNewDocument()
{
	if (!COleServerDoc::OnNewDocument())
		return FALSE;

	return TRUE;
}

IlwisDocument::~IlwisDocument()
{
	delete ilwisobj;
}

/////////////////////////////////////////////////////////////////////////////
// IlwisDocument diagnostics

#ifdef _DEBUG
void IlwisDocument::AssertValid() const
{
	// otherwise gives assertion errors in connected windows
	if (m_dwCreationThreadID != ::GetCurrentThreadId())
		return;
	COleServerDoc::AssertValid();
}

void IlwisDocument::Dump(CDumpContext& dc) const
{
	if (m_dwCreationThreadID != ::GetCurrentThreadId())
		return;
	COleServerDoc::Dump(dc);
}
#endif //_DEBUG

BOOL IlwisDocument::OnOpenDocument(LPCTSTR lpszPathName, int os) 
{
	try
	{
		delete ilwisobj;
		ilwisobj = 0;
		FileName fn(lpszPathName);
		String sFileName = fn.sFullName();
		SetPathName(sFileName.c_str());
		if (fUseSerialize)
			return COleServerDoc::OnOpenDocument(sFileName.c_str());

		ilwisobj = new IlwisObject(IlwisObject::obj(fn));
		if (!ilwisobj->fValid())
			return FALSE;
		if (!(*ilwisobj)->fCalculated()) {
			int iPrior = AfxGetThread()->GetThreadPriority();
			AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
			(*ilwisobj)->Calc();
			AfxGetThread()->SetThreadPriority(iPrior);
			if (!(*ilwisobj)->fCalculated())
				return FALSE;
		}
		IlwisSettings settings("DefaultSettings");
		bool fWarnNotUpToDate = settings.fValue("WarnNotUpToDate", false);
		if (fWarnNotUpToDate && !(*ilwisobj)->fUpToDate()) 
		{
			String sMsg(TR("Warning: %S is not up-to-date.\nDo you wish to make it up-to-date ?").c_str(), (*ilwisobj)->sTypeName());
			switch (MessageBox(0, sMsg.c_str(), TR("Warning: Object Not Up-To-Date").c_str(), 
				MB_YESNOCANCEL|MB_ICONWARNING|MB_DEFBUTTON2|MB_TOPMOST))
			{
			case IDYES:
				{
					int iPrior = AfxGetThread()->GetThreadPriority();
					AfxGetThread()->SetThreadPriority(THREAD_PRIORITY_LOWEST);
					(*ilwisobj)->Calc();
					AfxGetThread()->SetThreadPriority(iPrior);
					if (!(*ilwisobj)->fCalculated())
						return FALSE;
				} break;
			case IDNO:
				// continue as if no message
				break;
			case IDCANCEL:
				return FALSE;
			default:
				return FALSE;
			}
		}

		String sTN = (*ilwisobj)->sTypeName();
		SetTitle(sTN.c_str());
		return TRUE;
	}
	catch (ErrorObject &err)
	{
		err.Show();
		return FALSE;
	}
	catch (CException *err)
	{
		MessageBeep(MB_ICONHAND);
		err->ReportError(MB_OK|MB_ICONHAND|MB_TOPMOST);
		err->Delete();
		return FALSE;
	}		
	return FALSE;
}

void IlwisDocument::OnOpenExpressionDoc(IlwisObject& ob)
{
	delete ilwisobj;
	ilwisobj = new IlwisObject(ob);
	//SetPathName(ob->fnObj.sFullPath().c_str());	

}
BOOL IlwisDocument::OnOpenDocument(LPCTSTR lpszPathName, OpenType ot, int os)
{
	// by default ignore second parameter
	return OnOpenDocument(lpszPathName);
}

BOOL IlwisDocument::OnOpenDocument(LPCTSTR lpszPathName, ParmList& pm, int os)
{
	// by default ignore second parameter
	return OnOpenDocument(lpszPathName);
}

BOOL IlwisDocument::OnSaveDocument(LPCTSTR lpszPathName)
{
	if (fUseSerialize) 
	{	 
		// ensure that path is set correctly
		FileName fn(lpszPathName);
		IlwWinApp()->SetCurDir(fn.sPath());
		BOOL fRet = COleServerDoc::OnSaveDocument(lpszPathName);
		if (fRet) {
			FileName* pfn = new FileName(lpszPathName);
			AfxGetApp()->GetMainWnd()->PostMessage(ILW_READCATALOG, 0, (long)pfn);
		}
		return fRet;
	}
	else
	{
		IlwisObject io = obj();
		if (!io.fValid())
			return FALSE;
		io->Store();

		FileName fnFrom(obj()->fnObj);
		FileName fnTo(lpszPathName);
		if (fnFrom != fnTo) // save copy as
		{
			ObjectCopierUI cop(NULL, fnFrom, fnTo);
			cop.Copy(true);
		}			
		return TRUE;
	}
}

IlwisObject IlwisDocument::obj() const
{
	return IlwisObject::objInvalid();
}

void IlwisDocument::SetObjectDescription(const String& s)
{
	if (obj().fValid())
	{
		obj()->SetDescription(s);
		obj()->Updated();
	}
}

void IlwisDocument::DelaySaveDoc(bool fDelay)
{
	fDelaySave = fDelay;
}

bool IlwisDocument::fDelaySaveDoc()
{
	return fDelaySave;
}

string IlwisDocument::sObjectDescription()
{
	if (obj().fValid())
		return obj()->sGetDescription();
	else
		return "";
}

zIcon IlwisDocument::icon() const
{
	return zIcon("LogoIcon");
}

bool IlwisDocument::fReadOnly()	const
{
	return obj()->fReadOnly();
}

void IlwisDocument::OnProperties()
{
	IlwWinApp()->Execute(String("prop %S", obj()->fnObj.sFullNameQuoted()));
}

void IlwisDocument::OnUpdateProperties(CCmdUI* pCmdUI)
{
	bool fValid = obj().fValid() && "" != obj()->fnObj.sFile;
	pCmdUI->Enable(fValid);
}

CWnd* IlwisDocument::wndGetActiveView() const
{
	POSITION pos = GetFirstViewPosition();
	if (0 == pos)
		return 0;
	return GetNextView(pos);
}

void IlwisDocument::ShowAddInfo(bool fDock)
{
	CWnd* wnd = wndGetActiveView();
	CFrameWnd* fw = wnd->GetTopLevelFrame();
	if (0 == fw)
		return;
	AddInfoView* aiv = new AddInfoView;
	gbAddInfo = new GeneralBar;
	gbAddInfo->view = aiv;
	gbAddInfo->Create(fw, ID_ADDINFOBAR, CSize(250,120));
	String sTit = obj()->sTypeName();
	gbAddInfo->SetWindowText(sTit.c_str());
	aiv->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0,0,0,0), gbAddInfo, 100, 0);
	AddView(aiv);
	aiv->OnInitialUpdate();
	if (fDock) 
		fw->DockControlBar(gbAddInfo, AFX_IDW_DOCKBAR_BOTTOM);
	else
		fw->FloatControlBar(gbAddInfo,CPoint(100,100));
	fw->ShowControlBar(gbAddInfo,TRUE,FALSE);
}

void IlwisDocument::OnShowAddInfo()
{
	CWnd* wnd = wndGetActiveView();
	CFrameWnd* fw = wnd->GetTopLevelFrame();
	if (0 == fw)
		return;
	if (gbAddInfo) {
		BOOL fShown = gbAddInfo->IsWindowVisible();
		RemoveView(gbAddInfo->view);
		delete gbAddInfo;
		gbAddInfo = 0;
		fw->RecalcLayout();
		if (fShown)
			return;
	}
	ShowAddInfo(false);
}

void IlwisDocument::OnUpdateShowAddInfo(CCmdUI* pCmdUI)
{
	BOOL fShown = false;							 
	if (gbAddInfo) 
		fShown = gbAddInfo->IsWindowVisible();
	bool fPossible = false;
	if (obj().fValid())
		fPossible = obj()->fAdditionalInfo();
	pCmdUI->Enable(fPossible||fShown);
	pCmdUI->SetCheck(fShown);
}

COleServerItem* IlwisDocument::OnGetEmbeddedItem() // should be overruled!
{
	// zero means that operation failed
	return 0;
}

void IlwisDocument::OnIdle()
{
	// COleDocument::OnIdle() causes ASSERT errors
	// what needs overruling?
}

// by default do nothing
// not implementing would give errors
void IlwisDocument::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

void IlwisDocument::OnOperationCmd(UINT iCmd)
{
	if (!obj().fValid())
		return;
	FileName fn = obj()->fnObj;

	IlwisWinApp* iwa = IlwWinApp();
	const ActionList& actList = *iwa->acl();

	String sCmd = actList[iCmd-ID_OPERATION1]->sExec(fn);
	if (sCmd == "")
		return;
	char* str = sCmd.sVal();
	iwa->GetMainWnd()->SendMessage(ILWM_EXECUTE, 0, (LPARAM)str);	
}

void IlwisDocument::OnSaveCopyAs()
{
	DoSave(0, FALSE);
}

// largely copied from CDocument::SaveModified()
BOOL IlwisDocument::SaveModified()
{
	if (!IsModified())
		return TRUE;        // ok to continue

	// when no template (eg graph in HistogramWindow) do not ask silly questions
	CDocTemplate* pTemplate = GetDocTemplate();
	if (0 == pTemplate)
		return TRUE;

	// get name/title of document
	CString name;
	if (m_strPathName.IsEmpty())
	{
		// get name based on caption
		name = m_strTitle;
		if (name.IsEmpty())
			VERIFY(name.LoadString(AFX_IDS_UNTITLED));
	}
	else
	{
		// get name based on file title of path name
		name = m_strPathName;
		AfxGetFileTitle(m_strPathName, name.GetBuffer(_MAX_PATH), _MAX_PATH);
		name.ReleaseBuffer();
	}

	CString prompt;
	AfxFormatString1(prompt, AFX_IDP_ASK_TO_SAVE, name);
	switch (AfxMessageBox(prompt, MB_YESNOCANCEL|MB_TOPMOST, AFX_IDP_ASK_TO_SAVE))
	{
	case IDCANCEL:
		return FALSE;       // don't continue

	case IDYES:
		// If so, either Save or Update, as appropriate
		if (!DoFileSave())
			return FALSE;       // don't continue
		break;

	case IDNO:
		// If not saving changes, revert the document
		break;

	default:
		ASSERT(FALSE);
		break;
	}
	return TRUE;    // keep going
}

void IlwisDocument::OnUpdateFileSave(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(IsModified());
}

bool IlwisDocument::fEditable() const
{
	IlwisObject io = obj();
	return !io->fReadOnly() && !io->fDependent() && !io->fUseAs();
}

bool IlwisDocument::usesObject(const IlwisObject& ob) const {
	if (ob.fValid() && ilwisobj && ilwisobj->fValid())
		return ob == obj();
	return false;
}

bool IlwisDocument::fIsEmpty() const
{
	return false;
}

class IlwisDocumentMirrorFile : public CMirrorFile
{
	// This class is one of the items that help work around bug Q274487:
	// ReplaceFile() Requires WRITE_DAC Access on Replacement File
	// Implementation: class derived from CMirrorFile;
	// implementation of virtual function Close() follows.
public:
	virtual void Close();
private:
	BOOL SimpleReplaceFile(LPTSTR szReplaced, LPTSTR szReplacement);
};

CFile* IlwisDocument::GetFile(LPCTSTR lpszFileName, UINT nOpenFlags, CFileException* pError)
{
	// This function is one of the items that help work around bug Q274487:
	// ReplaceFile() Requires WRITE_DAC Access on Replacement File
	// The bug mentioned affects us because "Save Layout/MapView" does not work on the "Scratch"
	// See also b #5921 in the bugbase
	// Implementation: taken over from DOCCORE.CPP, CDocument::GetFile (and changed)
	IlwisDocumentMirrorFile* pFile = new IlwisDocumentMirrorFile;
	ASSERT(pFile != NULL);
	if (!pFile->Open(lpszFileName, nOpenFlags, pError))
	{
		delete pFile;
		pFile = NULL;
	}
	return pFile;
}

void IlwisDocumentMirrorFile::Close()
{
	// This function is one of the items that help work around bug Q274487:
	// ReplaceFile() Requires WRITE_DAC Access on Replacement File
	// Implementation: taken over from DOCCORE.CPP, CMirrorFile::Close (and changed)
	CString m_strName = m_strFileName; //file close empties string
	CFile::Close();
	if (!m_strMirrorName.IsEmpty())
	{
		BOOL bWorked = FALSE;
		DWORD dwResult = 0;

		// The line below does now the ReplaceFile
		bWorked = SimpleReplaceFile((LPTSTR)(LPCTSTR)m_strName, (LPTSTR)(LPCTSTR)m_strMirrorName);

		if (!bWorked)
		{
			dwResult = GetLastError();

			if (dwResult == 0)
				CFile::Remove(m_strName);

			CFile::Rename(m_strMirrorName, m_strName);
		}
	}
}



/*****************************************************************************
*
* GetFileAttributesEx
*
* only the necessary part was taken from NewAPIs.h, starting at the following line
*
*  Copyright (c) 1997-1999, Microsoft Corporation
*
*  Wrapper module that "stubs" APIs that were not implemented
*  on Windows 95 or Windows NT versions less than 4.0 SP 3.
*
*  By using this header, your code will run on older platforms.
*
*  To enable a particular wrapper, define the corresponding symbol.
*
*  Function                Symbol
*
*  GetDiskFreeSpaceEx      WANT_GETDISKFREESPACEEX_WRAPPER
*  GetLongPathName         WANT_GETLONGPATHNAME_WRAPPER
*  GetFileAttributesEx     WANT_GETFILEATTRIBUTESEX_WRAPPER
*  IsDebuggerPresent       WANT_ISDEBUGGERPRESENT_WRAPPER
*
*  Exactly one source file must include the line
*
*  #define COMPILE_NEWAPIS_STUBS
*
*  before including this file.
*
*
*****************************************************************************/

#undef GetFileAttributesEx
#define GetFileAttributesEx _GetFileAttributesEx

extern BOOL (CALLBACK *GetFileAttributesEx)
(LPCTSTR, GET_FILEEX_INFO_LEVELS, LPVOID);

/*
* The version to use if we are forced to emulate.
*/
static BOOL WINAPI
Emulate_GetFileAttributesEx(LPCTSTR ptszFile, GET_FILEEX_INFO_LEVELS level,
							LPVOID pv)
{
	BOOL fRc;

	if (level == GetFileExInfoStandard) {

		/*
		*  Must call GetFileAttributes first to avoid returning random
		*  values if the so-called filename contains wildcards.
		*/
		if (GetFileAttributes(ptszFile) != 0xFFFFFFFF) {
			HANDLE hfind;
			WIN32_FIND_DATA wfd;
			hfind = FindFirstFile(ptszFile, &wfd);
			if (hfind != INVALID_HANDLE_VALUE) {
				LPWIN32_FILE_ATTRIBUTE_DATA pfad = (LPWIN32_FILE_ATTRIBUTE_DATA)pv;
				FindClose(hfind);

				pfad->dwFileAttributes = wfd.dwFileAttributes;
				pfad->ftCreationTime   = wfd.ftCreationTime;
				pfad->ftLastAccessTime = wfd.ftLastAccessTime;
				pfad->ftLastWriteTime  = wfd.ftLastWriteTime;
				pfad->nFileSizeHigh    = wfd.nFileSizeHigh;
				pfad->nFileSizeLow     = wfd.nFileSizeLow;

				fRc = TRUE;

			} else {
				/*
				*  FindFirstFile already called SetLastError() for us.
				*/
				fRc = FALSE;
			}
		} else {
			/*
			*  GetFileAttributes already called SetLastError() for us.
			*/
			fRc = FALSE;
		}
	} else {
		/*
		*  Unknown info level.
		*/
		SetLastError(ERROR_INVALID_PARAMETER);
		fRc = FALSE;
	}

	return fRc;
}

/*
* The stub that probes to decide which version to use.
*/
static BOOL WINAPI
Probe_GetFileAttributesEx(LPCTSTR ptszFile, GET_FILEEX_INFO_LEVELS level,
						  LPVOID pv)
{
	HINSTANCE hinst;
	FARPROC fp;
	BOOL fRc;
	BOOL (CALLBACK *RealGetFileAttributesEx)
		(LPCTSTR, GET_FILEEX_INFO_LEVELS, LPVOID);

	hinst = GetModuleHandle(TEXT("KERNEL32"));
#ifdef UNICODE
	fp = GetProcAddress(hinst, "GetFileAttributesExW");
#else
	fp = GetProcAddress(hinst, "GetFileAttributesExA");
#endif

	if (fp) {
		*(FARPROC *)&RealGetFileAttributesEx = fp;
		fRc = RealGetFileAttributesEx(ptszFile, level, pv);
		if (fRc || GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) {
			GetFileAttributesEx = RealGetFileAttributesEx;
		} else {
			GetFileAttributesEx = Emulate_GetFileAttributesEx;
			fRc = GetFileAttributesEx(ptszFile, level, pv);
		}
	} else {
		GetFileAttributesEx = Emulate_GetFileAttributesEx;
		fRc = GetFileAttributesEx(ptszFile, level, pv);
	}

	return fRc;

}

BOOL (CALLBACK *GetFileAttributesEx)
(LPCTSTR, GET_FILEEX_INFO_LEVELS, LPVOID) =
Probe_GetFileAttributesEx;

/*****************************************************************************
*
* END OF GetFileAttributesEx FROM NewAPIs.h
*
*****************************************************************************/



BOOL IlwisDocumentMirrorFile::SimpleReplaceFile(LPTSTR szReplaced, LPTSTR szReplacement)
{
	// This function is one of the items that help work around bug Q274487:
	// ReplaceFile() Requires WRITE_DAC Access on Replacement File
	// Implementation: taken from MSDN documentation as a workaround for the bug in ReplaceFile:
	// BUG Q274487: ReplaceFile() Requires WRITE_DAC Access on Replacement File 

	HANDLE hReplaced = INVALID_HANDLE_VALUE;
	BOOL   fSuccess  = FALSE;

	WIN32_FILE_ATTRIBUTE_DATA fad;

	__try {

		// Validate parameters.
		if (szReplaced == NULL || szReplacement == NULL) {
			SetLastError(ERROR_INVALID_PARAMETER);
			__leave;
		}

		// Retrieve attributes from the file to be replaced.
		if (!GetFileAttributesEx(szReplaced, GetFileExInfoStandard, &fad))
			__leave;

		// Delete the file that is being replaced.
		if (!DeleteFile(szReplaced))
			__leave;

		// Rename the replacement file.
		if (!MoveFile(szReplacement, szReplaced))
			__leave;

		// This is enough to report success.
		fSuccess = TRUE;

		// Try to preserve the following attributes from the original file:
		fad.dwFileAttributes &= FILE_ATTRIBUTE_ARCHIVE 
			| FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_NORMAL
			| FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
			| FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM
			| FILE_ATTRIBUTE_TEMPORARY;
		if (!SetFileAttributes(szReplaced, fad.dwFileAttributes))
			__leave;

		// Try to set the creation time to match the original file.
		hReplaced = CreateFile(szReplaced, 0, FILE_SHARE_READ 
			| FILE_SHARE_WRITE | FILE_SHARE_DELETE,
			NULL, OPEN_EXISTING, 0, NULL);
		if (hReplaced == NULL)
			__leave;

		if (!SetFileTime(hReplaced, &(fad.ftCreationTime), NULL, NULL))
			__leave;

	} __finally {

		if (hReplaced != INVALID_HANDLE_VALUE)
			CloseHandle(hReplaced);
	}

	return fSuccess;
}
