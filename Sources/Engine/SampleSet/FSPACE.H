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
/* tls/fspace.h
   Feature Space Interface for ILWIS 2.0
   march 1995, by Jan-Willem Rensman
   (c) Computer Department ITC
	Last change:  JEL   7 Sep 96   10:01 am
*/

#ifndef ILWFSPACE_H
#define ILWFSPACE_H


#include "Headers\base.h"
#include "Engine\Domain\dm.h"
#include "Engine\SampleSet\SAMPLSET.H"

//#define MAXBYTE    256L
#define MXBYTsqr   65536L


// auxiliary class to store x, y and class:
class DATEXPORT FeatureSpace
{
public:
  class DATEXPORT XY
  {
  public:
    XY(short bBX, short bBY): bX(bBX), bY(bBY) {}
    ~XY(){}
    short bX, bY;
  };
 // temporarily made public for testing purposes.
//**********************************************************  
  
  _export FeatureSpace(const SampleSet& sms, short iBnd1, short iBnd2);
  _export ~FeatureSpace();
  // returns the raw value of xy (accounting for 
  //                              zoomfactor for xy)
  long  _export   iRaw(XY& xy) const;
//**********************************************************  
  void            AddSample(const RowCol&, byte bClassVal);
  void            AddSample(byte bVal1, byte bVal2, byte bClassVal);
  void            DelSample(const RowCol&);
  void            DelSample(byte bVal1, byte bVal2);
  void            AddSelect(byte bVal1, byte bVal2);
//void            DelSelect(byte bVal1, byte bVal2);
  void            DelSelect();
  short           iBand1() const { return  _iBand1; }
  short           iBand2() const { return  _iBand2; } 
//  void            Backup();
//  void            Restore();
  DomainClass* dcp() const { return dc; }
  void ReleaseMem();
  int  _export Compute();      // possibly returns errUsrABO
protected:
/*  FeatureSpace(MapSample* mpSmp, short iBnd1, short iBnd2);
  ~FeatureSpace();*/
  
  // returns the raw value of xy (not accounting for 
  // zoomfactor for xy) So it is only for internal use
  // in THIS class.
  byte            operator ()(XY& xy) const 
                  { return bFSptr[long(xy.bX) + MAXBYTE * long(xy.bY)]; }
/* returns the raw value of xy (accounting for 
                                zoomfactor for xy)
  byte            iRaw(XY& xy) const;*/
  
  // returns a linepiece of raw values of xy (accounting 
  //                          for zoomfactor for xy)
public:  
  void _export   GetLineRaw(long iLine, ByteBuf& bb, long iFrom=0, long iNum=0) const;
protected:  
  bool            fNeedCompute() { return  _fNeedCompute; }
  virtual void    _export Changed(XY& xy)=0;  // this function updates the 
                  // derived window class's window whenever it is 
                  // certain that the feature space has changed. It
                  // accounts for the zoomfactor for xy.
  short           _iBand1;  
  short           _iBand2;
  bool            _fNeedCompute;
//  int             iZoomFac;  // can attain values 0, 1, 2, 3, 4 
  byte *     bFSptr;  
  word *     bNRptr;  
  word *     bSNRptr;  
/*  byte HUGE *     bCLptr;  
  byte HUGE *     bFSBptr;  
  byte HUGE *     bNRBptr;  
  byte HUGE *     bCLBptr;  */
  SampleSet       sampleset;
  DomainClass*    dc;
  Map map1, map2;
};

#endif //ILWFSPACE_H





