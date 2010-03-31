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
/* Filter, FilterPtr
   Copyright Ilwis System Development ITC
   june 1995, by Jelle Wind
	Last change:  JEL  18 Feb 97    2:37 pm
*/

enum DirBound { dbNONE, dbUP, dbDOWN, dbLEFT, dbRIGHT };

class ChainRec {
public:
  ChainRec() { iLength = 1; dbCode = dbNONE; }
  ChainRec(DirBound db, long iLen){ iLength = iLen; dbCode = db; }  
  DirBound dbCode;  
  long iLength;
};  

//#ifdef ILWPOLFRMRAS_C
static ChainRec undef(const ChainRec& ) { return ChainRec(); }

//#endif
class SegBound {
public: 
  long iSegNr; 
  bool fBeginSeg, fEndSeg;
  Coord crdFrom, crdTo;
  DList<ChainRec> dlChain;
  long iLeftRaw, iRightRaw;
  double rLeftVal, rRightVal;
//long iForw, iBackw;
  void Insert(DirBound db, long iLength) {
    dlChain.insert(ChainRec(db, iLength));
  }  
  void Append(DirBound db, long iLength) {
    dlChain.append(ChainRec(db, iLength));
  }  
  ChainRec& crFirst() {
    return dlChain.first();
  }  
  ChainRec& crLast() {
    return dlChain.last();
  }
};
