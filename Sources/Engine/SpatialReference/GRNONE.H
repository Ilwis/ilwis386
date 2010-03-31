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
/* GeoRefNone
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  31 May 96    2:23 pm
*/


#ifndef ILWGRNONE_H
#define ILWGRNONE_H

class DATEXPORT GeoRefNone: public GeoRefPtr
{
  friend class GeoRef;
  friend class GeoRefPtr;
  static GeoRefNone* create(const FileName& fn, const String& sExpression);
public:  
  GeoRefNone(const FileName& fn, RowCol rc)
    : GeoRefPtr(fn, rc) {}
  static const char* sSyntax();
protected:
  GeoRefNone(const FileName& fn)
    : GeoRefPtr(fn) {}
  virtual void _export Store();
public:
  virtual void _export Coord2RowCol(const Coord& c, double& rRow, double& rCol) const;
  virtual void _export RowCol2Coord(double rRow, double rCol, Coord& c) const;
//  virtual double rPixSize() const;
  virtual bool _export fEqual(const IlwisObjectPtr&) const;
	virtual bool _export fGeoRefNone() const;
};

#endif




