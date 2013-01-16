#if !defined(AFX_COLUMNTREECTRL_H__363F616A_0791_476F_89E1_8B6E0881D054__INCLUDED_)
#define AFX_COLUMNTREECTRL_H__363F616A_0791_476F_89E1_8B6E0881D054__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColumnTreeCtrl.h : header file
//
#include <vector>
using namespace std;

namespace ILWIS {

/////////////////////////////////////////////////////////////////////////////
// ColumnTreeCtrl window

class ColInfo;
class ParallelCoordinateGraphWindow;
class ColumnTreeCtrl : public CTreeCtrl
{
// Construction
public:
	ColumnTreeCtrl(Table & _tblAttr, vector<ColInfo*> & _columns, CCriticalSection & _csColumns, ParallelCoordinateGraphWindow * _pgw);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ColumnTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void InitColumnTree();
	void HandleButtonUp(UINT nFlags, CPoint point);
	vector<int> viColumns();
	virtual ~ColumnTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(ColumnTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

private:
	void ComputePcp();
	vector<ColInfo*> & columns;
	CCriticalSection & csColumns;
	Table tblAttr;
	ParallelCoordinateGraphWindow * pgw;
	bool fLMouseButtonDown;
	HTREEITEM hDragSource;
	CPoint pDragStartPoint;
	DECLARE_MESSAGE_MAP()
};

}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLUMNTREECTRL_H__363F616A_0791_476F_89E1_8B6E0881D054__INCLUDED_)
