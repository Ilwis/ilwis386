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
// Copyright (c) 1998-1999 KeyWorks Software. 

// This code is free; you can redistribute it and/or modify it as you see fit.
// In addition, you may also charge for any application using the CHHCtrl
// class, and are under no obligation to supply source code. 

// This software code is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.

// Because no income is generated from this code, the author cannot spend
// copious amounts of time answering questions. Nevertheless, if you are stuck
// on an implementation problem, first check www.keyworks.net and if you can't
// find what you need there, send email to ralphw@exmsft.com who will answer
// based on whether he is in town, how many other questions have been
// submitted, etc.

// This code is not static. Additions and enhancements will be added to it over
// time and posted to www.keyworks.net (information types and scripting are
// currently under development). It is suggested that if you make changes, you
// subclass CHHCtrl rather then modifying this module directly. That way you
// can pick up an update and not break your existing code.

#include "Headers\stdafx.h"
#include "Headers\compatib.h"
#include "Client\Help\HHCTRL.H"
#include "Client\Help\CITS.H"
#include "Client\Help\CHHRES.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static const char THIS_FILE[] = __FILE__;
#endif

const int TB_PAD = 2;   // vertical padding between toolbar and HTML window

IMPLEMENT_DYNAMIC(CHHCtrl, CWnd)

BEGIN_MESSAGE_MAP(CHHCtrl, CWnd)
    //{{AFX_MSG_MAP(CHHCtrl)
    ON_WM_SIZE()
    ON_WM_PAINT()
    ON_WM_DESTROY()
    //}}AFX_MSG_MAP
    // Standard printing commands
    ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHHCtrl, CWnd)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 102, OnStatusTextChange, VTS_BSTR)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 108, OnProgressChange, VTS_I4 VTS_I4)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 105, OnCommandStateChange, VTS_I4 VTS_BOOL)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 106, OnDownloadBegin, VTS_NONE)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 104, OnDownloadComplete, VTS_NONE)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 113, OnTitleChange, VTS_BSTR)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 252, NavigateComplete2, VTS_DISPATCH VTS_PVARIANT)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 250, BeforeNavigate2, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 112, OnPropertyChange, VTS_BSTR)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 251, OnNewWindow2, VTS_PDISPATCH VTS_PBOOL)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 259, DocumentComplete, VTS_DISPATCH VTS_PVARIANT)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 253, OnQuit, VTS_NONE)
    ON_EVENT(CHHCtrl, AFX_IDW_PANE_FIRST, 254, OnVisible, VTS_BOOL)
END_EVENTSINK_MAP()

CHHCtrl::CHHCtrl() : CWnd()
{
    m_pBrowserApp = NULL;
    m_paMapIds = NULL;
    m_pimg = NULL;
    m_pimgHot = NULL;
    m_pTB = NULL;
    m_mapHome = 0;
    m_idLastButton = HH_TB_CLOSE;
}

CHHCtrl::~CHHCtrl()
{
    if (m_pBrowserApp != NULL)
        m_pBrowserApp->Release();
    if (m_paMapIds)
        LocalFree((HLOCAL) m_paMapIds);
    if (m_pimg)
        delete m_pimg;
    if (m_pimgHot)
        delete m_pimgHot;
    if (m_pTB) 
        delete m_pTB;
    if (m_hWnd && IsWindow(m_hWnd))
        DestroyWindow();
}

void CHHCtrl::OnFilePrint()
{
    if (m_pBrowserApp != NULL) {
        LPDISPATCH lpDisp = GetHtmlDocument();

        if (lpDisp != NULL) {
            // the control will handle all printing UI

            LPOLECOMMANDTARGET lpTarget;
            if (SUCCEEDED(lpDisp->QueryInterface(IID_IOleCommandTarget,
                    (LPVOID*) &lpTarget))) {
                lpTarget->Exec(NULL, OLECMDID_PRINT, 0, NULL, NULL);
                lpTarget->Release();
            }
            lpDisp->Release();
        }
    }
}

#ifdef _DEBUG
void CHHCtrl::AssertValid() const
{
    CWnd::AssertValid();
}

void CHHCtrl::Dump(CDumpContext& dc) const
{
    CWnd::Dump(dc);
}
#endif //_DEBUG

void CHHCtrl::OnDestroy()
{
    if (m_pBrowserApp) {
        m_pBrowserApp->Release();
        m_pBrowserApp = NULL;
    }
}

void CHHCtrl::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    if (::IsWindow(m_hWnd)) {
        // need to push non-client borders out of the client area
        CRect rect;
        GetClientRect(rect);
        ::AdjustWindowRectEx(rect, GetStyle(), FALSE, WS_EX_CLIENTEDGE);
        SetWindowPos(NULL, rect.left, rect.top,
            rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
    }
}

void CHHCtrl::OnPaint()
{
    Default();
}

#define TB_HEIGHT   50
#define TB_IMAGE_CX 20

bool CHHCtrl::Create(CWnd* pParent, const RECT& rc, DWORD dwBtns, LPCTSTR pszHomeURL, bool fText)
{
    AfxEnableControlContainer();
    CRect rcClient(rc);
    if (IsBadReadPtr(pszHomeURL, sizeof(LPCTSTR)))
        m_mapHome = (int) pszHomeURL;
    else
        m_cszHome = pszHomeURL;

    if (dwBtns) {
        if (m_pTB) {
            delete m_pTB;
            delete m_pimg;
            delete m_pimgHot;
#ifdef _DEBUG
            m_pTB = NULL;
            m_pimg = NULL;
            m_pimgHot = NULL;
#endif
        }

        // BUGBUG: Need to calculate height based on system font
        rcClient.bottom = rcClient.top + TB_IMAGE_CX + (fText ? 23 : 8);
        ASSERT(!m_pTB);
        m_pTB = new CToolBarCtrl;
        if (!m_pTB->Create(WS_CHILD | WS_VISIBLE | 
                TBSTYLE_FLAT | TBSTYLE_TOOLTIPS |
                CCS_NODIVIDER | CCS_NORESIZE,
                rcClient, pParent, 1)) {
            TRACE0("Unable to create CToolBarCtrl");
            ASSERT(FALSE);
            delete m_pTB;
            return false;
        }
        ASSERT(!m_pimg);
        ASSERT(!m_pimgHot);
        m_pimg = new CImageList;
        m_pimgHot = new CImageList;
        m_pimg->Create(IDB_CHHCTRL_TBGRAY, TB_IMAGE_CX, 0, RGB(255,0,255));
        m_pimgHot->Create(IDB_CHHCTRL_TB, TB_IMAGE_CX, 0, RGB(255,0,255));
        m_pTB->SetImageList(m_pimg);
        m_pTB->SetHotImageList(m_pimgHot);

        TBBUTTON tb[31];    // since we use bit flags, this is as high as you can go
        ASSERT((m_idLastButton - HH_TB_HOME) < 31);

        // Load the text for our buttons

        if (fText) {

            // Load the button string and change all pipe characters into null terminators

            TCHAR szBuf[512];
            if (LoadString(AfxGetInstanceHandle(), IDS_HHCTRL_TBSTRINGS, szBuf, sizeof(szBuf)) == 0) {
                TRACE0("Missing IDS_HHCTRL_TBSTRINGS resource string");
                ASSERT(FALSE);
                return false;
            }
            TCHAR* psz = szBuf;
            while ((psz = StrChr(psz, '|'))) 
                *psz++ =  '\0';
            m_pTB->AddStrings(szBuf);
        }

        // Now add the buttons we are going to use

        int iBtn = 0;
        for (int i = 0; i <= (m_idLastButton - HH_TB_HOME); i++) {
            if (dwBtns & (1 << i)) {

                // Special-case the Home button which requires a URL or map id

                if (i == 0 && !(pszHomeURL || m_mapHome))
                    continue;
                tb[iBtn].fsState = TBSTATE_ENABLED;
                tb[iBtn].fsStyle = TBSTYLE_BUTTON;
                tb[iBtn].dwData = 0;
                if (fText)
                    tb[iBtn].iString = i;
                else
                    tb[iBtn].iString = 0;
                tb[iBtn].iBitmap = i;
                tb[iBtn].idCommand = HH_TB_HOME + i;
                iBtn++;
            }
        }
        m_pTB->AddButtons(iBtn, tb);

        // Add some padding to the top of the window so there is a tiny bit of space
        // between the top of the button and the top of the host window.

        CRect rcBtn;
        m_pTB->GetWindowRect(&rcBtn);

        rcClient.top = rc.top + rcBtn.Height() + TB_PAD;
        rcClient.bottom = rc.bottom;
    }

    if (!CreateControl(CLSID_WebBrowser, NULL, 
            WS_VISIBLE | WS_CHILD, rcClient, pParent, AFX_IDW_PANE_FIRST)) {
        TRACE0("Unable to create control");
        return false;
    }

    LPUNKNOWN lpUnk = GetControlUnknown();
    HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
    if (FAILED(hr)) {
        m_pBrowserApp = NULL;
        TRACE0("Unable to create web browser");
        return false;
    }

    return true;
}

bool CHHCtrl::ReplaceControl(CWnd* pDlg, UINT idCtrl, DWORD dwBtns, LPCTSTR pszHomeURL, bool fBtnText)
{
    CRect rc;
    pDlg->GetDlgItem(idCtrl)->GetWindowRect(&rc);
    pDlg->ScreenToClient(&rc);
    pDlg->GetDlgItem(idCtrl)->DestroyWindow();

    return Create(pDlg, rc, dwBtns, pszHomeURL, fBtnText);
}

CString CHHCtrl::GetType() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_Type(&bstr);
    CString retVal(bstr);
    return retVal;
}

long CHHCtrl::GetLeft() const
{
    ASSERT(m_pBrowserApp != NULL);

    long result;
    m_pBrowserApp->get_Left(&result);
    return result;
}


long CHHCtrl::GetTop() const
{
    ASSERT(m_pBrowserApp != NULL);
    long result;
    m_pBrowserApp->get_Top(&result);
    return result;
}

long CHHCtrl::GetHeight() const
{
    ASSERT(m_pBrowserApp != NULL);
    long result;
    m_pBrowserApp->get_Height(&result);
    return result;
}

BOOL CHHCtrl::GetVisible() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_Visible(&result);
    return result;
}

CString CHHCtrl::GetLocationName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationName(&bstr);
    CString retVal(bstr);
    return retVal;
}

CString CHHCtrl::GetLocationURL() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_LocationURL(&bstr);
    CString retVal(bstr);
    return retVal;
}

BOOL CHHCtrl::GetBusy() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_Busy(&result);
    return result;
}

READYSTATE CHHCtrl::GetReadyState() const
{
    ASSERT(m_pBrowserApp != NULL);

    READYSTATE result;
    m_pBrowserApp->get_ReadyState(&result);
    return result;
}

BOOL CHHCtrl::GetOffline() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_Offline(&result);
    return result;
}

BOOL CHHCtrl::GetSilent() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_Silent(&result);
    return result;
}

LPDISPATCH CHHCtrl::GetApplication() const
{
    ASSERT(m_pBrowserApp != NULL);

    LPDISPATCH result;
    m_pBrowserApp->get_Application(&result);
    return result;
}


LPDISPATCH CHHCtrl::GetParentBrowser() const
{
    ASSERT(m_pBrowserApp != NULL);

    LPDISPATCH result;
    m_pBrowserApp->get_Parent(&result);
    return result;
}

LPDISPATCH CHHCtrl::GetContainer() const
{
    ASSERT(m_pBrowserApp != NULL);

    LPDISPATCH result;
    m_pBrowserApp->get_Container(&result);
    return result;
}

LPDISPATCH CHHCtrl::GetHtmlDocument() const
{
    ASSERT(m_pBrowserApp != NULL);

    LPDISPATCH result;
    m_pBrowserApp->get_Document(&result);
    return result;
}

BOOL CHHCtrl::GetTopLevelContainer() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_TopLevelContainer(&result);
    return result;
}

OLECMDF CHHCtrl::QueryStatusWB(OLECMDID cmdID) const
{
    ASSERT(m_pBrowserApp != NULL);

    OLECMDF result;
    m_pBrowserApp->QueryStatusWB(cmdID, &result);
    return result;
}

void CHHCtrl::ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt,
    VARIANT* pvaIn, VARIANT* pvaOut)
{
    ASSERT(m_pBrowserApp != NULL);

    m_pBrowserApp->ExecWB(cmdID, cmdexecopt, pvaIn, pvaOut);
}

BOOL CHHCtrl::GetRegisterAsDropTarget() const
{
    ASSERT(m_pBrowserApp != NULL);

    VARIANT_BOOL result;
    m_pBrowserApp->get_RegisterAsDropTarget(&result);
    return result;
}

HRESULT CHHCtrl::Navigate(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
    LPCTSTR lpszTargetFrameName /* = NULL */ ,
    LPCTSTR lpszHeaders /* = NULL */, LPVOID lpvPostData /* = NULL */,
    DWORD dwPostDataLen /* = 0 */)
{
    CString strURL(lpszURL);
    BSTR bstrURL = strURL.AllocSysString();

    COleSafeArray vPostData;
    if (lpvPostData != NULL)
    {
        if (dwPostDataLen == 0)
            dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

        vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
    }

    return m_pBrowserApp->Navigate(bstrURL,
        COleVariant((long) dwFlags, VT_I4),
        COleVariant(lpszTargetFrameName, VT_BSTR),
        vPostData,
        COleVariant(lpszHeaders, VT_BSTR));
}

void CHHCtrl::Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags /* = 0 */,
    LPCTSTR lpszTargetFrameName /* = NULL */)
{
    ASSERT(m_pBrowserApp != NULL);

    COleVariant vPIDL(pIDL);
    COleVariant empty;

    m_pBrowserApp->Navigate2(vPIDL,
        COleVariant((long) dwFlags, VT_I4),
        COleVariant(lpszTargetFrameName, VT_BSTR),
        empty, empty);
}

void CHHCtrl::Navigate2(LPCTSTR lpszURL, DWORD dwFlags /* = 0 */,
    LPCTSTR lpszTargetFrameName /* = NULL */,
    LPCTSTR lpszHeaders /* = NULL */,
    LPVOID lpvPostData /* = NULL */, DWORD dwPostDataLen /* = 0 */)
{
    ASSERT(m_pBrowserApp != NULL);

    COleSafeArray vPostData;
    if (lpvPostData != NULL)
    {
        if (dwPostDataLen == 0)
            dwPostDataLen = lstrlen((LPCTSTR) lpvPostData);

        vPostData.CreateOneDim(VT_UI1, dwPostDataLen, lpvPostData);
    }

    COleVariant vURL(lpszURL, VT_BSTR);
    COleVariant vHeaders(lpszHeaders, VT_BSTR);
    COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
    COleVariant vFlags((long) dwFlags, VT_I4);

    m_pBrowserApp->Navigate2(vURL,
        vFlags, vTargetFrameName, vPostData, vHeaders);
}

void CHHCtrl::Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
    CByteArray& baPostData, LPCTSTR lpszTargetFrameName /* = NULL */,
    LPCTSTR lpszHeaders /* = NULL */)
{
    ASSERT(m_pBrowserApp != NULL);

    COleVariant vPostData = baPostData;
    COleVariant vURL(lpszURL, VT_BSTR);
    COleVariant vHeaders(lpszHeaders, VT_BSTR);
    COleVariant vTargetFrameName(lpszTargetFrameName, VT_BSTR);
    COleVariant vFlags((long) dwFlags, VT_I4);

    ASSERT(m_pBrowserApp != NULL);

    m_pBrowserApp->Navigate2(vURL, vFlags, vTargetFrameName,
        vPostData, vHeaders);
}

void CHHCtrl::PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue)
{
    ASSERT(m_pBrowserApp != NULL);

    CString strProp(lpszProperty);
    BSTR bstrProp = strProp.AllocSysString();
    m_pBrowserApp->PutProperty(bstrProp, vtValue);
    ::SysFreeString(bstrProp);
}

BOOL CHHCtrl::GetProperty(LPCTSTR lpszProperty, CString& strValue)
{
    ASSERT(m_pBrowserApp != NULL);

    CString strProperty(lpszProperty);
    BSTR bstrProperty = strProperty.AllocSysString();

    BOOL bResult = FALSE;
    VARIANT vReturn;
    vReturn.vt = VT_BSTR;
    vReturn.bstrVal = NULL;
    HRESULT hr = m_pBrowserApp->GetProperty(bstrProperty, &vReturn);

    if (SUCCEEDED(hr)) {
        strValue = CString(vReturn.bstrVal);
        bResult = TRUE;
    }

    ::SysFreeString(bstrProperty);
    return bResult;
}

COleVariant CHHCtrl::GetProperty(LPCTSTR lpszProperty)
{
    COleVariant result;

    static BYTE parms[] =
        VTS_BSTR;
    InvokeHelper(0x12f, DISPATCH_METHOD,
        VT_VARIANT, (void*)&result, parms, lpszProperty);

    return result;
}

CString CHHCtrl::GetFullName() const
{
    ASSERT(m_pBrowserApp != NULL);

    BSTR bstr;
    m_pBrowserApp->get_FullName(&bstr);
    CString retVal(bstr);
    return retVal;
}

/////////////////////////////////////////////////////////////////////////////
// CHHCtrl event reflectors

void CHHCtrl::NavigateComplete2(LPDISPATCH /* pDisp */, VARIANT* URL)
{
    ASSERT(V_VT(URL) == VT_BSTR);

    // USES_CONVERSION;

    CString str(V_BSTR(URL));
    OnNavigateComplete2(str);
}

void CHHCtrl::BeforeNavigate2(LPDISPATCH /* pDisp */, VARIANT* URL,
        VARIANT* Flags, VARIANT* TargetFrameName,
        VARIANT* PostData, VARIANT* Headers, BOOL* pfCancel)
{
    ASSERT(V_VT(URL) == VT_BSTR);
    ASSERT(V_VT(TargetFrameName) == VT_BSTR);
    ASSERT(V_VT(PostData) == (VT_VARIANT | VT_BYREF));
    ASSERT(V_VT(Headers) == VT_BSTR);
    ASSERT(pfCancel != NULL);

    // USES_CONVERSION;

    VARIANT* vtPostedData = V_VARIANTREF(PostData);
    CByteArray array;
    if (V_VT(vtPostedData) & VT_ARRAY) {
        // must be a vector of bytes
        ASSERT(vtPostedData->parray->cDims == 1 && vtPostedData->parray->cbElements == 1);

        vtPostedData->vt |= VT_UI1;
        COleSafeArray safe(vtPostedData);

        DWORD dwSize = safe.GetOneDimSize();
        LPVOID pVoid;
        safe.AccessData(&pVoid);

        array.SetSize(dwSize);
        LPBYTE lpByte = array.GetData();

        memcpy(lpByte, pVoid, dwSize);
        safe.UnaccessData();
    }
    // make real parameters out of the notification

    CString strTargetFrameName(V_BSTR(TargetFrameName));
    CString strURL = V_BSTR(URL);
    CString strHeaders = V_BSTR(Headers);
    DWORD nFlags = V_I4(Flags);

    // notify the user's class
    OnBeforeNavigate2(strURL, nFlags, strTargetFrameName, array, strHeaders, pfCancel);
}

void CHHCtrl::DocumentComplete(LPDISPATCH pDisp, VARIANT* URL)
{
    UNUSED_ALWAYS(pDisp);
    ASSERT(V_VT(URL) == VT_BSTR);

    CString str(V_BSTR(URL));
    OnDocumentComplete(str);
}

/////////////////////////////////////////////////////////////////////////////
// CHHCtrl Events

void CHHCtrl::OnProgressChange(long lProgress, long lProgressMax)
{
    UNUSED_ALWAYS(lProgress);
    UNUSED_ALWAYS(lProgressMax);
}

void CHHCtrl::OnCommandStateChange(long lCommand, BOOL bEnable)
{
    switch (lCommand) {
        case CSC_NAVIGATEFORWARD:
            if (m_pTB)
                m_pTB->EnableButton(HH_TB_FORWARD, bEnable);
            break;

        case CSC_NAVIGATEBACK:
            if (m_pTB)
                m_pTB->EnableButton(HH_TB_BACK, bEnable);
            break;
    }
}

void CHHCtrl::OnDownloadBegin()
{
}

void CHHCtrl::OnQuit()
{
}

void CHHCtrl::OnDocumentComplete(LPCTSTR lpszURL)
{
    UNUSED_ALWAYS(lpszURL);
}

void CHHCtrl::OnDownloadComplete()
{
}

void CHHCtrl::OnTitleChange(LPCTSTR lpszText)
{
    UNUSED_ALWAYS(lpszText);
}

void CHHCtrl::OnPropertyChange(LPCTSTR lpszProperty)
{
    UNUSED_ALWAYS(lpszProperty);
}

void CHHCtrl::OnNewWindow2(LPDISPATCH* ppDisp, BOOL* pfCancel)
{
    *pfCancel = FALSE;  // default to continuing

    UNUSED_ALWAYS(ppDisp);
}

void CHHCtrl::OnVisible(BOOL fVisible)
{
    UNUSED_ALWAYS(fVisible);
}

void CHHCtrl::OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
    LPCTSTR lpszTargetFrameName, CByteArray& baPostData,
    LPCTSTR lpszHeaders, BOOL* pfCancel)
{
    *pfCancel = FALSE;    // default to continuing

    UNUSED_ALWAYS(lpszURL);
    UNUSED_ALWAYS(nFlags);
    UNUSED_ALWAYS(lpszTargetFrameName);
    UNUSED_ALWAYS(baPostData);
    UNUSED_ALWAYS(lpszHeaders);
}

void CHHCtrl::OnStatusTextChange(LPCTSTR pszText)
{
    UNUSED_ALWAYS(pszText);
}

void CHHCtrl::OnNavigateComplete2(LPCTSTR strURL)
{
    UNUSED_ALWAYS(strURL);
}


HRESULT CHHCtrl::NavigateChm(LPCTSTR URL, LPCTSTR lpszTargetFrameName)
{
    ASSERT(!m_cszChm.IsEmpty());    // You must call SetChmFile() first
    if (m_cszChm.IsEmpty())
        return STG_E_FILENOTFOUND;  // CHM file wasn't located

    char szURL[MAX_PATH * 2];
    wsprintf(szURL, "its:%s::%s", (LPCTSTR) m_cszChm, URL);
    return Navigate(szURL, NULL, lpszTargetFrameName);
}

bool CHHCtrl::SetChmFile(LPCTSTR pszChm)
{
    TCHAR szCHM[MAX_PATH];
    if (!FindChmFile(pszChm, szCHM)) {
        CString csz;
        AfxFormatString1(csz, IDS_HHCTRL_NO_CHM, pszChm);
        AfxMessageBox(csz, MB_ICONEXCLAMATION);
        return false;
    }
    m_cszChm = szCHM;
    return true;
}

// Look for the CHM file -- this is basically the same algorithm that
// HTML Help uses with the addition of looking in the same location as
// the executable.

bool CHHCtrl::FindChmFile(LPCTSTR pszFile, LPTSTR pszDst)
{
    if (GetFileAttributes(pszFile) != HFILE_ERROR) {
        GetFullPathName(pszFile, MAX_PATH, pszDst, NULL);
        return true;
    }

    // turn all forward slashes into back slashes

    TCHAR szCHM[1024];
    strcpy(szCHM, pszFile); // copy so we can modify
    // Convert forward slashes to backslashes for consistency
    LPTSTR pszTmp = StrChr(szCHM, '/');
    while (pszTmp) {
        *pszTmp = '\\';
        pszTmp = StrChr(pszTmp + 1, '/');
    }
    pszTmp = StrRChr(szCHM, '\\');
    if (pszTmp)
        pszTmp++;
    else
        pszTmp = szCHM;

    { // Try the same directory as the executable
        char szModule[MAX_PATH];
        GetModuleFileName(NULL, szModule, sizeof(szModule));
        LPTSTR pszFile = StrRChr(szModule, '\\');
        if (pszFile) {
            strcpy(pszFile + 1, pszTmp);
            if (GetFileAttributes(szModule) != HFILE_ERROR) {
                strcpy(pszDst, szModule);
                return true;
            }
        }
    }

    // Try the current directory

    if (GetFileAttributes(pszTmp) != HFILE_ERROR) {
        GetFullPathName(pszTmp, MAX_PATH, pszDst, NULL);
        return true;
    }

    // Try the registry

    HKEY hkey;
    LONG result;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,  "Software\\Microsoft\\Windows\\HTML Help",
            0, KEY_READ, &hkey) == ERROR_SUCCESS) {
        DWORD cbPath = MAX_PATH;
        DWORD type;
        result = RegQueryValueEx(hkey, pszTmp, 0, &type, (PBYTE) pszDst, &cbPath);
        RegCloseKey(hkey);

        if (result == ERROR_SUCCESS) {
            AddTrailingBackslash(pszDst);
            strcat(pszDst, pszTmp);
            if (GetFileAttributes(pszDst) != HFILE_ERROR) {
                return true;
            }
        }
    }

    // Try the Windows\help directory

    GetRegWindowsDirectory(pszDst);
    AddTrailingBackslash(pszDst);
    strcat(pszDst, "Help");
    AddTrailingBackslash(pszDst);
    strcat(pszDst, pszTmp);
    if (GetFileAttributes(pszDst) != HFILE_ERROR) 
        return true;

    // Try hh.ini

    if (GetPrivateProfileString("files", pszTmp, "", pszDst, MAX_PATH, "hh.ini") > 1) {
        TCHAR* pszComma = StrChr(pszDst, ',');
        if (pszComma)
            *pszComma++ = 0;
        if (GetFileAttributes(pszDst) != HFILE_ERROR) {
            return true;
        }
        if (pszComma && *pszComma) {
            do {
                if (AfxMessageBox(pszComma, MB_OKCANCEL | MB_ICONHAND) != IDOK)
                    break;
            } while (GetFileAttributes(pszDst) == HFILE_ERROR);
            return true;
        }
    }

    return false;
}

// This function supports diskless workstations -- GetWindowsDirectory() won't
// always give the same result in that environment

void CHHCtrl::GetRegWindowsDirectory(TCHAR* pszDst)
{
    HKEY hkey;

    DWORD cbPath = MAX_PATH;
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
           "Software\\Microsoft\\Windows\\CurrentVersion\\Setup", 0, KEY_READ, &hkey) ==
            ERROR_SUCCESS) {
        DWORD type;
        RegQueryValueEx(hkey, "SharedDir", 0, &type, (PBYTE) pszDst, &cbPath);
        RegCloseKey(hkey);
    }

    if (cbPath == MAX_PATH) // means couldn't read registry key
        GetWindowsDirectory(pszDst, MAX_PATH);
}

// DBCS-enabled searching

LPTSTR CHHCtrl::StrChr(LPCTSTR psz, TCHAR ch)
{
    while (*psz && *psz != ch) 
        psz = CharNext(psz);
    return (*psz ? (TCHAR*) psz : NULL);
}

LPTSTR CHHCtrl::StrRChr(LPCTSTR psz, TCHAR ch)
{
    LPCTSTR pszLastFound = StrChr(psz, ch);
    if (!pszLastFound)
        return NULL;
    while ((psz = StrChr(pszLastFound + 1, ch)))
        pszLastFound = psz;
    return (LPTSTR) pszLastFound;
}

void CHHCtrl::AddTrailingBackslash(LPTSTR psz)
{
    ASSERT(psz && *psz);
    TCHAR* pszEnd = psz + strlen(psz);
    if (*(CharPrev(psz, pszEnd)) != '\\' &&
            *(CharPrev(psz, pszEnd)) != '/' &&
            *(CharPrev(psz, pszEnd)) != ':') {
        *pszEnd++ = '\\';
        *pszEnd++ = '\0';
    }
}

HRESULT CHHCtrl::NavigateChm(UINT mapID, LPCTSTR lpszTargetFrameName)
{
    ASSERT(!m_cszChm.IsEmpty());    // You must call SetChmFile() first
    if (m_cszChm.IsEmpty())
        return STG_E_PATHNOTFOUND ;  // CHM file wasn't located

    if (!m_paMapIds) {
        CItsFile its;   // ITS and CHM files are effectively the same thing
        if (FAILED(its.OpenITS(m_cszChm))) // theoretically impossible as SetChmFile() should have failed first
            return STG_E_PATHNOTFOUND ;  // CHM file wasn't located

        IStream* pStream;
        if (FAILED(its.OpenStream("#IVB", &pStream, its.GetStorage()))) {
            TRACE1("Missing [MAP] section in the file %s", m_cszChm);
            return STG_E_FILENOTFOUND;
        }

        ULONG cbRead;
        DWORD cbMapIds;

        if (FAILED(pStream->Read(&cbMapIds, sizeof(DWORD), &cbRead)) || (cbRead != sizeof(DWORD))) {
            pStream->Release();
            return STG_E_READFAULT;
        }
        if (!(m_paMapIds = (MAPID*) LocalAlloc(LMEM_FIXED, cbMapIds))) {
            pStream->Release();
            return E_OUTOFMEMORY ;
        }
        if (FAILED(pStream->Read(m_paMapIds, cbMapIds, &cbRead)) || (cbRead != (ULONG) cbMapIds)) {
            pStream->Release();
            return STG_E_READFAULT;
        }
        pStream->Release();

        m_cMapIds = cbMapIds / sizeof(MAPID);
    }
    for (int i = 0; i < m_cMapIds; i++) {
        if (mapID == m_paMapIds[i].mapid) {
            CItsFile its;   // ITS and CHM files are effectively the same thing
            if (FAILED(its.OpenITS(m_cszChm))) // theoretically impossible as SetChmFile() should have failed first
                return STG_E_PATHNOTFOUND ;  // CHM file wasn't located
            CStringSubFile ssf(&its);
            TCHAR szURL[MAX_PATH * 2];
            wsprintf(szURL, "its:%s::", (LPCTSTR) m_cszChm);
            ssf.GetString(m_paMapIds[i].offset, szURL + strlen(szURL), sizeof(szURL) - strlen(szURL));
            return Navigate(szURL, NULL, lpszTargetFrameName);
        }
    }

    return E_FAIL;  // couldn't find the requested map id
}

void CHHCtrl::OnCommand(DWORD id)
{
    switch (id) {
        case HH_TB_HOME:
            ASSERT(m_pBrowserApp);

            // Check for a ':' in the URL in case the URL has a protocol (file:, http:, etc.)

            if (!m_cszChm.IsEmpty() && StrChr(m_cszHome, ':') == NULL) {
                if (!m_cszHome.IsEmpty())
                    NavigateChm(m_cszHome);
                else if (m_mapHome)
                    NavigateChm(m_mapHome);
            }
            else
                Navigate(m_cszHome);
            break;

        case HH_TB_BACK:
            GoBack();
            break;

        case HH_TB_FORWARD:
            GoForward();
            break;

        case HH_TB_PRINT:
            OnFilePrint();
            break;

        case HH_TB_CLOSE:
            GetParent()->PostMessage(WM_CLOSE, 0, 0);
            break;
    }
}

void CHHCtrl::MoveWindow(CRect* prc, BOOL fRepaint)
{
    if (m_pTB) {
        CRect rcBtn;
        m_pTB->GetClientRect(&rcBtn);
        rcBtn.OffsetRect(prc->left, prc->top);
        ::MoveWindow(m_pTB->m_hWnd, rcBtn.left, rcBtn.top, rcBtn.Width(), rcBtn.Height(), fRepaint);
        prc->top += rcBtn.Height() + TB_PAD;
    }
    ::MoveWindow(m_hWnd, prc->left, prc->top, prc->Width(), prc->Height(), fRepaint);
}
