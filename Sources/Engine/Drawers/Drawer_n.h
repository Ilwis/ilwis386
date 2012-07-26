#pragma once

#include <gl/gl.h>
#include <gl/glu.h>
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Domain\dm.h"
#include "Engine\Drawers\SVGElements.h"
#include "Engine\Drawers\SVGLoader.h"
#include "Engine\Base\System\Engine.h"

class LayerTreeView;
class LayerTreeItem;
class SetChecks;
class DisplayOptionTreeItem;

#define DEFAULT_POINT_SYMBOL_TYPE "open-rectangle"

namespace ILWIS {
	class RootDrawer;
	struct PreparationParameters;
	struct DrawerParameters;
	class NewDrawer;
};

typedef ILWIS::NewDrawer* (*DrawerCreate)(ILWIS::DrawerParameters *parms); 

struct RepresentationProperties;

namespace ILWIS {
	class RootDrawer;
	struct PreparationParameters;
	struct DrawerParameters;
	class NewDrawer;
	struct GeneralDrawerProperties { // placeholder , others derive from this
	};

	class _export NewDrawer {
	public:
		enum DrawMethod	{ drmNOTSET, drmINIT, drmRPR, drmSINGLE, drmMULTIPLE, drmIMAGE,	drmCOLOR, drmBOOL, drmBYATTRIBUTE, drmARROW, drmGRAPH };
		enum DrawColors { drcLIGHT, drcNORMAL, drcDARK, drcGREY };
		enum PreparationType{ptNONE=0,ptRENDER=1,ptGEOMETRY=2,ptINITOPENGL=4,ptUI=8,pt3D=16,ptANIMATION=32,ptRESTORE=64,ptOFFSCREENSTART=128,ptOFFSCREENEND=256,ptREDRAW=512,ptNEWCSY=1024,ptALL=4294967295};
		enum DataSourceMergeOptions{dsmEXTENDCB=1};
		enum UICode{ucNONE=0, ucNOREPRESENTATION=2, ucNOINFO=4, ucNOMASK=8, ucNOMULTICOLOR=16,ucNOTRANSPARENCY=32, ucALL=4294967295};
		enum SpecialDrawingOptions{sdoNone=0, sdoExtrusion=1, sdoSymbolLineNode=2, sdoYMarker=4, sdoFilled=8, sdoSELECTED=16, sdoTOCHILDEREN=32, sdoFootPrint=64, sdoOpen=128};
		enum LineDspType { ldtNone,
                   ldtSingle, ldtDouble, ldtTriple,
                   ldtDot, ldtDash, ldtDashDot, ldtDashDotDot,
                   ldtBlocked, ldtSymbol
                 };
		enum DataSourceOption{dsoNONE=0, dsoEXTENDBOUNDS=1};

		virtual ~NewDrawer() {}
		virtual bool draw( const CoordBounds& cb=CoordBounds()) const = 0;
		virtual void prepare(PreparationParameters *) =0;
		virtual String getType() const =0;
		virtual String getSubType() const =0;
		virtual void setSubType(const String & sSubType) = 0;
		virtual void addDataSource(void *, int options=0) = 0;
		virtual void removeDataSource(void *) = 0;
		virtual void* getDataSource() const = 0;
		virtual RootDrawer *getRootDrawer() = 0;
		virtual RootDrawer *getRootDrawer() const = 0;
		virtual String getName() const = 0;
		virtual void setName(const String&) = 0;
		virtual bool isActive() const = 0;
		virtual void setActive(bool yesno)=0;
		virtual DrawMethod getDrawMethod() const = 0;
		virtual void setDrawMethod(DrawMethod method=drmINIT) = 0;
		virtual bool isEditable() const = 0;
		virtual void setEditable(bool yesno) = 0;
		virtual bool hasInfo() const = 0;
		virtual void setInfo(bool yesno) = 0;
		virtual String getInfo(const Coord& crd) const = 0;
		virtual NewDrawer *getParentDrawer() const = 0;
		virtual double getTransparency() const = 0;
		virtual void setTransparency(double value) = 0;
		virtual String getId() const = 0;
		virtual void timedEvent(UINT eventid) = 0;
		virtual String description() const = 0;
		virtual String iconName(const String& subtype="?") const = 0;
		virtual void setSpecialDrawingOptions(int option, bool add, const vector<int>& coords=vector<int>())=0 ;
		virtual int getSpecialDrawingOption(int opt=sdoNone) const =0;
		virtual String store(const FileName& fnView, const String& parenSection) const = 0;
		virtual void load(const FileName& fnView, const String& parenSection) = 0;
		virtual bool isSimple() const =0;
		virtual bool isDirty() const =0;
		virtual void setDirty(bool yesno) = 0;
		virtual void shareVertices(vector<Coord *>& coords) = 0;
		virtual bool inEditMode() const = 0;
		virtual void drawLegendItem(CDC *dc, const CRect& rct, double rVal) const = 0;
		virtual GeneralDrawerProperties *getProperties() = 0;
		virtual bool isValid() const = 0;
		virtual void setValid(bool yesno) = 0;

		static NewDrawer *getDrawer(const String& type, const String& subType, ILWIS::DrawerParameters *parms) ;
		static NewDrawer *getDrawer(const String& type, PreparationParameters *pp=0, DrawerParameters *parms=0) ;
		static void  addDrawer(const String& type, const String& subtype, DrawerCreate);
		static SVGLoader* getSvgLoader()  ;

	private:
		static map<String, DrawerCreate> drawers;
		static SVGLoader		*svgContainer;
	};

	struct DrawerParameters {
		DrawerParameters(RootDrawer *c, NewDrawer *p) : rootDrawer(c), parent(p) {}
		RootDrawer *rootDrawer;
		NewDrawer *parent;
	};

	typedef void(ILWIS::NewDrawer::*DisplayOptionItemFunc)(CWnd *parent);
	

	struct PreparationParameters {
		PreparationParameters(const PreparationParameters* parms) :
			type(parms->type),dc(parms->dc)/*,rootDrawer(0),zOrder(parms->zOrder)*/,index(parms->index),contextMode(10) 
			{
				filteredRaws.resize(parms->filteredRaws.size());
				copy(parms->filteredRaws.begin(), parms->filteredRaws.end(),filteredRaws.begin());
				props = parms->props;
				rowSelect = parms->rowSelect;
			}
			PreparationParameters(int t=1,CDC *_dc = 0,int m=10) : type(t),dc(_dc), zOrder(-1),index(0),contextMode(m),props(0) {} 
		PreparationParameters(int t, const CoordSystem& cs) : type(t), csy(cs),dc(0),zOrder(-1),index(0),contextMode(10), props(0)  {}
		int type;
		CDC *dc;
		int contextMode;
		CoordSystem csy;
		String subType;
		vector<int> filteredRaws;
		int zOrder;
		int index;
		String displayOptions;
		RepresentationProperties *props;
		RowSelectInfo rowSelect;
	};

}


struct DrawerInfo {
	DrawerInfo(const String& n, const String& _subtype, DrawerCreate func) : name(n), subtype(_subtype), createFunc(func) {}
	String name;
	String subtype;
	DrawerCreate createFunc;
};

//struct ConfigureMethod {
//	ConfigureMethod(ILWIS::NewDrawer *_drw, Configure _conf) : drawer(_drw),configure(_conf) {}
//	ILWIS::NewDrawer *drawer;
//	Configure configure;
//
//};

typedef vector<DrawerInfo *> DrawerInfoVector;
typedef DrawerInfoVector *( *GetDrawers)();



