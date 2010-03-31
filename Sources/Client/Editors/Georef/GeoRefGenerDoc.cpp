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
#include "Headers\toolspch.h"
//#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\objdepen.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Client\Base\IlwisDocument.h"
#include "Engine\SpatialReference\Gr.h"
#include "Client\Editors\Georef\GeoRefDoc.h"
#include "Client\Editors\Georef\GeoRefGenerDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(GeoRefGeneratedDoc, GeoRefDoc)

BEGIN_MESSAGE_MAP(GeoRefGeneratedDoc, GeoRefDoc)
END_MESSAGE_MAP()

GeoRefGeneratedDoc::GeoRefGeneratedDoc()
{
}

GeoRefGeneratedDoc::~GeoRefGeneratedDoc()
{
}

BOOL GeoRefGeneratedDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	return GeoRefDoc::OnOpenDocument(lpszPathName);
}

// Get the name of the parent GeoRef from the ODF
FileName GeoRefGeneratedDoc::fnParent()
{
	ObjectDependency depNames;
	Array<FileName> afnNames;
	depNames.Read(m_gr->fnObj, afnNames);
	FileName fnFound;

	// search for a GeoRef in the list of dependencies; only one should be in there
	// so the first found should do
	if (afnNames.iSize() > 0)
	{
		unsigned int i = 0;
		while (i < afnNames.iSize() && !fCIStrEqual(afnNames[i].sExt, ".grf"))
			i++;

		if (i < afnNames.iSize())
			fnFound = afnNames[i];
	}

	return fnFound;
}

// Get the name of the first parent GeoRef that is not a generated/dependent GeoRef
FileName GeoRefGeneratedDoc::fnRoot()
{
	ObjectDependency depNames;
	Array<FileName> afnNames;
	FileName fnChild = m_gr->fnObj;
	bool fFound;

	do
	{
		depNames.Read(fnChild, afnNames);

		// search for a GeoRef in the list of dependencies; only one should be in there
		// so the first found should do
		fFound = afnNames.iSize() > 0;
		if (fFound)
		{
			unsigned int i = 0;
			while (i < afnNames.iSize() && !fCIStrEqual(afnNames[i].sExt, ".grf"))
				i++;

			fFound = i < afnNames.iSize();
			if (fFound)                // found GeoRef dependency
				fnChild = afnNames[i]; // fnChild is the top most parent GeoRef found so far
				                       // continue search starting with the new highest parent
		}
	}
	while (fFound);  // no more GeoRef dependencies found, we are done

	return fnChild;
}

RowCol GeoRefGeneratedDoc::rcParentSize()
{
	GeoRef grParent(fnParent());

	ASSERT(grParent.fValid());
		
	return grParent->rcSize();
}

double GeoRefGeneratedDoc::rParentPixelSize()
{
	GeoRef grParent(fnParent());

	ASSERT(grParent.fValid());
		
	return grParent->rPixSize();
}

zIcon GeoRefGeneratedDoc::icon() const
{
  return zIcon("GrfIcon");
}

