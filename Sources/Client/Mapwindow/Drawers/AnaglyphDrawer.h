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
// AnaglyphDrawer.h: interface for the AnaglyphDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ANAGLYPHDRAWER_H__E3675952_EA14_4125_B0E0_E1569FD50C8B__INCLUDED_)
#define AFX_ANAGLYPHDRAWER_H__E3675952_EA14_4125_B0E0_E1569FD50C8B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(AFX_MAPDRAWER_H__EEDE7CB5_E843_11D2_B744_00A0C9D5342F__INCLUDED_)
#include "Client\Mapwindow\Drawers\MapDrawer.h"
#endif

#if !defined(AFX_STEREOPAIR_H__B57B6765_CE6C_40E4_AFC6_0D8657C9968C__INCLUDED_)
#include "Engine\Stereoscopy\StereoPair.h"
#endif

class AnaglyphDrawer : public MapDrawer     
{
  friend class AnaglyphDrawerForm;
public:
  AnaglyphDrawer(MapCompositionDoc*, const StereoPair&);
  AnaglyphDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
	virtual ~AnaglyphDrawer();
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual String sInfo(const Coord&);
  virtual int Configure(bool fShow=true);
  virtual int Setup();
  virtual String sName();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
  virtual bool fEditable();
protected:
  void GetLine(const Map& map, const RangeReal& rr,
               long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer=0);
  void GetLine(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer=0);
  StereoPair stp;
  RangeReal rrLeft, rrRight;
  enum { eREDGREEN, eREDBLUE } eColors;
	int iPixelOffSet;
};

#endif // !defined(AFX_ANAGLYPHDRAWER_H__E3675952_EA14_4125_B0E0_E1569FD50C8B__INCLUDED_)
