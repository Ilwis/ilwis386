#pragma once

//typedef void (CCmdTarget::*NotifyRectProc)(CRect);

#include "Engine\Drawers\SimpleDrawer.h"

class ZoomableView;

ILWIS::NewDrawer *createSelectionRectangle(ILWIS::DrawerParameters *parms);

namespace ILWIS {
	class _export SelectionRectangle : public SimpleDrawer  {
	public:
		SelectionRectangle(DrawerParameters *parms);
		virtual ~SelectionRectangle();
		virtual bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		virtual void prepare(PreparationType t=ptALL,CDC *dc = 0);
		void calcWorldCoordinates(const CRect & rctZoom);
		CoordBounds getWorldCoordinates() const;
		void setColor(const Color& clr);
	protected:
		virtual HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) { return 0;}
		Coord c1,c2;
		Color clr;
	};

	//struct SelectionParameters : public DrawerParameters {
	//	CCmdTarget* cmt;
	//	NotifyRectProc np;
	//	ZoomableView* mappaneview;
	//};
}