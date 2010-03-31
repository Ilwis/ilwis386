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
// FieldExtent.cpp: implementation of the FieldExtent class.
//
//////////////////////////////////////////////////////////////////////

#include "Client\Headers\formelementspch.h"
#include "Headers\messages.h"
#include "Client\FormElements\InfoText.h"
#include "Client\FormElements\FieldExtent.h"
#include "Client\ilwis.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

FieldExtent::FieldExtent(FormEntry* feRoot, const Map& mp)
	: FieldGroup(feRoot, true)
{
	CalcCoordinates(mp);
	BuildField();
}

FieldExtent::FieldExtent(FormEntry* feRoot, const GeoRef& gr)
	: FieldGroup(feRoot, true)
{
	CalcCoordinates(gr);
	BuildField();
}

void FieldExtent::BuildField()
{
	m_stTitle = new InfoText(this, SUiExtCornersOfCorners);
	m_stTitle->SetWidth(250);
	m_stTitle->psn->iBndDown = 0;
	StaticText *stTL = new InfoText(this, SUiExtTopLeft);
	stTL->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
	m_fsTL = new FieldString(this, &m_sTL, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	m_fsTL->psn->SetBound(0,0,0,0);
	m_fsTL->SetIndependentPos();
	m_fsTL->SetWidth(180);
	m_fsTL->Align(stTL, AL_AFTER);

	StaticText *stAbove = stTL;
	StaticText *stTR = new InfoText(this, SUiExtTopRight);
	stTR->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
	stTR->Align(stAbove, AL_UNDER);
	m_fsTR = new FieldString(this, &m_sTR, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	m_fsTR->psn->SetBound(0,0,0,0);
	m_fsTR->SetIndependentPos();
	m_fsTR->SetWidth(180);
	m_fsTR->Align(stTR, AL_AFTER);

	stAbove = stTR;
	StaticText *stBL = new InfoText(this, SUiExtBottomLeft);
	stBL->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
	stBL->Align(stAbove, AL_UNDER);
	m_fsBL = new FieldString(this, &m_sBL, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	m_fsBL->psn->SetBound(0,0,0,0);
	m_fsBL->SetIndependentPos();
	m_fsBL->SetWidth(180);
	m_fsBL->Align(stBL, AL_AFTER);

	stAbove = stBL;
	StaticText *st = new InfoText(this, SUiExtBottomRight);
	int iMaxW = st->psn->iMinWidth + 15;
	st->psn->SetBound(0,0,0,0);  // set space around previous InfoText to a minimum.
	st->Align(stAbove, AL_UNDER);
	m_fsBR = new FieldString(this, &m_sBR, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|ES_READONLY);
	m_fsBR->psn->SetBound(0,0,0,0);
	m_fsBR->SetIndependentPos();
	m_fsBR->SetWidth(180);
	m_fsBR->Align(st, AL_AFTER);

	stTL->SetFieldWidth(iMaxW);
	stTR->SetFieldWidth(iMaxW);
	stBL->SetFieldWidth(iMaxW);
	st->SetFieldWidth(iMaxW);
}

void FieldExtent::SetTitle(const String& sTitle)
{
	m_stTitle->SetVal(sTitle);
}

void FieldExtent::SetVal(const Map& mp)
{
	CalcCoordinates(mp);
	Update();
}

void FieldExtent::SetVal(const GeoRef& gr)
{
	CalcCoordinates(gr);
	Update();
}

void FieldExtent::Update()
{
	m_fsTL->SetVal(m_sTL);
	m_fsTR->SetVal(m_sTR);
	m_fsBL->SetVal(m_sBL);
	m_fsBR->SetVal(m_sBR);
}

void FieldExtent::CalcCoordinates(const Map& mp)
{
	if (mp.fValid())
	{
		GeoRef gr = mp->gr();
		Coord crdTL, crdBL, crdTR, crdBR;
		gr->RowCol2Coord(0, 0, crdTL);
		gr->RowCol2Coord(0, mp->iCols(), crdTR);
		gr->RowCol2Coord(mp->iLines(), 0, crdBL);
		gr->RowCol2Coord(mp->iLines(), mp->iCols(), crdBR);
		m_sTL = gr->cs()->sValue(crdTL, 0);
		m_sTR = gr->cs()->sValue(crdTR, 0);
		m_sBL = gr->cs()->sValue(crdBL, 0);
		m_sBR = gr->cs()->sValue(crdBR, 0);
	}
	else
	{
		m_sTL = String();
		m_sTR = String();
		m_sBL = String();
		m_sBR = String();
	}
}

void FieldExtent::CalcCoordinates(const GeoRef& gr)
{
	if (gr.fValid())
	{
		Coord crdTL, crdBL, crdTR, crdBR;
		RowCol rc = gr->rcSize();
		gr->RowCol2Coord(0, 0, crdTL);
		gr->RowCol2Coord(0, rc.Col, crdTR);
		gr->RowCol2Coord(rc.Row, 0, crdBL);
		gr->RowCol2Coord(rc.Row, rc.Col, crdBR);
		m_sTL = gr->cs()->sValue(crdTL, 0);
		m_sTR = gr->cs()->sValue(crdTR, 0);
		m_sBL = gr->cs()->sValue(crdBL, 0);
		m_sBR = gr->cs()->sValue(crdBR, 0);
	}
	else
	{
		m_sTL = String();
		m_sTR = String();
		m_sBL = String();
		m_sBR = String();
	}
}

FieldExtent::~FieldExtent()
{

}
