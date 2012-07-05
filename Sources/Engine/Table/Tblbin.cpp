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
/* TableBinary
   A table stored in the 2.0 Binary format
   Copyright Ilwis System Development ITC
   sept 1995, by Jelle Wind
	Last change:  WK   28 Apr 98   10:19 am
*/

#include "Headers\toolspch.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Table\Colbinar.h"
#include "Engine\Table\COLCOORD.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Table\TBLBIN.H"
#include "Engine\Base\DataObjects\valrange.h"

TableBinary::TableBinary(const FileName& fn, TablePtr& p)
: TableStore(fn, p)
{
	if ( fn.fValid())
		LoadBinary(); // contents should be moved to here!
}

TableBinary::TableBinary(const FileName& fnFil, TablePtr& p,
                         const FileName& fnDat)
: TableStore(fnFil, p, fnDat)
{
}

void TableBinary::Store()
{
  if (ptr.fnObj.sFile.length() == 0)  // empty file name
    return;
  TableStore::Store();
  StoreAsBinary(fnData());
}

bool TableBinary::fWriteAvailable() const
{
  return true;
}


