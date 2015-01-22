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
#pragma warning( disable : 4786 )

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\MainWindow\FileFilter.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Headers\Hs\Mainwind.hs"
#include <time.h>


//
static const String sOperators("<>=<=contains.");

ParseTree::ParseTree() : or(NULL), and(NULL), entry(NULL)
{}

ParseTree::~ParseTree() 
{
	delete or;
	delete and;
	delete entry;
}
//-----------------------
SectionEntry::SectionEntry(const String& _sSection, 
													 const String& _sAttribute, const String& _sUIAttribName, 
													 const String& _sValue) :
	sSection(_sSection),
  sAttribute(_sAttribute),
	sUIAttribName( _sUIAttribName != "" ? _sUIAttribName : _sAttribute),
	sValue(_sValue),
	fWildCardValue(false),
	next(NULL)
{}

SectionEntry::~SectionEntry()
{
	delete next;
}

//-----------------------
FileFilter::FileFilter() : 
	iC(0),
  root(NULL),
	fSystemObjects(false),
	fShowCollectionContents(false)
{
	init();
}

FileFilter::~FileFilter()
{
	if ( root) delete root;
	root = 0;
}

void FileFilter::init()
{
	//types map contains all ilwis objects. It also contains some super-objects for searching in object-sets
	//The map contains an objtypeinfo object that defines the type and the extensions for the object
	Types["ilwisObject"]  = ObjTypeInfo(otIlwisObject, 
		IlwisObject::sAllExtensions());
	Types["allobjects"]  = ObjTypeInfo(otAllObjects,
			IlwisObject::sAllExtensions());
	Types["map"        ]  = ObjTypeInfo(otMap, ".mpr .mpa .mps .mpp");
	Types["rastermap"  ]  = ObjTypeInfo(otRasterMap, ".mpr");
	Types["polygonmap" ]  = ObjTypeInfo(otPolygonMap, ".mpa");
	Types["segmentmap" ]  = ObjTypeInfo(otSegmentMap, ".mps");
	Types["pointmap  " ]  = ObjTypeInfo(otPointMap, ".mpp");
	Types["domain"     ]  = ObjTypeInfo(otDomain, ".dom");
	Types["coordsystem"]  = ObjTypeInfo(otCoordSystem, ".csy");
	Types["table"      ]  = ObjTypeInfo(otTable, ".tbt");
	Types["filter"     ]  = ObjTypeInfo(otFilter, ".fil");
	Types["function"   ]  = ObjTypeInfo(otFunction, ".fun");
	Types["georef"     ]  = ObjTypeInfo(otGeoRef, ".grf");
	Types["representation" ]  = ObjTypeInfo(otRepresentation, ".rpr");
	Types["mapview"      ]  = 	ObjTypeInfo(otMapView, ".mpv");
	Types["matrix"       ]  = 	ObjTypeInfo(otMatrix, ".mat");
	Types["polhistogram" ]  = 	ObjTypeInfo(otHistPol, ".hsa");
	Types["histogram"    ]  = 	ObjTypeInfo(otHistRas, ".his");
	Types["seghistogram" ]  = 	ObjTypeInfo(otHistSeg, ".his");
	Types["pnthistogram" ]  = 	ObjTypeInfo(otHistPnt, ".his");
	Types["sampleset"    ]  = 	ObjTypeInfo(otSampleSet, ".sms");
	Types["ObjectCollection" ]  = ObjTypeInfo(otObjCol, ".ioc");
	Types["MapList"] = ObjTypeInfo(otMapList, ".mpl");
	Types["Layout"]  = ObjTypeInfo(otLayout, ".ilo");
	Types["Script"] = ObjTypeInfo(otScript, ".isl");
	Types["Table2D"] = ObjTypeInfo(otTable2D, ".t2d");
	Types["AnnotationText"] = ObjTypeInfo(otAnnotationText, ".atx");
	Types["Graph"]  = ObjTypeInfo(otGraph, ".grh");

	UINames["map"] = "basemap";
	UINames["rastermap"] = "map";
	UINames["source"] = "mapstore";
	UINames["dependent" ] = "mapvirtual";
	UINames["class"] = "domainclass";
	UINames["id"] = "domainidentifier";
	UINames["tiepoints"] = "georefctp";
	UINames["corners"] = "georefcorners";
	UINames["identifier"] = "domainidentifier";
	UINames["corners"] = "georefcorners";
	UINames["tiepoints"] = "georefctp";
	UINames["gradual"] = "representationgradual";
	UINames["rprclass"] = "representationclass";
	UINames["rprvalue"] = "representationvalue";	
	UINames["rprvalue"] = "representationvalue";	
	UINames["rashistogram"] = "tablehistogram";
	UINames["seghistogram"] = "tablehistogramseg";	
	UINames["polhistogram"] = "tablehistogrampol";		
	UINames["pnthistogram"] = "tablehistogrampnt";
	UINames["uniqueid"] = "domainuniqueid";
	UINames["picture"] = "domainpicture";		
	UINames["color"] = "domaincolor";			

	Sections.resize((int)stAllSections);
	ObjSections.resize((int)otEnd);

	//The sections array is a set of lists that describe a section in an odf. 
	//an Section entry has a section name and an entry in the section as member. usually the section
	//name is sufficient to to be used in the query but sometimes a UI name is demanded.
	Sections[	stIlwis				].push_back(SectionEntry("ilwis",					"time"));
	Sections[	stIlwis				].push_back(SectionEntry("ilwis",					"description"));
	Sections[	stIlwis				].push_back(SectionEntry("ilwis",					"version"));
	Sections[	stIlwis				].push_back(SectionEntry("ilwis",					"name"));
	Sections[	stIlwis				].push_back(SectionEntry("ilwis",					"type"));
	Sections[	stBasemap			].push_back(SectionEntry("basemap",				"domain"));
	Sections[	stBasemap			].push_back(SectionEntry("basemap",				"coordsystem"));
	Sections[	stBasemap			].push_back(SectionEntry("basemap",				"attributetable"));
	Sections[ stBasemap     ].push_back(SectionEntry("basemap",       "coordbounds"));
	Sections[	stMap					].push_back(SectionEntry("map",						"georef"));
 	Sections[	stCoordSystem	].push_back(SectionEntry("coordsystem",		"datum"));
	Sections[	stCoordSystem	].push_back(SectionEntry("coordsystem",		"projection"));
	Sections[	stProjection	].push_back(SectionEntry("projection",		"northern hemisphere","north"));
	Sections[	stProjection	].push_back(SectionEntry("projection",		"zone"));
	Sections[	stDomain			].push_back(SectionEntry("domain",				"representation"));
	Sections[	stGeoRef			].push_back(SectionEntry("georef",				"coordsystem"));
	Sections[	stRepresentation ].push_back(SectionEntry("representation",	"domain"));
	Sections[	stTable 			].push_back(SectionEntry("table",				  "domain"));
	Sections[	stMapList			].push_back(SectionEntry("maplist",				  "georef"));

	
	// for each object type the section that may be in its odf can be added
	ObjSections[otIlwisObject].push_back(stIlwis);

	ObjSections[otMap].push_back(stIlwis);
	ObjSections[otMap].push_back(stBasemap);
	ObjSections[otMap].push_back(stMap);
	ObjSections[otMap].push_back(stPolygonMap);
	ObjSections[otMap].push_back(stSegmentMap);
	ObjSections[otMap].push_back(stPointMap);

	ObjSections[otRasterMap].push_back(stMap);
	ObjSections[otRasterMap].push_back(stBasemap);
	ObjSections[otRasterMap].push_back(stIlwis);

	ObjSections[otPolygonMap].push_back(stPolygonMap);
	ObjSections[otPolygonMap].push_back(stBasemap);
	ObjSections[otPolygonMap].push_back(stIlwis);

	ObjSections[otSegmentMap].push_back(stSegmentMap);
	ObjSections[otSegmentMap].push_back(stBasemap);
	ObjSections[otSegmentMap].push_back(stIlwis);

	ObjSections[otPointMap].push_back(stPointMap);
	ObjSections[otPointMap].push_back(stBasemap);
	ObjSections[otPointMap].push_back(stIlwis);

	ObjSections[otCoordSystem].push_back(stIlwis);
	ObjSections[otCoordSystem].push_back(stCoordSystem);
	ObjSections[otCoordSystem].push_back(stProjection);

	ObjSections[otDomain].push_back(stIlwis);
	ObjSections[otDomain].push_back(stDomain);

	ObjSections[otGeoRef].push_back(stIlwis);
	ObjSections[otGeoRef].push_back(stGeoRef);

	ObjSections[otRepresentation].push_back(stIlwis);
	ObjSections[otRepresentation].push_back(stRepresentation);

	ObjSections[otTable].push_back(stIlwis);
	ObjSections[otTable].push_back(stTable);

	ObjSections[otFilter].push_back(stIlwis);
	ObjSections[otFilter].push_back(stFunction);

	ObjSections[otHistRas].push_back(stIlwis);

	ObjSections[otHistPol].push_back(stIlwis);
	ObjSections[otHistSeg].push_back(stIlwis);
	ObjSections[otHistPnt].push_back(stIlwis);
	ObjSections[otObjCol].push_back(stIlwis);
	ObjSections[otSampleSet].push_back(stIlwis);
	ObjSections[otMatrix].push_back(stIlwis);
	ObjSections[otScript].push_back(stIlwis);
	ObjSections[otAnnotationText].push_back(stIlwis);
	ObjSections[otTable2D].push_back(stIlwis);
	ObjSections[otMapList].push_back(stIlwis);
	ObjSections[otLayout].push_back(stIlwis);

	lsExt.resize(IlwisObject::iotANY + 2);
  lsExt[IlwisObject::iotMAPVIEW]						= NameExt(TR("Map View"), ".mpv", true);
  lsExt[IlwisObject::iotRASMAP]						= NameExt(TR("Raster Map"), ".mpr", true);
  lsExt[IlwisObject::iotPOLYGONMAP]				= NameExt(TR("Polygon Map"), ".mpa", true);
  lsExt[IlwisObject::iotSEGMENTMAP]				= NameExt(TR("Segment Map"), ".mps", true);
  lsExt[IlwisObject::iotPOINTMAP]					= NameExt(TR("Point Map"), ".mpp", true);
  lsExt[IlwisObject::iotMAPLIST]						= NameExt(TR("Map List"), ".mpl", true);
  lsExt[IlwisObject::iotLAYOUT]					= NameExt(TR("Layout"), ".ilo", true);
	lsExt[IlwisObject::iotOBJECTCOLLECTION] = NameExt(TR("Object Collection"), ".ioc", true);
  lsExt[IlwisObject::iotTABLE]            = NameExt(TR("Table"), ".tbt", true);
  lsExt[IlwisObject::iotTABLE2DIM]        = NameExt(TR("2-Dimensional Table"), ".ta2", true);
  lsExt[IlwisObject::iotANNOTATIONTEXT]   = NameExt(TR("Annotation Text"), ".atx", true);
  lsExt[IlwisObject::iotDOMAIN]           = NameExt(TR("Domain"), ".dom", true);
  lsExt[IlwisObject::iotREPRESENTATION]   = NameExt(TR("Representation"), ".rpr", true);
  lsExt[IlwisObject::iotGEOREF]					 = NameExt(TR("GeoReference"), ".grf", true /*false*/);
  lsExt[IlwisObject::iotCOORDSYSTEM]			 = NameExt(TR("Coordinate System"), ".csy", true /*false*/);
  lsExt[IlwisObject::iotHISTRAS]					 = NameExt(TR("Histogram"), ".his", true /*false*/);
  lsExt[IlwisObject::iotHISTPOL]					 = NameExt(TR("Polygon Histogram"), ".hsa", true /*false*/);
  lsExt[IlwisObject::iotHISTSEG]					 = NameExt(TR("Segment Histogram"), ".hss", true /*false*/);
  lsExt[IlwisObject::iotHISTPNT]					 = NameExt(TR("Point Histogram"), ".hsp", true /*false*/);
  lsExt[IlwisObject::iotSAMPLESET]				 = NameExt(TR("Sample Set"), ".sms", true /*false*/);
  lsExt[IlwisObject::iotMATRIX]					 = NameExt(TR("Matrix"), ".mat", true /*false*/);
  lsExt[IlwisObject::iotFILTER]					 = NameExt(TR("Filter"), ".fil", true /*false*/);
  lsExt[IlwisObject::iotFUNCTION]				 = NameExt(TR("Function"), ".fun", true);
  lsExt[IlwisObject::iotSCRIPT]					 = NameExt(TR("Script"), ".isl", true);
  lsExt[IlwisObject::iotGRAPH]					 = NameExt(TR("Graph"), ".grh", true);
  lsExt[IlwisObject::iotSTEREOPAIR]				 = NameExt(TR("Stereo Pair"), ".stp", true);
  lsExt[IlwisObject::iotCRITERIATREE]			 = NameExt(TR("Criteria Tree"), ".smc", true);
  lsExt[IlwisObject::iotANY]							 = NameExt(TR("Directories"), "directory", true);
  lsExt[IlwisObject::iotANY + 1]					 = NameExt(TR("Disk drives"), "drive", true);

}

bool FileFilter::fCheckEntry(const FileName& fnObj, SectionEntry* entry)
{
  bool fAttribTypeOk= true, fNmOK = true, fOpOK = true;

	bool fTypeOK = entry->type.sExt.find(fnObj.sExt) != -1;
	if (!fTypeOK) 
    return false;
	
	String sVal;

	if ( entry->next != NULL )
	{
		ObjectInfo::ReadElement(entry->sSection.c_str(), entry->sAttribute.c_str(), fnObj, sVal);
		if ( sVal == "") return false;
		FileName fnNext(sVal);
		return fCheckEntry(fnNext, entry->next) && fTypeOK;
	}

	if (entry->sAttribute == "type")
	{
		switch(entry->op)
		{
			case opEQ:
				fAttribTypeOk = fAttributeTypeOK(entry, fnObj); 
				break;
			case opNOTEQ:
				fAttribTypeOk = !fAttributeTypeOK(entry, fnObj); 
				break;
		}
		if (!fAttribTypeOk) 
      return false;		
	}
	else if (entry->sAttribute != "name") // special attribute
	{
		if (entry->sAttribute == "time")
		{
			HANDLE hFile = CreateFile(fnObj.sFullName().c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
		                             FILE_ATTRIBUTE_NORMAL, NULL);
			if (0 == hFile || INVALID_HANDLE_VALUE == hFile ) 
				sVal = "0";
			else
			{
				FILETIME time;
				GetFileTime(hFile, NULL, NULL, &time);
				CloseHandle(hFile);
				CTime t(time);
				sVal = String("%lu", t.GetTime());			
			}				
		}	
		else
			ObjectInfo::ReadElement(entry->sSection.c_str(), entry->sAttribute.c_str(), fnObj, sVal);
		
		sVal.toLower();
		switch ( entry->op)
		{
			case opEQ:
				fOpOK = fStringOK(sVal, entry->sValue, entry->fWildCardValue); break;
			case opNOTEQ:
				fOpOK = !fStringOK(sVal, entry->sValue, entry->fWildCardValue); break;
			case opGREAT:
				fOpOK =  sVal.rVal() > entry->sValue.rVal(); break;
			case opGREATEQ:
				fOpOK = sVal.rVal() >= entry->sValue.rVal(); break;
			case opLESS:
				fOpOK =  sVal.rVal() < entry->sValue.rVal(); break;
			case opLESSEQ:
				fOpOK =  sVal.rVal() <= entry->sValue.rVal(); break;
			case opCONTAINS:
				fOpOK = fCheckContains(fnObj, entry, sVal); break;
			default:
				throw ErrorObject("Unknown operator found ");
		};
	}
	else
	{
		switch(entry->op)
		{
			case opEQ:
				fNmOK = fStringOK(fnObj.sFile, entry->sValue, entry->fWildCardValue); 
				break;
			case opNOTEQ:
				fNmOK = !fStringOK(fnObj.sFile, entry->sValue, entry->fWildCardValue); 
				break;
		};
	}

	return fOpOK && fAttribTypeOk && fTypeOK && fNmOK;
}

bool FileFilter::fCheckContains(const FileName& fnObj, SectionEntry *entry, const String& sVal)
{
	if ( sVal == "") return false;
	if ( entry->sAttribute == "coordbounds")
	{
		double rX = entry->sValue.sHead(",").rVal();
		double rY = entry->sValue.sTail(",").rVal();
		if ( rX == rUNDEF || rY == rUNDEF ) return false;
		Coord crd(rX, rY);
		CoordBounds cb;
		ObjectInfo::ReadElement(entry->sSection.c_str(), entry->sAttribute.c_str(), fnObj, cb);
		return cb.fContains(crd);
	}
	else 
		return sVal.find(entry->sValue) != -1;
}

bool FileFilter::fOK(const FileName& fnObj, ParseTree* node)
{
	if (!IOTYPEBASEMAP(fnObj)) {
		if (!fSystemObjects)
			if ( ObjectInfo::fSystemObject(fnObj)) 
				return false;
	}

	if (!fShowCollectionContents)
		if ( ObjectInfo::fInCollection(fnObj))
			return false;

	if ( sFilter == "") return true;
	if ("drive" == fnObj.sExt || "directory" == fnObj.sExt) return true;


	if ( node == NULL)
		node = root;

	if (node == NULL || node->entry == NULL) return false;

	bool fAcceptAnd=true, fAcceptOr=false;
	
	if ( node->and)
		fAcceptAnd = fOK(fnObj, node->and);
	if (node->or)
		fAcceptOr = fOK(fnObj, node->or);


	bool fReturn  =  fCheckEntry(fnObj, node->entry) && fAcceptAnd || fAcceptOr;

	if ( fReturn)
		UsedTypes.insert(fnObj.sExt);

	return fReturn;
}

bool FileFilter::fAttributeTypeOK(SectionEntry *entry, const FileName& fnObj)
{
	String sFindVal;
	map<String, String>::iterator where = UINames.find(entry->sValue);
	if ( where != UINames.end() )	
		sFindVal = (*where).second;
	if ( sFindVal == "") sFindVal = entry->sValue;
	if ( entry->type.kind == otDomain && entry->sAttribute == "type")
	{
		if ( fCIStrEqual(fnObj.sFile, sFindVal)) return true;
		if ( fCIStrEqual(fnObj.sFile ,"image") && fCIStrEqual(sFindVal, "value")) return true;
	}

	String sVal = entry->sSection.c_str();
	if ( sFindVal.find("tablehistogram") != -1 )
	{
		String sNewVal;
		ObjectInfo::ReadElement(sFindVal.c_str(), "map", fnObj, sNewVal);
		return sNewVal != "";
	}		

	while( sVal != "" )
	{
		String sNewVal;
		ObjectInfo::ReadElement(sVal.c_str(), "type", fnObj, sNewVal);
		sNewVal.toLower();
		if ( sVal == sNewVal ) return false; // end of line default value(=section name) is returned
		if ( sNewVal == sFindVal )
			return true;

		sVal = sNewVal;
	}
	return false;
}

bool FileFilter::fStringOK(const String& sN, const String& sMask, bool fWildCardValue)
{
	String sName = sN;
	sName.toLower();
	if ( !fWildCardValue )
		return sName == sMask;

	unsigned int j=0, i=0;
	size_t iSzMask = sMask.size();
	size_t iSzName = sName.size();

	while( i < sName.size())
	{
		if ( sMask[j] != '*')
		{
			if ( sMask[j] != sName[i] && sMask[j] != '?')
				return false;
			else
			{
				++i;
				++j;
			}
		}
		else
		{
			if ( j == iSzMask - 1) 
				return true;
			++j;
			char cNext; 
			while( j < iSzMask && (cNext = sMask[j]) == '*')
				++j;
			if ( cNext == '*' ) return true; // everything goes, wildcard at the end
			while( i < iSzName && sName[i] != cNext )
				++i;
			if ( i == iSzName ) 
				return false;
		}
	}
	return true;

}

bool FileFilter::fParse(const String& _sFilter)
{
	try
	{
		if ( root) delete root;
		root = NULL;
		sFilter = _sFilter;

		if ( sFilter == "") return false;
		size_t iPos;
		if ( (iPos = sFilter.find("-quiet", sFilter.size() - 8)) != string::npos)
			sFilter = sFilter.substr(0, iPos)	;			

		root = new ParseTree;
		iC= 0;
		UsedTypes.clear();
		sToken = "";
//		SetPossibleExt(true);

		Expr(root);
	}
	catch (ErrorObject& err)
	{
		delete root;
		root = NULL;
		sFilter = "";
		err.Show();
		return false;
	}
	return true;
}

void FileFilter::Expr(ParseTree * node)
{
  Next();
	if ( sToken == "")
		throw ErrorObject(String("Missing context after : %S", sLastToken));
	if ( sToken == "(" )
	{
		Expr(node);
		if ( sToken == ")" )
			Next();
		else
			throw ErrorObject(String("Missing \')\' at character %d", iC));
	} 
	else
	{
		Term(node);
	}
	if ( sToken == "or")
	{
		ParseTree *newNode = new ParseTree;
		node->or = newNode;
		Expr(newNode);
	} 
	else if ( sToken == "and" )
	{
		ParseTree *newNode = new ParseTree;
		node->and = newNode;
		Expr(newNode);
	}
	if ( sToken.sTrimSpaces() != "" )
		throw ErrorObject(String("Unknown token %S :", sToken));
}

void FileFilter::Term(ParseTree *node)
{
	SectionEntry *entry = new SectionEntry;
	node->entry = entry;
	entry->type = Types[sToken];
	Next();
	if ( sOperators.find(sToken) == -1)
		throw ErrorObject(String("Unknown token : %S", sToken));

	if ( sToken != ".")
	{
		entry->type = Types["ilwisobject"];
		Previous();
	}
	Attribute(entry);	
	Next();
	SectionEntry *NextLevel, *LastLevel;
	NextLevel = LastLevel = entry;
	while( sToken == "." )
	{
		NextLevel = new SectionEntry;
		NextLevel->sSection = LastLevel->sAttribute;
//		NextLevel->sSection = LastLevel->sAttribute;		
		LastLevel->next = NextLevel;
		NextLevel->type = Types[LastLevel->sAttribute];
		Attribute(NextLevel);
		Next();
		LastLevel = NextLevel;
	}

	Value(NextLevel);
	Next();
}

typedef list<SectionEntry>::iterator SecIter;
typedef list<stSection>::iterator STIter;

void FileFilter::Attribute(SectionEntry *entry)
{
	Next();
	list<stSection>& l = ObjSections[otGeoRef];
	list<stSection> &lst= ObjSections[entry->type.kind];
	for(STIter cur = lst.begin(); cur != lst.end(); ++cur)
	{
		stSection sss = *cur;
		list<SectionEntry> &lst2 = Sections[(*cur)];
		for(SecIter cur2 = lst2.begin(); cur2 != lst2.end(); ++cur2)
		{
			SectionEntry ss = *cur2;
			if ( (*cur2).sUIAttribName == sToken)
			{
				if ( entry->sSection == "" )
				{
					entry->sSection = (*cur2).sSection;				
				}
				entry->sAttribute = (*cur2).sAttribute;				
				entry->sUIAttribName = sToken;
				return;
			}
		}
	}
	throw ErrorObject(String("Unknown attribute %S", sToken));
}

int FileFilter::iConvertTime(const String& sVal)
{
	struct tm when;
	memset(&when, 0, sizeof(when));
	Array<String> arTime;
	if ( sVal.find("now") == -1 )
	{
		Split(sVal, arTime, ":");
		if ( arTime.size() < 4)
		{
			arTime.resize(6);
			if ( arTime[0].iVal() > 31 )
			{
				arTime[3] = arTime[4] = arTime[5] = String("%d", 0);
			}
			else
			{
				for(int i=0; i<3; ++i)
					arTime[i+3] = arTime[i];

				CTime time = CTime::GetCurrentTime();

				arTime[0] = String("%d", time.GetYear());
				arTime[1] = String("%d", time.GetMonth());
				arTime[2] = String("%d", time.GetDay());

			}
			CTime tim(arTime[0].iVal(), arTime[1].iVal(), arTime[2].iVal(),
									 arTime[3].iVal(), arTime[4].iVal(), arTime[5].iVal());
  		return tim.GetTime();
		}
	}
	else
	{
		CTime time = CTime::GetCurrentTime();
		time -= CTimeSpan(sVal.sTail("-").iVal(), 0, 0, 0);
		return time.GetTime();
	}
	return 0;
}

String FileFilter::sMapType(SectionEntry *entry, const String& sVal)
{
	switch ( entry->type.kind )
	{
		case otRasterMap:
			return sVal == "true" ? "mapvirtual" : "mapstore";
		case otPolygonMap:
			return sVal == "true" ? "polygonmapvirtual" : "polygonmapstore";
		case otSegmentMap:
			return sVal == "true" ? "segmentmapvirtual" : "segmentmapstore";
		case otPointMap:
			return sVal == "true" ? "pointmapvirtual" : "pointmapstore";
	};

	throw ErrorObject("No type defined");

	return "?";
}

void FileFilter::Value(SectionEntry *entry)
{
	bool fOKOp= true;
	if ( sToken == "=")
		entry->op = opEQ;
	else if ( sToken == "<>")
		entry->op = opNOTEQ;
	else if ( sToken == ">" )
		entry->op = opGREAT;
	else if ( sToken == ">=")
		entry->op = opGREATEQ;
	else if ( sToken == "<" )
		entry->op = opLESS;
	else if ( sToken == "<=")
		entry->op = opLESSEQ;
	else if ( sToken == "contains" )
		entry->op = opCONTAINS;
	else
		fOKOp = false;

	if ( fOKOp )
	{
		Next(rsReadValue);
		SectionEntry *last = entry;
		while(last->next != NULL) // go to end of chain
			last = last->next;
		if ( last->sAttribute == "time")
			last->sValue = String("%lu", iConvertTime(sToken));
		else if (entry->sUIAttribName == "dependent")
			last->sValue = sMapType(last, sToken);
    else
		{
			if ( last->sAttribute == "type")
			{
				map<String, String>::iterator where = UINames.find(sToken);
				if ( where != UINames.end() )
						sToken = (*where).second;
			}				
			last->sValue = sToken;
			if ( sToken.find('.') == -1 )
			{
				map<String, ObjTypeInfo>::iterator cur;

				cur = Types.find(last->sAttribute);

				if ( cur != Types.end())
				{
					last->sValue = sToken + (*cur).second.sExt;
				}
				if ( last->sAttribute == "coordbounds")
				{
					if(last->sValue.sHead(",").rVal() == rUNDEF || last->sValue.sTail(",").rVal() == rUNDEF )
						throw ErrorObject(String("Incorrect coordinate pair : %S", last->sValue));
				}
			}
			if ( last->sValue[0] != '\"' && last->sValue.find('*') != -1)
					last->fWildCardValue = true;
		}
		return ;
	}
	throw ErrorObject(String("Unknown operator : %S at character %d", sToken, iC - sToken.size()));
}

void FileFilter::Next(rsReadState rs)
{
	sToken = "";
	size_t iSz = sFilter.size();
	if ( iC == iSz ) return;
	char c = sFilter[iC];
	while ( iC < iSz && isspace((unsigned char)c) )
		c = sFilter[++iC];

	if ( c == '\"')
	{
		c = sFilter[++iC];
		while ( c != '\"' && iC < iSz)
		{
			sToken += c;
			c = sFilter[++iC];
		}
		++iC; // last char was a ", skip it.
	}
	else if ( rs == rsReadValue )
	{
		while ( isspace((unsigned char)c) == 0 && c!= ')' && iC < iSz)
		{
			sToken += c;
			c = sFilter[++iC];
		}
	}
	else if ( isdigit((unsigned char)c))
	{
		int iOneComma=0;
		while ( isdigit((unsigned char)c) && iC < iSz)
		{
			sToken += c;
			c = sFilter[++iC];
			if ( c = '.' )
			{
				if ( iOneComma != 0) return;
				sToken + c;
				c = sFilter[++iC];
			}
		}
	}
	else if ( isalnum((unsigned char)c) )
	{
		while(isalnum((unsigned char)c) && iC < iSz )
		{
			sToken += c;
			c = sFilter[++iC];
		}
	}
	else
	{
		while ( iC < iSz && isspace((unsigned char)c) == 0 && !isalnum((unsigned char)c) && c != '\"' && c!='*')
		{
			sToken += c;
			c = sFilter[++iC];
		}
	}
	sLastToken = sToken;

}

void FileFilter::Previous()
{
	iC -= (sToken.size() + 1);
	if ( iC <= 0 ) return;
	char c = sFilter[iC];
	while (iC > 0 && isspace((unsigned char)c))
	   c = sFilter[--iC];
	while( iC > 0 )
	{
		if ( iC > 1 )
			c = sFilter[iC];
		if ( !isalnum((unsigned char)c) )
			break;
		--iC;
	}
}

vector<NameExt>& FileFilter::GetNameExt()
{
	return lsExt;
}

void FileFilter::SetPossibleExt(bool fYesNo)
{
	for(vector<NameExt>::iterator cur=lsExt.begin(); cur != lsExt.end(); ++cur)
		(*cur).fShow = fYesNo;
}

void FileFilter::SaveSettings(IlwisSettings& settings)
{
	settings.SetValue("IncludeSystemObjects", fSystemObjects);
	settings.SetValue("ShowCollectionContents", fShowCollectionContents);
	settings.SetValue("NrOfObjects", (long)lsExt.size());
	int i=0;
	for(vector<NameExt>::iterator cur = lsExt.begin(); cur != lsExt.end(); ++cur)
	{
		settings.SetValue(String("Object%d", i++), (*cur).fShow);
	}
}

void FileFilter::LoadSettings(IlwisSettings& settings)
{
	int iNr = settings.iValue("NrOfObjects");
	fSystemObjects = settings.fValue("IncludeSystemObjects", false);
	fShowCollectionContents = settings.fValue("ShowCollectionContents", false);
	if ( iNr != lsExt.size() ) return; // nr of objects has changed, settings can not be trusted
	int i=0;
	for(vector<NameExt>::iterator cur = lsExt.begin(); cur != lsExt.end(); ++cur)
	{
		(*cur).fShow = settings.fValue(String("Object%d", i++));
	}
}

void FileFilter::IncludeSystemObjects( bool fYesNo)
{
	fSystemObjects = fYesNo;
}

bool FileFilter::fIncludeSystemObjects()
{
	return fSystemObjects;
}

void FileFilter::ShowContainerContents( bool fYesNo)
{
	fShowCollectionContents = fYesNo;
}

bool FileFilter::fShowContainerContents()
{
	return fShowCollectionContents;
}
