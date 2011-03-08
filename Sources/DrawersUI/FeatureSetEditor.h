#pragma once

#include "Client\Base\Res.h"
#include "BaseMapEditor.h"

class MapCompositionDoc;
class BaseMap;
class Feature;
class BaseMapPtr;

namespace ILWIS{


	class _export FeatureSetEditor : public BaseMapEditor{
	public:
		FeatureSetEditor(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		virtual ~FeatureSetEditor();
		void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnLButtonDblClk(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnSetCursor(BaseMapEditor::Mode m=mUNKNOWN);
		void OnContextMenu(CWnd* pWnd, CPoint point);
		zIcon icon() const { return zIcon(); }
		String sTitle() const { return "?"; }
		LRESULT OnUpdate(WPARAM, LPARAM);
		virtual bool fCopyOk();
		virtual bool fPasteOk();
		void init(ILWIS::ComplexDrawer *drw,PixelInfoDoc *pdoc);
		void clear();
		bool hasSelection() const;
		HTREEITEM configure( HTREEITEM parentItem);
		void setcheckEditMode(void *value);
		virtual void OnInsertMode() ;
		virtual void OnMoveMode() ;
		virtual void OnSelectMode() ;


	protected:
		virtual bool select(UINT nFlags, CPoint point);
		virtual bool insertFeature(UINT nFlags, CPoint point) = 0;
		virtual bool insertVertex(UINT nFlags, CPoint point) { return false;}
		virtual bool selectMove(UINT nFlags, CPoint point);

		long iCoordIndex(const vector<Coord *>& coords, const Coord& c) const;
		void addToSelectedFeatures(Feature *f, const Coord& crd, const vector<ILWIS::NewDrawer *>& drawers, int coordIndex=iUNDEF);
		MapCompositionDoc *mdoc;
		BaseMapPtr *bmapptr;
		SFMap selectedFeatures;
		int currentCoordIndex;
		String currentGuid;
		zCursor curActive;
		HMENU hmenFile, hmenEdit;
		int mode;
		ILWIS::ComplexDrawer *setdrawer;
		PixelInfoDoc *pixdoc;
		HelpTopic htpTopic;
		String sHelpKeywords;
		SetChecks *editModeItems;

		DECLARE_MESSAGE_MAP()
	};
}
