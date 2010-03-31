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
/* $Log: /ILWIS 3.0/BasicDataStructures/LayoutObject.h $
 * 
 * 5     20-11-00 10:42a Martin
 * implemented fUsesDependentObjects()
 * 
 * 4     11/01/00 2:05p Martin
 * changed the GetObjectStructure function, uses now the container variant
 * of AddFile
 * 
 * 3     30-10-00 2:16p Martin
 * added getobjectstructure
 * 
 * 2     11/07/00 11:23 Willem
 * Added sType function
 * 
 * 1     11/07/00 10:53 Willem
 * Moved LayoutObject from LayoutDoc
 * 
 */

#ifndef ILW_LAYOUT_OBJECT_H
#define ILW_LAYOUT_OBJECT_H

#pragma warning( disable : 4786 )

class _export LayoutObjectPtr: public IlwisObjectPtr
{
public:
	LayoutObjectPtr(); 
	LayoutObjectPtr(const ElementContainer& ec) ;
	String sType() const;
	void Store() ;
	void GetObjectStructure( ObjectStructure& os);
	bool fUsesDependentObjects() const	;
};

class _export LayoutObject: public IlwisObject
{
public:
	LayoutObject() ;
	LayoutObject(const ElementContainer& ec) ;
	LayoutObject(const LayoutObject& lo);

private:
	static IlwisObjectPtrList listLayoutObject;
};

#endif
