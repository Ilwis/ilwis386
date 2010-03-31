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
/* CoordSystemCTP
   Copyright Ilwis System Development ITC
   april 1998, by Wim Koolhoven
	Last change:  WK   17 Apr 98   12:41 pm
*/

#ifndef ILWCSCTP_H
#define ILWCSCTP_H
#include "Engine\SpatialReference\csdirect.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Base\Algorithm\Fpolynom.h"

class CoordSystemCTP: public CoordSystemDirect
{
  friend class CoordSystemPtr;
  friend class _export CoordSystemEditor;
public:
  CoordSystemCTP(const FileName&, bool fUseColZ=false);
  _export CoordSystemCTP(const FileName&, const CoordSystem& csRef, const FileName& fnBackgrMap, bool fUseColZ=false);
     // fUseColZ true in case of 3D groundcontrol (grfOrtho and grfDirectLinear)
  _export ~CoordSystemCTP();
  virtual void _export Store();
  virtual String _export sType() const;
  virtual void _export GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  bool fValid() const { return _fValid; }
  bool fActive(int i)
    { return colAct->iValue(i)!=0; }
  void SetActive(int i, bool f)
    { colAct->PutVal(i, (long)f); }     
  Coord crd(int i) const
    { return Coord(colX->rValue(i), colY->rValue(i)); }
  Coord crdRef(int i) const
    { return Coord(colRefX->rValue(i), colRefY->rValue(i)); }
  void SetCoord(int i, Coord crd)
    { colX->PutVal(i, crd.x); colY->PutVal(i, crd.y); }    
  void SetCoordRef(int i, Coord crd)
    { colRefX->PutVal(i, crd.x); colRefY->PutVal(i, crd.y); }
  int iNr() const { return tblCTP->iRecs(); }
  int _export iRec(const Coord&) const;
  int _export AddRec(const Coord& crd, const Coord& crdRef);
  Table tbl() const { return tblCTP; }
	void GetObjectStructure(ObjectStructure& os)	;	
  //enum Transf { CONFORM, AFFINE,
  //              SECONDORDER, FULLSECONDORDER,
  //              THIRDORDER, PROJECTIVE } transf;
  FileName fnBackgroundMap;
protected:
  virtual bool _export fInverse() const;
  //virtual Coord cConvFromOther(const Coord&) const;
  //virtual Coord cConvToOther(const Coord&) ;
//private:
  //String sFormula() const;
  virtual int Compute() = 0;
  virtual int iMinNr() const = 0;
  Table tblCTP;
  Column colX, colY, colRefX, colRefY, colRefZ, colAct;
  bool _fValid;
	int iActive; // number of active ground contr points
};

#endif // ILWCSCTP_H


