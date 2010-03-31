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
// MapListColorCompDrawer.h: interface for the MapListColorCompDrawer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAPLISTCOLORCOMPDRAWER_H__16CA23D4_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_)
#define AFX_MAPLISTCOLORCOMPDRAWER_H__16CA23D4_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MapListColorCompDrawer : public MapDrawer    
{
  friend class MapListColorCompDrawerForm;
public:
  MapListColorCompDrawer(MapCompositionDoc*, const MapList&);
  MapListColorCompDrawer(MapCompositionDoc*, const MapView&, const char* sSection);
	virtual ~MapListColorCompDrawer();
	//{{AFX_VIRTUAL(MapDrawer)
  virtual int draw(CDC*, zRect, Positioner*, volatile bool* fDrawStop);
  virtual String sInfo(const Coord&);
  virtual int Configure(bool fShow=true);
  virtual int Setup();
  virtual String sName();
  virtual void WriteLayer(MapView&, const char* sSection);
  virtual IlwisObject obj() const;
	virtual zIcon icon() const;
  virtual bool fEditable();
//  virtual void Prop();
	//}}AFX_VIRTUAL
	//{{AFX_MSG(MapListDrawer)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
  MapList ml() const { return maplist; }
	Map mapRed() const { return maplist[iRed]; }
	Map mapGreen() const { return maplist[iGreen]; }
	Map mapBlue() const { return maplist[iBlue]; }
protected:
  void GetLine(const Map& map, const RangeInt& ri, const RangeReal& rr,
               const Array<long>& array,
               long iLine, ByteBuf& buf, long iFrom, long iNum, int iPyrLayer=0);
  void GetLine(long iLine, LongBuf& buf, long iFrom, long iNum, int iPyrLayer=0);
  void InitArray(const Map& map, const RangeInt& ri, Array<long>& array);
  MapList maplist;
  int iRed, iGreen, iBlue;
  RangeInt riRed, riGreen, riBlue;
  RangeReal rrRed, rrGreen, rrBlue;
  Array<long> aRed, aGreen, aBlue;
	enum { eRGB, eYMC, eHSI } eMethod;
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_MAPLISTCOLORCOMPDRAWER_H__16CA23D4_5B8E_11D3_B7D6_00A0C9D5342F__INCLUDED_)
