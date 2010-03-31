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
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Representation\Rprgrad.h"
#include "Client\Base\Res.h"
#include "Engine\Base\AssertD.h"
#include "Engine\Scripting\Script.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\Editors\Representation\RepresentationDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//-- [RepresentationValueDoc ]--------------------------------------------------------------------------------------

IMPLEMENT_DYNCREATE(RepresentationValueDoc, RepresentationDoc)

BEGIN_MESSAGE_MAP(RepresentationValueDoc, RepresentationDoc)
	//{{AFX_MSG_MAP(RepresentationValueDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

RepresentationValueDoc::RepresentationValueDoc()
{
	// TODO: add one-time construction code here
}

RepresentationValueDoc::~RepresentationValueDoc()
{
}

/*BOOL RepresentationValueDoc::OnNewDocument()
{
	return RepresentationDoc::OnNewDocument();
}*/

/*BOOL RepresentationValueDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
 return RepresentationDoc::OnOpenDocument(lpszPathName);
}*/

#ifdef _DEBUG
void RepresentationValueDoc::AssertValid() const
{
	RepresentationDoc::AssertValid();
}

void RepresentationValueDoc::Dump(CDumpContext& dc) const
{
	RepresentationDoc::Dump(dc);
}
#endif //_DEBUG

IlwisObject RepresentationValueDoc::obj() const
{
  return	RepresentationDoc::obj();
}

zIcon RepresentationValueDoc::icon() const
{
  return RepresentationDoc::icon();
}

int RepresentationValueDoc::iNoColors() 
{
	return prg()->iNoColors();
}

RepresentationGradual::ColorRange RepresentationValueDoc::GetColorMethod(int iIndex) 
{ 
	return prg()->GetColorMethod(iIndex);
}

double RepresentationValueDoc::rGetLimitValue(int iIndex) 
{
	return prg()->rGetLimitValue(iIndex);
}

Color RepresentationValueDoc::GetColor(int iIndex) 
{
	return prg()->GetColor(iIndex);
}

int RepresentationValueDoc::iGetColorIndex(int iIndex) 
{
	return prg()->iGetColorIndex(iIndex);
}

int RepresentationValueDoc::iGetStretchSteps() 
{
	return prg()->iGetStretchSteps();
}

void RepresentationValueDoc::SetStretchSteps(int iSteps)
{
	if ( iSteps == iGetStretchSteps()) return ;

	prg()->SetStretchSteps( min(30, max( 2, iSteps)));
	prg()->Updated();
}

String RepresentationValueDoc::sValue(double rVal)
{
	return _rpr->dm()->pdv()->sValue(rVal);
}

Color RepresentationValueDoc::clrRaw(int iClr)
{
	return prg()->clrRaw(iClr);
}

bool RepresentationValueDoc::fRepresentationValue()
{
	return _rpr->prv() != 0;
}

void RepresentationValueDoc::Edit(int iIndex, double rV, Color clr)
{
	if (fReadOnly() ) return;
	prg()->SetLimitValue(iIndex, rV);
	prg()->SetLimitColor(iIndex, clr);
	prg()->Updated();
}

void RepresentationValueDoc::Insert(double rVal, Color clr)
{
	if (fReadOnly() ) return;
	prg()->insert(rVal, clr);
	prg()->Updated();
}

void RepresentationValueDoc::Edit(int iIndex, RepresentationGradual::ColorRange cr)
{
	if (fReadOnly() ) return;
	prg()->SetColorMethod(iIndex, cr);
	prg()->Updated();
}

void RepresentationValueDoc::Remove(int iIndex)
{
	if (fReadOnly() ) return;
	prg()->remove(iIndex);
	prg()->Updated();
}

bool RepresentationValueDoc::fUsesGradual()
{
	String sType, sName;
	ObjectInfo::ReadElement("Representation", "Type", _rpr->fnObj, sType);
	return fCIStrEqual(sType, "RepresentationGradual");

}







