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
/* TblFlowPathLonsitudinalProfile
   Copyright Ilwis System Development ITC
   april 2006, by Lichun Wang
*/
#ifndef TblFlowPathLonsitudinalProfile_H
#define TblFlowPathLonsitudinalProfile_H

#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Applications\MAPFMAP.H"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\SpatialReference\Ellips.h"
#include "Engine\SpatialReference\csviall.h"
#include "LargeVector.h"

struct LongitudinalProfile
{
    long   X;
    double Y;
	Coord  cd;
};

class DATEXPORT TblFlowPathLongitudinalProfile: public TableVirtual
{
    friend class TableVirtual;

    public:
        static const char*      sSyntax();
        Column                  colMap1;
        Column                  colMap2;

    protected:
        TblFlowPathLongitudinalProfile(const FileName& fn, TablePtr& p);
        TblFlowPathLongitudinalProfile(const FileName& fn, TablePtr& p, 
									   const SegmentMap& segmpFlowPath,
									   int iSegmentID,
									   double rDistance,
									   const Map& mpAttributeMap);
		~TblFlowPathLongitudinalProfile();

        virtual void     Store();
        virtual bool     fFreezing();
        virtual String   sExpression() const;
    
    private:
        static TblFlowPathLongitudinalProfile*  create(const FileName& fn, TablePtr& p, const String& sExpression);
        void                      Init();
        void                      CreateColumns(vector<LongitudinalProfile> vlp);
        void                      AddDomainItem(Domain dm,long item);
		double					  rDistance(Coord cd1, Coord cd2);
		bool					  fEllipsoidalCoords();
		bool					  fLatLonCoords();	
        SegmentMap                m_segmpFlowPath;
        Map                       m_mpAttributeMap;
		long                      m_iSegmentID;
		double                    m_rDist;
		LargeVector<RealBuf>           m_vAttributeMap;
};

#endif 




