#pragma once

ILWIS::NewDrawer *createSpaceTimeElementsDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

		struct _export PathElement {
		PathElement() {
			color = colorUNDEF;
			transparency = rUNDEF;
			visible = false;
			label = sUNDEF;
		}
		PathElement(const String& labl, const Color& clr, double transp, bool vis) : 
		color(clr), transparency(transp),visible(vis),label(labl){
		}

		PathElement(const PathElement& ce) {
			color = ce.color;
			transparency = ce.transparency;
			visible = ce.visible;
			label = ce.label;
		}
		bool isValid() const{
			return color == colorUNDEF && transparency == rUNDEF && visible == false;
		}
		static PathElement undefElement;
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);

		Color color;
		double transparency;
		bool visible;
		String label;
	};

	struct _export PathElementProperties : public GeneralDrawerProperties {
		PathElementProperties();
		PathElementProperties(PathElementProperties *lp);
		
		map<String, PathElement> elements;
		PathElement& operator[](const String& key) const;

		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);
	};

	class SpaceTimeDrawer;
	class _export SpaceTimeElementsDrawer : public ComplexDrawer {
	public:
		ILWIS::NewDrawer *createSpaceTimeElementsDrawer(DrawerParameters *parms);

		SpaceTimeElementsDrawer(DrawerParameters *parms);
		virtual ~SpaceTimeElementsDrawer();
		bool draw( const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *parms);
		void SetSpaceTimeDrawer(SpaceTimeDrawer * _spaceTimeDrawer);
		void RefreshDisplayList() const;
		GeneralDrawerProperties *getProperties();
	protected:
		bool * fFootprintComputed;
		GLuint * displayList;
		bool * fRefreshDisplayList;
		SpaceTimeDrawer * spaceTimeDrawer;
		BaseMapPtr *basemap;
		PathElementProperties properties;
	};
}