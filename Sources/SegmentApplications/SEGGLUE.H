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
/* $Log: /ILWIS 3.0/SegmentMap/SEGGLUE.H $
 * 
 * 5     15-11-01 12:07 Retsios
 * Now also return clipping region in sExpression (bug#4931), analog to
 * polglue.
 * Removed unused variable.
 * 
 * 4     20-02-01 19:43 Hendrikse
 * added bool fAllAttrTablesEqual; to avoid redundant new attr table(s)
 * 
 * 3     30-01-01 17:46 Hendrikse
 * corrected header comment
 * 
 * 2     30-01-01 17:05 Hendrikse
 * adde enum OutputDomain and related members similar to pointmapglue
 * additions
// Revision 1.4  1998/09/16 17:25:20  Wim
// 22beta2
//
// Revision 1.3  1997/07/30 15:04:07  Wim
// Syntax error
//
// Revision 1.2  1997-07-30 15:59:35+02  Wim
// Added sNewDom option
//
/* SegmentMapGlue
   Copyright Ilwis System Development ITC
   september 1996, by Jan Hendrikse
	Last change:  WK   30 Jul 97    5:03 pm
*/

#ifndef ILWSEGMAPGLUE_H
#define ILWSEGMAPGLUE_H
#include "Engine\Applications\SEGVIRT.H"
#include "Engine\Base\mask.h"

IlwisObjectPtr * createSegmentMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class  _export SegmentMapGlue: public SegmentMapVirtual
{
  friend class SegmentMapVirtual;
	
public:
  SegmentMapGlue(const FileName&, SegmentMapPtr& p);
  SegmentMapGlue(const FileName& fn, SegmentMapPtr& p, const CoordBounds& cb,
                 const Array<SegmentMap>& asmp, const Array<String>& asMask,
                 const String& sNewDomain);
  ~SegmentMapGlue();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static SegmentMapGlue* create(const FileName&, SegmentMapPtr& p, const String& sExpression);
	void Init();	
	enum OutputDomain
		{odNOTYETKNOWN, odINVALID, odUNIQUEID, odID, odCLASS, odVALUE, odBOOL};
	static void CheckAndFindOutputDomain(const long iSegMap, const Array<SegmentMap> apmp,
																				String sTypeN, OutputDomain& odOut);
	static OutputDomain FindOutputDomain(OutputDomain odOut , const Domain dm_i); 

private:	
	OutputDomain odFinal;
	void GlueAttributeTables();
  bool fAllAttrTablesEqual;
	Array<SegmentMap> asmp;
	Array<String> asMask;
	int iSegMap;
	String sNewDom;
	bool fUsesClipping;
};

#endif




