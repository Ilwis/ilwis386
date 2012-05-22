#pragma once

#include "Engine\Drawers\RootDrawer.h"
#include "TemporalDrawer.h"

ILWIS::NewDrawer *createPreTimeOffsetDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	class _export PreTimeOffsetDrawer : public ComplexDrawer, public TemporalDrawer {
	public:
		ILWIS::NewDrawer *createPreTimeOffsetDrawer(DrawerParameters *parms);

		PreTimeOffsetDrawer(DrawerParameters *parms);
		virtual ~PreTimeOffsetDrawer();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		void SetTime(double time);
	protected:
		double timeOffset;
		CoordBounds cube;
	};

}