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
	sizeStretch = &(properties->stretchRange);
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

void SizableDrawer::SetSize2Attribute(const Column & col)
{
	if (col.fValid())
		colSize2 = col;
}

const Column & SizableDrawer::getSizeAttribute() const
{
	return colSize;
}

const Column & SizableDrawer::getSize2Attribute() const
{
	return colSize2;
}

const bool SizableDrawer::fGetUseSize() const
{
	return properties->scaleMode != PointProperties::sNONE;
}

void SizableDrawer::SetNoSize()
{
	properties->scaleMode = PointProperties::sNONE;
}

const double SizableDrawer::getSizeValue(Feature * f) const
{
	if (properties->scaleMode != PointProperties::sNONE) {
		double rLo = sizeStretch->rLo();
		double rHi = sizeStretch->rHi();
		if (properties->scaleMode == PointProperties::sLOGARITHMIC) {
			rLo = (rLo < 1e-10) ? 0 : log(rLo);
			rHi = (rHi < 1e-10) ? 0 : log(rHi);
		}
		if (rHi <= rLo)
			rHi = rLo + 1;
		double v = (colSize->rValue(f->iValue()) - rLo) / (rHi - rLo);
		if (v < 0)
			v = 0;
		else if (v > 1)
			v = 1;
		if (properties->radiusArea == PointProperties::sAREA)
			v = sqrt(v);
		return properties->exaggeration * v;
	} else
		return properties->exaggeration;
}

const double SizableDrawer::getSize2Value(Feature * f) const
{
	return colSize2->rValue(f->iValue());
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
