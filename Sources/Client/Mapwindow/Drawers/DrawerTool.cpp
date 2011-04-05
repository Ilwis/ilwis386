#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Representation\Rpr.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\MapPaneViewTool.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Engine\Drawers\DrawerContext.h"
#include "Client\Mapwindow\Drawers\DrawerTool.h"


using namespace ILWIS;

map<String, CreateDrawerTool> DrawerTool::factory;

DrawerTools::~DrawerTools() {
	for(int i=0; i < size(); ++i) {
		delete at(i);
	}
	resize(0);
}
//---------------------------------------
DrawerTool *DrawerTool::createTool(const String& name, ZoomableView *zv, LayerTreeView *view, NewDrawer *drw) {
	map<String, CreateDrawerTool>::iterator cur = factory.find(name);
	if ( cur  != factory.end()) {
		return ((*cur).second)(zv, view, drw);
	}
	return 0;

}

DrawerTool *DrawerTool::getTool(const String& type) const{
	for(int i=0; i < tools.size(); ++i) {
		DrawerTool *tool = tools[i];
		if ( tool->getType() == type)
			return tool;
		else {
			DrawerTool *ftool =  tool->getTool(type);
			if ( ftool)
				return ftool;
		}
	}
	return 0;
}

void DrawerTool::addCreateTool(const String& name, CreateDrawerTool func) {
	map<String, CreateDrawerTool>::iterator cur = factory.find(name);
	if ( cur  == factory.end()) {
		factory[name] = func;
	}
}

// - - - - - - - - - - - - - - - - - - - - -
DrawerTool::DrawerTool(const String& tpe, ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : 
MapPaneViewTool(zv),
drawer(drw), tree(view), visible(true),parentTool(0),isConfigured(false), type(tpe),htiNode(0)
{

}

DrawerTool::~DrawerTool() {
	MapCompositionDoc *mdoc = tree->GetDocument();
	MapPaneView *mpv = mdoc->mpvGetView();
	mpv->noTool(getId());
}

HTREEITEM DrawerTool::configure( HTREEITEM parentItem){
	if ( isConfigured)
		return parentItem;

	for(int i=0; i < tools.size(); ++i) {
		if ( tools[i]->isActive())
			tools[i]->configure(parentItem);
	}
	isConfigured = true;
	return parentItem;
}

//pseudo destructor; regular destructor is called too often(by design), this method can be called when a real cleanup mys be done
void DrawerTool::clear() {
	for(int i=0; i < tools.size(); ++i)
		tools[i]->clear();
	isConfigured = false;
}

DrawerTool *DrawerTool::getTool(int index) const{
	if ( index < tools.size())
		return tools[index];
	return 0;
}

void DrawerTool::addDrawer(ComplexDrawer *cdrw) {
	if (!cdrw) 
		return; 

	for( map<String, CreateDrawerTool>::iterator cur = factory.begin(); cur != factory.end(); ++cur) {
		DrawerTool *dtool = ((*cur).second)(mpv,tree,cdrw);
		if ( dtool->isToolUseableFor(cdrw) ) {
			addTool(dtool);
		} else
			delete dtool;
	}
}

bool DrawerTool::addTool(DrawerTool *tool, int proposedIndex) {
	for(int i=0; i < tools.size(); ++i) {
		if ( tools[i]->getId() == tool->getId())
			return false;
	}
	if ( proposedIndex == iUNDEF)
		tools.push_back(tool);
	else
		tools.insert(tools.begin() + proposedIndex, tool);
	for( map<String, CreateDrawerTool>::iterator cur = factory.begin(); cur != factory.end(); ++cur) {
		DrawerTool *dtool = ((*cur).second)(mpv,tree,(ComplexDrawer *)tool->getDrawer());
		if ( dtool->isToolUseableFor(tool) ) {
			tool->addTool(dtool);			
		} else
			delete dtool;
	}
	return true;
}

void DrawerTool::removeTool(DrawerTool *tool) {
	for(vector<DrawerTool *>::iterator cur=tools.begin(); cur != tools.end(); ++cur) {
		if ( (*cur)->getId() == tool->getId()) {
				delete (*cur);
				tools.erase(cur);
		}
	}
}

HTREEITEM DrawerTool::insertItem(const String& name,const String& icon, DisplayOptionTreeItem *item, int checkstatus , HTREEITEM after) {
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = tree->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, item->getParent(), after);
	item->setTreeItem(htiDisplayOptions);
	if ( checkstatus >=0) {
		tree->GetTreeCtrl().SetCheck(htiDisplayOptions, checkstatus );
	}
	tree->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);

	return htiDisplayOptions;
}

HTREEITEM DrawerTool::insertItem(HTREEITEM parent, const String& name,const String& icon, LayerTreeItem *item) {
	int iImg = IlwWinApp()->iImage(icon);
	HTREEITEM htiDisplayOptions = tree->GetTreeCtrl().InsertItem(name.scVal(), iImg, iImg, parent);
	tree->GetTreeCtrl().SetItemData(htiDisplayOptions, (DWORD_PTR)item);
	return htiDisplayOptions; 
}

HTREEITEM DrawerTool::findTreeItemByName(HTREEITEM parent, const String& name) const {
	HTREEITEM currentItem = tree->GetTreeCtrl().GetNextItem(parent, TVGN_CHILD);
	while(currentItem != 0) {
		TVITEM item;
		TCHAR szText[1024];
		item.hItem = currentItem;
		item.mask = TVIF_TEXT | TVIF_HANDLE;
		item.pszText = szText;
		item.cchTextMax = 1024;

		BOOL bWorked = tree->GetTreeCtrl().GetItem(&item);
		if ( name == item.pszText )
			return item.hItem;
		currentItem =tree->GetTreeCtrl().GetNextItem(currentItem, TVGN_NEXT);
	}
	return 0;
}

bool DrawerTool::isVisible() const{
	return visible;
}

void DrawerTool::setVisible(bool yesno){
	visible = yesno;
}

NewDrawer *DrawerTool::getDrawer() const {
	return drawer;
}

DrawerTool *DrawerTool::getParentTool() const {
	return parentTool;
}

int DrawerTool::getToolCount() const {
	return tools.size();
}

void DrawerTool::setActiveMode(bool yesno) {
	active = yesno;
	//for(int i=0; i < tools.size(); ++i) {
	//	tools[i]->setActiveMode(yesno);
	//}
	if ( !yesno) {
		isConfigured = false; // the configuration becomes invalid at a false
		tree->DeleteAllItems(htiNode);
		htiNode = 0;
	} else {
		if ( parentTool) {
			HTREEITEM parentNode = getParentTool()->getTreeItem();
			configure(parentNode);
		}
	}
}

MapCompositionDoc *DrawerTool::getDocument() const {
	return (MapCompositionDoc *)mpv->GetDocument();
}

MapPaneView *DrawerTool::mpvGetView() const {
	return getDocument()->mpvGetView();
}

void DrawerTool::timedEvent(UINT timerid) {
	for(vector<DrawerTool *>::iterator cur=tools.begin(); cur != tools.end(); ++cur) {
		(*cur)->timedEvent(timerid);
	}
}
//----------------------------------------------------------------------------

DisplayOptionsForm::DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title, int style) : 
FormBaseDialog(par,title,style),
view((LayerTreeView *)par),
drw(dr),
initial(true)
{
}

void DisplayOptionsForm::OnCancel() {
	apply();
}

int DisplayOptionsForm::exec() {
	return 1;
}

void DisplayOptionsForm::apply() {
}

void DisplayOptionsForm::updateMapView() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->mpvGetView()->Invalidate();
}

void DisplayOptionsForm::create() {
	FormBaseDialog::create();
	initial = false;
}

//--------------------------------
DisplayOptionsForm2::DisplayOptionsForm2(ComplexDrawer *dr,CWnd *par, const String& title, int style) : 
FormBaseDialog(par,title,style),
view((LayerTreeView *)par),
drw(dr),
initial(true)
{
}

int DisplayOptionsForm2::exec() {
	return 1;
}

void DisplayOptionsForm2::updateMapView() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->mpvGetView()->Invalidate();
}

void DisplayOptionsForm2::create() {
	FormBaseDialog::create();
	initial = false;
}



