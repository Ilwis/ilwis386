#include "Client\Headers\formelementspch.h"
#include "SizableDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

SizableDrawer::SizableDrawer()
{
}

SizableDrawer::~SizableDrawer()
{
}

void SizableDrawer::SetSizeAttribute(const Column & col)
{
	fSizeAttribute = col.fValid();
	if (fSizeAttribute)
		colSize = col;
}

void SizableDrawer::SetSelfSize()
{
	fSizeAttribute = false;
}

const double SizableDrawer::getSizeValue(Feature * f) const
{
	if (f) {
		if (fSizeAttribute)
			return colSize->rValue(f->iValue()) - sizeStretch->rLo();
		else
			return f->rValue() - sizeStretch->rLo();
	} else
		return 0;
}
