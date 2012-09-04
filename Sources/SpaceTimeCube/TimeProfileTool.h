#ifndef TIMEPROFILETOOL_H
#define TIMEPROFILETOOL_H

ILWIS::DrawerTool *createTimeProfileTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

#include "Client\FormElements\FieldGraph.h"
#include "Engine\Base\Algorithm\SimpleFunction.h"

class InfoLine;

namespace ILWIS {

	class TimeProfileTool : public DrawerTool {
	public:
		TimeProfileTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);
		virtual ~TimeProfileTool();
		bool isToolUseableFor(ILWIS::DrawerTool *tool);
		HTREEITEM configure( HTREEITEM parentItem);
		String getMenuString() const;
	private:
		void startTimeProfileForm();
		SpaceTimePathDrawer *stpdrw;
	};

	class ProfileGraphWindow : public SimpleGraphWindowWrapper
	{
	public:
		ProfileGraphWindow(FormEntry *f);
		virtual ~ProfileGraphWindow();
		void SetFunctions(SimpleFunction * funPtr, int _iNrFunctions);
		void SetGridTicks(vector<double> & gridXNodes, vector<double> & gridXTicks, vector<double> & gridYTicks);
		void SetGrid(bool gridXN, bool gridXT, bool gridYT);
		virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	protected:
		virtual void DrawFunction(CDC* pDC, const SimpleFunction * pFunc);
		virtual void DrawAxes(CDC* pDC);
		String getInfo(CPoint point);
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
		afx_msg void OnMouseMove(UINT nFlags, CPoint point);
		vector<double> m_gridXNodes;
		vector<double> m_gridXTicks;
		vector<double> m_gridYTicks;
		bool m_gridXN;
		bool m_gridXT;
		bool m_gridYT;
		int iNrFunctions;
		InfoLine* info;

		DECLARE_MESSAGE_MAP()
	};

	class ProfileFieldGraph : public FieldGraph  
	{
	public:
		ProfileFieldGraph(FormEntry* parent);
		void create();             // overriden
		void SetFunctions(SimpleFunction * funPtr, int iNrFunctions);
		void SetGridTicks(vector<double> & gridXNodes, vector<double> & gridXTicks, vector<double> & gridYTicks);
		void SetGrid(bool gridXN, bool gridXT, bool gridYT);
	};

	class ProfileGraphFunction : public SimpleFunction
	{
	public:
		ProfileGraphFunction();
		virtual ~ProfileGraphFunction();
		virtual double rGetFx(double x) const; // the most important function of this class that applies f to x and returns y
		virtual void SetDefaultAnchors(); // also call SolveParams in the implementation of this function
		void SetData(vector<double> dataX, vector<double> dataY);
		void SetColors(vector<Color> colors);
		Color GetColor(double x) const;
		virtual void SetAnchor(DoublePoint pAnchor);
		int iGetAnchorNr();
		void SetDomain(double l, double t, double r, double b);
		double rFirstX();
		double rLastX();
	protected:
		virtual void SolveParams();
	private:
		vector <double> m_rDataX;
		vector <double> m_rDataY;
		vector <Color> m_colors;
		int iSelectedAnchorNr;
	};

	class _export TimeProfileForm: public FormWithDest
	{
	public:
	  TimeProfileForm(CWnd* mw, SpaceTimePathDrawer * stp);
	  virtual ~TimeProfileForm();
	private:
		void ComputeGraphs();
		int CallBackAnchorChangedInGraph(Event*);
		int CallBackSegmentMapChanged(Event*);
		int CallBackXNGrid(Event*);
		int CallBackXTGrid(Event*);
		int CallBackYTGrid(Event*);
		String sSegmentMapProfile;
		FieldSegmentMap * fsm;
		ProfileFieldGraph * fgFunctionGraph;
		ProfileGraphFunction * m_functions;
		SpaceTimePathDrawer *stpdrw;
		CheckBox * cbXNgrid;
		CheckBox * cbXTgrid;
		CheckBox * cbYTgrid;
		CheckBox * cbRadiusThreshold;
		FieldReal * frRadiusThreshold;
		bool m_gridXN;
		bool m_gridXT;
		bool m_gridYT;
		bool m_fUseRadiusThreshold;
		double m_radiusThreshold;
	};
}

#endif // TIMEPROFILETOOL_H