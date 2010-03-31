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
/* $Log: /ILWIS 3.0/SegmentMap/SEGNUMB.H $
 * 
 * 4     16-06-00 12:21 Koolhoven
 * now create a DomainUinqueID. specifying a domain name has no influence
 * anymore
 * 
 * 3     10-11-99 4:28p Martin
 * // -> /*
 * 
 * 2     4/11/99 16:57 Willem
 * Extended the syntax to accept a domain prefix
// Revision 1.3  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.2  1997/08/26 08:46:10  Wim
// Added sNewDom
//
/* SegmentMapNumbering
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   26 Aug 97   10:27 am
*/

#ifndef ILWSEGMAPNUMB_H
#define ILWSEGMAPNUMB_H
#include "Engine\Applications\SEGVIRT.H"

IlwisObjectPtr * createSegmentMapNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT SegmentMapNumbering: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
public:
  SegmentMapNumbering(const FileName&, SegmentMapPtr& p);
  SegmentMapNumbering(const FileName& fn, SegmentMapPtr& p, const SegmentMap& sm, const String& sNewPrefix);
  ~SegmentMapNumbering();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  static SegmentMapNumbering* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
  void Init();
private:
  SegmentMap smp;
};

#endif




