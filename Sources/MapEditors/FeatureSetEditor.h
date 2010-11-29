#pragma once

#include "Client\Base\Res.h"
#include "Client\Editors\Map\BaseMapEditor.h"

class MapCompositionDoc;
class BaseMap;
class Feature;
class BaseMapPtr;

namespace ILWIS{


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
		HelpTopic htpTopic;
		String sHelpKeywords;
	};
}

//typedef ILWIS::FeatureSetEditor* (*FeatureSetEditorCreate)(MapCompositionDoc *, const BaseMap& );




