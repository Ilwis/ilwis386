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
#define QSORT_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Qsort.h"

void QuickSort(long l, long r,
                    bool (*Less)(long, long),
                    void (*Swap)(long, long))
{ long m, i, j;
  i = l; j = r;
  m = (l + r) / 2;
  do {
    while (Less(i, m))
      i++;
    while (Less(m, j))
      j--;
    if (i <= j) {
      if (i==m)
        m = j;
      else if (j==m)
        m = i;
      Swap(i, j);
      i++;
      j--;
    }
  } while (i<j);
  if (l < j) QuickSort(l, j, Less, Swap);
  if (i < r) QuickSort(i, r, Less, Swap);
} /* QuickSort */

void QuickSort(long l, long r,
                    bool (*Less)(long, long, void*),
                    void (*Swap)(long, long, void*), void*p)
{ long m, i, j;
  i = l; j = r;
  m = (l + r) / 2;
  do {
    while (Less(i, m, p))
      i++;
    while (Less(m, j, p))
      j--;
    if (i <= j) {
      if (i==m)
        m = j;
      else if (j==m)
        m = i;
      Swap(i, j, p);
      i++;
      j--;
    }
  } while (i<j);
  if (l < j) QuickSort(l, j, Less, Swap, p);
  if (i < r) QuickSort(i, r, Less, Swap, p);
} /* QuickSort */









