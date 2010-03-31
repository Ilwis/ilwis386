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
/* $Log: /ILWIS 3.0/Representation/Rprclass.h $
 * 
 * 7     10/30/01 16:59 Willem
 * Added the GetObjectStructure function now to be able to add the domain
 * reference for the [table] ODF section
 * 
 * 6     8/24/01 13:03 Willem
 * Removed the SetReadOnly() function. This is now handled by
 * IlwisObjectPtr::SetReadOnly() for all ilwis objects
 * 
 * 5     8/01/01 2:56p Martin
 * record is now in the Ilwis namepsace
 * 
 * 4     13-09-00 4:40p Martin
 * added _Export
 * 
 * 3     5/27/99 4:39p Martin
 * //->/*
 * 
 * 2     5/27/99 4:37p Martin
 * added some exports
// Revision 1.2  1998/09/16 17:25:53  Wim
// 22beta2
//
/* RepresentationClass
   by Wim Koolhoven
  (c) Ilwis System Development ITC
	Last change:  WK   11 Mar 98    9:24 am
*/

#ifndef ILWRPRCLASS_H
#define ILWRPRCLASS_H
#include "Engine\Representation\Rpritems.h"
#include "Engine\Table\tblstore.h"

namespace Ilwis
{
class DATEXPORT Record;
}

// iPattern:
// 0    = full solid color
// 9999 = use pattern data
// 1..6 = hatching, transparent background
// 0x11..0x16 = hatching with background
// 0x21..0x26 = hatching with background, inversed colors

class RepresentationClass: public RepresentationItems
{
	friend class RepresentationPtr;
	RepresentationClass(const FileName&);
public:
	RepresentationClass(const FileName&, const Domain&);
	RepresentationClass(const FileName&, long iNr);
	virtual ~RepresentationClass();

	void CheckNrRecs() { tbl->CheckNrRecs(); }
	Ilwis::Record _export rec(long iRaw) const;

	virtual String sType() const;
	virtual void Store();
	virtual void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
	virtual void _export GetObjectStructure(ObjectStructure& os);

	long _export iAdd(const Color&);
	void _export PutColor(long iRaw, Color);
	void _export PutSecondColor(long iRaw, Color);
	
	void _export PutPattern(long iRaw, short iPattern);
	void _export PutPatternData(long iRaw, const short*);
	void _export PutSymbolType(long iRaw, short iSmb);
	void _export PutSymbolSize(long iRaw, short iSize);
	void _export PutSymbolWidth(long iRaw, short iWidth);
	void _export PutSymbolColor(long iRaw, Color);
	void _export PutSymbolFill(long iRaw, Color);
	void _export PutSymbolFont(long iRaw, const String&);
	void _export PutSymbolRotation(long iRaw, double rRot);
	void _export PutLine(long iRaw, short iLine);
	void _export PutLineFillColor(long iRaw, Color);
	void _export PutLineWidth(long iRaw, double rWidth);
	void _export PutLineDist(long iRaw, double rDist);
	_export static const int iSIZE_FACTOR;
private:
	Table tbl;
};

#endif // ILWRPRCLASS_H




