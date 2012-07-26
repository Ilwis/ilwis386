#include "Client\Headers\formelementspch.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "SizableDrawer.h"
#include "Engine\Map\Feature.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Headers\Hs\Drwforms.hs"
#include "Drawers\TextureHeap.h"

using namespace ILWIS;

SizableDrawer::SizableDrawer()
{
	properties = new PointProperties();
	properties->scaleMode = PointProperties::sNONE;
	properties->exaggeration = 1.0;
}

SizableDrawer::~SizableDrawer()
{
	delete properties;
}

void SizableDrawer::SetSizeStretch(RangeReal * _sizeStretch)
{
	sizeStretch = _sizeStretch;
}

void SizableDrawer::SetSizeAttribute(const Column & col)
{
	if (col.fValid()) {
		colSize = col;
		properties->stretchColumn = col->sName();
		if (!properties->stretchRange.fValid())
			properties->stretchRange = col->rrMinMax();
		if (properties->scaleMode == PointProperties::sNONE)
			properties->scaleMode = PointProperties::sLINEAR;
	} else
		properties->scaleMode = PointProperties::sNONE;
}

void SizableDrawer::SetNoSize()
{
	properties->scaleMode = PointProperties::sNONE;
}

const double SizableDrawer::getSizeValue(Feature * f) const
{
	if ( properties->scaleMode == PointProperties::sLINEAR)
		return properties->exaggeration * (colSize->rValue(f->iValue()) - sizeStretch->rLo()) / sizeStretch->rWidth();
	else if (properties->scaleMode == PointProperties::sLOGARITHMIC)
		return properties->exaggeration * log(1.0 + (colSize->rValue(f->iValue()) - sizeStretch->rLo()) / sizeStretch->rWidth());
	else
		return properties->exaggeration;
}

String SizableDrawer::storeSizable(const FileName& fnView, const String& parentSection) const
{
	properties->store(fnView, parentSection);
	if (colSize.fValid()) {
		ObjectInfo::WriteElement(parentSection.c_str(), "SizeTable", fnView, colSize->fnTbl.sRelativeQuoted());
		ObjectInfo::WriteElement(parentSection.c_str(), "SizeColumn", fnView, colSize);
	}
	return parentSection;
}

void SizableDrawer::loadSizable(const FileName& fnView, const String& currentSection)
{
	properties->load(fnView, currentSection);
	Table tbl;
	ObjectInfo::ReadElement(currentSection.c_str(), "SizeTable", fnView, tbl);
	if (tbl.fValid()) {
		String sCol;
		ObjectInfo::ReadElement(currentSection.c_str(), "SizeColumn", fnView, sCol);
		if (sCol.length() > 0)
			colSize = tbl->col(sCol);
		else
			SetNoSize();
	} else
		SetNoSize();
}
