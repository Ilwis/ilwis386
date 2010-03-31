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
/* GeoRefCTP
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JH   17 Jan 98    2:21 pm
*/

#ifndef ILWGRCTP_H
#define ILWGRCTP_H
#include "Engine\SpatialReference\Gr.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"

class DATEXPORT GeoRefCTP: public GeoRefPtr
{
  friend class GeoRefPtr;
  friend class _export GeoRefEditor;
public:   // was protected,  made public to be accessible from FormCreateGeoRef::exec()
          //  in src\ui\fldgrf.c   
  GeoRefCTP(const FileName&, bool fUseColZ=false);
  _export GeoRefCTP(const FileName&, const CoordSystem&, RowCol, bool fUseColZ = false, bool fSubPixelPr=false);
          // fUseColZ true in case of 3D groundcontrol (grfOrtho and grfDirectLinear)
public: 
  ~GeoRefCTP();
  bool fValid() const { return _fValid; }
  virtual void Store();
  virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
  int iNr() const { return tblCTP->iRecs(); }
  RowCol rc(int i) const 
    { return RowCol(colRow->iValue(i), colCol->iValue(i)); }
  Coord crdRC(int i) const 
    { return Coord(colRow->rValue(i), colCol->rValue(i)); }
  Coord crd(int i) const 
    { return Coord(colX->rValue(i), colY->rValue(i)); }
  LatLon ll(int i) const 
    { return LatLon(colLat->rValue(i), colLon->rValue(i)); }
  int _export iRec(RowCol) const;
  int _export iRec(Coord) const;  
  void SetRowCol(int i, RowCol rc)
    { colRow->PutVal(i, rc.Row); colCol->PutVal(i, rc.Col); }
  void SetcrdRC(int i, Coord crdRC)
    { colRow->PutVal(i, crdRC.x); colCol->PutVal(i, crdRC.y); } 
  void SetCoord(int i, Coord crd)
    { colX->PutVal(i, crd.x); colY->PutVal(i, crd.y); }    
  void SetLatLon(int i, LatLon ll)
    { colLat->PutVal(i, ll.Lat); colLon->PutVal(i, ll.Lon); }
  int _export AddRec(RowCol rc, Coord crd); 
	int _export AddRec(RowCol rc, LatLon ll); 
  int _export AddRec(Coord crdRC, Coord crd); 
  int _export AddRec(Coord crdRC, LatLon ll); 
  bool fActive(int i)
    { return colAct->iValue(i)!=0; }
  void SetActive(int i, bool f)
    { colAct->PutVal(i, (long)f); }     
  virtual int Compute()
    { return 0;  }         // was abstract, made empty to allow creating georefCTP 
                          // instances in many functions using it
  enum Transf { CONFORM, AFFINE,                 // this member is placed in parent in
                SECONDORDER, FULLSECONDORDER,    // stead of in ctpplanar (where it is exclusively used)
                THIRDORDER, PROJECTIVE } transf; // to remain accessible for current tiepoint editor
                                                 // (mainly for compile and link reasons)
  virtual int iMinNr() const  // was abstract, made empty to allow creating georefCTP 
    { return 2; }       // instances in functions in \stc\ui\fldgrf.c using it 		
  Table tbl() const { return tblCTP; }
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();	
	
  virtual bool fDependent() const;
  FileName fnBackgroundMap;
	bool fHasSubpixelPrecision() const { return fSubPixelPrecision; }

protected:
  Table tblCTP;
  Column colX, colY, colZ, colRow, colCol, colAct; // RowCol and Coord should become column types !
  Column colLat, colLon;
  bool _fValid;
	int iActive;
  bool fSubPixelPrecision;
};

#endif // ILWGRCTP_H




