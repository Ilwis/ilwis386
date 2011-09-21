// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
	~CMainFrame();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	CChildView    m_wndView;


protected:
	DECLARE_DYNAMIC(CMainFrame)

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg LONG OnInitDraw(UINT wParam, LONG lParam);
	DECLARE_MESSAGE_MAP()
};


