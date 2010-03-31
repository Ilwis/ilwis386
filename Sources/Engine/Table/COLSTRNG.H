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
/* ColumnString
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    1:57 pm
*/

#ifndef ILWCOLSTRING_H
#define ILWCOLSTRING_H
#include "Engine\Table\COLSTORE.H"
#include "Engine\Domain\dmstring.h"

class DATEXPORT ColumnString: public ColumnStoreBase
{
friend class ColumnStore;
friend class ColumnStoreBase;
public:
//  virtual ColumnPtr* Copy(const Table& ptr) const;
//  virtual ColumnPtr* Copy(const String& sColName) const;
  virtual ~ColumnString();
  virtual String sValue(long iKey, short iWidth=-1, short=-1) const;
  virtual void PutVal(long iKey, const String&);
  virtual void GetBufVal(StringBuf& b, long iKey, long iNr, short iWid=-1, short iDec=-1) const;
  virtual void PutBufVal(const StringBuf& b, long iKey, long iNr);
  virtual void DeleteRec(long iStartRec, long iRecs=1);
  virtual void AppendRec(long iRecs = 1);
  virtual void Fill();
  virtual String sType() const;
private:
  ColumnString(long iRecs, long iOffset, ColumnStore&, bool fCreate);
  ArrayLarge<String> buf;
};

#endif // ILWCOLSTRING_H




