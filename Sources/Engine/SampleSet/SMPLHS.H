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
/* tls/smplhs.h
   Sampling histogram and statistics Interface for ILWIS 2.0
   march 1995, by Jan-Willem Rensman
   (c) Computer Department ITC
	Last change:  JEL  21 Jun 97    1:15 pm
*/

#ifndef ILWSMPLHS_H
#define ILWSMPLHS_H

#define MAXCLASS 250
#define CLASSMEAN 0
#define CLASSSTD  1

#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Domain\dmclass.h"

class DATEXPORT SmplHist;
class DATEXPORT SmplStats;
class DATEXPORT SmplSum;
class DATEXPORT SmplSumXY;

class SmplHist
{
public:
  SmplHist(){ iSHptr = 0; }
  ~SmplHist();
  long& operator() (byte bKey, short iBand, byte bVal);
  void      Allocate(DomainClass* domc, const MapList& ml);
  void      AppClass(byte bKey);
  void      DelClass(byte bKey);
  void      MrgClass(byte bKey1, byte bKey2);
  void      ResetClass(byte bKey);
  RangeInt* riRanges;
  bool fExist(byte bKey) const
    { return 0 != iSHptr[bKey]; }
private:
  long***   iSHptr;
  short     iNrBands;
  DomainClass* dc;
};

class SmplStats
{
public:
  SmplStats(){ rSSptr = 0; bSSptr = 0; }
  ~SmplStats();
  double& operator()(byte bKey, short iBand, byte b) const;
  double& rMean(byte bKey, short iBand) const
    { return rSSptr[bKey][iBand][CLASSMEAN]; }
  byte& bMean(byte bKey, short iBand) const
    { return bSSptr[bKey][iBand]; }
  double& rStd(byte bKey, short iBand) const
    { return rSSptr[bKey][iBand][CLASSSTD]; }
  void      Allocate(DomainClass* domc, short iNrBands);
  void      AppClass(byte bKey);
  void      DelClass(byte bKey);
  void      ResetClass(byte bKey);
  void      MrgClass(byte bKey1, byte bKey2,
                     const SmplSum&   sSum,
                     const SmplSumXY& sSumXY);
private:
  double*** rSSptr;
  byte** bSSptr;
  short     iNrBands;
  DomainClass* dc;
};

class SmplSum
{
public:
  SmplSum(){ iSumptr = 0; }
  ~SmplSum();
  long& operator() (short bClass, short iBand) const
    { return iSumptr[bClass][iBand]; }
  void      Allocate(DomainClass* domc, short iNrBnd);
  void      AppClass(byte bKey);
  void      DelClass(byte bKey);
  void      ResetClass(byte bKey);
  void      MrgClass(byte bKey1, byte bKey2);
  double _export rPixInClass(byte bKey) const;
private:
  long** iSumptr;
  short    iNrBands;
  DomainClass* dc;
};

class SmplSumXY
{
public:
  SmplSumXY(){ iSumptr = 0;}
  ~SmplSumXY();
//  double& operator() (byte bClass, short iBand1, short iBand2) const
//    { if (iBand1 > iBand2) return rSumptr[bClass][iBand1][iBand2];
//      else return rSumptr[bClass][iBand2][iBand1]; }
  long& operator() (byte bClass, short iBand1, short iBand2) const
    { if (iBand1 > iBand2) return iSumptr[bClass][iBand1][iBand2];
      else return iSumptr[bClass][iBand2][iBand1]; }
  void      Allocate(DomainClass* domc, short iNrBnd);
  void      AppClass(byte bKey);
  void      DelClass(byte bKey);
  void      ResetClass(byte bKey);
  void      MrgClass(byte bKey1, byte bKey2);
private:
//  double*** rSumptr;
  long*** iSumptr;
  short     iNrBands;
  DomainClass* dc;
};

#endif // ILWSMPLHS_H





