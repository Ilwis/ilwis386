#ifndef ODLBCB_H
#define ODLBCB_H

#include "Client\FormElements\itemdraw.h"
#include "Client\Base\ZappToMFC.h"

class OwnerDrawListBox : public CListBox, public BaseZapp
{
public:
    OwnerDrawListBox(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id);
	void SetShowPopup(bool fShowPopup);

    void DrawItem(DRAWITEMSTRUCT* dis);
    void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	
    void OnLBDblClk();
    void OnMouseMove( UINT nFlags, CPoint point );
    void OnLButtonDown( UINT nFlags, CPoint point );
    void OnLButtonUp( UINT nFlags, CPoint point );
	void OnContextMenu(CWnd* pWnd, CPoint point);
	void OnSelChange();
	void OnSetFocus(CWnd *old);
    void OnKillFocus(CWnd *old);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
    ItemDrawer idw;
	bool m_fShowPopup;

    DECLARE_MESSAGE_MAP();
};

class OwnerDrawCB : public CComboBox, public BaseZapp
{
  public:
    OwnerDrawCB(FormEntry *f, DWORD iStyle, const CRect& rct, CWnd *parent, int id);
    void DrawItem(DRAWITEMSTRUCT* dis);
    void MeasureItem( LPMEASUREITEMSTRUCT lpMeasureItemStruct );

    void OnCBDblClk();
    void OnCBCloseUp();
    void OnSelChange();
		void OnSelAndOK();
    
    ItemDrawer idw;

    DECLARE_MESSAGE_MAP();
};

#endif // ODLB&CB_H



