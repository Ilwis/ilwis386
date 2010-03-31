#pragma once

#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Map\Feature.h"
#include "Engine\Domain\dm.h"

namespace ILWIS {
	class _export DrawerContext {
	public:
		CoordBounds getCoordBounds() const { return cb; }
		CoordSystem getCoordinateSystem() const { return cs;}
		RowCol getViewPort() const { return pixArea; }
		void setCoordinateSystem(CoordSystem _cs) { cs = _cs; }
		void setCoordBounds(CoordBounds _cb) { cb = _cb; }
		void setPixArea(RowCol rc) { pixArea = rc; }

	private:
		CoordBounds cb;
		CoordSystem cs;
		RowCol pixArea;
		//Georef grf;
	};

	class _export NewDrawer {
	public:
		enum PreparationType{ptALL,ptRENDER,ptGEOMETRY};
		virtual void draw() = 0;
		virtual void prepare(PreparationType t=ptALL) =0;
		virtual String getType() const =0;
		virtual void setDataSource(void *) = 0;
	};

	class _export AbstractDrawer : public NewDrawer {
	public:
		String getType() const;
	protected:
		AbstractDrawer(DrawerContext *context, const String& ty);
		~AbstractDrawer();
		String type;
		vector<NewDrawer *> drawers;
		DrawerContext *drawcontext;
	} ;

}

typedef ILWIS::NewDrawer* (*DrawerCreate)(ILWIS::DrawerContext *c);

struct DrawerInfo {
	DrawerInfo(const String& n, DrawerCreate func) : name(n), createFunc(func) {}
	String name;
	DrawerCreate createFunc;
};

typedef vector<DrawerInfo *> DrawerInfoVector;
typedef DrawerInfoVector *( *GetDrawers)();

