#pragma once

#include <gl/gl.h>

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
		void InvalidateWindow();
		bool fUrgentRequestWaiting; // intentionally public!! used in volatile* to abort texture generation in thread

	private:
		MapCompositionDoc *doc;
		GLint maxTextureSize;
		CCriticalSection csOpenglContext;
		HDC m_hdc;
		HGLRC m_hrc;
		CWnd * m_wnd;
		bool fGLInitialized;
	};
}
