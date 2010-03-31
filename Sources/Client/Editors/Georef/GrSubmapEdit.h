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
/* GeoRefSubmap editor
   by Willem Nieuwenhuis, 17 August 1999
   Copyright Ilwis System Development ITC
*/

#ifndef GeoRefSubmapView_H
#define GeoRefSubmapView_H

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif

class IMPEXP GeoRefSubmapView : public GeoRefGeneratedView
{
public:
	GeoRefSubmapView();
	virtual ~GeoRefSubmapView();

	DECLARE_DYNCREATE(GeoRefSubmapView)

	virtual void FillDerivedFields(FieldGroup* fgGenerRoot);
	virtual bool fRemarkNeeded();

//#ifdef _DEBUG
//	virtual void  AssertValid() const;
//	virtual void  Dump(CDumpContext& dc) const;
//#endif

protected:
	int            CheckWithSize(Event*);
	int            exec();

	RowCol         m_rcOffset;
	FieldInt       *fiRow, *fiCol;
	FieldInt       *fiWidth, *fiHeight;

	DECLARE_MESSAGE_MAP()
};

#endif GeoRefSubmapView_H
