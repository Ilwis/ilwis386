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
/* Impras.c
   by Li Fei 18 Sep 95
   Last change:  DV    8 Aug 97   11:59 am
*/
#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"

class ErrorImportRaster {
public:
  ErrorImportRaster(const String& ss) { s=ss; sExcError(); }
  String& sExcError() { return s; }
private:
  String s;
};

void ImpExp::ImpRas(const FileName& fnFile, const FileName& fnObject) {
  try {
    File FileIn( fnFile );
//    if ( FileIn.err() )
//      throw ErrorImportRaster( FileIn.sError() );
    if (".GIS" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportErdasLG);
      ImportGISLAN(FileIn, fnObject);
    }
    else if (".LAN" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportErdasLG);
      ImportGISLAN(FileIn, fnObject);
    }
    else if (".DOC" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportIDRISI);
      ImportIDRISI(fnFile, fnObject);
    }
    else if (".TIF" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportTIFF);
      ImportTIF(FileIn, fnObject);
    }
    else if (".GIF" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportGIF);
      ImportGIF(FileIn, fnObject);
    }
    else if (".PCX" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportPCX);
      ImportPCX(FileIn, fnObject);
    }
    else if (".NAS" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportNAS);
      ImportNAS(FileIn, fnObject);
    }
    else if (".BMP" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportBMP);
      ImportBMP(FileIn, fnObject);
    }
    else if (".ASC" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportIlwisASC);
      ImportASC(FileIn, fnObject);
    }
    else if (".IMG" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportIDA);
      ImportIMG(FileIn, fnObject);
    }
/*    else if (".LIP" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportLIP);
      ImportLIP(FileIn, fnObject);
    }
    else if (".GRD" == fnFile.sExt) {
      trq.SetTitle(SCVTitleImportGeoGrid);
      ImportGRD(FileIn, fnObject);
    }*/
    else {
      trq.SetTitle(SCVTitleImportOther);
//	ImportOTH(FileIn, fnObject);
    }
  }
  catch (ErrorImportRaster err) {
    ErrorObject eo(err.sExcError(), 0);
    eo.Show();
  }
}


