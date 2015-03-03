#pragma once

#include "drawer_n.h" 

class LayerTreeView;
class LayerTreeItem;
class SetChecks;
class Feature;

namespace ILWIS {
	class DrawerContext;
	struct PreparationParameters;

	class _export SimpleDrawer : public NewDrawer {
	public:
		String getType() const;
		virtual String getSubType() const;
		virtual void setSubType(const String & sSubType);
		bool draw(const DrawLoop drawLoop, const CoordBounds& cb=CoordBounds()) const = 0;
		void prepare(PreparationParameters *);
		virtual void addDataSource(void *,int options=0) {};
		virtual void removeDataSource(void *){}
		virtual void* getDataSource() const {return 0;}
		String getId() const;
		String getName() const;
		virtual void setName(const String&);
		bool isActive() const {return fActive; }
		void setActive(bool yesno);
		DrawMethod getDrawMethod() const { return parentDrawer->getDrawMethod(); }
		void setDrawMethod(DrawMethod method=drmINIT) {};
		virtual bool isEditable() const;
		virtual void setEditable(bool yesno);
		bool hasInfo() const;
		void setInfo(bool yesno);
		NewDrawer *getParentDrawer() const;
		double getAlpha() const { return parentDrawer->getAlpha();}
		void setAlpha(double value) {};
		virtual RootDrawer *getRootDrawer() const { return rootDrawer; }
		virtual RootDrawer *getRootDrawer() { return rootDrawer; }
		virtual HTREEITEM configure(LayerTreeView  *tv, HTREEITEM parent) { return parent;}
		virtual void timedEvent(UINT timerid) {};
		virtual String description() const { return "?";}
		virtual String iconName(const String& subtype="?") const { return "?";}
		void setSpecialDrawingOptions(int option, bool add, const vector<int>& coords=vector<int>()); 
		int getSpecialDrawingOption(int opt=sdoNone) const; 
		String store(const FileName& fnView, const String& parenSection) const { return parenSection;}
		void load(const FileName& fnView, const String& parenSection) {};
		bool isSimple() const { return true;}
		bool isDirty() const { return true;} 
		void setDirty(bool yesno) {}
		String getInfo(const Coord& crd) const;
		void shareVertices(vector<Coord *>& coords) {};
		bool inEditMode() const { return parentDrawer->inEditMode();}
		void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const {}
		virtual GeneralDrawerProperties *getProperties() { return 0;}
		void setSupportingDrawer(bool yesno) { isSupportingDrawer=yesno;};
		void setExtrustionAlpha(double v);
		bool isValid() const;
		void setValid(bool yesno);
		virtual Feature *getFeature() const {return 0;};
		bool isManaged() const { return managed;}
		void setDrawerParameters(DrawerParameters *);
		void setLabelDrawer(TextDrawer *txtdr);
		TextDrawer *getLabelDrawer() const;
		void setGeneralProperties(GeneralDrawerProperties *) {}
		virtual CoordBounds getBounds() const  { return CoordBounds(); }

		CCriticalSection cs;
	protected:
	
		NewDrawer *parentDrawer;
		RootDrawer *rootDrawer;
		String type;
		String subType;
		String id;
		String name;
		bool info;
		bool editable;
		bool fActive;
		double extrAlpha;
		int specialOptions;
		bool isSupportingDrawer;
		bool valid;
		bool managed;
		TextDrawer *label; // TextDrawer isnt owned by this drawer

		SimpleDrawer(DrawerParameters *context, const String& ty);
		SimpleDrawer();
		virtual ~SimpleDrawer();
		void drawExtrusion(const Coord& c1, const Coord& c2, double z, int option) const;
		void setTableSelection(const FileName& fn, long v,PreparationParameters* p);
	private:
		void init();
	} ;

}



