#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
//#include "Engine\Map\Feature.h"
#include "Engine\Domain\dm.h"

class MapCompositionDoc;

namespace ILWIS { 
	class RootDrawer;
	class _export DrawerContext {
	public:
		~DrawerContext();
		DrawerContext(MapCompositionDoc *);
		bool initOpenGL(CDC *dc);
		void clear();
		MapCompositionDoc * getDocument() const;
		GLint getMaxTextureSize() const { return maxTextureSize; };

		void TakeContext(bool urgent);
		void ReleaseContext();
		bool isUrgentRequestWaiting();
		void InvalidateWindow();
		bool fUrgentRequestWaiting; // intentionally public!! used in volatile* to abort texture generation in thread

	private:
		MapCompositionDoc *doc;
		GLint maxTextureSize;

		HDC m_hdc;
		HGLRC m_hrc;
		CWnd * m_wnd;
		bool glBusy;
		bool fGLInitialized;
	};
}
