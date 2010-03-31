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
// $Log: REALFRMT.H $
// Revision 1.4  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.3  1997/09/13 14:37:07  Wim
// Added getChars()
//
// Revision 1.2  1997-09-13 16:02:38+02  Wim
// Should now work as wished
//
// Revision 1.1  1997-09-13 14:53:57+02  Wim
// Initial revision
//
// Real Formatter,
// to format a DoubleEdit
// replacement for zNumFormatter

#include "Client\Base\ZappToMFC.h"
#include "Engine\Base\DataObjects\Pair.h"

//#include "Engine\Base\DataObjects\valrange.h"

class _export RealFormatter: public zFormatter
{
public:
  RealFormatter();
  virtual zFormatterStat addChars(String &s, const char *c, int count, zRange &pos);
  virtual bool    checkChars(String &s, zRange &pos);
  virtual int     getChars(String &str, unsigned long flags);
  virtual int     getFracDigits();
private:
  bool fAdd( const char& c);
  int i, iDot, iExp;
};





