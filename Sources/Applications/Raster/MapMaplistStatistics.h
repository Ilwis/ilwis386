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
/* MapMaplistStatistics
   Copyright Ilwis System Development ITC
   April 2005, by Willem Nieuwenhuis
*/

#ifndef ILWMapMaplistStatistics_H
#define ILWMapMaplistStatistics_H
#include "Engine\Applications\MAPFMPL.H"

IlwisObjectPtr * createMapMaplistStatistics(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms=vector<void*>() );

class DATEXPORT MapMaplistStatistics: public MapFromMapList
{
    friend DATEXPORT MapVirtual;
    friend DATEXPORT MapFromMapList;
public:
    MapMaplistStatistics(const FileName& fn, MapPtr&);
    static MapMaplistStatistics* create(const FileName& fn, MapPtr&, const String& sExpression);
    MapMaplistStatistics(const FileName fn, MapPtr& p, const MapList& mpl,
            const string sStatFunction, const int iStartBand, const int iEndBand);
    ~MapMaplistStatistics();
    static const char* sSyntax();
    virtual String sExpression() const;
    virtual void Store();
    virtual bool fFreezing();

protected:

private:
	void clear(Map&, double value);
	bool CalculateSingleBuffer(Map&);
	bool CalculateDoubleBuffer(Map&);
	bool CalculatePixelBuffer(Map&);
	void SetDVRS(RangeReal&, double);

    string m_StatFunction;
    int m_iStartBand, m_iEndBand;
};

#endif
