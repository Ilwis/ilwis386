// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include <string>
#include "Headers\toolspch.h"
#include "IlwisServer.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\RootDrawer.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


using namespace ILWIS;

CChildView::CChildView()
{
	pDC = 0;
	root = 0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	if (pDC == 0 && root != 0) {
		pDC = GetDC();
		PreparationParameters pp(NewDrawer::ptINITOPENGL, pDC);
		root->prepare(&pp);
		DrawerContext *ctx = root->getDrawerContext();
		ctx->TakeContext();
		root->draw();
		root->getDrawerContext()->ReleaseContext();
	}

}

void CChildView::initDraw(const FileName& fn) {
	if ( root)
		delete root;

	ILWIS::NewDrawer *drw = new ILWIS::RootDrawer();
	ILWIS::ComplexDrawer * cdrw = (ILWIS::ComplexDrawer *)drw;
	root = reinterpret_cast<RootDrawer *>(drw);
	root->load(fn,"");
	CoordBounds cb = root->getCoordBoundsZoom();
	PreparationParameters pp(NewDrawer::ptRESTORE);
	root->prepare(&pp);
	Invalidate();

}

