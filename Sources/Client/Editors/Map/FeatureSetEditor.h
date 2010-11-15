#pragma once

#include "Client\Base\Res.h"

class MapCompositionDoc;
class BaseMap;
class Feature;
class BaseMapPtr;

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

namespace ILWIS{
	class NewDrawer;
	class ComplexDrawer;

	class _export BaseMapEditor : public CCmdTarget{
	public:
		enum Mode{mINSERT, mMOVE, mMOVING, mSELECT, mDELETE, mCHANGE, mUNKNOWN};
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

	class _export FeatureSetEditor : public BaseMapEditor{
	public:
		FeatureSetEditor(MapCompositionDoc *, const BaseMap& bmap);
		virtual ~FeatureSetEditor();
		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		bool OnLButtonDown(UINT nFlags, CPoint point);
		bool OnLButtonUp(UINT nFlags, CPoint point);
		bool OnLButtonDblClk(UINT nFlags, CPoint point);
		bool OnMouseMove(UINT nFlags, CPoint point);
		bool OnSetCursor(BaseMapEditor::Mode m=mUNKNOWN);
		bool OnContextMenu(CWnd* pWnd, CPoint point);
		zIcon icon() const { return zIcon(); }
		String sTitle() const { return "?"; }
		LRESULT OnUpdate(WPARAM, LPARAM);
		virtual bool fCopyOk();
		virtual bool fPasteOk();
		void init(ILWIS::ComplexDrawer *drw,PixelInfoDoc *pdoc);
		void clear();
		bool hasSelection() const;


	protected:
		virtual bool select(UINT nFlags, CPoint point);
		virtual bool insert(UINT nFlags, CPoint point) = 0;

		long iCoordIndex(const vector<Coord *>& coords, const Coord& c) const;
		void addToSelectedFeatures(Feature *f, const Coord& crd, const vector<ILWIS::NewDrawer *>& drawers);
		MapCompositionDoc *mdoc;
		BaseMapPtr *bmapptr;
		SFMap selectedFeatures;
		int currentCoordIndex;
		String currentGuid;
		zCursor curActive;
		HMENU hmenFile, hmenEdit;
		Mode mode;
		ILWIS::ComplexDrawer *setdrawer;
		PixelInfoDoc *pixdoc;
	};
}

//typedef ILWIS::FeatureSetEditor* (*FeatureSetEditorCreate)(MapCompositionDoc *, const BaseMap& );

struct FeatureSetEditorInfo {
	FeatureSetEditorInfo(const String& n, const String& _subtype, FeatureSetEditorCreate func) : name(n), subtype(_subtype), createFunc(func) {}
	String name;
	String subtype;
	FeatureSetEditorCreate createFunc;
};

typedef vector<FeatureSetEditorInfo *> FSEditors;
typedef FSEditors *( *GetMEditors)();


