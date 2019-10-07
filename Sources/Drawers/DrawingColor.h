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
		double rValByRaw(int raw, const RangeReal& range = RangeReal()) const;
		long iRawAttr(long iRaw) const;
		bool fUsesCol() const;
		const Column getCol() const;
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
	static Color clrPrimary(int iNr, int set=0);
	void setDataColumn(const Column& c);
	void setTransparentValues(const RangeReal& rr);
	Color getTresholdColor() const;
	void setTresholdColor(const Color&clr);
	RangeReal getTresholdRange() const;
	void setTresholdRange(const RangeReal& tr, bool s=false);
	RangeReal getTransparentValues() const;
	String store(const FileName& fnView, const String& section) const;
	void load(const FileName& fnView, const String& section);
	void setMultiColors(int index, bool fInitRandom = true) ;
	int multiColors() const;
	void setColorSet(int index);
	int colorSet() const;
	void color1(const Color& clr);
	void color2(const Color& clr);
	Color getColor1() const;
	Color getColor2() const;
	void copyClrRandomFrom(DrawingColor & dcOther);

private:
	void InitClrRandom();
	RangeReal getStretchRangeReal() const;
	inline void setTransparency(double v, Color& clr) const;
	inline bool setTresholdColors(double v, Color& clr) const;

	LayerDrawer *drw;
	SetDrawer *setDrawer;
	MapCompositionDoc *mcd;
    vector<Color> m_clrRandom;
	Color clr1;
	Color clr2;
	int iMultColors;
	int colorSetIndex;
	IlwisData dataValues;
	int index;
	IlwisObject::iotIlwisObjectType type;
	RangeReal transpValues;
	RangeReal tresholdValues;
	RangeReal rangeData;
	Color tresholdColor;
	bool useSingleValueForTreshold;
};
}