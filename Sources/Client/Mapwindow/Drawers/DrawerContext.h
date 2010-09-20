#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
//#include "Engine\Map\Feature.h"
#include "Engine\Domain\dm.h"

class MapCompositionDoc;

namespace ILWIS {

	class _export DrawerContext {
	public:
		DrawerContext(MapCompositionDoc *);
		~DrawerContext();
		MapCompositionDoc * getDocument() const;
		bool initOpenGL(CDC *dc);

	private:
		void clear();
			//RootDrawer *rootDrawer;
		HGLRC mhRC;
		MapCompositionDoc *doc;
	};
}
