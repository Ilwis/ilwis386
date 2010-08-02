#pragma once

typedef void (CCmdTarget::*NotifyRectProc)(CRect);

#include "Client\Mapwindow\Drawers\Drawer_n.h"

class ZoomableView;

ILWIS::NewDrawer *createSelectionRectangle(ILWIS::DrawerParameters *parms);

namespace ILWIS {
	class _export SelectionRectangle : public ComplexDrawer  {
	public:
		SelectionRectangle(DrawerParameters *parms);
		virtual ~SelectionRectangle();
		virtual void draw(bool norecursion = false);
		virtual void prepare(PreparationType t=ptALL,CDC *dc = 0);
		void calcWorldCoordinates(const CRect & rctZoom);
		CoordBounds getWorldCoordinates() const;
	protected:
		Coord c1,c2;
	};

	struct SelectionParameters : public DrawerParameters {
		CCmdTarget* cmt;
		NotifyRectProc np;
		ZoomableView* mappaneview;
	};
}