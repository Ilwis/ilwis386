#pragma once

namespace ILWIS {

	class AbstractMapDrawer;

class DrawingColor {
public:
	DrawingColor(AbstractMapDrawer *dr);
	Color clrVal(double rVal) const;
	Color clrRaw(long iRaw) const;
	Color clrRandom(int iRaw) const;
	Color clrPrimary(int iNr) const;

private:
	void InitClrRandom();

	AbstractMapDrawer *drw;
	MapCompositionDoc *mcd;
    vector<Color> m_clrRandom;
	Color clr1;
	Color clr2;
	int iMultColors;
	double gamma;

};
}