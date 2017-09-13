#if !defined(FALLBACKVIEW_H)
#define FALLBACKVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FallbackView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// FallbackView view

class FallbackView : public FormBaseView
{
protected:
 	FallbackView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(FallbackView)
	~FallbackView();
  void CreateForm();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(FALLBACKVIEW_H)
