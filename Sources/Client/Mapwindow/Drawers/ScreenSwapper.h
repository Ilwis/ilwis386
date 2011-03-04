#pragma once

namespace ILWIS {
	class ScreenSwapper {
	public:
		ScreenSwapper();
		~ScreenSwapper();
		void swapBufferToScreen(const CRect& rct) const;
		void saveScreenBuffer(const CRect& rct);
		void bitmapBufferRedraw(MapCompositionDoc *mdoc);
		void setBitmapRedraw(bool yesno);
		void bitmapBufferRedraw() const;
	private:
		float * swapBitmap;
		bool useBitmapRedraw;


	};
}