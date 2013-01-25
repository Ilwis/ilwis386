#pragma once

namespace ILWIS {
	class ScreenSwapper {
	public:
		ScreenSwapper();
		~ScreenSwapper();
		void setBitmapRedraw(bool yesno);
		void bitmapBufferRedraw(MapCompositionDoc *mdoc);
	private:
		void swapBufferToScreen(const CRect& rct);
		void saveScreenBuffer(const CRect& rct);
		float * swapBitmap;
		bool useBitmapRedraw;
		CCriticalSection csBitmap;

	};
}