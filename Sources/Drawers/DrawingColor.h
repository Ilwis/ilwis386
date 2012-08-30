#pragma once

class MapCompositionDoc;

namespace ILWIS {

	class SpatialDataDrawer;
	class LayerDrawer;
	class SetDrawer;

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
	DrawingColor(ComplexDrawer *dr, int _index=0);
	Color clrVal(double rVal) const;
	Color clrRaw(long iRaw, NewDrawer::DrawMethod drm) const;
	void clrVal(const double * buf, long * bufOut, long iLen) const;
	void clrRaw(const long * buf, long * bufOut, long iLen, NewDrawer::DrawMethod drm) const;
	Color clrRandom(int iRaw) const;
	static Color clrPrimary(int iNr);
	void setDataColumn(const Column& c);
	void setTransparentValues(const RangeReal& rr);
	Color getTresholdColor() const;
	void setTresholdColor(const Color&clr);
	RangeReal getTresholdRange() const;
	void setTresholdRange(const RangeReal& tr);
	RangeReal getTransparentValues() const;
	String store(const FileName& fnView, const String& parenSection) const;
	void load(const FileName& fnView, const String& parenSection);

private:
	void InitClrRandom();
	RangeReal getStretchRangeReal() const;
	inline void setTransparency(double v, Color& clr) const;
	inline void setTresholdColors(double v, Color& clr) const;

	LayerDrawer *drw;
	SetDrawer *setDrawer;
	MapCompositionDoc *mcd;
    vector<Color> m_clrRandom;
	Color clr1;
	Color clr2;
	int iMultColors;
	double gamma;
	IlwisData dataValues;
	int index;
	IlwisObject::iotIlwisObjectType type;
	RangeReal transpValues;
	RangeReal tresholdValues;
	Color tresholdColor;
};
}