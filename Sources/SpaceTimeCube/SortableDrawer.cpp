#include "Client\Headers\formelementspch.h"
#include "SortableDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

SortableDrawer::SortableDrawer()
: fUseSort(false)
{
}

SortableDrawer::~SortableDrawer()
{
}

void SortableDrawer::SetSortAttribute(const Column & col)
{
	fUseSort = col.fValid();
	if (fUseSort)
		colSort = col;
}

void SortableDrawer::SetNoSort()
{
	fUseSort = false;
}

const double SortableDrawer::getSortValue(Feature * f) const
{
	if (f && fUseSort)
		return colSort->rValue(f->iValue());
	else
		return 0;
}
