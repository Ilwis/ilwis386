#pragma once


ILWIS::NewDrawer *createGridDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createGridLine(ILWIS::DrawerParameters *parms);

class FieldColor;
enum LineDspType;

namespace ILWIS{

	class GridDrawer : public ComplexDrawer {
		friend class GridForm;
		friend class GridLine;

	public:
		ILWIS::NewDrawer *createGridDrawer(DrawerParameters *parms);

		GridDrawer(DrawerParameters *parms);
		virtual ~GridDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);

	protected:
		void AddGridLine(Coord c1, Coord c2);
		//void DrawCurvedLine(Coord c1, Coord c2);
		void gridOptions(CWnd *parent);
		void gridActive(void *value, LayerTreeView *v);

		double          rDist;
		Color           color;
		LineDspType		ldt;
	};

	class GridLine: public LineDrawer {
	public:
		GridLine(DrawerParameters *parms);
		virtual ~GridLine();
		//HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent) { return 0};
		bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		void addDataSource(void *c, int options = 0);
	private:
	};

	class GridForm: public DisplayOptionsForm
	{
	public:
		GridForm(CWnd *par, GridDrawer *gdr);
		void apply();
	private:
		FieldReal *fr;
		FieldColor *fc;
		GridDrawer *gd;
		int transparency;
		FieldIntSliderEx *slider;
	};
}