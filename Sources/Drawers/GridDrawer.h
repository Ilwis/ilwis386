#pragma once


ILWIS::NewDrawer *createGridDrawer(ILWIS::DrawerParameters *parms);
ILWIS::NewDrawer *createGridLine(ILWIS::DrawerParameters *parms);

class FieldColor;
enum LineDspType;

namespace ILWIS{

	class GridDrawer : public ComplexDrawer {
		friend class GridForm;
		friend class GridLine;
		friend class GridLineStyleForm;
		friend class Grid3DOptions;

	public:
		enum Mode{mGRID=1, mPLANE=2, mMARKERS=4,mAXIS=8,mVERTICALS=16};
		ILWIS::NewDrawer *createGridDrawer(DrawerParameters *parms);

		GridDrawer(DrawerParameters *parms);
		virtual ~GridDrawer();
		HTREEITEM  configure(LayerTreeView  *tv, HTREEITEM parent);
		bool draw(bool norecursion = false, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		bool is3D() const { return threeD; }

	protected:
		void AddGridLine(Coord c1, Coord c2);
		void displayOptionSetLineStyle(CWnd *parent);
		void gridOptions(CWnd *parent);
		void displayOptionGrid3D(CWnd *parent);
		void gridActive(void *value, LayerTreeView *v);
		void grid3D(void *v, LayerTreeView *tv);
		void prepareGrid( double rDist, const Coord& cMax, const Coord& cMin );
		void prepareVerticals( double rDist,const Coord& cMax, const Coord& cMin);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		bool drawPlane(bool norecursion, const CoordBounds& cbArea) const;
		void resizeQuadsVector(int planes);
		void prepareVAxis(double rDist,const Coord& cMax, const Coord& cMin);
		void preparePlanes(double rDist, const Coord& cMax, const Coord& cMin );

		double          rDist;
		Color           color;
		LineDspType		linestyle;
		double			linethickness;
		bool			threeD;
		double			zdist;
		double			maxz;
		int				mode;
		Color			planeColor;
		vector< Coord * >   planeQuads;
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
	};

	class GridLineStyleForm: public DisplayOptionsForm
	{
	public:
		GridLineStyleForm(CWnd *par, GridDrawer *gdr);
		void apply();
	private:
		FieldReal *fi;
		FieldLineType *flt;
		FieldColor *fc;
	};

	class Grid3DOptions : public DisplayOptionsForm {
	public:
		Grid3DOptions(CWnd *par, GridDrawer *gdr);
	private:
		void apply();
		FieldReal *frDistance;
		CheckBox *cbgrid;
		CheckBox *cbplane; 
		CheckBox *cbmarker;
		CheckBox *cbaxis, *cbverticals;
		FieldGroup *fg;
		bool hasplane, hasgrid, hasmarker, hasverticals, hasaxis;



	};
}