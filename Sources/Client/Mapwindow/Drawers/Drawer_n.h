#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Domain\dm.h"

class LayerTreeView;
class LayerTreeItem;
class SetChecks;
class DisplayOptionTreeItem;

namespace ILWIS {
	class DrawerContext;
	struct PreparationParameters;


	class _export NewDrawer {
	public:
		enum DrawMethod	{ drmNOTSET, drmINIT, drmRPR, drmSINGLE, drmMULTIPLE, drmIMAGE,	drmCOLOR, drmBOOL, drmBYATTRIBUTE, drmARROW, drmGRAPH };
		enum DrawColors { drcLIGHT, drcNORMAL, drcDARK, drcGREY };
		enum PreparationType{ptALL=1,ptRENDER=2,ptGEOMETRY=4,ptINITOPENGL=8,ptUI=16};
		enum DataSourceMergeOptions{dsmEXTENDCB=1};
		enum UICode{ucALL=1, ucBOUNDARIES=2};
		virtual void draw(bool norecursion = false) = 0;
		virtual void prepare(PreparationParameters *) =0;
		virtual void prepareChildDrawers(PreparationParameters *) = 0;
		virtual String getType() const =0;
		virtual void addDataSource(void *, int options=0) = 0;
		virtual void removeDataSource(void *) = 0;
		virtual DrawerContext *getDrawerContext() = 0;
		virtual NewDrawer *getDrawer(const String& did) = 0;
		virtual String getName() const = 0;
		virtual void setName(const String&) = 0;
		virtual bool isActive() const = 0;
		virtual void setActive(bool yesno)=0;
		virtual int getDrawerCount() const = 0;
		virtual NewDrawer * getDrawer(int index) = 0;
		virtual String addDrawer(NewDrawer *drw) = 0;
		virtual void addPostDrawer(int order, NewDrawer *drw) = 0;
		virtual void addPreDrawer(int order, NewDrawer *drw) = 0;
		virtual DrawMethod getDrawMethod() const = 0;
		virtual void setDrawMethod(DrawMethod method=drmINIT) = 0;
		virtual bool isEditable() const = 0;
		virtual void setEditable(bool yesno) = 0;
		virtual bool hasInfo() const = 0;
		virtual void setInfo(bool yesno) = 0;
		virtual NewDrawer *getParentDrawer() const = 0;
		virtual HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent)=0;
		virtual double getTransparency() const = 0;
		virtual void setTransparency(double value) = 0;
		virtual int getUICode() const = 0;
		virtual void setUICode(int c) = 0;
		virtual NewDrawer *getRootDrawer() const = 0;
		virtual String getId() const = 0;
	};

	struct DrawerParameters {
		DrawerParameters(DrawerContext *c, NewDrawer *p) : context(c), parent(p) {}
		DrawerContext *context;
		NewDrawer *parent;
	};

	typedef void(ILWIS::NewDrawer::*DisplayOptionItemFunc)(CWnd *parent);
	typedef void (ILWIS::NewDrawer::*SetCheckFunc)(void *value, LayerTreeView *v);

	class _export ComplexDrawer : public NewDrawer {
	public:
		String getType() const;
		DrawerContext *getDrawerContext();
		void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
		void prepareChildDrawers(PreparationParameters *);
		int getDrawerCount() const;
		NewDrawer * getDrawer(int index);
		NewDrawer *getDrawer(const String& did);
		virtual String addDrawer(NewDrawer *drw);
		virtual void removeDrawer(const String& did);
		virtual void addPostDrawer(int order, NewDrawer *drw);
		virtual void addPreDrawer(int order, NewDrawer *drw);
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

		CCriticalSection cs;
	protected:
		vector<NewDrawer *> drawers;
		map<String, NewDrawer *> postDrawers;
		map<String, NewDrawer *> preDrawers;
		map<String, NewDrawer *> drawersById;

		DrawerContext *drawcontext;
		NewDrawer *parentDrawer;
		String type;
		String id;
		String name;
		bool active;
		bool editable;
		int uiCode;
		bool info;
		DrawMethod drm;
		double transparency;

		ComplexDrawer(DrawerParameters *context, const String& ty);
		ComplexDrawer();
		virtual ~ComplexDrawer();
		HTREEITEM InsertItem(const String& name,const String& icon, DisplayOptionTreeItem *item=0, int checkstatus = -1);
		HTREEITEM InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon);
		/*HTREEITEM InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon,
			                 int checkStatus=-1,DisplayOptionItemFunc fun=0,SetChecks *checks=0);
		HTREEITEM InsertItem(LayerTreeView  *tv,HTREEITEM parent, const String& name,const String& icon,
			                 bool checkStatus, SetCheckFunc fun,DisplayOptionItemFunc fun2=0, NewDrawer *altHandler=0);*/
		HTREEITEM findTreeItemByName(LayerTreeView  *tv, HTREEITEM parent, const String& name) const;
		void setInfoMode(void *v,LayerTreeView *tv);
	private:
		void init();
	} ;

	struct PreparationParameters {
		PreparationParameters(const PreparationParameters* parms) :
			type(parms->type),dc(parms->dc) {}
		PreparationParameters(int t=1,CDC *_dc = 0) : type(t),dc(_dc) {} 
		int type;
		CDC *dc;
		CoordSystem csy;
		map<String,String> typeMapping;
	};

}

typedef ILWIS::NewDrawer* (*DrawerCreate)(ILWIS::DrawerParameters *parms);
typedef HTREEITEM (ILWIS::NewDrawer::*Configure)(LayerTreeView  *tv, HTREEITEM parent);

struct DrawerInfo {
	DrawerInfo(const String& n, const String& _subtype, DrawerCreate func) : name(n), subtype(_subtype), createFunc(func) {}
	String name;
	String subtype;
	DrawerCreate createFunc;
};

struct ConfigureMethod {
	ConfigureMethod(ILWIS::NewDrawer *_drw, Configure _conf) : drawer(_drw),configure(_conf) {}
	ILWIS::NewDrawer *drawer;
	Configure configure;

};

typedef vector<DrawerInfo *> DrawerInfoVector;
typedef DrawerInfoVector *( *GetDrawers)();

