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
// HelpCtrl.h: interface for the HelpCtrl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HELPCTRL_H__E48EB4C3_D576_11D3_B849_00A0C9D5342F__INCLUDED_)
#define AFX_HELPCTRL_H__E48EB4C3_D576_11D3_B849_00A0C9D5342F__INCLUDED_

#ifndef __HHCTRL_H__
#include "Client\Help\HHCTRL.H"
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP HelpCtrl: public CHHCtrl  
{
public:
	HelpCtrl();
	~HelpCtrl();
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags,
                                   LPCTSTR lpszTargetFrameName, 
                                   CByteArray& baPostedData,
                                   LPCTSTR lpszHeaders, BOOL* pfCancel);
private:
	bool fFirst;
};

#endif // !defined(AFX_HELPCTRL_H__E48EB4C3_D576_11D3_B849_00A0C9D5342F__INCLUDED_)
