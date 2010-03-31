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
/* $Log: /ILWIS 3.0/UserInterfaceObjects/Pattern.h $
 * 
 * 3     1-11-00 18:13 Koolhoven
 * added DrawColors paramer to constructor from Representation
 * 
 * 2     5/27/99 3:39p Martin
 * 
 * 1     5/27/99 8:25a Martin
// Revision 1.2  1998/09/16 17:32:28  Wim
// 22beta2
//
/* pattern.h
   Interface for Pattern
   by Wim Koolhoven, may 1996
   (c) ILWIS System Development, ITC
	Last change:  WK    6 Mar 98    1:12 pm
*/
#ifndef PATTERN_H
#define PATTERN_H

enum PolDspType { pFull, pDownHatch, pUpHatch, pCrossHatch,
                  pDiagCrossHatch, pHorzHatch, pVertHatch };

class _export Pattern
{
public:
  Pattern();
  Pattern(const Representation& rpr, long iRaw, DrawColors drc=drcNORMAL);
  Pattern(Color color);
  Pattern(PolDspType typ, Color color);
  Pattern(const short aPat[8], Color col1, Color col2);
  ~Pattern();
  void drawPolygon(CDC*, vector<Array<zPoint>>&);
  void drawRectangle(HDC, CRect*);
  bool fValid() { return 0 != br; }
private:
  Pattern(const Pattern&); // not allowed!
  void operator=(const Pattern&); // not allowed!
  CBrush* br;
  bool fTransparent;
  Color clrFG, clrBG;
};

#endif // PATTERN_H
