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
#ifndef RRPEDITFRM_H
#define RPREDITFRM_H

class MapDrawer;

class _export MapRprEditForm: public FormBaseDialog
{
public:
  MapRprEditForm(CWnd* wPar, RepresentationClass* rc, long raw, bool fModal=false);
  int exec();
private:
  RepresentationClass* rcl;  
  ColorSelector* cs;
  int CustomColor(Event*);
  long iRaw;
  Color col;
};

class _export SegmentMapRprEditForm: public FormBaseDialog
{
public:
  SegmentMapRprEditForm(CWnd* wPar, 
                        RepresentationClass* rc, long raw, bool fModal=false);
  int exec();
private:
  RepresentationClass* rcl;  
  ColorSelector* cs;
  int CustomColor(Event *);
  long iRaw;
  Color col;
//  int iWidth;
  Line line;
};

class _export PolygonMapRprEditForm: public FormBaseDialog
{
public:
  PolygonMapRprEditForm(CWnd* wPar, 
                        RepresentationClass* rc, long raw, bool fModal=false);
  int exec();
private:
  RepresentationClass* rcl;  
  ColorSelector* cs;
  RadioGroup* rgType;
  FieldFillColor *feHatchBG;
  FieldColor *fePatFG, *fePatBG, *feHatching;
  PushButton *pbInverse;
  int CustomColor(Event*);
  int OptionChanged(Event*);
  int InverseColors(Event*);
  long iRaw;
  Color col, col2, colHBG;
  int iType; // Solid, Hatch, Pattern
  bool fInverse;
  long pHatch, pPattern;
};

class _export PointMapRprEditForm: public FormBaseDialog
{
public:
  PointMapRprEditForm(CWnd* wPar, 
                      RepresentationClass* rc, long raw, bool fModal=false);
  int exec();
private:
  RepresentationClass* rcl;  
  FieldExtendedSymbol* fs;
  long iRaw;
  ExtendedSymbol smb;
};

#endif // RPREDITFRN_H

