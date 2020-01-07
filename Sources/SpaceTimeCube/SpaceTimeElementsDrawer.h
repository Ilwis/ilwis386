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
			return color.fEqual(colorUNDEF) && (transparency == rUNDEF) && (!visible);
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
		bool drawFootprint(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		bool drawXT(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		bool drawXY(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		bool drawYT(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		void callFootprintList() const;
		void callXTList() const;
		void callXYList() const;
		void callYTList() const;
		void prepare(PreparationParameters *parms);
		void SetSpaceTimeDrawer(SpaceTimeDrawer * _spaceTimeDrawer);
		void RefreshDisplayList() const;
		GeneralDrawerProperties *getProperties();
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);
	protected:
		GLuint * displayListFootprint;
		GLuint * displayListFootprintBeforeOffscreen;
		bool * fRefreshDisplayListFootprint;
		GLuint * displayListXT;
		GLuint * displayListXTBeforeOffscreen;
		bool * fRefreshDisplayListXT;
		GLuint * displayListXY;
		GLuint * displayListXYBeforeOffscreen;
		bool * fRefreshDisplayListXY;
		GLuint * displayListYT;
		GLuint * displayListYTBeforeOffscreen;
		bool * fRefreshDisplayListYT;
		SpaceTimeDrawer * spaceTimeDrawer;
		BaseMapPtr *basemap;
		PathElementProperties properties;
	};
}