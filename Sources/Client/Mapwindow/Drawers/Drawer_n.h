#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Domain\dm.h"

namespace ILWIS {
	class DrawerContext;

	struct DrawerParameters {
		DrawerContext *context;
	};

	class _export NewDrawer {
	public:
		enum PreparationType{ptALL=1,ptRENDER=2,ptGEOMETRY=4,ptINITOPENGL=8};
		virtual void draw(bool norecursion = false) = 0;
		virtual void prepare(PreparationType t=ptALL,CDC *dc = 0) =0;
		virtual String getType() const =0;
		virtual void setDataSource(void *) = 0;
		virtual DrawerContext *getDrawerContext() = 0;
		virtual String getId() const = 0;
	};

	class _export AbstractDrawer : public NewDrawer {
	public:
		AbstractDrawer() {};
		String getType() const;
		DrawerContext *getDrawerContext();
		void draw(bool norecursion = false);
		void prepare(PreparationType t=ptALL,CDC *dc = 0);
		virtual String addDrawer(NewDrawer *drw);
		virtual void removeDrawer(const String& did);
		void setDataSource(void *){}
		void clear();
		virtual String getId() const;
	protected:
		AbstractDrawer(DrawerParameters *context, const String& ty);
		~AbstractDrawer();
		String type;
		vector<NewDrawer *> drawers;
		DrawerContext *drawcontext;
		String id;
	} ;

}

typedef ILWIS::NewDrawer* (*DrawerCreate)(ILWIS::DrawerParameters *parms);

struct DrawerInfo {
	DrawerInfo(const String& n, DrawerCreate func) : name(n), createFunc(func) {}
	String name;
	DrawerCreate createFunc;
};

typedef vector<DrawerInfo *> DrawerInfoVector;
typedef DrawerInfoVector *( *GetDrawers)();

