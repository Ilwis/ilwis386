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
// $Log: FEPOSIT.C $
// Revision 1.3  1998/09/16 17:37:53  Wim
// 22beta2
//
// Revision 1.2  1997/09/02 14:17:00  Wim
// Only touch own arrays in destructor.
//
#define FEPOSIT_C
#include "Client\Headers\formelementspch.h"

FormEntryPositioner::FormEntryPositioner() // constructor for root
{ 
    Init();
    fIndependentPos = true;
}

FormEntryPositioner::FormEntryPositioner(FormEntryPositioner *psnPar, 
                                         FormEntryPositioner *psnU,
                                         FormEntryPositioner *psnL,
                                         short iWid, short iHght, short iOff)
{ 
    assert(psnPar); // parent should be defined
    assert(!psnU || !psnL); // not both positioners for alignment allowed
    Init();
    psnParent = psnPar;
    psnParent->psnChild &= this;
    AlignLeft(psnL);
    AlignUp(psnU);
    iOffset = iOff;
    iMinWidth = iWid;
    iMinHeight = iHght;
    iHeight = iMinHeight;
    fIndependentPos = false;
}

void FormEntryPositioner::Init()
{ 
    psnParent = psnUp = psnLeft = 0;
    iCol = -1;
    iMinWidth = iMinHeight = 0;
    iWidth = iHeight = 0;
    iPosX = iPosY = 0;
    iBndUp = iBndDown = iBndLeft = iBndRight = 0;
    iOffset = 0;
}

FormEntryPositioner::~FormEntryPositioner()
{
    short i, j;
    for (i = 0; i < psnChild.iSize(); i++)
        delete psnChild[i];
    if (psnParent) 
    {
        for (j = 0; j < psnParent->psnChild.iSize(); j++)
        {
            if (psnParent->psnChild[j] == this)
                psnParent->psnChild.Remove(j, 1);
        }
    }
}

void FormEntryPositioner::ChangeParent(FormEntryPositioner *psn)
{
    if (psnParent) 
    {
        for (short i = 0; i < psnParent->psnChild.iSize(); i++)
        if (psnParent->psnChild[i] == this)
            psnParent->psnChild.Remove(i, 1);
    }
    psnParent = psn;
    if (psnParent)
        psnParent->psnChild &= this;
}

void FormEntryPositioner::MaxWidthOfColumn(short iColNr, short& iMax)
// gets the maximum of the width of each positioner in column iColNr
// positioner checks it's child positioners
{
    if (fIndependentPos) 
    {
        short iMaxNew = 0;
        if ((iCol == iColNr) && (iMinWidth > iMaxNew))
            iMaxNew = iMinWidth;
        for (short i = 0; i < psnChild.iSize(); i++)
            psnChild[i]->MaxWidthOfColumn(iColNr, iMaxNew);
        SetWidthOfColumn(iColNr, iMaxNew);
    }
    else 
    {
        if ((iCol == iColNr) && (iMinWidth > iMax))
            iMax = iMinWidth;
        for (short i = 0; i < psnChild.iSize(); i++)
            psnChild[i]->MaxWidthOfColumn(iColNr, iMax);
    }
}

void FormEntryPositioner::SetWidthOfColumn(short iColNr, short iWid)
// sets the the width of each positioner in column iColNr to iWid
// positioner sets it's child positioners
{
    if (iCol == iColNr)
        iWidth = iWid;
    for (short i = 0; i < psnChild.iSize(); i++)
    {
        if (!psnChild[i]->fIndependentPos)
            psnChild[i]->SetWidthOfColumn(iColNr, iWid);
    }
}

void FormEntryPositioner::SetPos()
{
  if (psnLeft) 
  {
    iPosX = psnLeft->iPosX + psnLeft->iWidth +
            psnLeft->iBndRight + iBndLeft + iOffset;
    iPosY = psnLeft->iPosY + iBndUp;
  }
  else
  {
    if (psnParent) 
    {
      iPosX = psnParent->iPosX + iBndLeft;
      iPosY = psnParent->iPosY + iBndUp;
    }
    else
      iPosX = iBndLeft;
  }
  if (psnUp) 
  {
      iPosY = psnUp->iPosY + psnUp->iHeight + psnUp->iBndDown + iBndUp + iOffset;
      if (!psnLeft)
        iPosX = psnUp->iPosX + iBndLeft;

  }
  else
  {
    if (!psnLeft) 
    {
      if (psnParent) 
      {
        iPosX = psnParent->iPosX + iBndLeft;
        iPosY = psnParent->iPosY + iBndUp;
      }
      else
        iPosY = iBndUp;
    }
  }
  for (short i = 0; i < psnChild.iSize(); i++)
    psnChild[i]->SetPos();
  if (psnParent) 
  {
    psnParent->iWidth = max(psnParent->iWidth,
                            (short)((iPosX - psnParent->iPosX) + iWidth + iBndRight));
    psnParent->iHeight = max(psnParent->iHeight,
                             (short)((iPosY  - psnParent->iPosY) + iHeight + iBndDown));
  }
}

void FormEntryPositioner::RemoveRight(FormEntryPositioner* psn)
{
    for (short i=0; i < psnRight.iSize(); i++)
    {
        if (psnRight[i] == psn)
            psnRight.Remove(i, 1);
    }
}

void FormEntryPositioner::RemoveDown(FormEntryPositioner* psn)
{
    for (short i=0; i < psnDown.iSize(); i++)
    {
        if (psnDown[i] == psn)
            psnDown.Remove(i, 1);
    }
}

void FormEntryPositioner::AlignLeft(FormEntryPositioner* psn, short iOff)
{
    if (psn == psnLeft) 
    {
        iOffset = iOff;
        return;
    }
    if (psnUp)  // remove alignment that now exists
        AlignUp(0);
    if (psnLeft != 0)  // remove alignment
        psnLeft->RemoveRight(this);
    psnLeft = psn;
    if (psnLeft == 0) return;
    psnLeft->psnRight &= this;
    iOffset = iOff;
}

void FormEntryPositioner::AlignUp(FormEntryPositioner* psn, short iOff)
{
    if (psn == psnUp) 
    {
        iOffset = iOff;
        return;
  }
  if (psnLeft) // remove alignment that now exists
    AlignLeft(0);
  if (psnUp != 0)  // remove alignment
    psnUp->RemoveDown(this);
  psnUp = psn;
  if (psnUp == 0) return;
  psnUp->psnDown &= this;
  iOffset = iOff;
}

void FormEntryPositioner::SetCol(short iColumn)
{
    iCol = iColumn;
    if (fGroup())
        psnChild[0]->SetCol(iCol);
    short i;
    for (i=0; i < psnDown.iSize(); i++)
        psnDown[i]->SetCol(iCol);
    if (psnRight.iSize())
    {
        if (!fGroup())
        {
            for (i=0; i < psnRight.iSize(); i++)
                psnRight[i]->SetCol(iCol+1);
        }
        else 
        {
            short iMaxCol = 0;
            MaxCol(iMaxCol);
            for (i=0; i < psnRight.iSize(); i++)
                psnRight[i]->SetCol(iMaxCol+1);
        }
    }
}

void FormEntryPositioner::MaxCol(short& iMaxCol)
{
    if (iCol > iMaxCol)
        iMaxCol = iCol;
    for (short i = 0; i < psnChild.iSize(); i++)
        psnChild[i]->MaxCol(iMaxCol);
}





