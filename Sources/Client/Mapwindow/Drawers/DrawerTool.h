#pragma once

class MapPaneView;
class DisplayOptionTreeItem;
class LayerTreeItem;

namespace ILWIS {

class DrawerTool;
class ComplexDrawer;

typedef DrawerTool* (*CreateDrawerTool)(ZoomableView* zv, LayerTreeView  *tv, NewDrawer* drw);


class DrawerTools :public vector<DrawerTool *> {
public:
~DrawerTools();
};

class _export DrawerTool : public MapPaneViewTool {
public:
	DrawerTool(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
	virtual ~DrawerTool();
	virtual bool addTool(DrawerTool *tool, int proposedIndex=iUNDEF);
	virtual HTREEITEM configure( HTREEITEM parentItem);
	void removeTool(DrawerTool *tool);
	bool isVisible() const;
	void setVisible(bool yesno);
	static DrawerTool *createTool(const String& name, ZoomableView *zv, LayerTreeView *view, NewDrawer *drw);
	DrawerTool *getTool(const String& type) const;
	DrawerTool *getTool(int index) const;
	static void addCreateTool(const String& name, CreateDrawerTool func);
	virtual void addDrawer(ComplexDrawer *cdrw);
	void addChildTools(DrawerTool *tool);
	virtual bool isToolUseableFor(ILWIS::NewDrawer *drw) { return false;}
	virtual bool isToolUseableFor(ILWIS::DrawerTool *drw) { return false;}
	NewDrawer *getDrawer() const;
	DrawerTool *getParentTool() const;
	void setParentTool(DrawerTool *p) { parentTool = p; }
	String getType() const { return type; }
	virtual void update() {}
	virtual void prepare() {}
	int getToolCount() const;
	HTREEITEM getTreeItem() const { return htiNode; }
	MapCompositionDoc *getDocument() const;
	MapPaneView *mpvGetView() const;
	virtual void clear();
	virtual void timedEvent(UINT timerid);
	virtual void setActiveMode(bool yesno);
	virtual void doAction(int options=0) {}
	int getIcon() const;
	DrawerTool *findChildToolByType(const String& name, bool recursive = false);

protected:
	HTREEITEM insertItem(const String& name,const String& icon, DisplayOptionTreeItem *item=0, int checkstatus = -1, HTREEITEM after=TVI_LAST);
	HTREEITEM insertItem(HTREEITEM parent, const String& name,const String& icon, LayerTreeItem *item=0);
	HTREEITEM findTreeItemByName(HTREEITEM parent, const String& name) const;
	//virtual void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const {}
	virtual HTREEITEM make3D(bool yeno) {return 0;}
	void resetChildHtiNodes(DrawerTools & tools);

	NewDrawer *drawer;
	LayerTreeView *tree;
	DrawerTool *parentTool;
	bool isConfigured;
	String type;
	HTREEITEM htiNode;

private:
	DrawerTools tools;
	bool visible;
	static map<String, CreateDrawerTool> factory;

};

struct DrawerToolInfo {
	DrawerToolInfo(const String& n, CreateDrawerTool func) : name(n), createFunc(func) {}
	String name;
	CreateDrawerTool createFunc;
};

class _export DisplayOptionsForm : public FormBaseDialog {
public:
	DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title, int style=fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS);
	//afx_msg virtual void OnCancel();
	int exec();
	virtual void apply();
protected:
	void create();
	void updateMapView();
	ComplexDrawer *drw;
	LayerTreeView *view;
	bool initial;
};

class _export DisplayOptionsForm2 : public FormBaseDialog {
public:
	DisplayOptionsForm2(ComplexDrawer *dr,CWnd *par, const String& title,int style=fbsBUTTONSUNDER | fbsSHOWALWAYS | fbsNOCANCELBUTTON);
	int exec();
protected:
	void create();
	void updateMapView();
	ComplexDrawer *drw;
	LayerTreeView *view;
	bool initial;
};


typedef vector<DrawerToolInfo *> DrawerToolInfoVector;
typedef DrawerToolInfoVector *( *GetDrawerTools)();
typedef void(ILWIS::DrawerTool::*DTDoubleClickActionFunc)();
typedef void (ILWIS::DrawerTool::*DTSetCheckFunc)(void *value, HTREEITEM hti);


}