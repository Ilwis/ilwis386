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
/* ColumnCoord
   Copyright Ilwis System Development ITC
   oct 1996, by Jelle Wind
	Last change:  JEL  27 May 97    1:58 pm
*/

#ifndef ILWCOLCOORD_H
#define ILWCOLCOORD_H
#include "Engine\Table\COLSTORE.H"

class DATEXPORT ColumnCoord: public ColumnStoreBase
{
    friend class ColumnStore;
    friend class ColumnCoordXorY;
    friend class ColumnStoreBase;

    public:
        virtual ~ColumnCoord();
        virtual Coord           cValue(long iKey) const;
        virtual String          sValue(long iKey, short iWidth=-1, short iDec=-1) const;
        virtual void            GetBufVal(StringBuf&, long iKey=0, long iNr=0, short iWid=-1, short iDec=-1) const;
        virtual void            GetBufVal(CoordBuf&, long iKey=0, long iNr=0) const;
        virtual void            PutVal(long iKey, const Coord& crd);
        virtual void            PutVal(long iKey, const String& sVal);
        virtual void            PutBufVal(const StringBuf&, long iKey=0, long iNr=0);
        virtual void            PutBufVal(const CoordBuf&, long iKey=0, long iNr=0);
        virtual void            DeleteRec(long iStartRec, long iRecs=1);
        virtual void            AppendRec(long iRecs = 1);
        virtual void            Fill();
        virtual String          sType() const;
    private:
        ColumnCoord(long iRecs, long iOffset, ColumnStore&, bool fCreate);

        ArrayLarge<Coord>       buf;
        DomainCoord*            domCrd;
};

#endif // ILWCOLCOORD_H/* ColumnCoord




