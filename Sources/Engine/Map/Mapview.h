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
/* $Log: /ILWIS 3.0/BasicDataStructures/Mapview.h $
 * 
 * 6     26-05-05 17:53 Retsios
 * [bug=6412] Added an up-to-date check and calc-if-not-up-to-date to the
 * MapView that didn't have this yet. Note that it is not extended to be a
 * virtual ILWIS object as this is much more work. This will simply help
 * preventing the display of mapviews that are not up-to-date.
 * 
 * 5     20-11-00 9:42a Martin
 * Implemented fUsesDependentObjects 
 * 
 * 4     8-09-00 3:21p Martin
 * added function to set the fChanged member of all the 'members of an
 * object.
 * added function to retrieve the 'structure' of an object (filenames)
 * 
 * 3     13-04-99 13:00 Koolhoven
 * Corrected header comment
 * 
 * 2     13-04-99 12:55 Koolhoven
 * Added _export to MapView functions
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/07 13:31:18  Wim
// Added GetDataFiles() so that Copier can do its work
//
/* MapView
   Copyright Ilwis System Development ITC
   august 1995, by Wim Koolhoven
	Last change:  WK    7 Aug 97    3:02 pm
*/

#ifndef ILWMAPVIEW_H
#define ILWMAPVIEW_H
#include "Engine\Base\DataObjects\ilwisobj.h"

class MapView;
class DATEXPORT MapViewPtr;

class DATEXPORT MapViewPtr : public IlwisObjectPtr
{
    friend class MapView;

    public:
        virtual ~MapViewPtr();
        virtual void        GetObjectDependencies(Array<FileName>& afnObjDep);
        virtual void        GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection=0, Array<String>* asEntry=0) const;
				virtual void _export GetObjectStructure(ObjectStructure& os);
        virtual void        Store();
        virtual String      sType() const;
				bool                fUsesDependentObjects() const;
			  virtual bool _export fCalculated() const;
			  virtual void _export Calc(bool fMakeUpToDate=false);


    private:
        MapViewPtr(const FileName&);
        MapViewPtr(const FileName&, bool /* fCreate */);
};

class MapView: public IlwisObject
{
public:
	_export MapView();
	_export MapView(const FileName& fn);
	_export MapView(const FileName& fn, bool /* fCreate*/);
	_export MapView(const MapView& mv);

	void operator = (const MapView& mv) 
		{ SetPointer(mv.pointer()); }
	MapViewPtr* ptr() const
		{ return static_cast<MapViewPtr*>(pointer()); }
  MapViewPtr* operator -> () const 
		{ return ptr(); }
private:
	static IlwisObjectPtrList listMapView;
};

inline MapView undef(const MapView&) 
{ return MapView(); }

#endif



