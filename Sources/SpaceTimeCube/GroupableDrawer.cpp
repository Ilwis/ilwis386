#include "Client\Headers\formelementspch.h"
#include "GroupableDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

GroupableDrawer::GroupableDrawer()
: fUseGroup(false)
{
}

GroupableDrawer::~GroupableDrawer()
{
}

void GroupableDrawer::SetGroupAttribute(const Column & col)
{
	fUseGroup = col.fValid();
	if (fUseGroup)
		colGroup = col;
}

void GroupableDrawer::SetNoGroup()
{
	fUseGroup = false;
}

const String GroupableDrawer::getGroupValue(Feature * f) const
{
	if (f && fUseGroup)
		return colGroup->sValue(f->iValue());
	else
		return "?";
}
