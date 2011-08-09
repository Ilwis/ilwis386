#include "stdafx.h"
#include "MainFrm.h"
 


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

//static UINT BASED_CODE styles[] =
//{
//	// same order as in the bitmap 'styles.bmp'
//	ID_SEPARATOR,           // for combofont box (placeholder)
//	ID_SEPARATOR,
//	ID_SEPARATOR,           // for combosize box (placeholder)
//	ID_SEPARATOR,
//	IDS_A, 
//	IDS_B, 
//	IDS_C, 
//	IDS_D, 
//};

static int nFontSizes[] = 
	{8, 9, 10, 11, 12, 14, 16, 18, 20, 22, 24, 26, 28, 36, 48, 72};



/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//if (!m_wndToolBar.Create(this) ||
	//	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	//{
	//	TRACE0("Failed to create toolbar\n");
	//	return -1;      // fail to create
	//}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	//m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
	//	CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

 
	if (!CreateRulerBar())
		return -1;

	
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	return CFrameWnd::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

BOOL CMainFrame::CreateRulerBar()
{

	if (!m_wndRulerBarH.Create(this, 
		CBRS_TOP|CBRS_HIDE_INPLACE, ID_VIEW_RULERH))
	{
		TRACE0("Failed to create ruler\n");
		return FALSE;      // fail to create
	}

	if (!m_wndRulerBarV.Create(this, 
		CBRS_LEFT|CBRS_HIDE_INPLACE, ID_VIEW_RULERV))
	{
		TRACE0("Failed to create ruler\n");
		return FALSE;      // fail to create
	}
	
	return TRUE;	
	 
} 

  
 
