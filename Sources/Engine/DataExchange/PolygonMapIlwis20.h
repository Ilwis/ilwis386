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
/*-----------------------------------------------
   $Log: /ILWIS 3.0/ForeignFormat/PolygonMapIlwis20.h $
 * 
 * 4     29/11/00 9:21 Willem
 * Adjusted and reduced the number of interface function necessary.
 * The correct virtual functions in PolygonMapIlwis20 are now called from
 * SegmentMapIlwis20
 * 
 * 3     16/11/00 12:43 Willem
 * Added virtual Store() functions that store the data
 * 
 * 2     19/04/00 16:57 Willem
 * Removed unnecessary include files
 * 
 * 1     19/04/00 16:48 Willem
 * Added polygon map export to Ilwis version 2

   PolygonMapIlwis20.h
   by Willem Nieuwenhuis, 4/17/00
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_POLYGONMAPILWIS20_H
#define ILW_POLYGONMAPILWIS20_H

#include "Engine\DataExchange\SegmentMapIlwis20.h"
#include "Engine\Map\Polygon\POL.H"

class PolygonMapIlwis20 : public SegmentMapIlwis20
{
	public:
		_export PolygonMapIlwis20();
		_export PolygonMapIlwis20(const FileName& fnForeign, ForeignFormat::mtMapType _mtType = mtPolygonMap);

		virtual _export ~PolygonMapIlwis20();

		void virtual _export         Store(IlwisObject);

	protected:
		void virtual _export         WriteODF(IlwisObject);
		virtual void _export         WriteData(IlwisObject);
		virtual void _export         CreateFiles();
		virtual void _export         CloseFiles();
		void _export                 WritePolMapData();

		virtual void _export         WriteTopologyLinks(const Segment&);
		virtual void _export         GetCoordinates(const Segment&, long& iCrd, CoordBuf& cbuf);

		virtual CoordSystem _export  GetCoordSystem();

	private:
		void                         AddTopology(LayerInfo& obj, CoordBuf& cBuf, const CoordBounds& cb, long iSize, long iRec, long iTop);
		String                       sType(ForeignFormat::mtMapType);
		String                       sExt(ForeignFormat::mtMapType);
		void                         SetTable(LayerInfo& li, ForeignFormat::mtMapType mtType);
		virtual void _export         LoadTable(TablePtr* tbl);

		PolygonMap     polmap;
		File           *m_filPolygons;
		File           *m_filTopology;

		long           m_iNrPols;
};

#endif
