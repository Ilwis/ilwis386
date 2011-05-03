#pragma once

class MapCompositionDoc;

namespace ILWIS {

	class SpatialDataDrawer;
	class LayerDrawer;

	class IlwisData{
	public:
		void setBaseMap(const BaseMap& bm);
		void setColumn(const Column& col);
		DomainValueRangeStruct dvrs() const;
		Domain dm() const;
		double rValByRaw(int raw) const;
	private:
		BaseMap bmap;
		Column col;
	};
class _export DrawingColor {
public:
	DrawingColor(LayerDrawer *dr, int _index=0);
	Color clrVal(double rVal) const;
	Color clrRaw(long iRaw, NewDrawer::DrawMethod drm) const;
	void clrVal(const double * buf, long * bufOut, long iLen) const;
	void clrRaw(const long * buf, long * bufOut, long iLen, NewDrawer::DrawMethod drm) const;
	Color clrRandom(int iRaw) const;
	Color clrPrimary(int iNr) const;
	void setDataColumn(const Column& c);

private:
	void InitClrRandom();

	LayerDrawer *drw;
	MapCompositionDoc *mcd;
    vector<Color> m_clrRandom;
	Color clr1;
	Color clr2;
	int iMultColors;
	double gamma;
	IlwisData dataValues;
	int index;
	IlwisObject::iotIlwisObjectType type;
};
}