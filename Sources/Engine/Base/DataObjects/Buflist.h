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
/* tls/buflist.h
   BufList class for ILWIS 2.0
   march 1995, by Jan-Willem Rensman
   (c) Computer Department ITC
	Last change:  WK   28 Aug 98    4:14 pm
*/

#ifndef BUFLIST_H
#define BUFLIST_H

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP ByteBufList
{
public:
  ByteBufList(short iBufs, short iSize);
  ~ByteBufList();
  ByteBuf& operator [] (short i) { return bufs[i]; }
  const ByteBuf& operator [] (short i) const { return bufs[i]; }
protected:
  ByteBuf* bufs;
};

class IMPEXP IntBufList
{
public:
  IntBufList(short iBufs, short iSize);
  ~IntBufList();
  IntBuf& operator [] (short i) { return bufs[i]; }
  const IntBuf& operator [] (short i) const { return bufs[i]; }
protected:
  IntBuf* bufs;
};

class IMPEXP LongBufList
{
public:
  LongBufList(short iBufs, short iSize);
  ~LongBufList();
  LongBuf& operator [] (short i) { return bufs[i]; }
  const LongBuf& operator [] (short i) const { return bufs[i]; }
protected:
  LongBuf* bufs;
};

class IMPEXP RealBufList
{
public:
  RealBufList(short iBufs, short iSize);
  ~RealBufList();
  RealBuf& operator [] (short i);
  const RealBuf& operator [] (short i) const { return bufs[i]; }
protected:
  RealBuf* bufs;
};


#endif







