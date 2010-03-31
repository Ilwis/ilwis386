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
#ifndef FILEFILTER_H
#define FILEFILTER_H

#include <map>
#include <set>

enum otObjectType{ otRasterMap, otPolygonMap, otSegmentMap, otPointMap, otMap,
                   otDomain, otCoordSystem, otFunction, otTable, otRepresentation, otFilter,
									 otGeoRef, otIlwisObject, otMapView, otHistRas, otHistPol, otHistPnt, otHistSeg, 
									 otMatrix, otSampleSet, otObjCol, otMapList, otLayout, otScript, otTable2D, otAnnotationText,
                   otGraph, otSTEREOPAIR,
									 otAllObjects, otEnd};
enum stSection{stIlwis, stBasemap, stMap, stPolygonMap, stSegmentMap, stPointMap, 
               stTable, stProjection, stCoordSystem, stDomain, stGeoRef, stRepresentation,
							 stFunction, stMapList,
               stAllSections};
enum opOperator{opEQ, opLESS, opLESSEQ, opGREAT, opGREATEQ, opNOTEQ, opCONTAINS};
enum rsReadState{rsDontCare, rsReadValue};


struct NameExt 
{
  NameExt() {}
  NameExt(const String& name, const char* ext, bool fShw)
    : sName(name), sExt(ext), fShow(fShw) {}
  String sName;
  String sExt;
  bool fShow; 
};  

class ObjTypeInfo
{
	public:
		ObjTypeInfo() {}
		ObjTypeInfo(otObjectType _kind, String _sExt) : sExt(_sExt), kind(_kind) {}

	String sExt;
	otObjectType kind;	
};


class SectionEntry
{
	public:
		SectionEntry() : fWildCardValue(false), next(NULL) {}
		~SectionEntry();
		SectionEntry(const String& _sSection, const String& _sAttribute, const String& _sUIAttribName="", const String& _sValue="");

	  ObjTypeInfo type;

		String sSection;
		String sUIAttribName; // name as in syntax, ODF attributes need not to have the same name as in the syntax
		String sAttribute;
		String sValue;
		bool fWildCardValue;
		opOperator op;
		SectionEntry *next; //two level deep query
};

class Section
{
	list<SectionEntry> attribs;
};

class ParseTree
{
	public:
		ParseTree();
		~ParseTree();

		SectionEntry *entry;
		ParseTree *or;
		ParseTree *and;
};

typedef map<String, ObjTypeInfo>::iterator TypeIter;

class FileFilter
{
	public:
		FileFilter();
		~FileFilter();
		bool fParse(const String& sFilter);
		bool fOK(const FileName& fnObj, ParseTree* node=NULL);
		set<String> UsedTypes;
		vector<NameExt> _export &GetNameExt();
		void SaveSettings(IlwisSettings& settings);
		void LoadSettings(IlwisSettings& settings);
		void IncludeSystemObjects( bool fYesNo);
		bool fIncludeSystemObjects();
		bool fShowContainerContents();
		void ShowContainerContents( bool fYesNo);

	private:
		ParseTree *root;
		String sFilter;
		String sToken;
		String sLastToken; // erro message purposes
		int iC;
		map<String, ObjTypeInfo> Types;
		map<String, String> UINames;
		vector< list<SectionEntry> > Sections;
		vector< list<stSection> > ObjSections;
		vector<NameExt> lsExt;
		bool fSystemObjects;
		bool fShowCollectionContents;

		void init();
		void Next(rsReadState rs=rsDontCare);
		void Previous();
		void Expr(ParseTree * node)	;
		void Term(ParseTree *node);
		void Attribute(SectionEntry *entry);
		void Value(SectionEntry *entry);
		int  iConvertTime(const String& sVal);
		String sMapType(SectionEntry *entry, const String& sVal);
		bool fStringOK(const String& sName, const String& sMask, bool fWildCardValue);
		bool fAttributeTypeOK(SectionEntry* entry, const FileName& fnObj);
		bool fCheckEntry(const FileName& fnObj, SectionEntry* entry);
		bool fCheckContains(const FileName& fnObj, SectionEntry *entry, const String& sVal);
		void SetPossibleExt(bool fYesNo);


};

#endif
