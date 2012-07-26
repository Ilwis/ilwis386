#include "Client\Headers\formelementspch.h"
#include "SortableDrawer.h"
#include "Engine\Map\Feature.h"

using namespace ILWIS;

SortableDrawer::SortableDrawer()
: fUseSort(false)
, fSortValues(false)
{
}

SortableDrawer::~SortableDrawer()
{
}

void SortableDrawer::SetSortAttribute(const Column & col)
{
	fUseSort = col.fValid();
	if (fUseSort) {
		colSort = col;
		fSortValues = col->fValues();
	}
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

const String SortableDrawer::getSortString(Feature * f) const
{
	if (f && fUseSort)
		return colSort->sValue(f->iValue());
	else
		return "?";
}

String SortableDrawer::storeSortable(const FileName& fnView, const String& parentSection) const
{
	ObjectInfo::WriteElement(parentSection.c_str(), "UseSort", fnView, fUseSort);
	ObjectInfo::WriteElement(parentSection.c_str(), "SortColumnIsValue", fnView, fSortValues);
	if (colSort.fValid()) {
		ObjectInfo::WriteElement(parentSection.c_str(), "SortTable", fnView, colSort->fnTbl.sRelativeQuoted());
		ObjectInfo::WriteElement(parentSection.c_str(), "SortColumn", fnView, colSort);
	}
	return parentSection;
}

void SortableDrawer::loadSortable(const FileName& fnView, const String& currentSection)
{
	ObjectInfo::ReadElement(currentSection.c_str(), "UseSort", fnView, fUseSort);
	ObjectInfo::ReadElement(currentSection.c_str(), "SortColumnIsValue", fnView, fSortValues);
	Table tbl;
	ObjectInfo::ReadElement(currentSection.c_str(), "SortTable", fnView, tbl);
	if (tbl.fValid()) {
		String sCol;
		ObjectInfo::ReadElement(currentSection.c_str(), "SortColumn", fnView, sCol);
		if (sCol.length() > 0)
			colSort = tbl->col(sCol);
		else
			fUseSort = false;
	} else
		fUseSort = false;
}
