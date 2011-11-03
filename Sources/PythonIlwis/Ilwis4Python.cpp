// Python.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include "pcolumn.h"
#include "pboundingbox.h"
#include "pcoordinate.h"
#include "platlon.h"
#include "PCoordinateSystem.h"
#include "pcoverage.h"
#include "pcoveragecollection.h"
#include "pcoveragelist.h"
#include "pdatatype.h"
#include "pellipsoid.h"
#include "pdatum.h"
#include "pfeature.h"
#include "pfeaturecoverage.h"
#include "pgeoreference.h"
#include "pgridcoverage.h"
#include "rootobject.h"
#include "pthematicData.h"
#include "ppixel.h"
#include "PSRObject.h"
#include "pprojection.h"
#include "PCRSBoundingBox.h"
#include "pstringdata.h"
#include "ptable.h"
#include "pthematicdata.h"
#include "pvaluerange.h"


int _tmain(int argc, _TCHAR* argv[])
{
	PGridCoverage cover("d:\\Data\\ILWIS\\delta_kenya2_11.mpr");
	PCoordinateSystem pcc = cover.getCoordinateSystem();
	string name = pcc.getName();
	PGrid grid;
	PExtent ext(10);
	cover.getGrid(ext,grid);
	double v = grid.getd(37);

	PFeatureCoverage fcover("d:\\Data\\ILWIS\\rainfall.mpp");
	long n = fcover.getNumberOfFeatures();
	return 0;
}

