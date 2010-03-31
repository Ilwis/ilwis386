/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
#ifndef REPRESENTATIONVALUE_H
#define REPRESENTATIONVALUE_H

class RepresentationValueView;
class RepresentationValueDoc;
class GradientBand;

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


// !class InPlaceColorMethodCB
class InPlaceColorMethodCB : public CComboBox
{
public:
	InPlaceColorMethodCB();
	~InPlaceColorMethodCB();

protected:
	void OnKillFocus( CWnd* pNewWnd );

	DECLARE_MESSAGE_MAP();
};

// !class InPlaceValueEdit
class InPlaceValueEdit : public CEdit
{
	friend class RepresentationValueLB;

public:
	InPlaceValueEdit();
	~InPlaceValueEdit();

	BOOL				Create(RepresentationValueDoc *rpvDoc, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID );
	void				SetEditId(int id);
	void				SetRange ( double rLMin, double rLMax );

private:
	double			rValidValue(double rVal);
	BOOL        PreTranslateMessage(MSG* pMsg);
	void				OnKillFocus( CWnd* pNewWnd );
	void        OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void        OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	RepresentationValueDoc *rpvDoc;
	int					iIndex;
	double			rMax;
	double			rMin;
	bool        fIgnoreReturn; // hack hack, return keeps going to LB

	DECLARE_MESSAGE_MAP();
};

class RepresentationBar : public CSizingControlBar
{
public:
	RepresentationBar();
	~RepresentationBar();

	BOOL Create(CWnd *pPar, CView *v, RepresentationValueDoc *doc);
	CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	void OnSize(UINT nType, int cx, int cy);
	void SetLowerLimit(double rVal);
	void SetUpperLimit(double rVal);

private:
	void OnPaint();

	RepresentationValueDoc *rprValDoc;
	CView *view;
	CWnd *parent; // needs this for parent changes when floating
	GradientBand *band;
	bool         fFloating;

	DECLARE_MESSAGE_MAP();
};

class RepresentationValueLB : public CListBox
{
	friend class RepresentationValueView;

protected:
	RepresentationValueLB();
	~RepresentationValueLB();

	BOOL          Create(RepresentationValueView *rprVV);

	void          DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct );
	void          MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct );
	void          OnLButtonDblClk(UINT , CPoint);
	void          OnLButtonUp(UINT nFlags, CPoint point );
	void          OnKillFocus( CWnd* pNewWnd );
	void          InPlaceCBChanged();
	void          OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	void          OnContextMenu( CWnd* pWnd, CPoint point );
	void          OnUpper();
	void          OnLower();
	void          OnStretch();
	void          OnEdit();
	void          OnColorMethod(CCmdUI *cmd);

private:
	RepresentationValueView             *view;
	zFontInfo                           *info;
	InPlaceValueEdit                         value;
	InPlaceColorMethodCB                clrMethod;

	DECLARE_MESSAGE_MAP()
};

class IMPEXP RepresentationValueView : public RepresentationView
{
	friend class RepresentationValueLB;
	friend class InPlaceValueEdit;

protected: // create from serialization only
	RepresentationValueView();
	DECLARE_DYNCREATE(RepresentationValueView)

public:
	RepresentationValueDoc*	   GetDocument();
	virtual                    ~RepresentationValueView();
											
	virtual void               OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL               PreCreateWindow(CREATESTRUCT& cs);
	void                       OnFileSave();
	void                       OnMultSteps();
											
#ifdef _DEBUG					
	virtual void               AssertValid() const;
	virtual void               Dump(CDumpContext& dc) const;

#endif

protected:
	virtual void        OnInitialUpdate();

	virtual void        LoadState(IlwisSettings& settings);
	virtual void        SaveState(IlwisSettings& settings);

	void                OnSize( UINT nType, int cx, int cy );
	void                OnEdit();
	void                OnInsert();
	void                OnDelete();
	void                OnStretchSteps();
	void                OnUpdateEditItem(CCmdUI *cmd);
	void                OnRepresentationBar();
	void                OnUpdateRepresentationBar(CCmdUI *cmd);
	void                SetColorMethod(int iLBIndex, RepresentationGradual::ColorRange method);
	void                SetLimitValue(int iLBIndex, double rV);
	void                OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	void								OnUpdateRprButtonBar(CCmdUI *cmd);
	void                OnRprButtonBar();

	RepresentationValueLB  lb;
	RepresentationBar      rprBar; 

private:
	void                init();
	ButtonBar            m_bbTools;	

	DECLARE_MESSAGE_MAP()
};


/* !\class  InPlaceColorMethodCB
*           Combobox for choosing the color fill method for the values between two limits
*/
/*!	\fn void InPlaceColorMethodCB::OnKillFocus( CWnd* pNewWnd )
*  if the focus shifts from the combobox the combobox is hidden
*/
/* !\class InPlaceValueEdit
*           Input edit control for values of the bands in the editor. There is only one edit control that will be
*           repositioned as need be.
*/
/*! \fn void	InPlaceValueEdit::SetEditId(int id)
*           Sets the index of the band used for the value of the edit control.
*  \param id id of the band whose value is displayed.
*/
/*! \fn void InPlaceValueEdit::SetRange ( double rLMin, double rLMax )
*           Input values in an edit will be checked against a certain range.
*  \param   rLMin Minimum input value allowed in the control (exclusive)
*  \param   rLMax Maximium input value allowed in the control (exclusive)
*/
/*!  \fn void	InPlaceValueEdit::OnKillFocus( CWnd* pNewWnd )
*           shifting the focus works the same as pressing return
*/
/*!  \fn 	void InPlaceValueEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags )
*           pressing down arrow or up arrow works the same as pressing return
*/
/*! \fn void InPlaceValueEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
*           handles the input of numbers in the edit if a return/enter is pressed. It checks the input 
*           against the value range defined previously and sets the value of the limit in the rpr. 
*           It also handles the ESC key. In both cases it hides the edit control after input.
*/
/*! \var  InPlaceValueEdit::iIndex
*           Holds the index of the limit band
*/
/*! \var  double InPlaceValueEdit::rMax  
*           Maximum allowed value (exclusive)
*/
/*! \var  double InPlaceValueEdit::rMin
*           Minimum allowed value (exclusive)
*/
/*! \var   bool InPlaceValueEdit::fIgnoreReturn
*           used to prevent unwanted returns falling through to the listbox that owns the edit control
*/
#endif //REPRESENTATIONVALUE_H
