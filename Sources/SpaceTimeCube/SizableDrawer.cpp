#include "Client\Headers\formelementspch.h"
#include "SizableDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

SizableDrawer::SizableDrawer()
: fUseSize(false)
{
}

SizableDrawer::~SizableDrawer()
{
}

void SizableDrawer::SetSizeAttribute(const Column & col)
{
	fUseSize = col.fValid();
	if (fUseSize)
		colSize = col;
}

void SizableDrawer::SetNoSize()
{
	fUseSize = false;
}

const double SizableDrawer::getSizeValue(Feature * f) const
{
	if (f && fUseSize)
		return colSize->rValue(f->iValue()) - sizeStretch->rLo();
	else
		return 0;
}
