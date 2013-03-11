#pragma once
#include "Engine\Drawers\OpenGLText.h"
#include "TemporalDrawer.h"

ILWIS::NewDrawer *createCubeDrawer(ILWIS::DrawerParameters *parms);

namespace ILWIS {

	struct _export CubeElement {
		CubeElement() {
			color = colorUNDEF;
			transparency = rUNDEF;
			visible = false;
			label = sUNDEF;
		}
		CubeElement(const String& labl, const Color& clr, double transp, bool vis) : 
		color(clr), transparency(transp),visible(vis),label(labl){
		}

		CubeElement(const CubeElement& ce) {
			color = ce.color;
			transparency = ce.transparency;
			visible = ce.visible;
			label = ce.label;
		}
		bool isValid() const{
			return color == colorUNDEF && transparency == rUNDEF && visible == false;
		}
		static CubeElement undefElement;
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);

		Color color;
		double transparency;
		bool visible;
		String label;
	};

	struct _export CubeProperties : public GeneralDrawerProperties {
		CubeProperties();
		CubeProperties(CubeProperties *lp);
		
		map<String, CubeElement> elements;
		CubeElement& operator[](const String& key) const;

		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& parentSection);
	};

	class _export CubeDrawer : public ComplexDrawer, public TemporalDrawer {
	public:
		ILWIS::NewDrawer *createCubeDrawer(DrawerParameters *parms);

		CubeDrawer(DrawerParameters *parms);
		virtual ~CubeDrawer();
		bool draw(const DrawLoop drawLoop, const CoordBounds& cbArea=CoordBounds()) const;
		void prepare(PreparationParameters *pp);
		GeneralDrawerProperties *getProperties();
		void SetTimePosVariables(double * _timePos, String * _sTimePosText);
	protected:
		void drawCube() const;
		void drawTicMarks() const;
		void drawLabels(const Color & clr) const;
		void drawCoords(const Color & clr) const;
		void drawTimes(const Color & clr) const;
		void renderText(OpenGLText *font, const Coordinate & c, const String & text, bool center=false) const;
		String store(const FileName& fnView, const String& parentSection) const;
		void load(const FileName& fnView, const String& currentSection);
		String sxMin, sxMax, syMin, syMax, stMin, stMax;
		CoordBounds cube;
		double * timePos;
		String * sTimePosText;
		OpenGLText * font;
		OpenGLText * mediumFont;
		CubeProperties properties;
		CCriticalSection * csFont;
	};
}