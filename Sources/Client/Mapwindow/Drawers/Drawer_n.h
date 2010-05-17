#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Domain\dm.h"

class LayerTreeView;

namespace ILWIS {
	class DrawerContext;
	struct PreparationParameters;

	struct DrawerParameters {
		DrawerParameters(DrawerContext *c) : context(c) {}
		DrawerContext *context;
	};


	class _export NewDrawer {
	public:
		enum DrawMethod	{ drmRPR, drmSINGLE, drmMULTIPLE, drmIMAGE,	drmCOLOR, drmBOOL, drmBYATTRIBUTE, drmARROW, drmGRAPH };
		enum DrawColors { drcLIGHT, drcNORMAL, drcDARK, drcGREY };
		enum PreparationType{ptALL=1,ptRENDER=2,ptGEOMETRY=4,ptINITOPENGL=8};
		virtual void draw(bool norecursion = false) = 0;
		virtual void prepare(PreparationParameters *) =0;
		virtual String getType() const =0;
		virtual void setDataSource(void *) = 0;
		virtual DrawerContext *getDrawerContext() = 0;
		virtual String getId() const = 0;
		virtual NewDrawer *getDrawer(const String& did) = 0;
		virtual String getName() const = 0;
		virtual bool isActive() const = 0;
		virtual void setActive(bool yesno)=0;
		virtual int getDrawerCount() const = 0;
		virtual NewDrawer * getDrawer(int index) = 0;
		virtual DrawMethod getDrawMethod() const = 0;
		virtual void setDrawMethod(DrawMethod method) = 0;
		virtual bool isEditable() const = 0;
		virtual void setEditable(bool yesno) = 0;
		virtual bool hasInfo() const = 0;
		virtual void setInfo(bool yesno) = 0;
		virtual HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent)=0;

	};

	class _export AbstractDrawer : public NewDrawer {
	public:
		AbstractDrawer() {};
		String getType() const;
		DrawerContext *getDrawerContext();
		void draw(bool norecursion = false);
		void prepare(PreparationParameters *);
		virtual String addDrawer(NewDrawer *drw);
		virtual void removeDrawer(const String& did);
		void setDataSource(void *){}
		void clear();
		String getId() const;
		String getName() const;
		NewDrawer *getDrawer(const String& did);
		int getDrawerCount() const;
		NewDrawer * getDrawer(int index);
		bool isActive() const;
		void setActive(bool yesno);
		DrawMethod getDrawMethod() const;
		void setDrawMethod(DrawMethod method);
		virtual bool isEditable() const;
		virtual void setEditable(bool yesno);
		bool hasInfo() const;
		void setInfo(bool yesno);
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);

		CCriticalSection cs;
	protected:
		vector<NewDrawer *> drawers;
		map<String, NewDrawer *> drawersById;
		DrawerContext *drawcontext;
		String type;
		String id;
		String name;
		bool active;
		bool editable;
		bool info;
		DrawMethod drm;

		AbstractDrawer(DrawerParameters *context, const String& ty);
		~AbstractDrawer();
	} ;

	struct PreparationParameters {
		PreparationParameters(int t=1,CDC *_dc = 0,NewDrawer *pp=0) : type(t),dc(_dc),parentDrawer(pp) {} 
		int type;
		CDC *dc;
		NewDrawer *parentDrawer;
	};

}

typedef ILWIS::NewDrawer* (*DrawerCreate)(ILWIS::DrawerParameters *parms);
typedef void(*DisplayOptionItemFunc)(ILWIS::NewDrawer *drw, CWnd *parent);
typedef void (*SetCheckFunc)(ILWIS::NewDrawer *, void *value);

struct DrawerInfo {
	DrawerInfo(const String& n, DrawerCreate func) : name(n), createFunc(func) {}
	String name;
	DrawerCreate createFunc;
};

typedef vector<DrawerInfo *> DrawerInfoVector;
typedef DrawerInfoVector *( *GetDrawers)();

