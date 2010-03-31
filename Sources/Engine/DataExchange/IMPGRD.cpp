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
/*
  import .GRD format
  by Li Fei, Oct. 94
  modified by Li Fei, Dec. 95
  ILWIS Department ITC
	Last change:  WN   29 Feb 96    3:06 pm
*/

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\DataExchange\Convloc.h"

void ImpExp::ImportGRD(File& FileIn, const FileName& fnObject)
{
  ErrorObject eo("Under construction", 0);
  eo.Show();
/*  Map mp1("aaa", "MapMirrorRotate(bmp,Rotate90)");
  mp1->fErase=false;
  mp1->pmv()->Freeze();*/
}

/*
  struct {
    long ES,SF,NE,NV,KX;
    double DE,DV,X0,Y0,ROT,ZBASE,ZMULT;
    unsigned char LABLE[48],MAPNO[16];
    long PROJ,UNITX,UNITY,UNITZ,NVPTS,IZMIN,IZMAX,IZMED,IZMEA;
    double ZVAR;
    long PRCS;
    unsigned char USER[324];
  }GrdHd;
  if ( 512 != FileIn.Read(512,&GrdHd) )
    throw ErrorImportExport( SDRCVIFRErr );
  int iLine=GrdHd.NV;
  int iCol=GrdHd.NE;
  Domain dm;
  switch (GrdHd.ES) {
    case 1: dm="Image"; break;
    case 2: dm="int"; break;
    default: ilwapp->mh()->Error(SDRCVIGrdErr);
             return;
  }
  int Coeff;
  float a11, a12, a21, a22, b1, b2;
  if ( GrdHd.DE==0 || GrdHd.DV==0 )
    Coeff = 0;
  else {
    float sinRot = sin(M_PI/180*GrdHd.ROT);
    float cosRot = cos(M_PI/180*GrdHd.ROT);
    bool Switched = (GrdHd.KX==-1 || GrdHd.KX==2 || GrdHd.KX==-3 || GrdHd.KX==4);
    if (Switched) {
      double Tmp8 = GrdHd.DE;
      GrdHd.DE = GrdHd.DV;
      GrdHd.DV = Tmp8;
    }
    float cx =  GrdHd.DE * cosRot;
    float lx =  GrdHd.DE * sinRot;
    float cy =  GrdHd.DV * sinRot;
    float ly = -GrdHd.DV * cosRot;
    float Det = cx * ly - lx * cy;
    if (Det== 0) 
      Coeff = 0;
    else {
      Coeff = 1;
      a11 = ly / Det;
      a12 = cy / Det;
      a21 = lx / Det;
      a22 = cx / Det;
      b1 = 1 - a11 * GrdHd.X0 - a12 * GrdHd.Y0;
      if (Switched) 
        b2 = GrdHd.NE - a21 * GrdHd.X0 - a22 * GrdHd.Y0;
      else
        b2 = iLine - a21 * GrdHd.X0 - a22 * GrdHd.Y0;
    }
  }
  GeoRef gr;
  if (Coeff==0) {
    gr=GeoRef( RowCol( (long)iLine, (long)iCol ) );
    ilwapp->mh()->fTranq(SDRCVIGrdErr2);
  }
  else {
    gr=GeoRef( RowCol( (long)iLine, (long)iCol ),
               a11, a12, a21, a22, b1, b2);
  }
  String fn=fnObject.sFile;
  {
    if ( GrdHd.KX != -2 )
      fn="a______";
    Map mp( FileName(fn), gr, dm);
    mp->fErase=true;
    IntBuf ibuf(iCol);
    for ( int i=0; i<iLine; i++ ) {
      if ( GrdHd.ES==2 )
        FileIn.Read(2*iCol, ibuf.buf() );
      else
        for ( int j=0; j<iCol; j++ )
          FileIn.Read(1, &ibuf[j] );
      mp->pms()->PutLineRaw(i, ibuf);
      if ( ilwapp->mh()->fTranq(SDRCVIMsgWk, i, iLine ) )
        return;
      if (  FileIn.err() )
        throw ErrorImportExport( FileIn.sError() );
    }
    mp->fErase=false;
  }
  if ( ilwapp->mh()->fTranq(SDRCVIMsgWk, iLine, iLine ) )
    return;
  if ( GrdHd.KX != -2 ) {
    String sOut="MapMirrorRotate(";
    sOut&=String(FileName(fn));
    switch( GrdHd.KX ) {
      case 1 : sOut&=",Rotate90)";break;  //MirrHor)";  break;
      case 2 : sOut&=",Transpose)";break;
      case 3 : sOut&=",MirrVert)"; break;
      case 4 : sOut&=",MirrDiag)"; break;
      case -1: sOut&=",Rotate90)";  break;
      case -3: sOut&=",Rotate270)"; break;
      case -4: sOut&=",Rotate180)"; break;
    }
    Map mp1(fnObject, sOut);
    mp1->fErase=false;
    mp1->pmv()->Freeze();
    ilwapp->mh()->Error("jhgff");
    if ( Coeff )
      mp1->SetGeoRef(gr);
    mp1->Store();
  }
}
*/


