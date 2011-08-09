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
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\DataExchange\ForeignCollection.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\mutex.h"
#include "Headers\constant.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\SpatialReference\prj.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\DataObjects\ObjectCollection.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\DataExchange\WMSCollection.h"

WMSCollectionPtr::~WMSCollectionPtr() {
}

//-------------------------------------

WMSCollection::WMSCollection()
: ForeignCollection()
{}

WMSCollection::~WMSCollection()
{
}

WMSCollection::WMSCollection(const WMSCollection& mv)
: ForeignCollection(mv) 
{}

WMSCollection::WMSCollection(const FileName& fn)
 : ForeignCollection(fn)
{
}

WMSCollection::WMSCollection(const FileName& fn, ParmList& pm)
 : ForeignCollection(FileName(fn.sShortNameQuoted(),".ioc"), sTypeWMSCollection, pm)
{
}

WMSCollection::WMSCollection(const FileName& fn, const String& sType, ParmList& pm)
 : ForeignCollection(fn, sType, pm)
{
}

void WMSCollectionPtr::Store()
{
	ObjectCollectionPtr::Store();
	WriteElement("Ilwis", "Type", "ObjectCollection");	
	WriteElement("WMSCollection", "GetCapabilities", urlGetCapabilities.sVal());
	WriteElement("WMSCollection", "Method", "WMS");
	int count = 0;
	for(map<String, String>::iterator cur = layers.begin(); cur != layers.end(); ++cur) {
		String lyr("%S|%S", (*cur).first, (*cur).second);
		WriteElement("WMSCollection", String("layer%d", count++).c_str(), lyr);
	}
}

String WMSCollectionPtr::sType() const
{
	return sTypeWMSCollection;
}

ObjectCollectionPtr *WMSCollectionPtr::Create(const FileName& fn, ParmList& pm)
{
	MutexFileName mut(fn);
	WMSCollectionPtr *dcp = dynamic_cast<WMSCollectionPtr *>(ObjectCollection::pGet(fn));	
	if (dcp)
	{ // if already open return it
		return dcp;
	}		
	return new WMSCollectionPtr(fn, pm);
}

WMSCollectionPtr::WMSCollectionPtr(const FileName& fn, ParmList& pm) :
	ForeignCollectionPtr(fn, pm), urlGetCapabilities(URL(""))
{
	String sMethod = pm.sGet("method");
	//sDataBaseName = pm.sGet("WMS");
	if ( sMethod == "" ) {
		String temp;
		ReadElement("WMSCollection", "GetCapabilities", temp);
		int count;
		ReadElement("ObjectCollection", "NrObjects", count);
		for(int i = 0; i < count; ++i) {
			String lyr;
			ReadElement("WMSCollection", String("layer%d", i).c_str(), lyr);
			String file = lyr.sHead("|");
			String name = lyr.sTail("|");
			layers[file] = name;

		}
		urlGetCapabilities = URL(temp);
	} else
		urlGetCapabilities = URL(pm.sGet(0));
}

void WMSCollectionPtr::Add(const ilwFileName &fn, const String &layerName) {
	layers[fn.sRelativeQuoted()] = layerName;
	ObjectCollectionPtr::Add(fn);
	ObjectInfo::WriteAdditionOfFileToCollection(fn, fnObj);
	Store();
}

void WMSCollectionPtr::Add(const IlwisObject &obj) {
	ObjectCollectionPtr::Add(obj);
	ObjectInfo::WriteAdditionOfFileToCollection(obj->fnObj, fnObj);
	Store();
}

String WMSCollectionPtr::getLayerName(const FileName& fn) {
	String n = fn.sRelativeQuoted();
	return layers[n];
}

Map	WMSCollection::CreateImplicitObject(const FileName& fnObject, ParmList& pm) {
	Map map;
	String sC = pm.sGet("collection");
	FileName fnCollection(sC);
	WMSCollection wms(fnCollection);
	if ( !wms.fValid() )
		throw ErrorObject(String(TR("Could not open collection %S").c_str(), fnCollection.sFile));
	
	if ( !pm.fExist("method"))
		pm.Add(new Parm("method", wms->sGetMethod()));
	if ( !pm.fExist("getcapabilities"))
		pm.Add(new Parm("url", wms->getCapabilities().sVal()));
	String output = pm.sGet("output");
	FileName fn(output != "" ? output : fnObject.sFullPathQuoted());
	ForeignFormat *ff = ForeignFormat::Create(fn, pm);
	if ( ff )
	{
		LayerInfo info;
		String fullname = wms->getLayerName(fn);
		if ( fullname == "") {
			fullname = fnObject.sFile;
		} else {
			if ( pm.fExist("extras")) {
				String rest = pm.sGet("extras");
				Array<String> parts;
				Split(rest,parts, ",");
				for(unsigned int i = 0; i < parts.size(); ++i) {
					FileName fnextra(parts[i]);
					String n = wms->getLayerName(fnextra);
					fullname += "+" + n;
				}
			}
		}
		ff->GetRasterInfo(info, fullname);
		fn = FileName(info.fnObj, ".mpr");
		map = Map(fn, info);	
		if ( !pm.fExist("import"))
			map->SetUseAs(true);
		else
		{
			map->SetUseAs(false);			
		}			
		map->Store();
		map->gr()->Store();
		ff->Store(map);
		wms->Add(map->gr());
		wms->Add(map->cs());
		wms->Add(map->fnObj, fullname);
		delete ff;				

	}
	return map;
}
