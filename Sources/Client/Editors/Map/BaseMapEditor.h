#pragma once

// registers which feature is selected, which drawer is drawing it, the coords of the feature and the selected coords(indexes in coords vector).
// note that the coords list is the "real" coords list. so any changes to them is reflected in the drawing.
namespace ILWIS {
	class NewDrawer;
}

struct SelectedFeature {
	SelectedFeature(Feature *f=0) : feature(f) {}
	Feature *feature;
	vector<Coord *> coords;
	vector<int> selectedCoords;
	vector<ILWIS::NewDrawer *> drawers;
};

typedef map<String, SelectedFeature *> SFMap;
typedef SFMap::iterator SFMIter;
typedef SFMap::const_iterator SFMCIter;
class PixelInfoDoc;


namespace ILWIS {	
class NewDrawer;
	class ComplexDrawer;

	class _export BaseMapEditor : public DrawerTool{
	public:
		BaseMapEditor(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw) : DrawerTool(tp,zv,view, drw) {}
		enum Mode{mUNKNOWN=0, mINSERT=1, mMOVE=2, mMOVING=4, mSELECT=8, mDELETE=16, mCHANGE=32, mSPLIT=64, mMERGE=128};
		// empty class for the moment will be moved to seperate file as soon as there is a rastermap editor
		virtual ~BaseMapEditor() {};
		virtual zIcon icon() const = 0;
		virtual String sTitle() const = 0 ;
		virtual LRESULT OnUpdate(WPARAM, LPARAM) = 0;
		virtual bool hasSelection() const = 0;
		virtual void updateFeature(SelectedFeature *f) = 0;
		virtual void setMode(BaseMapEditor::Mode m) {};
	};
}

//struct BaseMapEditorInfo {
//	BaseMapEditorInfo(const String& n, const String& _subtype, BaseMapEditorCreate func) : name(n), subtype(_subtype), createFunc(func) {}
//	String name;
//	String subtype;
//	BaseMapEditorCreate createFunc;
//};
