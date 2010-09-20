#pragma once

class MapCompositionDoc;

namespace ILWIS {

	class AbstractMapDrawer;
	class SetDrawer;

class _export DrawingColor {
public:
	DrawingColor(SetDrawer *dr);
	Color clrVal(double rVal) const;
	Color clrRaw(long iRaw, NewDrawer::DrawMethod drm) const;
	Color clrRandom(int iRaw) const;
	Color clrPrimary(int iNr) const;

private:
	void InitClrRandom();

	SetDrawer *drw;
	MapCompositionDoc *mcd;
    vector<Color> m_clrRandom;
	Color clr1;
	Color clr2;
	int iMultColors;
	double gamma;
	BaseMap bmap;

};
}