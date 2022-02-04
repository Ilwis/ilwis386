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
// MaplistGraphFunction.cpp: implementation of the MaplistGraphFunction class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\Algorithm\MaplistGraphFunction.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// MaplistGraphFunction
//////////////////////////////////////////////////////////////////////

MaplistGraphFunction::MaplistGraphFunction()
: SimpleFunction(0, DoubleRect(0, 0, 1, 1))
, m_iStartBand(0)
, m_iEndBand(0)
, m_iOffset(0)
, m_fFixedStretch(false)
, m_rStretchMin(0)
, m_rStretchMax(0)
, m_iDec(3)
{
	m_rData.resize(0);
}

MaplistGraphFunction::~MaplistGraphFunction()
{
}

double MaplistGraphFunction::rGetFx(double x) const
{
	int iDataSize = m_rData.size();
	if (iDataSize > 0)
	{
		int iX = floor(x) - m_iStartBand + m_iOffset;
		if (iX < 0)
			return m_rData[0];
		else if (iX > iDataSize - 1)
			return m_rData[iDataSize - 1];
		else if ((double)(iX + m_iStartBand - m_iOffset) == x)
			return m_rData[iX];
		else // normal case, linear interpolation of Fx(floor(x)) and Fx(ceil(x)) at x
		{
			double rLeftFx = m_rData[iX];
			double rRightFx = m_rData[iX + 1];
			if ( rLeftFx == rUNDEF || rRightFx == rUNDEF)
				return rUNDEF;
			double rRemaining = x - iX - m_iStartBand + m_iOffset;
			double rResult = rLeftFx + rRemaining * (rRightFx - rLeftFx);
			return rResult;
		}
	}
	else
		return m_Domain.bottom;
}

void MaplistGraphFunction::SetMaplist(String sMapList)
{
	m_mpl = MapList(FileName(sMapList));
	if (m_mpl.fValid())
	{
		m_rData.resize(m_mpl->iSize());
		m_iStartBand = m_mpl->iLower();
		m_iEndBand = m_mpl->iUpper();
		m_iOffset = m_iStartBand - 1; // check implementation of mpl->iLower() .. some improvement may be needed there

		m_Domain.left = m_iStartBand - m_iOffset;
		m_Domain.right = m_iEndBand - m_iOffset;

		if ((m_mpl->iSize() > 0) && m_mpl->map(m_iStartBand).fValid())
			m_iDec = m_mpl->map(m_iStartBand)->dvrs().iDec();
	}

	ReadData();
}

void MaplistGraphFunction::ReadData()
{
	if (!m_crd.fUndef() && m_mpl.fValid())
	{
		if (m_mpl->map(m_iStartBand).fValid() && m_mpl->map(m_iStartBand)->gr().fValid() && m_mpl->map(m_iStartBand)->gr()->cs().fValid())
		{
			// assuming all maps in maplist have same grf and csy
			m_grfMpl = m_mpl->map(m_iStartBand)->gr();
			m_csyMpl = m_grfMpl->cs();
			// convert m_crd to the coordsytem of the map
			m_crdMpl = m_csyMpl->cConv(m_csy, m_crd);
			double rMin = +1e100;
			double rMax = -1e100;
			for (int i = m_iStartBand; i <= m_iEndBand; ++i)
			{
				double rVal = m_mpl->map(i)->rValue(m_crdMpl);
				m_rData[i - m_iStartBand] = rVal;
				if ( rVal == rUNDEF)
					continue;

				rMin = min(rVal, rMin);
				rMax = max(rVal, rMax);
			}
			if (m_fFixedStretch)
			{
				m_Domain.top = m_rStretchMax;
				m_Domain.bottom = m_rStretchMin;
			}
			else
			{
				m_Domain.top = rMax;
				m_Domain.bottom = rMin;
			}
		}
	}
}

void MaplistGraphFunction::SetStretch(bool fFixedStretch, double rMin, double rMax)
{
	m_fFixedStretch = fFixedStretch;
	m_rStretchMin = rMin;
	m_rStretchMax = rMax;
	if (m_fFixedStretch)
	{
		m_Domain.top = m_rStretchMax;
		m_Domain.bottom = m_rStretchMin;
	}
	else
		ReadData();
}

void MaplistGraphFunction::SetDefaultAnchors()
{
	// no anchors to set
	SolveParams();
}

void MaplistGraphFunction::SetAnchor(DoublePoint pAnchor)
{
	if (m_Domain.bottom <= pAnchor.Y && pAnchor.Y <= m_Domain.top && m_Domain.left <= pAnchor.X && pAnchor.X <= m_Domain.right)
		m_dpDragPoint = pAnchor;
	else
		m_dpDragPoint = DoublePoint(rUNDEF, rUNDEF);
}

void MaplistGraphFunction::SolveParams()
{
	// nothing to solve
}

void MaplistGraphFunction::SetCoordAndCsy(CoordWithCoordSystem & cwcs)
{
	m_crd = cwcs.c();
	m_csy = cwcs;
}

String MaplistGraphFunction::sClipboardText()
{
	// return the m_rData array
	String sText;
	for (int i = 0; i < m_rData.size(); ++i)
		sText += String("%.*f\r\n", iDec(), m_rData[i]);
	return sText;
}

const CoordSystem & MaplistGraphFunction::csyMpl() const
{
	return m_csyMpl;
}

const GeoRef & MaplistGraphFunction::grfMpl() const
{
	return m_grfMpl;
}

const Coord & MaplistGraphFunction::crdMpl() const
{
	return m_crdMpl;
}

const DoublePoint & MaplistGraphFunction::dpDragPoint() const
{
	return m_dpDragPoint;
}

const int MaplistGraphFunction::iDec() const
{
	return m_iDec;
}
