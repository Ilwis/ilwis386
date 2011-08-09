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
/*
*/
#define TBLGLUE_C

#include "Applications\Table\TblGlue.h"
#include "Headers\Hs\tbl.hs"

IlwisObjectPtr * createTableGlue(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)TableGlue::create(fn, (TablePtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new TableGlue(fn, (TablePtr &)ptr);
}

const char* TableGlue::sSyntax()
{
		return "TableGlue(table1, table2, ..., tableN)\n"
					 "TableGlue(tabel1, table2, ..., tableN, Vertical)\n"
					 "TableGlue(Domain, table1, table2, ..., TableN)\n";
}

void ThrowCouldNotCreateTable(const FileName& fn, String& sName)
{
		String s(TR("Could not create table %S").c_str(), sName);
		
		throw ErrorObject(WhatError(s, errTableGlue+1), fn);
}

void ThrowIllegalDomainCombination(const FileName& fn)
{
		String s(TR("Tables to be merged do not have \nthe correct domain combinations")); 	 
		throw ErrorObject(WhatError(s, errTableGlue+2), fn);
} 	 
 
TableGlue *TableGlue::create(const FileName& fn, TablePtr& p, const String& sExpr)
{

		Array<String> as;
		int iParms = IlwisObjectPtr::iParseParm(sExpr, as); 
		if ( iParms<2)
				throw ErrorExpression(sExpr, sSyntax());

		Array<Table> tabList;
		bool fUnder = false;
		FileName fnDom(as[0]);
		
		for(int i = fnDom.fExist() ? 1 : 0 ; i<iParms; ++i)
		{
				if (i == iParms-1 && fCIStrEqual(as[i],"Vertical")) fUnder=true;
				else
				{
						Table tab(as[i]);
						if (tab.fValid())
								tabList&=tab;
						else
								ThrowCouldNotCreateTable(fn, as[i]);
				} 							 
		}

		if (fnDom.fExist())
				return new TableGlue(fn, p, Domain(fnDom), tabList);
		else		
				return new TableGlue(fn, p, tabList, fUnder);
	
}

TableGlue::TableGlue(const FileName _fileName, TablePtr& p, Domain& dom, const Array<Table>& _tabs) :
		TableVirtual(_fileName, p, true),
		fUnder(false),
		tmComb(tmUnknown)
{
		for(unsigned int i=0; i<_tabs.iSize(); ++i)
		{
				tableAr&=_tabs[i];
				objdep.Add(tableAr[i]);
		}
		fTablesHaveSameDom = false;
		if (dom->pdsrt())
			tmComb = CheckDomain();
		if (tmComb == tmAllUniqueID) 		
			fUnder = true;
//				tmComb = dom->pdc()==NULL ? tmAllId : tmAllClass;
		
		ptr.SetDomain(dom);
		
		if (!fnObj.fValid())
				objtime = objdep.tmNewest();
		fNeedFreeze = true;
		sFreezeTitle = "TableGlue"; 			 
		ptr.sDescription = sExpression();
}

TableGlue::TableGlue(const FileName _fileName, TablePtr& p, const Array<Table>& _tabs, bool _fUnder) :
		TableVirtual(_fileName, p, true),
		fUnder(_fUnder)
{
		for(unsigned int i=0; i<_tabs.iSize(); ++i)
		{
				tableAr&=_tabs[i];
				objdep.Add(tableAr[i]);
		} 			 
		
		fTablesHaveSameDom = false;
		tmComb=CheckDomain();
		switch (tmComb)
		{
			case tmAllUniqueID: 		
				fUnder = true;
			case tmAllClass:
			case tmAllId:	
			{
 // 			fUnder=false; // should already be the case.
				FileName fn=FileName::fnUnique(FileName(ptr.fnObj,".dom",true));
				Domain dom; 			
				switch (tmComb)
				{
					case tmAllId:
						 dom = Domain(fn, 0, dmtID );; break;
					case tmAllClass:
						 dom = Domain(fn, 0, dmtCLASS );; break;
					case tmAllUniqueID:
						 dom = Domain(_fileName, 0, dmtUNIQUEID, tableAr[0]->dm()->pdsrt()->sGetPrefix()); break;
						
				} 				
				ptr.SetDomain(dom);
			} break;
			case tmAllNone:
			{
				ptr.SetDomain(Domain("none"));
			} break;
	/*		case tmAllSame:
			{
				ptr.SetDomain(tableAr[0]->dm());
			} break; */
		}
		if (fTablesHaveSameDom)
			ptr.SetDomain(tableAr[0]->dm());
		if (!fnObj.fValid())
				objtime = objdep.tmNewest();
		fNeedFreeze = true;
		sFreezeTitle = "TableGlue"; 			 
		ptr.sDescription = sExpression();
} 	 

TableGlue::TableGlue(const FileName& fName, TablePtr &tab) :
		TableVirtual(fName, tab)
{
		int iNo;
		
		ReadElement("TableGlue", "NoOfTables", iNo);
		ReadElement("TableGlue", "Under", fUnder);
		for(int i=0; i<iNo; ++i)
		{
				FileName fn;
				String s("Table%d", i);
				ReadElement("TableGlue", s.c_str(), fn);
				Table tab(fn);
				if (tab.fValid())
				{
						tableAr&=tab;
						objdep.Add(tab);
				} 					 
		}
		tmComb=CheckDomain();
		fNeedFreeze = true;
		sFreezeTitle = "TableGlue"; 
}


void TableGlue::Store()
{
		TableVirtual::Store();
		WriteElement("TableVirtual", "Type", "TableGlue");
		WriteElement("TableGlue", "NoOfTables", (long)tableAr.iSize());
		WriteElement("TableGlue", "Under", fUnder);

		for(unsigned int i=0; i<tableAr.iSize(); ++i)
		{
				String s("Table%d", i);
				WriteElement("TableGlue", s.c_str(), tableAr[i]->fnObj);
		}
}

bool TableGlue::fFreezing()
{
		trq.SetTitle(TR("TableGlue"));
		trq.SetText(TR("Adding records"));
		ArrayLarge<long> aiRecode;
		long iStartRecNo = 1, iTotalUniqueIDs = 0;
		DomainSort* pdsrt1 = dm()->pdsrt();
		DomainNone* pdnon = dm()->pdnone();
		bool fEmptyInputDomain = true;
		if (pdsrt1)
			fEmptyInputDomain = (pdsrt1->iSize() == 0);
		else if (pdnon)
			fEmptyInputDomain = true;
		if (fTablesHaveSameDom && tmComb != tmAllNone && tmComb != tmAllUniqueID )
			tmComb = tmAllSame;
		switch (tmComb) 
		{
			case tmAllClass:
			case tmAllId:
			case tmAllUniqueID:
			case tmAllSame:
			{
				for(unsigned int i=0; i<tableAr.iSize(); ++i)
				{
					// make distinction between TableGlue using inp Tables directly (from cmd line of apaform
					// and TableGlue called via an sExpression made by MapGlue, PointGlue or SegmentGlue
					if (pdsrt1 && fEmptyInputDomain)
					{
						pdsrt1->Merge(tableAr[i]->dm()->pdsrt(), &trq);
					}
					if (!fMakeRecodeTable(tableAr[i]->dm()->pdsrt(), aiRecode, iTotalUniqueIDs )) return false;
					if (!fMergeTable(tableAr[i], aiRecode, iStartRecNo)) return false;
					aiRecode.Resize(0);
				} 					 
			} break;
			case tmAllNone:
			{
				long iMaxSz=0;
				for(unsigned int i=0; i<tableAr.iSize(); ++i)
						iMaxSz= fUnder ? iMaxSz + tableAr[i]->iRecs() : max(iMaxSz, tableAr[i]->iRecs());
				ptr.iRecNew(iMaxSz);
				for(unsigned i=0; i<tableAr.iSize(); ++i)
				{
					 if (!fUnder) iStartRecNo=1;
					 if(!fMergeTable(tableAr[i], aiRecode, iStartRecNo)) return false;
				} 					 
			} break;
			default:
				ThrowIllegalDomainCombination(fnObj);
		}
		return true;
}

bool TableGlue::fMakeRecodeTable(DomainSort *pdsrt2, ArrayLarge<long>& aiRecode, long& iStartRecNo)
{
	trq.SetText(TR("Making recoding table"));
	DomainSort* pdsrt1 = dm()->pdsrt();
//		aiRecode.Resize(pdsrt2->iSize(),1); 			
	aiRecode.Resize(pdsrt2->iSize()); // zero based
	for ( unsigned int i = 1; i <= aiRecode.iSize(); i++)
	{
		if ( trq.fUpdate(i, aiRecode.iSize())) return false;
		if ( pdsrt2->pdUniqueID() )
			aiRecode[i-1] = iStartRecNo + i;
		else											
			aiRecode[i-1] = pdsrt1->iRaw(pdsrt2->sNameByRaw(i,0));
	} 
	iStartRecNo += pdsrt2->iSize();
	return true;
} 	 

bool TableGlue::fMergeTable(Table& SourceTable, ArrayLarge<long>& aiRecode, long& iInsertRecNo)
{
	long iAddedRecords=0;
	for(int i = 0; i<SourceTable->iCols(); ++i)
	{
		bool fSort = ( tmComb == tmAllClass) || ( tmComb == tmAllId);
		Column colSource = SourceTable->col(i);
		if (!colSource.fValid()) continue;
		//String sColSrcName = colSource->sName();
		//if (sColSrcName == "npix" || sColSrcName == "Area")
		//	continue; //don't copy numerical area info in the glued result map
		String sN(TR("Merging table %S : Col %S").c_str(), SourceTable->sName(), colSource->sName());
		trq.SetText(sN);
		
		Column colNew = NewColumn(colSource);
		if ( colNew.fValid())
		{
			iAddedRecords = 0;
			for(long j = 0; j < SourceTable->iRecs(); ++j)
			{
				if ( trq.fUpdate(j, SourceTable->iRecs())) return false;
				long iRecNo = iInsertRecNo+j;
				String sVal = colSource->sValue(j+1);
				long iNewRaw; 
				if (fSort) 
					iNewRaw = aiRecode[iRecNo-1]; 
				else
					iNewRaw = iRecNo;
				if (iNewRaw == iUNDEF) continue;
				colNew->PutVal(iNewRaw, sVal);
				++iAddedRecords;
			}
		} 					 
	}
	if (fUnder) iInsertRecNo += iAddedRecords;

	return true;
}


TableGlue::tmDomainCombinations TableGlue::CheckDomain()
{
	Domain dm0 = tableAr[0]->dm();
		bool fAllClass = true, fAllId = true, fAllNone = true, fAllSame = true, fAllUniqueID = true;
		if (tableAr.iSize() == 0) return tmUnknown;
		
		for(unsigned int i = 0; i < tableAr.iSize(); ++i)
		{
				const Domain &dom = tableAr[i]->dm();
				fAllSame = fAllSame && dom == dm0;
				DomainType dmType = dom->dmt();
				if ( dmType == dmtCLASS)	fAllUniqueID = fAllNone = false;
				else if ( dmType == dmtID ) fAllUniqueID = fAllClass = fAllNone = false;
				else if ( dmType == dmtNONE ) fAllUniqueID = fAllId = fAllClass = false;
				else if ( dmType == dmtUNIQUEID) fAllClass = fAllNone = false;
				else fAllId=fAllClass=fAllNone=false;
		} 

		if (fAllNone) {
			fTablesHaveSameDom = true;
			return tmAllNone;
		}
		else if (fAllSame) {
			fTablesHaveSameDom = true;//this property must not exclude the other tmDomainCombinations
			//return tmAllSame;
		}
		if (fAllClass && fAllId) return tmAllClass;
		else if ( fAllUniqueID ) return tmAllUniqueID;		
		if (fAllId) return tmAllId;
		return tmUnknown;
} 					 

Column TableGlue::NewColumn(const Column& col)
{ 	
	String sColName = col->sName();
	for(int iColnr = 0; iColnr < ptr.iCols(); ++iColnr)
	{ 	
		Column colCur = ptr.col(iColnr);
		if ( !colCur.fValid()) continue;
		String sColCurName = colCur->sName();
		if ( fCIStrEqual(col->sName(),colCur->sName()))
		{
//						String sN("%S%d", col->sName(),ptr.iCols());
			
			String sN;
			for (int iNr = 2;; iNr++) {
				sN = String("%S%i", sColName, iNr);
				long iCol = ptr.iCol(sN);
				if (iCol < 0)
					break;
			}

//					if (tmComb == tmAllClass || tmComb == tmAllId)
			if ((col->dm()->pdv() == NULL) && (col->dm() == colCur->dm()))
				return colCur;
///			if (fUnder && ( tmComb == tmAllNone || tmComb == tmAllUniqueID))
			{
				if ( col->dm()->pdv() && colCur->dm()->pdv())
				{
					ValueRange valr = MergeColumnValueRanges(col,colCur);
					ptr.RemoveCol(colCur);
					colCur->fErase = true;
					Column colNew = ptr.colNew(col->sName(), DomainValueRangeStruct(Domain("value"), valr));
					if (colNew.fValid())
							colNew->Fill();
					for(int j = 1; j < colCur->iRecs(); ++j)
						colNew->PutVal(j, colCur->sValue(j));
					
					return colNew;
				}
			}
	///		else 
				if (col->dm() == colCur->dm() && (tmComb != tmAllUniqueID) && (!fTablesHaveSameDom))
					return Column(); // invalid column
			else
			{
					Column colNew = ptr.colNew(sN, col->dvrs());
					colNew->Fill();
					return colNew;
			}

		}
	} 	 
	
	Column colNew=ptr.colNew(col->sName(), col->dvrs());
	String sNtest = String(colNew->sName());
	colNew->Fill();

	return colNew;
} 	 

ValueRange TableGlue::MergeColumnValueRanges(const Column& col1, const Column& col2)
{
		RangeReal rrR1 = col1->vr()->rrMinMax();
		RangeReal rrR2 = col2->vr()->rrMinMax();
										
		double rStep = min(col1->vr()->rStep(), col2->vr()->rStep());
		double rMax = max(rrR1.rHi(), rrR2.rHi());
		double rMin = min(rrR1.rLo(), rrR2.rLo());
		return ValueRange(rMin, rMax, rStep);
}

String TableGlue::sExpression() const
{
		String sExpr("TableGlue(");
		
		for(unsigned int i=0; i<tableAr.iSize(); ++i)
		{
				if (i!=0) sExpr&=",";
				sExpr&=tableAr[i]->sNameQuoted();
		}
		if ( fUnder) sExpr&=",Vertical";
		
		sExpr&=")";

		return sExpr;
} 	 
		




