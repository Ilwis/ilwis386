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
/*
// $Log: /ILWIS 3.0/ApplicationForms/frmPolygonMapUnionion.h $
 * 
 * 5     16-06-00 12:30 Koolhoven
 * sNewDom is no longer asked
 * 
 * 4     11/11/99 15:37 Willem
 * Added a callback in the from to disallow ':' (colon) as legal character
 * in the domain prefix
 * 
 * 3     5/11/99 12:01 Willem
 * Comments adapted
 * 
 * 2     4/11/99 17:01 Willem
 * Added a field to enter the domain prefix for PolygonMapUnionion operations for
 * pointmap, segmentmap and polygonmap
 */
// Revision 1.3  1998/09/16 17:32:28  Wim
// 22beta2
//
// Revision 1.2  1997/08/26 12:55:00  Wim
// Added sNewDom with AreaNumbering and PolygonMapUnionion
//
/* FormGeneralMapApplications
   Copyright Ilwis System Development ITC
   march 1997, by Wim Koolhoven
	Last change:  WK   26 Aug 97    2:51 pm
*/
#pragma once
 
#include "Client\Forms\frmpolcr.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fldcol.h"

LRESULT Cmdpolygonmapunion(CWnd *wnd, const String& s);

class _export FormPolygonMapUnionion: public FormPolygonMapCreate
{
public:
	FormPolygonMapUnionion(CWnd* mw, const char* sPar);
protected:

private:  
	int exec();
	FormEntry *CheckData();

	String sInMap1;
	String sInMap2;

	FieldReal *frDistance;
};









