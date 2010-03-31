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
/* clipline.h  rectangular lineclipping function
   Copyright Ilwis System Development ITC
   aug 1996, Jan Hendrikse
    Last change:  WK   28 Aug 98    4:44 pm
*/

#ifndef CLIPLINE_H
#define CLIPLINE_H

#include "Engine\Base\DataObjects\Dat2.h"

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

void IMPEXP Locate(const CoordBounds& cb, double rx, double ry, int& px, int& py );
void IMPEXP Relocate(const CoordBounds& cb,double& rx, double& ry, int& px, int& py,
                const Coord& cPnt0, const Coord& cPnt1 );
bool IMPEXP fClipLine(const CoordBounds& cb, Coord& cPnt0, Coord& cPnt1);

#endif CLIPLINE_H







