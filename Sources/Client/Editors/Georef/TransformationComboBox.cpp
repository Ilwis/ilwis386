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
// TransformationComboBox.cpp : implementation file
//

#include "Client\Headers\formelementspch.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Mapwindow\Positioner.h"
#include "Client\Editors\Editor.h"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\ilwis.h"
#include "Client\Base\datawind.h"
#include "Client\Editors\Utils\BaseBar.h"
#include "Client\MainWindow\CommandCombo.h"
#include "Headers\constant.h"
#include "Client\Base\IlwisDocument.h"
#include "Client\TableWindow\TableDoc.h"
#include "Client\TableWindow\BaseTablePaneView.h"
#include "Client\TableWindow\TablePaneView.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Client\Editors\Utils\SYMBOL.H"
#include "Client\Editors\Utils\sizecbar.h"
#include "Client\Editors\Georef\GeoRefEditorTableView.h"
#include "Client\Editors\Georef\GeoRefEditorTableBar.h"
#include "Client\Editors\Georef\TransformationComboBox.h"
#include "Client\Editors\Georef\TiePointEditor.h"
#include "Headers\Hs\Georef.hs"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// TransformationComboBox

TransformationComboBox::TransformationComboBox()
{
	gre = 0;
}

TransformationComboBox::~TransformationComboBox()
{
}

BEGIN_MESSAGE_MAP(TransformationComboBox, CComboBox)
	//{{AFX_MSG_MAP(TransformationComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelendok)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// TransformationComboBox message handlers

void TransformationComboBox::Create(CWnd* wPar,CRect rect,TiePointEditor* grfed, int iTransf)
{
	gre = grfed;
	CComboBox::Create(WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST|CBS_HASSTRINGS,rect,wPar,ID_EDITGRFTRANSF);
	AddString(TR("Conformal").c_str());
	AddString(TR("Affine").c_str());
	AddString(TR("Second Order Bilinear").c_str());
	AddString(TR("Full Second Order").c_str());
	AddString(TR("Third Order").c_str());
	AddString(TR("Projective").c_str());
	SetCurSel(iTransf);
}

void TransformationComboBox::OnSelendok() 
{
	gre->OnTransfChanged();	
}
