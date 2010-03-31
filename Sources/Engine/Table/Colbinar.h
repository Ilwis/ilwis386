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
/* ColumnBinary
   Copyright Ilwis System Development ITC
   march 1996, by Jelle Wind
	Last change:  WK    4 Mar 98    6:13 pm
*/

#ifndef ILWCOLBINARY_H
#define ILWCOLBINARY_H
#include "Engine\Table\COLSTORE.H"
#include "Engine\Domain\dmstring.h"

class BinMemBlock 
{
public:
  _export BinMemBlock();
  _export BinMemBlock(long iSiz, const void* p);
  BinMemBlock(const BinMemBlock& bmb);
  void operator = (const BinMemBlock& bmb);
  _export ~BinMemBlock();
  long iSize() const { return _iSize; }
  const void* ptr() const { return _ptr; }
private:  
  long _iSize;
  char* _ptr;
};

inline BinMemBlock undef(const BinMemBlock&)
{ return BinMemBlock(); }

class DATEXPORT ColumnBinary: public ColumnStoreBase
{
	friend class ColumnStore;
	friend class ColumnStoreBase;
	friend class ColumnCoordBuf;
public:
	~ColumnBinary();
	long iBufSize(long iKey) const;
	void GetVal(long iKey, BinMemBlock&) const;
	const BinMemBlock& bmbGetVal(long iKey) const;
	void PutVal(long iKey, const BinMemBlock&);
	void GetBufVal(Buf<BinMemBlock>& b, long iKey, long iNr) const;
	void PutBufVal(const Buf<BinMemBlock>& b, long iKey, long iNr);
	void DeleteRec(long iStartRec, long iRecs=1);
	void AppendRec(long iRecs = 1);
	String sType() const;
private:
  ColumnBinary(long iRecs, long iOffset, ColumnStore&, bool fCreate);
	ArrayLarge<BinMemBlock> buf;
};

#endif




