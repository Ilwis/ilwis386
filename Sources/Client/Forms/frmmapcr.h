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
/* $Log: /ILWIS 3.0/ApplicationForms/frmmapcr.h $
 * 
 * 9     7-09-01 18:41 Koolhoven
 * removed stImage in FormMapCreate, because it is no longer needed (image
 * is now just a normal domain with fixed range and precision), and it
 * disturbes the layout
 * 
 * 8     7-08-01 19:46 Koolhoven
 * added fOutMapList which can be set by the derived exec() function to
 * indicate that the output object should be a maplist instead
 * 
 * 7     25/08/00 17:38 Willem
 * FormMapCreate now has a GeoRef member (used in Resample amongst others)
 * 
 * 6     25/08/00 14:54 Willem
 * The description of the GeoRef will now also be displayed in all raster
 * forms directly below the GeoRef field
 * 
 * 5     5-07-00 9:36 Koolhoven
 * Application Forms now use Show and Define buttons instead of OK
 * 
 * 4     6-12-99 10:20 Wind
 * adpated to use of grTypes for GeoRefField
 * 
 * 3     3/23/99 9:26a Martin
 * WinExec -> IlwisApp->Execute
 * 
 * 2     3/22/99 9:17a Martin
 * Some dependecies of .h files had changed. WinExec removed from all
// Revision 1.4  1998/09/16 17:32:28  Wim
// 22beta2
//
// Revision 1.3  1997/09/10 16:16:48  Wim
// Added SetRemarjOnBytesPerPixel()
//
// Revision 1.2  1997-08-26 16:06:27+02  Wim
// Added initRemark()
//
/* FormMapCreate
   Copyright Ilwis System Development ITC
   july 1995, by Wim Koolhoven
	Last change:  WK   10 Sep 97    6:09 pm
*/
#ifndef FRMMAPCR_H
#define FRMMAPCR_H
//#include "Client\MainWindow\mainwind.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\frmgenap.h"
#include "Client\FormElements\fentvalr.h"

class _export FormMapCreate: public FormGeneralApplication
{
protected:
  FormMapCreate(CWnd* mwin, const String& sTitle);
  void initMapOut(bool fAskGeoRef, bool fAskDomain);
  void initMapOut(bool fAskGeoRef, long dmTypes);
  void initMapOut(long grTypes, long dmTypes);
  void initMapOutGeoRef3D();
  void initMapOutValRange(bool fAskGeoRef);
  void initRemark();
  int exec();
  void execMapOut(const String& sExpression);
  String sOutMap, sGeoRef, sDomain, sDescr;
  ValueRange vr;
  int OutMapCallBack(Event*);
	int ChangeGeoRef(Event*);
  int DomainCallBack(Event*);   
  int ValueRangeCallBack(Event*);   
  void SetDefaultValueRange(const ValueRange&);
  void SetRemarkOnBytesPerPixel();
  FieldMapCreate* fmc;
  FieldGeoRefC* fgr;
  FieldDomainC* fdc;
  FieldValueRange* fvr;  
  CWnd* mw;
  StaticText* stRemark;
  StaticText *stGRDesc;
  GeoRef m_gr;
  CoordSystem m_cs;
  CoordBounds m_cb;
  bool fOutMapList;
};

#endif  // FRMMAPCR_H





