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
   March 2005, by Lichun Wang
*/

#include "HydroFlowApplications\TblHortonPlots.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\COLSTORE.H"
#include "Engine\Applications\COLVIRT.H"
#include "Engine\Applications\MAPVIRT.H"
#include "Engine\Domain\dmsort.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\tbl.hs"
#include "Headers\Hs\map.hs"

const char* TableHortonPlots::sSyntax()
{
  return "TableHortonPlots(CatchmentMap,DrainageOrderingMap)\n";
}

IlwisObjectPtr * createTableHortonPlots(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableHortonPlots::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableHortonPlots(fn, (TablePtr &)ptr);
}

String TableHortonPlots::sExpression() const
{
  String sMap1 = m_mpCatchment->sNameQuoted(true, fnObj.sPath());
  String sMap3 = m_mpDrainage->sNameQuoted(true, fnObj.sPath());
  String sMap2 = m_mpMergedCatchment->sNameQuoted(true, fnObj.sPath());
  return String("TableHortonPlots(%S,%S, %S)", sMap1, sMap2, sMap3);
}

void CompitableGeorefs(FileName fn, Map mp1, Map mp2)
{
	bool fIncompGeoRef = false;
	if (mp1->gr()->fGeoRefNone() && mp2->gr()->fGeoRefNone())
		  fIncompGeoRef = mp1->rcSize() != mp2->rcSize();
	else
			fIncompGeoRef = mp1->gr() != mp2->gr();
	if (fIncompGeoRef)
			throw ErrorIncompatibleGeorefs(mp1->gr()->sName(true, fn.sPath()),
            mp2->gr()->sName(true, fn.sPath()), fn, errTableHortonPlots);
}

static Table VerifyAttributes(Map mpMap)
{
	
	Table tblAtt = mpMap->tblAtt();
	if (!tblAtt.fValid())
				throw ErrorNoAttTable(mpMap->fnObj);
	return tblAtt;
}

TableHortonPlots* TableHortonPlots::create(const FileName& fn, TablePtr& p, const String& sExpr)
{
  Array<String> as;
  int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
  if (iParms !=3 ) 
    throw ErrorExpression(sExpr, sSyntax());
  Map mpCatchment(as[0], fn.sPath());
  Map mpMergedCatchment(as[1], fn.sPath());
  Map mpDrainage(as[2], fn.sPath());
  
  CompitableGeorefs(fn, mpCatchment, mpMergedCatchment);
  CompitableGeorefs(fn, mpCatchment, mpDrainage);
  
  return new TableHortonPlots(fn, p, mpCatchment, mpMergedCatchment, mpDrainage);
}

TableHortonPlots::TableHortonPlots(const FileName& fn, TablePtr& p)
: TableVirtual(fn, p)
{
  ReadElement("TableHortonPlots", "CatchmentMap", m_mpCatchment);
  ReadElement("TableHortonPlots", "CatchmentMergedMap", m_mpMergedCatchment);
  ReadElement("TableHortonPlots", "DrainageOrderMap", m_mpDrainage);
  Init();
}

TableHortonPlots::TableHortonPlots(const FileName& fn, TablePtr& p, 
                                   const Map& mpCatchment,
                                   const Map& mpMergedCatchment,
                                   const Map& mpDrainage)
                                   
: TableVirtual(fn, p, true),
  m_mpCatchment(mpCatchment), m_mpMergedCatchment(mpMergedCatchment), m_mpDrainage(mpDrainage)
{
  Init();
  FileName fnDom = FileName::fnUnique(FileName(fnObj, ".dom"));
  ptr.SetDomain(Domain(fnDom, 0, dmtID));
  if (!fnObj.fValid())
    objtime = objdep.tmNewest();
}

TableHortonPlots::~TableHortonPlots()
{
}

void TableHortonPlots::Store()
{
  TableVirtual::Store();
  WriteElement("TableVirtual", "Type", "TableHortonPlots");
  WriteElement("TableHortonPlots", "CatchmentMap", m_mpCatchment);
  WriteElement("TableHortonPlots", "CatchmentMergedMap", m_mpMergedCatchment);
  WriteElement("TableHortonPlots", "DrainageOrderMap", m_mpDrainage);
}

static void SplitString(String s, vector<long> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	results.clear();
	for (unsigned int i=0; i < as.size(); i++)
	{
		long res = as[i].iVal();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

static void SplitString(String s, vector<double> &results)
{
	Array<String> as;
	Split(s, as, ",{}");
	results.clear();
	for (unsigned int i=0; i < as.size(); i++)
	{
		long res = as[i].rVal();
		if (res != iUNDEF)
			results.push_back(res);
	}
}

void TableHortonPlots::AddDomainItem(Domain dm, long iItem )
{
	String sUniqueID = String("%li", iItem);
	dm->pdsrt()->iAdd(sUniqueID,true);
}

bool TableHortonPlots::fFreezing()
{
  trq.SetTitle(sFreezeTitle);
  trq.SetText(SMAPTextInitializeMap);
  trq.Start();

  long iLines = m_mpCatchment->iLines();
  long iCols = m_mpCatchment->iCols();
  m_vCatchment.resize(iLines);  
  m_vMergedCatchment.resize(iLines);  
  m_vDrainage.resize(iLines);

  //Reading input maps
  for (long iRow = 0; iRow< iLines; iRow++ )
  {
	m_vCatchment[iRow].Size(iCols); 
	m_mpCatchment->GetLineRaw(iRow, m_vCatchment[iRow]);

    m_vMergedCatchment[iRow].Size(iCols); 
	m_mpMergedCatchment->GetLineRaw(iRow, m_vMergedCatchment[iRow]);
	m_vDrainage[iRow].Size(iCols);
	m_mpDrainage->GetLineRaw(iRow, m_vDrainage[iRow]);
		
	if (trq.fUpdate(iRow, iLines)) return false;	
  }

  //Create columns in the output table
  Table tblAtt = m_mpMergedCatchment->tblAtt();
  Column colDrainageID = tblAtt->col(String("DrainageID"));
  DomainSort* pdsrt = colDrainageID->dmKey()->pdsrt();
  if ( !pdsrt )
	throw ErrorObject(SMAPErrNoDomainSort);
  long iSize = pdsrt->iSize();
  CreateColumns(iSize);
  long iMaxOrderNumber = 0;

  //check if column DrainageLen is valid in the input attribute table of the catchment
  //this column was calculared during catchemnt merging process using outlet locations
  //it is used to store the drainage length from/starting at the outlet coordinate!  
  bool fDrainageLen = false;
  Column colDrainageLen;
  if (tblAtt[String("DrainageLen")].fValid())
  {
     colDrainageLen = tblAtt->col(String("DrainageLen"));
     fDrainageLen = true; 
  }

  //Create a seperate table for ratios
  String sRatioTable("%S_Ratio", fnObj.sFile);
  FileName fnTbl_Ratio(sRatioTable, ".tbt");
  //FileName fnTbl_Ratio(fnObj, ".tbt");
  fnTbl_Ratio = FileName::fnUnique(fnTbl_Ratio);
  m_tbl_ratio = Table(fnTbl_Ratio, Domain("HortonRatio"));
  

  trq.SetText("Compute Horton Statistics");
  for (long i=1; i<=iSize; i++) {
	long ID = pdsrt->iOrd(i); 
	if (ID == iUNDEF)
		continue;
    vector<long> vDrainageIDs;
	SplitString(colDrainageID->sValue(i), vDrainageIDs);

    vector<double> vDrainageLen;
    if (fDrainageLen)
      SplitString(colDrainageLen->sValue(i), vDrainageLen);

    vector<long> vOrder = GetOrderNumber(vDrainageIDs);
    
	//returns the position of the first element with max. in vOrder
	vector<long>::iterator pos = max_element(vOrder.begin(), vOrder.end());
	long iMax = *pos;
	ComputeHortonPlots(i, iMax, vDrainageIDs, vOrder,fDrainageLen, vDrainageLen);
    if (iMax > iMaxOrderNumber)
      iMaxOrderNumber = iMax;
    if (trq.fUpdate(i, iSize)) return false;
  }
  ptr.dm()->pdsrt()->Resize(iMaxOrderNumber);  
  m_vCatchment.resize(0);
  m_vMergedCatchment.resize(0);
  m_vDrainage.resize(0);
  return true;
}
void TableHortonPlots::Init()
{
  if (!m_mpCatchment.fValid() || !m_mpDrainage.fValid() || !m_mpMergedCatchment.fValid())
		throw ErrorDummy();

  //Verify domain and attribute table
	Domain dm = m_mpCatchment->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(SMAPErrIDDomain_S, errTableHortonPlots), m_mpCatchment->fnObj);

  dm = m_mpMergedCatchment->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(SMAPErrIDDomain_S, errTableHortonPlots), m_mpMergedCatchment->fnObj);

  dm = m_mpDrainage->dm();
	if (!(dm.fValid())) 
			throw ErrorObject(WhatError(SMAPErrIDDomain_S, errTableHortonPlots), m_mpDrainage->fnObj);

	Table tblCachment = VerifyAttributes(m_mpCatchment);
	if (!tblCachment[String("DrainageID")].fValid())
					ColumnNotFoundError(tblCachment->fnObj, String("DrainageID"));
  if (!tblCachment[String("CatchmentArea")].fValid())
					ColumnNotFoundError(tblCachment->fnObj, String("CatchmentArea"));
  
  Table tblMergedCachment = VerifyAttributes(m_mpMergedCatchment);
	if (!tblMergedCachment[String("DrainageID")].fValid())
					ColumnNotFoundError(tblCachment->fnObj, String("DrainageID"));

  Table tblDrainage = VerifyAttributes(m_mpDrainage);
	if (!tblDrainage[String("Strahler")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Strahler"));
	if (!tblDrainage[String("Shreve")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Shreve"));
  if (!tblDrainage[String("Length")].fValid())
					ColumnNotFoundError(tblDrainage->fnObj, String("Length"));
	objdep.Add(m_mpCatchment.ptr());
  objdep.Add(m_mpMergedCatchment.ptr());
  objdep.Add(m_mpDrainage.ptr());
  fNeedFreeze = true;
  sFreezeTitle = "TableHortonPlots";
  htpFreeze = htpHorntonPlotsT;
}

void TableHortonPlots::CreateColumns(long iSize)
{
  Column cOrder = pts->colNew("Order", Domain("value"), ValueRange(0,32767,1));   
  for (long i=1; i<=iSize; i++)
  {
     String sCol1 = String("C%li_N", i);
     String sCol2 = String("C%li_L", i);
     String sCol3 = String("C%li_A", i);
     Column cC_N = pts->colNew(sCol1, Domain("value"), ValueRange(1,32767,1));   
     Column cC_L = pts->colNew(sCol2, Domain("value"), ValueRange(-1,1.0e300,0.01));
     Column cC_A = pts->colNew(sCol3, Domain("value"), ValueRange(-1,1.0e300,0.01));
  }
}

vector<long> TableHortonPlots::GetOrderNumber(vector<long> vIDs)
{
  Table tblAtt = m_mpDrainage->tblAtt();
  Column colStralhaer = tblAtt->col(String("Strahler"));
  vector<long> vOrder;
  vector<long>::iterator pos;
	for (pos = vIDs.begin(); pos < vIDs.end(); ++pos)
	{		
      vOrder.push_back(colStralhaer->iValue((*pos))); 
	}
  return vOrder; 
}

bool TableHortonPlots::fUpstreamSameOrder(long id, long iOrder, vector<long> vDrainageIDs)
{
  Table tbl = m_mpDrainage->tblAtt();
  Column cUpstreamLink = tbl->col(String("UpstreamLinkID"));
  Column cOrder = tbl->col(String("Strahler"));
  String sLinks = cUpstreamLink->sValue(id);
  vector<long> vLinks;
  SplitString(sLinks, vLinks);

  vector<long>::iterator pos;
  for (pos = vLinks.begin(); pos < vLinks.end(); ++pos)
  {		
     if (cOrder->iValue((*pos)) == iOrder)
	 {
        long drainageID = (*pos);
	 	vector<long>::iterator pos = find(vDrainageIDs.begin(), vDrainageIDs.end(), drainageID);
        if (pos != vDrainageIDs.end())	
          return true;
	 }	
  }
  return false;
}

//Compute parameters for Horton Plots, and
//also store values in the ouput table
void TableHortonPlots::ComputeHortonPlots(long CatchmentNr, long iMaxOrder, vector<long> vDrainageIDs, vector<long> vOrder, bool fDrainageLen, vector<double> vDrainageLen)
{
    //Retrieve drainage attributes from related table 
    Table tblAtt = m_mpDrainage->tblAtt();
    Column colLength = tblAtt->col(String("Length"));

    //Retrieve catchment attributes from the related table 
    Table tblCam = m_mpCatchment->tblAtt();
    Column colArea = tblCam->col(String("CatchmentArea"));

    //Compute the number of channels of order starting from 1 to iMaxOrder
    String sCol1 = String("C%li_N", CatchmentNr);
    String sCol2 = String("C%li_L", CatchmentNr);
    String sCol3 = String("C%li_A", CatchmentNr);
    String sCol5 = String("C%li_N_LSq", CatchmentNr); 
    String sCol6 = String("C%li_L_LSq", CatchmentNr);  
    String sCol7 = String("C%li_A_LSq", CatchmentNr);  
    String sCol8 = String("C%li_RB", CatchmentNr); 
    String sCol9 = String("C%li_RL", CatchmentNr);  
    String sCol10 = String("C%li_RA", CatchmentNr);  
    
    String sCol5b = String("C%li_N_LSq_b", CatchmentNr); 
    String sCol6b = String("C%li_L_LSq_b", CatchmentNr);  
    String sCol7b = String("C%li_A_LSq_b", CatchmentNr);  
    String sCol8b = String("C%li_RB_b", CatchmentNr); 
    String sCol9b = String("C%li_RL_b", CatchmentNr);  
    String sCol10b = String("C%li_RA_b", CatchmentNr);  

    Column col1 = ptr.col(sCol1); 
    Column col2 = ptr.col(sCol2); 
    Column col3 = ptr.col(sCol3); 
    Column col4 = ptr.col("Order");
    //total upstream length and area 
    double rTotalLength = 0;
    double rTotalArea = 0;
    double rPreArea = 0;
    for (long i=1; i<=iMaxOrder; i++)
    {
      AddDomainItem(ptr.dm(), i);
      col4->PutVal(i,i);
      //count number of elements with the given order number
  	  long iCout = count(vOrder.begin(), vOrder.end(), i);
	  long iNrOrder = 0;
      if (iCout > 0)
      {
        //computer length of streams of each order
        //find the first element with given value
        double rLength = 0;
        double rArea = 0;
        vector<long>::iterator pos = find(vOrder.begin(), vOrder.end(), i);
        int iIndex = pos - vOrder.begin();
        while (pos != vOrder.end())
		{
          long id = vDrainageIDs[iIndex];
          if (i==1)
            iNrOrder = iCout;
          else
          {
            if (!fUpstreamSameOrder(id,i, vDrainageIDs))  
              iNrOrder++;
          }
          if (fDrainageLen)
            if (vDrainageLen[iIndex] > 0.0)
            {
              rLength += vDrainageLen[iIndex];
              rArea += colArea->rValue(id);
            }
            else
            {
              rLength += colLength->rValue(id);
              rArea += colArea->rValue(id);
            }
          else
          {
            rLength += colLength->rValue(id);
            rArea += colArea->rValue(id);
          }
		  pos = find(++pos, vOrder.end(), i);
		  iIndex = pos - vOrder.begin();
		}
        col1->PutVal(i,iNrOrder);
        rLength = (rLength/iNrOrder)/1000;
        rArea = (rArea/iNrOrder)/1000000;
        
        rTotalLength = rTotalLength + rLength;
        rTotalArea = (rPreArea + rArea*iNrOrder)/iNrOrder; 
        rPreArea = rPreArea + rArea*iNrOrder;
        col2->PutVal(i,rTotalLength);
        col3->PutVal(i,rTotalArea);
      }
      else
      {
        iMaxOrder = i-1;
        break;
      }
    }
    //Compute regression coefficients
    Column col;
    if (iMaxOrder>2)
    {
      //Calculate Rb
	  Regress(CatchmentNr,col1,col4,sCol5,iMaxOrder, false); 
      col = ptr.col(sCol5);
      CalculateRB(col, sCol8, true);
      Regress(CatchmentNr,col1,col4,sCol5b,iMaxOrder, true); //using all orders  
      col = ptr.col(sCol5b);
      CalculateRB(col, sCol8b, true);
      //This theoretical mim. value of ratio is 2, so if a value is smaller than 2, should be undefined   
      /*Column cRb = ptr.col(sCol5);
      for(int iOrder=1;iOrder<=iMaxOrder;iOrder++)
      {
        if(cRb->rValue(iOrder) < 2)
        {
          cRb->PutVal(iOrder, rUNDEF);
        }
      }*/

      //Calculate Rl
      Regress(CatchmentNr,col2,col4,sCol6,iMaxOrder, false); 
      col = ptr.col(sCol6);
      CalculateRB(col, sCol9, false);
      Regress(CatchmentNr,col2,col4,sCol6b,iMaxOrder, true); //using all orders
      col = ptr.col(sCol6b);
      CalculateRB(col, sCol9b, false);
      
      //Calculate Ra
      Regress(CatchmentNr,col3,col4,sCol7,iMaxOrder, false); 
      col = ptr.col(sCol7);
      CalculateRB(col, sCol10, false);
      Regress(CatchmentNr,col3,col4,sCol7b,iMaxOrder, true); //using all orders
      col = ptr.col(sCol7b);
      CalculateRB(col, sCol10b, false);
    

      //Extrat ratios to a seperate table
      String sCol_a = String("C%li_a", CatchmentNr);
      String sCol_b = String("C%li_b", CatchmentNr);
      Column cC_a = m_tbl_ratio->colNew(sCol_a, Domain("value"), ValueRange(-1,1.0e300,0.01));
      Column cC_b = m_tbl_ratio->colNew(sCol_b, Domain("value"), ValueRange(-1,1.0e300,0.01));
      DomainSort* gdsrt = m_tbl_ratio->dm()->pdsrt();
      double val;
      long iRaw;

      iRaw = gdsrt->iOrd("Rb");
      col = ptr.col(sCol8); //Rb
      val = col->rValue(1);
      cC_a->PutVal(iRaw, val); 
      col = ptr.col(sCol8b); //Rb_b
      val = col->rValue(1);
      cC_b->PutVal(iRaw, val);

      iRaw = gdsrt->iOrd("Rl");
      col = ptr.col(sCol9); //Rl
      val = col->rValue(1);
      cC_a->PutVal(iRaw, val); 
      col = ptr.col(sCol9b); //Rb_b
      val = col->rValue(1);
      cC_b->PutVal(iRaw, val);

      //Extract Ra 
      iRaw = gdsrt->iOrd("Ra");
      col = ptr.col(sCol10); //Ra
      val = col->rValue(1);
      cC_a->PutVal(iRaw, val); 
      col = ptr.col(sCol10b); //Ra_b
      val = col->rValue(1);
      cC_b->PutVal(iRaw, val);

      //Clean up temp. columns
      col = ptr.col(sCol8);
      pts->RemoveCol(col);
      col = ptr.col(sCol9);
      pts->RemoveCol(col);
      col = ptr.col(sCol10);
      pts->RemoveCol(col);
      col = ptr.col(sCol5b);
      pts->RemoveCol(col);
      col = ptr.col(sCol6b);
      pts->RemoveCol(col);
      col = ptr.col(sCol7b);
      pts->RemoveCol(col);
      col = ptr.col(sCol8b);
      pts->RemoveCol(col);
      col = ptr.col(sCol9b);
      pts->RemoveCol(col);
      col = ptr.col(sCol10b);
      pts->RemoveCol(col);
	}
}

void TableHortonPlots::Regress(long CatchmentNr, Column colY, Column colX, String sOutCol, int iOrders, bool usAllOrders)
{
    DomainValueRangeStruct dvrs (Domain("value"), ValueRange(-1,1.0e300,0));
    String sExpr4("%S", colY->sNameQuoted());
	String sY = String("C%li_new_cY", CatchmentNr); 
    Column new_cY(Table(ptr.fnObj), sY, sExpr4, dvrs);   
	new_cY->fErase = true;
	new_cY->SetReadOnly(false);
	new_cY->BreakDependency();
    new_cY->Calc();
    
	if (usAllOrders != true){
    
		if(iOrders>=5)
		{
			new_cY->PutVal(1, rUNDEF);
			new_cY->PutVal(iOrders, rUNDEF);
		}
		else if(iOrders==4)
		{
			new_cY->PutVal(iOrders, rUNDEF);
		}
		new_cY->Calc();
    }
    String sExpr1("log(%S)", new_cY->sNameQuoted());
	String sOutCol1 = String("C%li_OutCol1", CatchmentNr); 
    Column cOutCol1(Table(ptr.fnObj), sOutCol1, sExpr1, dvrs);   
    cOutCol1->fErase = true;
	cOutCol1->SetReadOnly(false);
	cOutCol1->BreakDependency();
    cOutCol1->Calc();

	String sExpr2("ColumnLeastSquaresFit(%S,%S,%S,%i)", colX->sNameQuoted(), cOutCol1->sNameQuoted(), String("polynomial"), 2);
	String sOutCol2 = String("C%li_OutCol2", CatchmentNr); 
    Column cOutCol2 (Table(ptr.fnObj), sOutCol2, sExpr2, dvrs);   
    cOutCol2->fErase = true;
	cOutCol2->SetReadOnly(false);
	cOutCol2->BreakDependency();
    cOutCol2->Calc();
    
    String sExpr3("10^(%S)", cOutCol2->sNameQuoted());
    Column cOutCol3 (Table(ptr.fnObj), sOutCol, sExpr3, dvrs);   
    cOutCol3->SetReadOnly(false);
    cOutCol3->BreakDependency();
    cOutCol3->Calc();

    //Clean up temp.
    pts->RemoveCol(cOutCol1);
    pts->RemoveCol(cOutCol2);
    pts->RemoveCol(new_cY);
}

void TableHortonPlots::CalculateRB(Column col, String sCol, bool isRB)
{
    String sExpr;
    DomainValueRangeStruct dvrs (Domain("value"), ValueRange(-1,1.0e300,0));
    if (isRB)
      sExpr= String("(%S)/(%S[%%R+1])", col->sNameQuoted(),col->sNameQuoted());
    else
      sExpr = String("(%S[%%R+1])/(%S)", col->sNameQuoted(),col->sNameQuoted());
    Column cOutCol (Table(ptr.fnObj), sCol, sExpr, dvrs);   
    cOutCol->SetReadOnly(false);
    cOutCol->BreakDependency();
    cOutCol->Calc();
}
