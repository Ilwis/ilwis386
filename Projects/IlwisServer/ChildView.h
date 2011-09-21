// ChildView.h : interface of the CChildView class
//


#pragma once


// CChildView window
namespace ILWIS {
	class RootDrawer;
}

class CChildView : public CWnd
{

public:
	CChildView();
	void initDraw(const FileName& fn);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	virtual ~CChildView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();

	CDC * pDC;
	ILWIS::RootDrawer *root;
	FileName *fnnn;

	DECLARE_MESSAGE_MAP()
};

