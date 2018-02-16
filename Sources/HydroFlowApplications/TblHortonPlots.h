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
/* TableHortonPlots
   Copyright Ilwis System Development ITC
   march 2005, by Lichun Wang
*/
#ifndef ILWTABLEHORTONPLOTS_H
#define ILWTABLEHORTONPLOTS_H

#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Map\Raster\Map.h"
#include "LargeVector.h"

IlwisObjectPtr * createTableHortonPlots(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms );

struct coefficients
{
    double a;
    double b;
};

struct PlotPoints
{
    double x;
    double y;
};

class DATEXPORT TableHortonPlots: public TableVirtual
{
    friend class TableVirtual;

    public:
        static const char*      sSyntax();
        Column                  colMap1;
        Column                  colMap2;
        static TableHortonPlots*  create(const FileName& fn, TablePtr& p, const String& sExpression);
        TableHortonPlots(const FileName& fn, TablePtr& p);

    protected:
        TableHortonPlots(const FileName& fn, TablePtr& p, 
                         const Map& mpCatchment,
                         const Map& mpMergedCatchment,
                         const Map& mpDrainage);
                         
                   
        ~TableHortonPlots();

        virtual void     Store();
        virtual bool     fFreezing();
        virtual String   sExpression() const;
    
    private:
        void                      Init();
        void                      CreateColumns(long iSize);
        vector<long>              GetOrderNumber(vector<long> vIDs);
        void                      AddDomainItem(Domain dm,long item);
        bool                      fUpstreamSameOrder(long,long,vector<long>);
        void                      ComputeHortonPlots(long, long, vector<long>, vector<long>,bool, vector<double>);
        //coefficients              Regress(int,vector<PlotPoints>);
        void                      Regress(long CatchmentNr, Column, Column,String,int, bool);
        void                      CalculateRB(Column, String, bool);
        Map                       m_mpCatchment;
        Map                       m_mpDrainage;
        Map                       m_mpMergedCatchment;
        LargeVector<LongBuf>     m_vCatchment;   
	    LargeVector<LongBuf>     m_vDrainage;        
        LargeVector<LongBuf>     m_vMergedCatchment;
        Table                     m_tbl_ratio;
};

#endif 




