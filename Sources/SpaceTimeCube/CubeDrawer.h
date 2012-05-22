#pragma once
#include "Engine\Drawers\OpenGLText.h"
#include "TemporalDrawer.h"

ILWIS::NewDrawer *createCubeDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	class _export CubeDrawer : public ComplexDrawer, public TemporalDrawer {
	public:
		ILWIS::NewDrawer *createCubeDrawer(DrawerParameters *parms);

		CubeDrawer(DrawerParameters *parms);
		virtual ~CubeDrawer();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
	protected:
		void drawCube() const;
		void drawTicMarks() const;
		void drawLabels() const;
		void drawCoords() const;
		void drawTimes() const;
		void renderText(const Coordinate & c, const String & text) const;
		String sxMin, sxMax, syMin, syMax, stMin, stMax;
		CoordBounds cube;
		OpenGLText * font;

	};

}