#pragma once

#include "TemporalDrawer.h"

ILWIS::NewDrawer *createPostTimeOffsetDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	class _export PostTimeOffsetDrawer : public ComplexDrawer, public TemporalDrawer {
	public:
		ILWIS::NewDrawer *createPostTimeOffsetDrawer(DrawerParameters *parms);

		PostTimeOffsetDrawer(DrawerParameters *parms);
		virtual ~PostTimeOffsetDrawer();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
	};
}