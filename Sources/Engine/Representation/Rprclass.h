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
	void _export PutTransparency(long iRaw, double transp);
	_export static const int iSIZE_FACTOR;
private:
	Table tbl;
};

#endif // ILWRPRCLASS_H




