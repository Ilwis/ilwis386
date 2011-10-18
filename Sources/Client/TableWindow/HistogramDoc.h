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
// HistogramDoc.h: interface for the HistogramDoc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HISTOGRAMDOC_H__6DC71B46_0190_11D5_B940_00A0C9D5342F__INCLUDED_)
#define AFX_HISTOGRAMDOC_H__6DC71B46_0190_11D5_B940_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_TABLEDOC_H__7DD2579D_1E42_11D3_B776_00A0C9D5342F__INCLUDED_)
#include "Client\TableWindow\TableDoc.h"
#endif

#if !defined(AFX_HISTOGRAMGRAPHDOC_H__665A106F_FD5D_4137_9C50_B3EF1A93EE08__INCLUDED_)
#include "Client\TableWindow\HistogramGraphDoc.h"
#endif

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP HistogramDoc: public TableDoc
{
public:
	HistogramDoc();
	virtual ~HistogramDoc();
	virtual zIcon	icon() const;
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName, int os= 0);
  HistogramGraphDoc hgd;
private:  
	DECLARE_DYNCREATE(HistogramDoc)
};

#endif // !defined(AFX_HISTOGRAMDOC_H__6DC71B46_0190_11D5_B940_00A0C9D5342F__INCLUDED_)
