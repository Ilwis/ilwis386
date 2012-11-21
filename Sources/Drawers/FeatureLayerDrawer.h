#pragma once


ILWIS::NewDrawer *createFeatureLayerDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class DisplayOptionColorItem;

namespace ILWIS{
	class PointFeatureDrawer;
	class SimpleDrawer;
	class _export FeatureLayerDrawer : public ILWIS::LayerDrawer {
	friend class SetSingleColorForm;

	public:
		ILWIS::NewDrawer *createFeatureLayerDrawer(DrawerParameters *parms);

		FeatureLayerDrawer(DrawerParameters *parms, const String& name);
		virtual ~FeatureLayerDrawer();
		virtual void prepare(PreparationParameters *pp);
		String getMask() const;
		void setMask(const String& sM);
		void setSingleColor(const Color& c);
		Color getSingleColor() const;
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const { return 0;}
		void getFeatures(vector<Feature *>& features) const;
		virtual void addDataSource(void *,int options=0);
		void *getDataSource() const;
		bool useRaw() const;
		virtual void prepareChildDrawers(PreparationParameters *parms);
		virtual void select(const CRect& rect, vector<long> & selectedRaws, SelectionMode selectionMode);
		template<class T> MemoryManager<T> *allocator() {
			if ( managedDrawers == 0)
				managedDrawers = new MemoryManager<T>();
			return (MemoryManager<T> *)managedDrawers;
		}

	protected:
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);
		String getInfo(const Coord& c) const;

		String mask;
		bool useMask;
		Color singleColor;
		BaseMap fbasemap;
		map<long, SimpleDrawer*> featureMap;
		void *managedDrawers;
	
	};
}