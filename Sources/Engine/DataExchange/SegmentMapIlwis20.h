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
#ifndef ILWSEGMAPILWIS20_H
#define ILWSEGMAPILWIS20_H

#include "Engine\Map\Segment\Seg.h"
#include "Engine\Base\File\File.h"

class SegmentMapIlwis20 : public ForeignFormat
{
	public:
		_export SegmentMapIlwis20();
		_export SegmentMapIlwis20(const FileName& fnForeign, ForeignFormat::mtMapType _mtType = mtSegmentMap);

		virtual _export ~SegmentMapIlwis20();

		virtual void _export         Store(IlwisObject);

	protected:
		virtual void _export         WriteODF(IlwisObject);
		virtual void _export         WriteData(IlwisObject);
		virtual void _export         CreateFiles();
		virtual void _export         CloseFiles();
		void _export                 WriteSegData(SegmentMap);
		virtual void _export         WriteTopologyLinks(const Segment&);
		virtual void _export         GetCoordinates(const Segment&, long& iCrd, CoordBuf& cbuf);

		virtual CoordSystem _export  GetCoordSystem();

		Tranquilizer   trq;

		File           *m_filSegments;
		File           *m_filCodes;
		File           *m_filCoords;
		FileName       m_fnODF;
		FileName       m_fnForeignAtt;
		bool           m_fInternalDom;
		
		Domain         m_dmMap;

		float          m_rAlfa, m_rBeta1, m_rBeta2;

	private:
		void           CopyAttributes(BaseMap bm);
		bool           fCheckCopyMap(BaseMap bm);
		void           AdjustMapAndTableDomain(BaseMap bm);

		String         sType(ForeignFormat::mtMapType);
		String         sExt(ForeignFormat::mtMapType);
		void           SetTable(LayerInfo& li, ForeignFormat::mtMapType mtType);
		virtual void _export   LoadTable(TablePtr* tbl);

		long           m_iNrSegs, m_iNrCoords;
		bool           m_fIsPolygonMap;
};

#endif
