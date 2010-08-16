#pragma once

#include "drawer_n.h" 

typedef map<String, ILWIS::NewDrawer *> DrawerMap;
typedef DrawerMap::iterator DrawerIter;
typedef DrawerMap::const_iterator DrawerIter_C;

namespace ILWIS {

class _export ComplexDrawer : public NewDrawer {
	public:
		String getType() const;
		DrawerContext *getDrawerContext();
		DrawerContext *getDrawerContext() const;
		bool draw(bool norecursion = false, const CoordBounds& cb=CoordBounds()) const;
		void prepare(PreparationParameters *);
		void prepareChildDrawers(PreparationParameters *);
		int getDrawerCount() const;
		NewDrawer * getDrawer(int index);
		NewDrawer *getDrawer(const String& did);
		virtual String addDrawer(NewDrawer *drw);
		virtual void removeDrawer(const String& did);
		virtual void addPostDrawer(int order, NewDrawer *drw);
		virtual void addPreDrawer(int order, NewDrawer *drw);
		void getDrawers(vector<NewDrawer *>&) ;
		virtual void addDataSource(void *,int options=0){}
		virtual void removeDataSource(void *) {}
		void clear();
		String getId() const;
		String getName() const;
		virtual void setName(const String&);
		bool isActive() const;
		void setActive(bool yesno);
		DrawMethod getDrawMethod() const;
		void setDrawMethod(DrawMethod method=drmINIT);
		virtual bool isEditable() const;
		virtual void setEditable(bool yesno);
		bool hasInfo() const;
		void setInfo(bool yesno);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		NewDrawer *getParentDrawer() const;
		double getTransparency() const;
		void setTransparency(double value);
		virtual int getUICode() const;
		virtual void setUICode(int c);
		virtual NewDrawer *getRootDrawer() const;
		virtual HTREEITEM set3D(bool yeno, LayerTreeView  *tvm, HTREEITEM parent=0,SetCheckFunc f=0);
		bool is3D() const;

		CCriticalSection cs;
	protected:
		vector<NewDrawer *> drawers;
		DrawerMap postDrawers;
		DrawerMap preDrawers;
		DrawerMap drawersById;

		int uiCode;
		bool info;
		DrawMethod drm;
		double transparency;
		DrawerContext *drawcontext;
		NewDrawer *parentDrawer;
		String type;
		String id;
		String name;
		bool active;
		bool editable;
		
		ComplexDrawer(DrawerParameters *context, const String& ty);
		ComplexDrawer();
		virtual ~ComplexDrawer();
		HTREEITEM InsertItem(const String& name,const String& icon, DisplayOptionTreeItem *item=0, int checkstatus = -1);
		HTREEITEM InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon);
		HTREEITEM findTreeItemByName(LayerTreeView  *tv, HTREEITEM parent, const String& name) const;
		void setInfoMode(void *v,LayerTreeView *tv);
	private:
		void init();
		bool threeD;
	} ;

	class _export DisplayOptionsForm : public FormBaseDialog {
	public:
		DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title);
		afx_msg virtual void OnCancel();
		int exec();
		virtual void apply();
	protected:
		void updateMapView();
		ComplexDrawer *drw;
		LayerTreeView *view;
	};
}