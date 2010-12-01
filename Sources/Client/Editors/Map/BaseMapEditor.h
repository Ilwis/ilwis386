#pragma once

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

	class _export BaseMapEditor : public CCmdTarget{
	public:
		enum Mode{mUNKNOWN=0, mINSERT=1, mMOVE=2, mMOVING=4, mSELECT=8, mDELETE=16, mCHANGE=32, mSPLIT=64};
		// empty class for the moment will be moved to seperate file as soon as there is a rastermap editor
		virtual bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)=0;
		virtual ~BaseMapEditor() {};
		virtual bool OnLButtonDown(UINT nFlags, CPoint point)=0;
		virtual bool OnLButtonUp(UINT nFlags, CPoint point)=0;
		virtual bool OnLButtonDblClk(UINT nFlags, CPoint point)=0;
		virtual bool OnMouseMove(UINT nFlags, CPoint point)=0;
		virtual bool OnSetCursor(BaseMapEditor::Mode m=mUNKNOWN)= 0;
		virtual bool OnContextMenu(CWnd* pWnd, CPoint point)=0;
		virtual zIcon icon() const = 0;
		virtual String sTitle() const = 0 ;
		virtual LRESULT OnUpdate(WPARAM, LPARAM) = 0;
		virtual bool hasSelection() const = 0;
		virtual void init(ILWIS::ComplexDrawer *drw,PixelInfoDoc *pdoc) = 0;
		virtual void updateFeature(SelectedFeature *f) = 0;
	};
}

struct BaseMapEditorInfo {
	BaseMapEditorInfo(const String& n, const String& _subtype, BaseMapEditorCreate func) : name(n), subtype(_subtype), createFunc(func) {}
	String name;
	String subtype;
	BaseMapEditorCreate createFunc;
};

typedef vector<BaseMapEditorInfo *> BMEditors;
typedef BMEditors *( *GetMEditors)();