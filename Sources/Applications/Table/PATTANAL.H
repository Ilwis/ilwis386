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
/* TablePattAnal
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind, Dick Visser
	Last change:  DV   26 Aug 97    2:15 pm
*/

#ifndef ILWMAPPATTANAL_H
#define ILWMAPPATTANAL_H
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Point\PNT.H"

#define iNEIGHBOURS 6   // Number of Neighbours of which distances
                        // and point number are registrated for each point

IlwisObjectPtr * createTablePattAnal(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

class NearestNeighbours {
  public:
    NearestNeighbours() {}
    long iPntNr[iNEIGHBOURS];
    float rDist[iNEIGHBOURS];
};  

// needed for contruction of array:
inline NearestNeighbours undef(const NearestNeighbours&) { return NearestNeighbours(); }

class DATEXPORT TablePattAnal: public TableVirtual
{
  friend class TableVirtual;
public:  
  static const char* sSyntax();
  long iNrNeigh;      //added in revision 1.3
  TablePattAnal(const FileName& fn, TablePtr& p);
  static TablePattAnal* create(const FileName& fn, TablePtr& p, const String& sExpression);
protected:

  TablePattAnal(const FileName& fn, TablePtr& p, const PointMap& pmp);
  ~TablePattAnal();
  virtual void Store();
  virtual bool fFreezing();
  virtual String sExpression() const;
  virtual void UnFreeze();
//virtual bool fAdditionalInfo() const;
//virtual String sAdditionalInfo() const;
private:
  void Init();
  void Calculate();
  String sRNN() const;
  PointMap pmp;
  Column colDist, colProbAllPnt;
  Array<Column> colProbPnt;
  Array<double> rReflexive, rReflexiveCsr, rAvgDist, rAvgDistCsr;
  ArrayLarge<NearestNeighbours> nn;
 
  void DistPrep(long iNrPnt);
  void DistInsert(long iPntNr, long iPntNrN, float rDist);
  double rDistNeighb(long iPntNr,long iPntNrN);
  long iDistPntNr(long iPntNr,long iPntNrN)
   // Return for a point iPnrNr the iPntNrN nearest neighbour point or iUndef
    { return nn[iPntNr-1].iPntNr[iPntNrN-1]; }
};

#endif




