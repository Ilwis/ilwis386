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


#ifndef ILWMAPHECKBERT_H
#define ILWMAPHECKBERT_H
#include "Engine\Applications\MAPFMPL.H"

struct ColorCubeRec { // defines cube in color space
  short iRGBMin[3], iRGBMax[3]; // min/max of cube (R, G, B)
  short iFirst;                 // first record in HistRGB that lays in cube
  short iNumRec;                // nr. records in HistRGB that lay in cube
  long iCount;                  // total nr. of pixels that lay in cube
};  

struct HistRGBRec {// RGB value with nr. of occurences in 3 input bands
  union {
    struct rgb { unsigned int b:5, g:5, r:5, x:1; } c;
    short iRGB;   // contains 5 bits (32 intensities) per color: 0rrrrrgggggbbbbb
  };
  long iCount; // nr of occurrences
};
  
inline ColorCubeRec undef(const ColorCubeRec&) 
{ ColorCubeRec x;  return x; }

inline HistRGBRec undef(const HistRGBRec&) 
{ HistRGBRec x; return x; }

IlwisObjectPtr * createMapHeckbert(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );
ApplicationMetadata metadataMapHeckbert(ApplicationQueryData *query);

class DATEXPORT MapHeckbert: public MapFromMapList
{
public:
  virtual void Store();
  virtual bool fFreezing();
  virtual String sExpression() const;
  MapHeckbert(const FileName& fn, MapPtr&);
  MapHeckbert(const FileName& fn, MapPtr&, const MapList& mpl, int iColors);
  ~MapHeckbert();
  static MapHeckbert* create(const FileName& fn, MapPtr&, const String& sExpr);
  static const char* sSyntax();
  virtual void InitName(const FileName& fn);
private:

  int iHeckCalc(ArrayLarge<ColorCubeRec>& pNewColor, int iColorComb, long iTotal, int iNumColors);
public:
  ArrayLarge<HistRGBRec> HistRGB;

  short iSortRGB;
private:  
  int iNumColors;  
};

#endif




