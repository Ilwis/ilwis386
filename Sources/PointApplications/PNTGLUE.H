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
/* $Log: /ILWIS 3.0/PointMap/PNTGLUE.H $
 * 
 * 6     20-02-01 19:45 Hendrikse
 * added bool fAllAttrTablesEqual; to avoid redundant new attr table(s)
 * 
 * 5     1-02-01 17:37 Hendrikse
 * added const qualif in  CheckAndFindOutputDomain(const long iPntMap,
 * const Array<PointMap> apmp,
 * 
 * 4     23-01-01 12:02 Hendrikse
 * made public: functions on domaintype to make available for app form
 * 
 * 3     16-01-01 14:03 Hendrikse
 * comment correction
 * 
 * 2     10-01-01 18:20 Hendrikse
 * added members:	enum OutputDomain
 * {odNOTYETKNOWN, odINVALID, odUNIQUEID, odID, odCLASS, odVALUE, odBOOL};
 * OutputDomain FindOutputDomain(OutputDomain odOut , const Domain dm_i); 
 *  OutputDomain odFinal;
 * 	void CheckAndFindOutputDomain();
 * 	void MergeDomainSorts(const PointMap& pmp);
// Revision 1.3  1998/09/16 17:26:27  Wim
// 22beta2
//
// Revision 1.2  1997/07/30 09:45:08  Wim
// Allow sNewDomain instead of internal domain
//
	 PointMapGlue
   Copyright Ilwis System Development ITC
   september 1996, by Jan Hendrikse
	Last change:  WK   30 Jul 97   10:53 am
*/

#ifndef ILWPNTMAPGLUE_H
#define ILWPNTMAPGLUE_H
#include "Engine\Applications\PNTVIRT.H"
#include "Engine\Base\mask.h"

IlwisObjectPtr * createPointMapGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class PointMapGlue: public PointMapVirtual
{
  friend class PointMapVirtual;
public:
  PointMapGlue(const FileName&, PointMapPtr&);
  PointMapGlue(const FileName& fn, PointMapPtr&, const CoordBounds& cb,
               const Array<PointMap>& apmp, const Array<String>& asMask,
               const String& sNewDomain);
  ~PointMapGlue();
  static const char* sSyntax();
  virtual String sExpression() const;
  virtual void Store();
  virtual bool fFreezing();
  virtual bool fDomainChangeable() const;
  static PointMapGlue* create(const FileName&, PointMapPtr&, const String& sExpression);
	void Init();
	enum OutputDomain
		{odNOTYETKNOWN, odINVALID, odUNIQUEID, odID, odCLASS, odVALUE, odBOOL};
	static void _export CheckAndFindOutputDomain(const long iPntMap, const Array<PointMap> apmp,
																				String sTypeN, OutputDomain& odOut);
	static OutputDomain FindOutputDomain(OutputDomain odOut, const Domain dm_i); 

private:
  OutputDomain odFinal;
	void GlueAttributeTables();
	bool fAllAttrTablesEqual;
	Array<PointMap> apmp;
	Array<String> asMask;
	long iPntMap;
	String sNewDom;
	bool fUsesClipping;
};

#endif




