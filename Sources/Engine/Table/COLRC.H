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
/* ColumnByte
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  11 Dec 95   10:27 am
*/

#ifndef ILWCOLBYTE_H
#define ILWCOLBYTE_H
#include "Engine\Table\COLSTORE.H"

class DATEXPORT ColumnByte: public ColumnStore
{
friend class ColumnPtr;
public:
  virtual long iRaw(long iKey) const;
  virtual void GetBuf(ByteBuf&, long iKey, long iNr) const;
  virtual void GetBuf(IntBuf& , long iKey, long iNr) const;
  virtual void GetBuf(LongBuf&, long iKey, long iNr) const;
  virtual void PutRaw(long iKey, long iRaw);
  virtual void PutBuf(const ByteBuf&, long iKey, long iNr);
  virtual void PutBuf(const IntBuf& , long iKey, long iNr); 
  virtual void PutBuf(const LongBuf&, long iKey, long iNr);
  virtual void Fill();
protected:
  virtual void Store();  
private:
  ColumnByte(const TablePtr* tbl, const String& sColName)
  : ColumnStore(tbl, sColName),  
    buf(tbl->iRecs()) {}
  ColumnByte(const TablePtr* tbl, const String& sColName, const Domain& dm)
  : ColumnStore(tbl, sColName, dm),  
    buf(tbl->iRecs()) {}
  ByteArrayLarge buf;
};  


#endif // ILWCOLBYTE_H




