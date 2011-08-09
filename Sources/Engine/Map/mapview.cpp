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
/* MapView, MapViewPtr
   Copyright Ilwis System Development ITC
   august 1995, by Wim Koolhoven
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\Map\Mapview.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\System\mutex.h"

IlwisObjectPtrList MapView::listMapView;


MapView::MapView()
	: IlwisObject(listMapView)
{}

MapView::MapView(const MapView& mv)
	: IlwisObject(listMapView, mv.pointer()) 
{}

MapView::MapView(const FileName& fn)
	: IlwisObject(listMapView, FileName(fn,".mpv"))
{
	MutexFileName mut(fn);
	if (!pointer())
		SetPointer(new MapViewPtr(fn));
}

MapView::MapView(const FileName& fn, bool fCreate)
	: IlwisObject(listMapView)
{
	SetPointer(new MapViewPtr(FileName(fn,".mpv"), fCreate));
}

MapViewPtr::~MapViewPtr()
{
}

MapViewPtr::MapViewPtr(const FileName& fn)
	: IlwisObjectPtr(fn)
{
}

MapViewPtr::MapViewPtr(const FileName& fn, bool fCreate)
	: IlwisObjectPtr(fn, fCreate)
{
	fChanged = false; // to prevent preliminary storage, which causes problems with Windoews 95/98
}

String MapViewPtr::sType() const
{
	return "Map View";
}

void MapViewPtr::Store()
{
	IlwisObjectPtr::Store();
	WriteElement("Ilwis", "Type", "MapView");
}

void MapViewPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	ObjectDependency(fnObj).Names(afnObjDep);
}

void MapViewPtr::GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const
{
	IlwisObjectPtr::GetDataFiles(afnDat, asSection, asEntry);
	int iLayers = iReadElement("MapView", "Layers");
	for (int i = 1; i <= iLayers; ++i)
	{
		String sSection("Layer%i", i);
		String sType;
		ReadElement(sSection.c_str(), "Type", sType);
		if ("MetafileDrawer" == sType || "BitmapDrawer" == sType)
		{
			FileName fnData;
			ReadElement(sSection.c_str(), "Data", fnData);
			ObjectInfo::Add(afnDat, fnData, fnObj.sPath());
			if (asSection != 0)
				(*asSection) &= sSection;
			if (asEntry != 0)
				(*asEntry) &= "Data";
		}
	}
}

void MapViewPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles() )
	{	
		int iLayers = iReadElement("MapView", "Layers");
		bool fLeaveFiles = os.fRetrieveContentsContainer() ? false : true;
		os.AddFile(fnObj,"MapView", "CoordSystem" ,"", fLeaveFiles );				
		os.AddFile(fnObj,"MapView", "GeoRef" ,"", fLeaveFiles );						
		for (int i = 1; i <= iLayers; ++i) 
		{
			String sSection("Layer%i", i);
			String sType;
			FileName fnData;
			ReadElement(sSection.c_str(), "Type", sType);
			if ("MetafileDrawer" == sType || "BitmapDrawer" == sType) 
			{
				ReadElement(sSection.c_str(), "Data", fnData);
				os.AddFile(fnObj, sSection.c_str(), "Data", "", fLeaveFiles);
			}
			else if (sType == "AnnotationTextDrawer")
			{
				// AnnotationText must be detected by its drawer, because in an
				// ATX layer sometimes the map from which it is created is mentioned
				String sLayer("Layer%d", i);
				ReadElement(sSection.c_str(), "AnnotationText", fnData);
				os.AddFile(fnObj, sLayer, "AnnotationText", "" , fLeaveFiles );
			}
			else
			{
				// The following code allows a drawer to have all maptypes at the same time (one of each)
				// Currently a drawer at most has one, but this may change in the future
				String sLayer("Layer%d", i);
				if ( ReadElement(sSection.c_str(), "Map" , fnData) != 0 )
				{
					os.AddFile(fnObj,sLayer, "Map", "" , fLeaveFiles );
					os.AddFile(fnObj,sLayer, "MapList", "", fLeaveFiles);
					os.AddFile(fnObj,sLayer, "Representation", ".rpr", fLeaveFiles);
				}					
				if ( ReadElement(sSection.c_str(), "PolygonMap", fnData) != 0 )
					os.AddFile(fnObj,sLayer, "PolygonMap", "" , fLeaveFiles );				
				if ( ReadElement(sSection.c_str(), "SegmentMap", fnData) != 0 )
					os.AddFile(fnObj, sLayer, "SegmentMap", "" , fLeaveFiles );								
				if ( ReadElement(sSection.c_str(), "PointMap", fnData) != 0 )
					os.AddFile(fnObj,sLayer, "PointMap", "" , fLeaveFiles );
				if ( ReadElement(sSection.c_str(), "Texture Map", fnData ) != 0 )
					os.AddFile(fnObj, sLayer, "Texture Map", "", fLeaveFiles);
			}				
		}
	}
}

bool MapViewPtr::fUsesDependentObjects() const
{
	int iLayers = iReadElement("MapView", "Layers");	
	for (int i = 1; i <= iLayers; ++i) 
	{
		String sSection("Layer%i", i);
		String sType;
		FileName fnData;
		ReadElement(sSection.c_str(), "Type", sType);
		if ("MetafileDrawer" == sType || "BitmapDrawer" == sType) 
			continue;
		else if (sType == "AnnotationTextDrawer")
		{
			// AnnotationText must be detected by its drawer, because in an
			// ATX layer sometimes the map from which it is created is mentioned
			continue;  // an AnnotationText cannot be dependent.
		}
		else
		{
			String sLayer("Layer%d", i);
			if ( ReadElement(sSection.c_str(), "Map" , fnData) == 0 )
				if ( ReadElement(sSection.c_str(), "PolygonMap", fnData) == 0 )
					if ( ReadElement(sSection.c_str(), "SegmentMap", fnData) == 0 )
						ReadElement(sSection.c_str(), "PointMap", fnData);

			if ( fnData != FileName() )
			{
				IlwisObject obj = IlwisObject::obj(fnData);
				if ( obj.fValid() && obj->fDependent() )
					return true;
				fnData = FileName();
			}				
		}				
	}
	return false;
}

bool MapViewPtr::fCalculated() const
{
	int iLayers = iReadElement("MapView", "Layers");	
	for (int i = 1; i <= iLayers; ++i) 
	{
		String sSection("Layer%i", i);
		String sType;
		FileName fnData;
		ReadElement(sSection.c_str(), "Type", sType);
		if ("MetafileDrawer" == sType || "BitmapDrawer" == sType) 
			continue;
		else if (sType == "AnnotationTextDrawer")
			continue;  // an AnnotationText cannot be calculated.
		else
		{
			String sLayer("Layer%d", i);
			if ( ReadElement(sSection.c_str(), "Map" , fnData) == 0 )
				if ( ReadElement(sSection.c_str(), "PolygonMap", fnData) == 0 )
					if ( ReadElement(sSection.c_str(), "SegmentMap", fnData) == 0 )
						ReadElement(sSection.c_str(), "PointMap", fnData);

			if ( fnData != FileName() )
			{
				IlwisObject obj = IlwisObject::obj(fnData);
				if ( obj.fValid() && (!obj->fCalculated()) )
					return false;
				fnData = FileName();
			}				
		}				
	}
	return IlwisObjectPtr::fCalculated();
}

void MapViewPtr::Calc(bool fMakeUpToDate)
{
  ILWISSingleLock sl(&csCalc, TRUE);
	int iLayers = iReadElement("MapView", "Layers");	
	for (int i = 1; i <= iLayers; ++i) 
	{
		String sSection("Layer%i", i);
		String sType;
		FileName fnData;
		ReadElement(sSection.c_str(), "Type", sType);
		if ("MetafileDrawer" == sType || "BitmapDrawer" == sType) 
			continue;
		else if (sType == "AnnotationTextDrawer")
			continue;  // an AnnotationText cannot be calculated.
		else
		{
			String sLayer("Layer%d", i);
			if ( ReadElement(sSection.c_str(), "Map" , fnData) == 0 )
				if ( ReadElement(sSection.c_str(), "PolygonMap", fnData) == 0 )
					if ( ReadElement(sSection.c_str(), "SegmentMap", fnData) == 0 )
						ReadElement(sSection.c_str(), "PointMap", fnData);

			if ( fnData != FileName() )
			{
				IlwisObject obj = IlwisObject::obj(fnData);
				if ( obj.fValid() && (!obj->fCalculated()) )
					obj->Calc(fMakeUpToDate);
					
				fnData = FileName();
			}
		}
	}
}
