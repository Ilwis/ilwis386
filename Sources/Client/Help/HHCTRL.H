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
// Copyright (C) 1998-1999 KeyWorks Software. 

// This class is designed for embedding a window that displays HTML content. It
// is modeled after the CHtmlView class, but can be used as a control and has
// additional functionality for supporting HTML Help (.CHM and .ITS file
// support). See hhctrl.cpp for more information.

#ifndef __HHCTRL_H__
#define __HHCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef AFX_OLE_TRUE
#define AFX_OLE_TRUE (-1)
#define AFX_OLE_FALSE 0
#endif

#define HHCTRL_BTN_HOME     (1 << 0)    
#define HHCTRL_BTN_BACK     (1 << 1)    
#define HHCTRL_BTN_FORWARD  (1 << 2)    
#define HHCTRL_BTN_PRINT    (1 << 3)   
#define HHCTRL_BTN_CLOSE    (1 << 4)   

// These values will be sent as the WPARAM member of a WM_COMMAND message that
// will be sent to the parent window when a button is clicked.

#define HH_TB_HOME      0xFF00
#define HH_TB_BACK      0xFF01
#define HH_TB_FORWARD   0xFF02
#define HH_TB_PRINT     0xFF03
#define HH_TB_CLOSE     0xFF04

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


class IMPEXP CHHCtrl : public CWnd
{
public: 

    CHHCtrl();
    ~CHHCtrl();

    virtual bool Create(CWnd* pParent, const RECT& rc, 
                DWORD dwBtns = HHCTRL_BTN_HOME | HHCTRL_BTN_BACK | HHCTRL_BTN_PRINT, 
                LPCTSTR pszHomeURL = NULL, bool fBtnText = true);

    // Use ReplaceControl instead of Create if you are replacing a static or
    // other control with a help window

    virtual bool ReplaceControl(CWnd* pDlg, UINT idCtrl, 
                DWORD dwBtns = HHCTRL_BTN_HOME | HHCTRL_BTN_BACK | HHCTRL_BTN_PRINT, 
                LPCTSTR pszHomeURL = NULL, bool fBtnText = true);

    DECLARE_DYNAMIC(CHHCtrl)
    DECLARE_EVENTSINK_MAP()

public:
    IWebBrowser2* GetBrowser() { return m_pBrowserApp; }
    LPCTSTR GetChmFile() { return (LPCTSTR) m_cszChm; }
    bool    SetChmFile(LPCTSTR pszChm); // returns false if CHM file cannot be found
    static bool    FindChmFile(LPCTSTR pszFile, LPTSTR pszDst);    // automatically called by SetChmFile()
    void    SetHomeFile(LPCTSTR pszUrl) { m_cszHome = pszUrl; }

    void    MoveWindow(CRect* prc, BOOL fRepaint);
                
    CString GetType() const;
    long GetLeft() const;
    void SetLeft(long nNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Left(nNewValue); }
    long GetTop() const;
    void SetTop(long nNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Top(nNewValue); }
    long GetHeight() const;
    void SetHeight(long nNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Height(nNewValue); }
    BOOL GetVisible() const;
    void SetVisible(bool fNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Visible((short) (fNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }
    CString GetLocationName() const;
    READYSTATE GetReadyState() const;
    BOOL GetOffline() const;
    void SetOffline(bool fNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Offline((short) (fNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }
    BOOL GetSilent() const;
    void SetSilent(bool fNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Silent((short) (fNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }
    BOOL GetTopLevelContainer() const;
    CString GetLocationURL() const;
    BOOL GetBusy() const;
    LPDISPATCH GetApplication() const;
    LPDISPATCH GetParentBrowser() const;
    LPDISPATCH GetContainer() const;
    LPDISPATCH GetHtmlDocument() const;
    CString GetFullName() const;
    OLECMDF QueryStatusWB(OLECMDID cmdID) const;
    BOOL GetRegisterAsDropTarget() const;
    void SetRegisterAsDropTarget(bool fNewValue) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_RegisterAsDropTarget((short) (fNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

    void GoBack() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoBack(); }
    void GoForward() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoForward(); }
    void GoHome() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoHome(); }
    void GoSearch() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoSearch(); }
    void Refresh() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Refresh(); }
    void Refresh2(int nLevel) { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Refresh2(COleVariant((long) nLevel, VT_I4)); }
    void Stop() { ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Stop(); }

    BOOL GetProperty(LPCTSTR lpszProperty, CString& strValue);
    COleVariant GetProperty(LPCTSTR lpszProperty);
    void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
    void PutProperty(LPCTSTR lpszPropertyName, long lValue) { ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(lValue, VT_I4)); }
    void PutProperty(LPCTSTR lpszPropertyName, short nValue) { ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(nValue, VT_I2)); }
    void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue) { ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(lpszValue, VT_BSTR)); }
    void PutProperty(LPCTSTR lpszPropertyName, double dValue) { ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(dValue)); }

    HRESULT Navigate(LPCTSTR URL, DWORD dwFlags = 0,
                  LPCTSTR lpszTargetFrameName = NULL,
                  LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
                  DWORD dwPostDataLen = 0);
    void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
                   LPCTSTR lpszTargetFrameName = NULL);
    void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
                   LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL,
                   LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
    void Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
                   CByteArray& baPostedData,
                   LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);
    HRESULT NavigateChm(LPCTSTR URL, LPCTSTR lpszTargetFrameName = NULL);
    HRESULT NavigateChm(UINT mapID, LPCTSTR lpszTargetFrameName = NULL);

    void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn, VARIANT* pvaOut);

// Miscellanious

    static LPTSTR StrChr(LPCTSTR psz, TCHAR ch);   // DBCS-enabled character search
    static LPTSTR StrRChr(LPCTSTR psz, TCHAR ch);  // DBCS-enabled character search
    static void   AddTrailingBackslash(LPTSTR psz);
    static void   GetRegWindowsDirectory(TCHAR* pszDst);
    CToolBarCtrl* GetToolBarCtrl() { return m_pTB; }

    virtual void OnDraw(CDC* pDC) {
        UNUSED_ALWAYS(pDC);    
        // this class should never do its own drawing;
        // the browser control should handle everything

        ASSERT(FALSE);  // make certain we aren't called
    }

    // Events
    virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
                                   LPCTSTR lpszTargetFrameName, 
                                   CByteArray& baPostedData,
                                   LPCTSTR lpszHeaders, BOOL* pfCancel);
    virtual void OnNavigateComplete2(LPCTSTR strURL);
    virtual void OnDownloadBegin();
    virtual void OnDownloadComplete();
    virtual void OnDocumentComplete(LPCTSTR lpszURL);
    virtual void OnProgressChange(long nProgress, long nProgressMax);
    virtual void OnStatusTextChange(LPCTSTR lpszText);
    virtual void OnTitleChange(LPCTSTR lpszText);
    virtual void OnCommandStateChange(long nCommand, BOOL fEnable);
    virtual void OnPropertyChange(LPCTSTR lpszProperty);
    virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
    virtual void OnQuit();
    virtual void OnVisible(BOOL fVisible);
    virtual void OnCommand(DWORD cmd);

public:
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

// Event reflectors (not normally overridden)
protected:
    virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
    virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL,
                                 VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData,
                                 VARIANT* Headers,   BOOL* Cancel);
    virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);

public:     // public so that host can call OnFilePrint
    //{{AFX_MSG(CHHCtrl)
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnPaint();
    afx_msg void OnDestroy();
    afx_msg void OnFilePrint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

protected:
    IWebBrowser2* m_pBrowserApp;
    CString m_cszChm;   // stores the location of the CHM file if any

    typedef struct {
        DWORD mapid;
        DWORD offset;   
    } MAPID;

    MAPID*  m_paMapIds;
    int     m_cMapIds;
    int     m_idLastButton;

    CString     m_cszHome;
    int         m_mapHome;
    CImageList* m_pimg;
    CImageList* m_pimgHot;
    CToolBarCtrl*  m_pTB;
};

#endif // __HHCTRL_H__
