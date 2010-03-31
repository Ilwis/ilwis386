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
/* random.c - random generator for ILWIS 1.4
 * written by Wim Koolhoven
 * Source from .EXE vol 7, issue 6, page 51.
	Last change:  WK   28 Aug 98    4:09 pm
 */
#define RANDOM_C
#include "Headers\toolspch.h"
#include "Engine\Base\Algorithm\Random.h"

#define NK 37
#define NJ 24
#define NV 14

static unsigned long rnb[NK];
static int ptr=-1;

static unsigned long borrow=0L;
void useed();

unsigned long urand()
{
	if (ptr == -1)
		seedrand((unsigned)time( NULL ) );

  ptr++;
  if (ptr<NK) return rnb[ptr];
  ptr = 0;
	useed();
  return rnb[0];
}

void useed()
{
  int i,k;
  unsigned long pdiff,t;
  for (i=0,k=NK-NJ;i<NK;i++,k++)
  {
    if (k==NK) k = 0;
    t = rnb[k];
    pdiff=t-rnb[i]-borrow;
    if (pdiff<t) borrow = 0;
    if (pdiff>t) borrow = 1;
    rnb[i] = pdiff;
  }
}

void seedrand(unsigned long seed)
{
  int i,in;
  unsigned long m=1L;
  borrow = 0L;
  ptr = 0;
  rnb[0] = seed;
  for (i=1;i<NK;i++)
  {
    in=(NV*i)%NK;
    rnb[in]=m;
    m=seed-m;
    seed=rnb[in];
  }
  for (i=0;i<1000;i++)
    urand();
}

double random()
{

  double r1 = urand();
  return r1 / ULONG_MAX;
//  double r1 = urand();
//  double r2 = ULONG_MAX;
//  return (r1 + r2 + 1) / ( 2 * r2 + 1);
}

long random(long iMax)
{ if (iMax == 0)
    return urand() % 2;
  if (iMax == 1)
    return 1;
  if (iMax < 0)
    return iUNDEF;
  long k, l, m;
  do {
    k = urand();
    l = k % iMax;
    if (l < 0) l = -l;
    m = l + 1;
  } while (abs(k / iMax) >= (LONG_MAX / iMax));
  return m;
}








