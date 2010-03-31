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
/* MapColorComp
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
   Last change:  JEL   7 May 97    9:32 am
*/

#ifndef ILWMAPCOLCOMP_H
#define ILWMAPCOLCOMP_H
#include "Engine\Applications\MAPFMPL.H"
#include "Engine\Table\TBLHIST.H"

IlwisObjectPtr * createMapColorComp(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );


class DATEXPORT MapColorComp: public MapFromMapList
{
public:
  virtual void Store();
  virtual String sExpression() const;
  MapColorComp(const FileName& fn, MapPtr&);
  MapColorComp(const FileName& fn, MapPtr&, const MapList& mpl, double _rPerc1, const RangeReal& _rr1,
               double _rPerc2, const RangeReal& _rr2, double _rPerc3, const RangeReal& _rr3, bool fLin, bool f24Bit, bool _fHSI=false);
  ~MapColorComp();
  void InitCalc();
  void InitCalc(const Map& mp, double rPerc, RangeReal& rr, Array<long>& iBnd, int iInf);
  static MapColorComp* create(const FileName& fn, MapPtr&, const String& sExpr);
  static const char* sSyntax();
  virtual long iComputePixelRaw(RowCol) const;
//virtual void ComputeLineRaw(long Line, ByteBuf&, long iFrom, long iNum) const;
//virtual void ComputeLineRaw(long Line, IntBuf&, long iFrom, long iNum) const;
  virtual void ComputeLineRaw(long Line, LongBuf&, long iFrom, long iNum) const;
private:   
  RangeReal rr1, rr2, rr3;
  double rPerc1, rPerc2, rPerc3;
  Array<long> iBnd1, iBnd2, iBnd3;
  bool fLinear, f24Bit, fHSI;
  TableHistogramInfo histinf[3];
};

#endif




