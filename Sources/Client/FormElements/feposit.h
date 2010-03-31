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
// ui/feposit.h Interface for positioner for form entries
// August 1994
// Jelle Wind
#ifndef ILW_FEPOS
#define ILW_FEPOS

class _export FormEntryPositioner
{
    public:
        FormEntryPositioner(); // constructor for root
        FormEntryPositioner(FormEntryPositioner *psnPar, FormEntryPositioner *psnU, FormEntryPositioner *psnL,
                            short iWid, short iHght, short iOff = 0);
        ~FormEntryPositioner();
        void                          Init();
        void                          ChangeParent(FormEntryPositioner *psn);
        void                          AlignLeft(FormEntryPositioner* psn, short iOff = 0);
        void                          AlignUp(FormEntryPositioner* psn, short iOff = 0);
        void                          MaxWidthOfColumn(short iColNr, short& iMax);
        void                          SetCol(short iColumn);
        void                          SetPos();
        void                          SetBound(short iBndL, short iBndR, short iBndU, short iBndD)
                                                { iBndLeft = iBndL; 
                                                  iBndRight = iBndR; 
                                                  iBndUp = iBndU; 
                                                  iBndDown = iBndD; }
        short                         iPosX; 
        short                         iPosY;
        short                         iWidth;
        short                         iHeight;
        short                         iBndUp;
        short                         iBndDown;
        short                         iBndLeft;
        short                         iBndRight; // outside boundary
        short                         iMinWidth; 
        short                         iMinHeight;
        short                         iOffset;
        short                         iCol;
        bool                          fIndependentPos;
        FormEntryPositioner*          psnParent;
        FormEntryPositioner*          psnUp; 
        FormEntryPositioner*          psnLeft;
        Array<FormEntryPositioner*>   psnDown;
        Array<FormEntryPositioner*>   psnRight;
        Array<FormEntryPositioner*>   psnChild;
        
    private:
        bool                          fGroup() 
                                            { return psnChild.iSize() != 0; }
        void                          SetWidthOfColumn(short iColNr, short iWid);
        void                          MaxCol(short& iMaxCol);
        void                          RemoveRight(FormEntryPositioner* psn);
        void                          RemoveDown(FormEntryPositioner* psn);
};

#endif




