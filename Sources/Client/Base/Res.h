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
// res.h
// class definitions for resources
// by Wim Koolhoven
// (c) ILWIS Department ITC

#ifndef ILWRES_H
#define ILWRES_H

//#include <windows.h>

enum StockIcon 
{
	Application=32512,
	Asterisk   =32513,
	Exclamation=32514,
	Hand	   =32515,
	Question   =32516
};

enum StockCursor 
{
	Arrow		=32512,
	Ibeam		=32513,
	Hourglass	=32514,
	Cross		=32515,
	UpArrow		=32516,
	SizeCursor	=32640,
	IconCursor	=32541,
	SizeNWSE	=32642,
	SizeNESW	=32643,
	SizeWE		=32644,
	SizeNS		=32645,
};

class zIcon
{
  HICON hi;
public:
  int fRes;
	_export zIcon();
	bool _export fLoad(const String& s);
  _export zIcon(const char*);
  _export zIcon(StockIcon);
  _export zIcon(HICON);
  _export ~zIcon();
  operator HICON() { return hi; }
};


class _export zCursor
{
  HCURSOR hc;
public:
  int fRes;
  zCursor(const char*);
  zCursor(StockCursor);
  zCursor();
  zCursor(const zCursor&);
  zCursor& operator = (const zCursor&);
  ~zCursor();
  HCURSOR &sysId() { return hc; }
  operator HCURSOR() const { return hc; }
};

void _export LoadIlwisButtonBitmap(const char* sName, CBitmap& bmp);

void _export LoadIlwisButtonBitmap(UINT uID, CBitmap& bmp);

void _export UnloadIlwisButtonBitmap(CBitmap& bmp);

void _export UnloadIlwisButtonBitmap(HBITMAP hb);

class IlwisBitmap: public CBitmap
{
public:
	void Load(const char* sName)
		{	LoadIlwisButtonBitmap(sName, *this); }
	void Load(UINT nID)
		{	LoadIlwisButtonBitmap(nID, *this); }
};



#endif
