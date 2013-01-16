#ifndef PARALLELCOORDINATEPLOTTOOL_H
#define PARALLELCOORDINATEPLOTTOOL_H

ILWIS::DrawerTool *createParallelCoordinatePlotTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

#include "Client\FormElements\FieldGraph.h"
#include "Client\FormElements\FormBaseWnd.h"

class InfoLine;

namespace ILWIS {

	class ParallelCoordinatePlotWindow;
	class ColumnTreeCtrl;

	class ParallelCoordinatePlotTool : public DrawerTool {
	public:
		ParallelCoordinatePlotTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		virtual ~ParallelCoordinatePlotTool();
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		String getMenuString() const;
		void closeParallelCoordinatePlotWindow();
	private:
		void startParallelCoordinatePlotForm();
		LayerDrawer *layerDrawer;
		ParallelCoordinatePlotWindow * tpw;
	};

	class ColInfo {
	public:
		ColInfo(const bool _fValue, const DomainSort * _pdSort, const Column & _column, const RangeReal _rrMinMax)
		: fValue(_fValue), pdSort(_pdSort), column(_column), rrMinMax(_rrMinMax) {};
		const bool fValue;
		const DomainSort* pdSort;
		const Column & column;
		const RangeReal rrMinMax;
	};

	class ParallelCoordinatePlotForm;

	class ParallelCoordinateGraphWindow : public SimpleGraphWindow
	{
	public:
		ParallelCoordinateGraphWindow(LayerDrawer *_layerDrawer, Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns);
		virtual ~ParallelCoordinateGraphWindow();
		void SetDomain(double l, double t, double r, double b);
		void SelectFeatures(RowSelectInfo & inf);
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
		void SetInfoForm(ParallelCoordinatePlotForm * _tpf);

	protected:
		virtual void DrawFunction(CDC* pDC);
		virtual void DrawSelectedFunctions(CDC* pDC);
		virtual void DrawAxes(CDC* pDC);
		virtual void DrawMouse(CDC* pDC);
		virtual void SetDirty(bool fRedraw = false);
		virtual void StartDrag(CPoint point);
		virtual void Drag(CPoint point);
		virtual void EndDrag(CPoint point);
		String getInfo(CPoint point);
		void SelectionChanged();
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		afx_msg void OnDestroy();
		InfoLine* info;
		CPoint mousePos;
	 	CBitmap* m_bmMemoryGraph;
		CBitmap* m_bmOldBitmapGraph;
		CDC* m_dcMemoryGraph;
		bool fDrawAxes;
		bool m_fSelectionChanged;
		bool m_fAbortSelectionThread;
		CWinThread * m_selectionThread;
		CCriticalSection csSelectionThread;

	private:
		LayerDrawer *layerDrawer;
		Table tblAttr;
		vector<ColInfo*> & columns;
		vector<long> selectedRaws;
		double rGetFx(long iRaw, ColInfo & colInfo);
		CCriticalSection & csColumns;
		static unsigned int iPointOnLine(CPoint c, int ax, int ay, int bx, int by);
		static UINT PaintInThread(LPVOID pParam);
		static UINT SelectionChangedInThread(LPVOID pParam);
		ParallelCoordinatePlotForm * tpf;

		DECLARE_MESSAGE_MAP()
	};

	class ParallelCoordinatePlotForm: public FormBaseWnd
	{
	public:
	  ParallelCoordinatePlotForm(CWnd* mw, LayerDrawer * _layerDrawer, Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns, ParallelCoordinateGraphWindow * _pgw);
	  virtual ~ParallelCoordinatePlotForm();
	  void SetInfo(String & sInfo);
	protected:
		afx_msg LONG OnSelectFeatures(UINT, LONG lParam);
	private:
		void ComputePcp();
		ParallelCoordinateGraphWindow * pgw;
		LayerDrawer *layerDrawer;
		vector<ColInfo*> & columns;
		CCriticalSection & csColumns;
		Table tblAttr;
		FieldString * fsInfo;
		String sDummy;

		DECLARE_MESSAGE_MAP()
	};

	class ParallelCoordinatePlotWindow: public CWnd
	{
	public:
		ParallelCoordinatePlotWindow(LayerDrawer * layerDrawer, Table & tblAttr, ParallelCoordinatePlotTool * _tpt);
		~ParallelCoordinatePlotWindow();
	protected:
		afx_msg void OnSize(UINT nType, int cx, int cy);
		afx_msg void OnClose();
		ParallelCoordinateGraphWindow * sgw;
		ColumnTreeCtrl * ctc;
		ParallelCoordinatePlotForm * tpf;
		ParallelCoordinatePlotTool * tpt;
	private:
		vector<ColInfo*> columns;
		CCriticalSection csColumns;

		DECLARE_MESSAGE_MAP()
	};
}

#endif // PARALLELCOORDINATEPLOTTOOL_H