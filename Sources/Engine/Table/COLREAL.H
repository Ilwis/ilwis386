/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52�North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52�North Initiative for Geospatial
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
/* ColumnLong
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL  27 May 97    1:53 pm
*/

#ifndef ILWCOLREAL_H
#define ILWCOLREAL_H
#include "Engine\Table\COLSTORE.H"

class DATEXPORT ColumnReal: public ColumnStoreBase
{
friend class ColumnStore;
friend class ColumnStoreBase;
public:
  virtual ~ColumnReal();
  virtual double rValue(long iKey) const;
  virtual void GetBufVal(RealBuf&, long iKey, long iNr) const;
  virtual void PutVal(long iKey, double rVal);
  virtual void PutBufVal(const RealBuf&, long iKey, long iNr);
  virtual void DeleteRec(long iStartRec, long iRecs=1);
  virtual void AppendRec(long iRecs = 1);
  virtual void Fill();
  virtual String sType() const;
private:
  ColumnReal(long iRecs, long iOffset, ColumnStore&, bool fCreate);
  RealArrayLarge buf;
};


#endif // ILWCOLREAL_H




