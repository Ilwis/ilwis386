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
		GLint getMaxPaletteSize() const { return maxPaletteSize; };

		void TakeContext();
		void ReleaseContext();
		void InvalidateWindow();

	private:
		MapCompositionDoc *doc;
		GLint maxTextureSize;
		GLint maxPaletteSize;
		CCriticalSection csOpenglContext;
		HDC m_hdc;
		HGLRC m_hrc;
		CWnd * m_wnd;
		bool fGLInitialized;
	};
}
