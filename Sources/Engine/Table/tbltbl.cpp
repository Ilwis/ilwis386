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
/* TableTBL
   A table stored as in the 1.x .TBL format
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   28 Apr 98   10:20 am
*/

#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Table\tbltbl.h"
#include "Engine\Domain\Dmvalue.h"


// implementation not checked!

TableTBL::TableTBL(const FileName& fn, TablePtr& p)
: TableStore(fn, p)
{
  File fil(fnData(), facRO);
  int c;
  Array<bool> fStored(iCols());
  for (c = 0; c < iCols(); c++)
  {
    String s, sColName;
    fStored[c] = false;
    if (ac[c]->st() == stBINARY)
      continue;
    if (ac[c].fValid()) {
      sColName = ac[c]->sName();
      ptr.ReadElement(ptr.sSection(String("Col:%S", sColName)).c_str(), "Stored", s);
      if (s.length())
        fStored[c] = s.fVal();
      else
        fStored[c] = true;
    }
  }

  char *sBuffer = new char[10001];
  // skip header
  fil.ReadLnAscii(sBuffer, 10000);
  // read columns
  char *sBuf, *str;
  for (long r = 0; r < iRecs(); r++) {
    long rec = r + iOffset();
    sBuf = sBuffer;
    bool fEOL = !fil.ReadLnAscii(sBuf, 10000);
    for (c = 0; c < iCols(); c++) {
      if (!fStored[c]) // skip reading
        continue;
      if (!fEOL) {
        // skip spaces
        while (isspace((unsigned char)*sBuf))
          sBuf++;
        str = sBuf;
        // find non space
        while (*sBuf && !isspace((unsigned char)*sBuf))
          sBuf++;
        *sBuf++ = '\0';
      }
      if (!ac[c]->dvrs().fValues())
        for (int i=0; i < strlen(str); i++)
          if (str[i] == '_') str[i] = ' ';
      ac[c]->PutVal(rec, str);
    } // for c
  } // for r
  delete [] sBuffer;
  for (c = 0; c < iCols(); c++)
    ac[c]->fChanged = false;
}

TableTBL::TableTBL(const FileName& fnFil, TablePtr& p,
                         const FileName& fnDat)
: TableStore(fnFil, p, fnDat)
{
}

void TableTBL::Store()
{
  StoreAsTBL(fnData());
}





