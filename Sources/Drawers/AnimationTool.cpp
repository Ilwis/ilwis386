#include "Client\Headers\formelementspch.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Drawers\AnimationDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\AnimationTool.h"
#include "Client\Base\Framewin.h"
#include "Client\Mapwindow\MapWindow.h"
//#include "Drawers\RepresentationTool.h"
//#include "Drawers\StretchTool.h"

DrawerTool *createAnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) {
	return new AnimationTool(zv, view, drw);
}

AnimationTool::AnimationTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
	DrawerTool("AnimationTool",zv, view, drw)
{
}

AnimationTool::~AnimationTool() {
}

bool AnimationTool::isToolUseableFor(ILWIS::NewDrawer *drw) { 

	return dynamic_cast<AnimationDrawer *>(drw) != 0;
}

HTREEITEM AnimationTool::configure( HTREEITEM parentItem) {
	DisplayOptionTree *item = new DisplayOptionTree(tree,parentItem,drawer, this);
	htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv",(LayerTreeItem *)item);

	//htiNode = insertItem(parentItem, TR("Display Tools"), ".mpv");
	DisplayOptionTreeItem *item2 = new DisplayOptionTreeItem(tree,htiNode,drawer);
	item2->setDoubleCickAction(this, (DTDoubleClickActionFunc) &AnimationTool::animationDefaultView);
	HTREEITEM hti = insertItem(TR("Restore default view"),".isl",item2);


	MapWindow *parent = (MapWindow *)drawer->getRootDrawer()->getDrawerContext()->getDocument()->mpvGetView()->GetParent();

	if ( animBar.GetSafeHwnd() == 0) {
		animBar.Create(parent);
		CRect rect;
		parent->barScale.GetWindowRect(&rect);
		rect.OffsetRect(1,0);
		parent->DockControlBar(&animBar,AFX_IDW_DOCKBAR_TOP, rect);
	}
	DrawerTool::configure(htiNode);

	return htiNode;
}

String AnimationTool::getMenuString() const {
	return TR("Animation");
}

void AnimationTool::animationDefaultView() {
	((AnimationDrawer *)drawer)->animationDefaultView();
}

//---------------------------------------------------

BEGIN_MESSAGE_MAP(AnimationBar, CToolBar)
	ON_EN_SETFOCUS(ID_AnimationBar,OnSetFocus)
	ON_EN_KILLFOCUS(ID_AnimationBar,OnKillFocus)
END_MESSAGE_MAP()

AnimationBar::AnimationBar()
{
	fActive = false;

	LOGFONT logFont;
	memset(&logFont, 0, sizeof(logFont));
	// Since design guide says toolbars are fixed height so is the font.
	logFont.lfHeight = -12;
	logFont.lfWeight = FW_BOLD;
	logFont.lfPitchAndFamily = VARIABLE_PITCH | FF_SWISS;
	lstrcpy(logFont.lfFaceName, "MS Sans Serif");
	fnt.CreateFontIndirect(&logFont);
}

AnimationBar::~AnimationBar()
{
}

void AnimationBar::Create(CWnd* pParent)
{
	int iWidth = 180;

	DWORD dwCtrlStyle = TBSTYLE_FLAT | TBSTYLE_TRANSPARENT | TBSTYLE_TOOLTIPS;
	DWORD dwStyle = WS_CHILD | WS_VISIBLE |
		              CBRS_TOP | CBRS_TOOLTIPS | CBRS_FLYBY;
	CRect rectBB;
	rectBB.SetRect(2,1,2,1);
	CToolBar::CreateEx(pParent, dwCtrlStyle, dwStyle, rectBB, ID_AnimationBar);
	UINT ai[2];
	ai[0] = ID_AnimationBar;
	ai[1] = ID_SEPARATOR;
	SetButtons(ai,2);
	CRect rect;
	GetItemRect(0, &rect);
	SetButtonInfo(1, ID_AnimationBar,	TBBS_SEPARATOR, iWidth - rect.Width());

	rect.top = 3;
	rect.bottom -= 2;
	rect.right = rect.left + iWidth;
	ed.Create(WS_VISIBLE|WS_CHILD|WS_BORDER|WS_DISABLED,rect,this,ID_AnimationBar);
	ed.SetFont(&fnt);
	SendMessage(DM_SETDEFID,IDOK);

	EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	SetBarStyle(GetBarStyle()|CBRS_GRIPPER|CBRS_BORDER_3D);

	SetWindowText(TR("Animation").scVal());
}

void AnimationBar::OnUpdateCmdUI(CFrameWnd* pParent, BOOL)
{
	if (fActive)
		return;
}

void AnimationBar::OnSetFocus()
{
	fActive = true;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(false);
}

void AnimationBar::OnKillFocus()
{
	fActive = false;
	FrameWindow* fw = dynamic_cast<FrameWindow*>(GetParentOwner());
	if (fw)
		fw->HandleAccelerators(true);
}


void AnimationBar::updateTime(const String& s) // called by AnimationBarEdit
{

	ed.SetWindowText(s.scVal());
}
