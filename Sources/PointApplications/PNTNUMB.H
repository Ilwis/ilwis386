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
/* $Log: /ILWIS 3.0/PointMap/PNTNUMB.H $
 * 
 * 5     16-06-00 12:07 Koolhoven
 * Now uses DomainUniqueID, no sNewDom can be specified
 * 
 * 4     4/11/99 14:18 Willem
 * Forgot to add the default value for prefix
 * 
 * 3     4/11/99 14:13 Willem
 * Changes the $Log comment
 * 
 * 2     4/11/99 14:10 Willem
 * Extended to accept a domain prefix. Changed the syntax and the form
 */
// Revision 1.3  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.2  1997/08/26 08:46:19  Wim
// Added sNewDom
//
/* PointMapNumbering
   Copyright Ilwis System Development ITC
   may 1996, by Jelle Wind
	Last change:  WK   26 Aug 97   10:27 am
*/

#ifndef ILWPNTMAPNUMB_H
#define ILWPNTMAPNUMB_H
#include "Engine\Applications\PNTVIRT.H"

IlwisObjectPtr * createPointMapNumbering(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT PointMapNumbering: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapNumbering(const FileName&, PointMapPtr&);
  PointMapNumbering(const FileName& fn, PointMapPtr&, const PointMap& pm, const String& sNewPrefix = "Pnt");
  ~PointMapNumbering();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  static PointMapNumbering* create(const FileName&, PointMapPtr&, const String& sExpression);
  void Init();
private:
  PointMap pmp;
};

#endif




