#pragma once

#include "Client\Base\Res.h"
#include "Client\Editors\map\BaseMapEditor.h"

class MapCompositionDoc;
class BaseMap;
class Feature;
class BaseMapPtr;
class FieldColor;

namespace ILWIS{


	class _export FeatureSetEditor2 : public BaseMapEditor{
	public:
		enum States{msNONE=0, msINSERT=1, msMOVE=4, msSELECT=8, msSPLIT=16, msMOVEINSERTVERTICES=32, msMOVEVERTICES=64,
					msLMOUSEUP=128, msLMOUSEDOWN=256, msMOUSEMOVE=512, 
					msENTER=1024, msESCAPE=2048, msCTRL=4096, msDELETE=8192, msOVERVERTEX=16384, msOVERLINE=32768, msEMPTY=65536};
		FeatureSetEditor2(const String& tp,  ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		virtual ~FeatureSetEditor2();
		bool OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void OnLButtonDown(UINT nFlags, CPoint point);
		void OnLButtonUp(UINT nFlags, CPoint point);
		void OnMouseMove(UINT nFlags, CPoint point);
		void OnSetCursor(BaseMapEditor::Mode m=mUNKNOWN);
		void OnContextMenu(CWnd* pWnd, CPoint point);
		zIcon icon() const { return zIcon(); }
		String sTitle() const { return "?"; }
		LRESULT OnUpdate(WPARAM, LPARAM);
		virtual bool fCopyOk();
		virtual bool fPasteOk();
		void clear();
		virtual bool hasSelection() const;
		HTREEITEM configure( HTREEITEM parentItem);
		void setcheckEditMode(void *value, HTREEITEM );
		virtual void OnInsertMode() ;
		virtual void OnMoveMode() ;
		virtual void OnSelectMode() ;
		virtual void OnSplitMode();
		virtual void OnMergeMode();
		void prepare();
		virtual void removeSelectedFeatures();
		void setActive(bool yesno);
		bool hasState(int state);


	protected:
		void OnUpdateMode(CCmdUI* pCmdUI);
		virtual void setMode(FeatureSetEditor2::States state) ;
		void setSelectionOptions();

		long iCoordIndex(const vector<Coord *>& coords, const Coord& c) const;
		MapCompositionDoc *mdoc;
		BaseMapPtr *bmapptr;
		SFMap selectedFeatures;
		int currentCoordIndex;
		zCursor curActive;
		HMENU hmenFile, hmenEdit;
		int mode;
		ILWIS::ComplexDrawer *layerDrawer;
		//PixelInfoDoc *pixdoc;
		HelpTopic htpTopic;
		String sHelpKeywords;
		SetChecks *editModeItems;
		zCursor curInsert, curEdit, curMove, curMoving;
		long editorState;
		HTREEITEM hitSelect, hitInsert, hitMove;

		DECLARE_MESSAGE_MAP()
	};

	class SelectionOptionsForm : public DisplayOptionsForm {
	public:
		SelectionOptionsForm(CWnd *wPar, ComplexDrawer *dr);
		void apply();
	private:
		FieldColor *fc;
		Color c;

	};
}
