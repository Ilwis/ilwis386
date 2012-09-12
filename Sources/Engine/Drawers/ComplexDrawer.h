#pragma once

#include "drawer_n.h" 

typedef map<String, ILWIS::NewDrawer *> DrawerMap;
typedef DrawerMap::iterator DrawerIter;
typedef DrawerMap::const_iterator DrawerIter_C;

class FieldIntSliderEx;
class MapPaneViewTool;
class ZoomableView;
class Feature;

namespace ILWIS {

class ZValueMaker;
class _export ComplexDrawer : public NewDrawer {
		friend class TransparencyForm;

	public:
		ComplexDrawer(DrawerParameters *context);
		enum DrawerType{dtDONTCARE=0, dtPOST=1, dtPRE=2, dtMAIN=4, dtPOLYGONLAYER=8, dtSEGMENTLAYER=16,dtPOINTLAYER=32,dtRASTERLAYER=64};
		String getType() const;
		virtual String getSubType() const;
		virtual void setSubType(const String & sSubType);
		RootDrawer *getRootDrawer();
		RootDrawer *getRootDrawer() const;
		bool draw( const CoordBounds& cb=CoordBounds()) const;
		void prepare(PreparationParameters *);
		virtual void prepareChildDrawers(PreparationParameters *);
		int getDrawerCount(int types=dtMAIN) const;
		NewDrawer * getDrawer(int index, int type=dtMAIN);
		NewDrawer *getDrawer(const String& did);
		virtual String addDrawer(NewDrawer *drw);
		virtual void setDrawer(int index, NewDrawer *drw);
		virtual int getDrawerIndex(NewDrawer *drw);
		virtual void removeDrawer(const String& did, bool dodelete=true);
		virtual void insertDrawer(int index, NewDrawer *drw);
		virtual void addPostDrawer(int order, NewDrawer *drw);
		virtual void addPreDrawer(int order, NewDrawer *drw);
		void getDrawers(vector<NewDrawer *>&) ;
		virtual void addDataSource(void *,int options=0){}
		virtual void* getDataSource() const {return 0;}
		virtual void removeDataSource(void *) {}
		virtual void clear();
		String getId() const;
		String getName() const;
		virtual void setName(const String&);
		bool isActive() const;
		void setActive(bool yesno);
		DrawMethod getDrawMethod() const;
		void setDrawMethod(DrawMethod method=drmINIT);
		virtual bool isEditable() const;
		virtual void setEditable(bool yesno);
		bool hasInfo() const;
		void setInfo(bool yesno);
		//void setActiveMode(void *v,LayerTreeView *tv);
		NewDrawer *getParentDrawer() const;
		double getTransparency() const;
		void setTransparency(double value);
		virtual int getUICode() const;
		virtual void setUICode(int c);
		//virtual HTREEITEM make3D(bool yeno, LayerTreeView  *tvm);
		ZValueMaker *getZMaker() const;
		virtual void timedEvent(UINT timerid);
		virtual String description() const { return "?";}
		virtual String iconName(const String& subtype="?") const { return "?";}
		virtual void setSpecialDrawingOptions(int option, bool add, const vector<int>& coords=vector<int>()) ;
		int getSpecialDrawingOption(int opt=sdoNone) const;
		bool isSimple() const { return false;}
		bool isDirty() const;
		void setDirty(bool yesno);
		String getInfo(const Coord& crd) const;
		//Tranquilizer getTranquilizer() { return trq; }
		long getCurrentIndex() const;
		void setCurrentIndex(long i);
		virtual void getDrawerFor(const Feature* feature,vector<NewDrawer *>& features);
		void shareVertices(vector<Coord *>& coords);
		bool inEditMode() const;
		void setEditMode(bool yesno);
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const {}
		virtual MapPaneViewTool *createTool(ZoomableView*) { return 0;}
		virtual GeneralDrawerProperties *getProperties() { return 0;}
		virtual bool isSet() const { return false; }
		Color getSelectionColor() const { return selectionColor; }
		void setSelectionColor(const Color& clr) { selectionColor = clr; }
		bool isValid() const;
		void setValid(bool yesno) ;
		virtual void select(const CoordBounds& cbSelect);
	
	protected:
		vector<NewDrawer *> drawers;
		DrawerMap postDrawers;
		DrawerMap preDrawers;
		DrawerMap drawersById;
		//Tranquilizer trq;
		long currentIndex; // for tranquilizer purposes;

		int uiCode;
		bool info;
		DrawMethod drm;
		double transparency;
		RootDrawer *rootDrawer;
		NewDrawer *parentDrawer;
		String type;
		String subType;
		String id;
		String name;
		bool active;
		bool editable;
		bool editmode;
		bool valid;
		ZValueMaker *zmaker;
		int specialOptions;
		bool dirty;
		bool selectable;
		int totalDrawerCount;
		Color selectionColor;

		
		ComplexDrawer(DrawerParameters *context, const String& ty);
		ComplexDrawer();
		virtual ~ComplexDrawer();
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		void displayOptionTransparency(CWnd *parent) ;
		void setInfoMode(void *v,LayerTreeView *tv);
		NewDrawer *loadDrawer(const FileName& fnView, const String& drawerSection);
		bool drawPreDrawers(const CoordBounds& cb) const;
		bool drawPostDrawers(const CoordBounds& cb) const;
	private:
		void init();
	} ;

}