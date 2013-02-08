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

const Column & GroupableDrawer::getGroupAttribute() const
{
	return colGroup;
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

const String GroupableDrawer::getGroupValue(long iRaw) const
{
	if (fUseGroup)
		return colGroup->sValue(iRaw);
	else
		return "?";
}

const bool GroupableDrawer::fGetUseGroup() const
{
	return fUseGroup;
}

String GroupableDrawer::storeGroupable(const FileName& fnView, const String& parentSection) const
{
	ObjectInfo::WriteElement(parentSection.c_str(), "UseGroup", fnView, fUseGroup);
	if (colGroup.fValid()) {
		ObjectInfo::WriteElement(parentSection.c_str(), "GroupTable", fnView, colGroup->fnTbl.sRelativeQuoted());
		ObjectInfo::WriteElement(parentSection.c_str(), "GroupColumn", fnView, colGroup);
	}
	return parentSection;
}

void GroupableDrawer::loadGroupable(const FileName& fnView, const String& currentSection)
{
	ObjectInfo::ReadElement(currentSection.c_str(), "UseGroup", fnView, fUseGroup);
	Table tbl;
	ObjectInfo::ReadElement(currentSection.c_str(), "GroupTable", fnView, tbl);
	if (tbl.fValid()) {
		String sCol;
		ObjectInfo::ReadElement(currentSection.c_str(), "GroupColumn", fnView, sCol);
		if (sCol.length() > 0)
			colGroup = tbl->col(sCol);
		else
			fUseGroup = false;
	} else
		fUseGroup = false;
}
