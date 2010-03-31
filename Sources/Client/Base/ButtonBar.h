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
#if !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
#define AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ButtonBar.h : header file
//

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

/////////////////////////////////////////////////////////////////////////////
// ButtonBar window

//! class ButtonBar manages buttons
/*! class ButtonBar manages buttons. In normal circumstances buttons
    are linked to menu items via the menu ID
*/
class IMPEXP ButtonBar : public CToolBar
{
// Construction
public:
	/*! ButtonBar() Creates an empty buttonbar, and set the default button size to 
	    16 pixels wide and 15 pixels high; the separator width is set to 8 pixels
	*/
	ButtonBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ButtonBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	/*! AddSeparator() add a separator
	    \param iIndex The (zero based) button bar position number for the separator
	*/
	void AddSeparator(int iIndex);
	/*! AddButton() add a button to the button bar
	    \param iIndex The (zero based) button bar position number
	    \param iID The command ID for the button. 
	    \param bmp The button image
	*/
	void AddButton(int iIndex, int iID, CBitmap& bmp);
	/*! iGetSeparatorWidth() get the current separator width
	    \return the current separator width
	*/
	int iGetSeparatorWidth();
	/*! SetSeparatorWidth() set a new value for the separator width
	    \param iNewWidth the new separator width
	*/
	void SetSeparatorWidth(int iNewWidth);
	/*! Create() Create the button bar by adding all images and setting the title for
	    the undocked buttonbar
	    \param pParent The window by which the buttonbar should be managed
	    \param fnButtons The name of the file containing the button bar definition
	    \param sTitle The caption text when the button bar is undocked (floating)
	    \param iID The identifier of the button bar
	    \return true when the creation is succesful.
	*/
	bool Create(CWnd* pParent, const FileName& fnButtons, const string& sTitle, int iID);
	/*! ~ButtonBar() cleanup the button bar
	*/
	virtual ~ButtonBar();

	/*! LoadButtons() loads all buttons
	    \param fnButtons The name of the file in which the buttons are defined
	*/
	bool LoadButtons(const FileName& fnButtons);
	void UnloadButtons();
protected:
	//{{AFX_MSG(ButtonBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	//! m_iSeparWidth member to store the separator width
	int m_iSeparWidth;
	CImageList m_ilButtons;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BUTTONBAR_H__054E1556_3E6F_11D3_9CE3_00A0C9D5320B__INCLUDED_)
