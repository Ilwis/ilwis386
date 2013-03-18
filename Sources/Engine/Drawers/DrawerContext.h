#pragma once

#include <gl/gl.h>

namespace ILWIS { 
	class RootDrawer;
	class Palette;
	class _export DrawerContext {
	public:
		enum Mode{mNONE=0,mFORCEINIT=1, mDRAWTOWINDOW=2, mDRAWTOBITMAP=4, mUSEDOUBLEBUFFER=8, mSOFTWARERENDERER=16};
		~DrawerContext();
		DrawerContext();
		bool initOpenGL(HDC hdc, CWnd * wnd, int m = (mDRAWTOWINDOW | mUSEDOUBLEBUFFER));
		void clear();
		GLint getMaxTextureSize() const { return maxTextureSize; };
		GLint getMaxPaletteSize() const { return maxPaletteSize; };

		bool TakeContext();
		void ReleaseContext();
		void doDraw();
		void swapBuffers() const;
		void setActivePalette(const Palette * palette);
		bool isActivePalette(const Palette * palette) const;
		HDC getHDC() const { return m_hdc; }
		bool getMode() const;

	private:
		GLint maxTextureSize;
		GLint maxPaletteSize;
		CCriticalSection csOpenglContext;
		HDC m_hdc;
		HGLRC m_hrc;
		CWnd * m_wnd;
		const Palette * palette;
		bool fGLInitialized;
		bool usedoubleBuffer;
		int mode;
	};
}
