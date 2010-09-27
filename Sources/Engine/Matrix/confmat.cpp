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
$Log: /ILWIS 3.0/Matrix/confmat.cpp $
 * 
 * 11    20-06-05 13:18 Retsios
 * [bug=6414] Special care for frequency columns with stREAL ("double"
 * values).
 * 1. The columns must be read with the correct function (rValue instead
 * of iValue)
 * 2. The rest of the confusion matrix code is designed for long int (4
 * bytes). Give an error message if the frequency column exceeds this.
 * Note that calculation could still go wrong if the column's values are
 * high (resulting in e.g. a sum that doesn't fit in 4 bytes), but this
 * overflow is not being checked.
 * 
 * 10    25-07-03 9:43 Willem
 * [Bug=6469,6190]
 * The calculation of the confusion matrix did not use the same order to
 * travers the domain items as was used in the header row and first
 * column. 
 * This gives a problem when the domain items are in a different order
 * that in which they are entered (because they are sorted)
 * 
 * The display order is now synchronized by using the iOrd() function of
 * DomainSort (although different from 3.11, because of bug 6190)
 * 
 * 9     7-01-03 17:44 Willem
 * - Removed: old commented code
 * - Changed: use the correct order of the domain items for the column and
 * row headers
 * 
 * 8     31-10-00 15:50 Hendrikse
 * made 3 text improvements
 * 
 * 7     28-10-00 11:35 Hendrikse
 * implemented members conditions to allow for 2 different input domains
 * including totalisation per domain item in the output table and
 * excluding the computation of accuracy and reliability in that case
 * 
 * 6     5/24/00 5:00p Wind
 * ConfusionMatrix activated and made available through commandline
 * MatrixConfusion(tbl,col1,col2,colfreq)
 * 
 * 5     9-02-00 10:54 Wind
 * replaced \n with \r\n in additional info
 * 
 * 4     9/10/99 12:55p Wind
 * comments
 * 
 * 3     9-09-99 3:45p Martin
 * //->/*
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
//Revision 1.4  1998/09/16 17:30:35  Wim
//22beta2
//
//Revision 1.3  1997/11/19 09:09:57  janh
//In GetRowText and GetColunText arrays are now filled correctly
//In MakeUsable() 2nd forloop counts now also last record from col1
//for(j=0;j<mat.iCols-2;j++) makes dat column unclassified will not be included in com
//in computation of rRel
//iTotSum is replaced by iTotalRowSum and placed in the rAcc computation
//where it should be  (see B.Gorte Image Classif RSD 1995 Ch 5)
//
//Revision 1.2  1997/09/11 11:29:45  janh
//In MakeUsable() the  cases iRaw(k) == iUNDEF are now treated correctly
//In fIndex iUNDEF -1 was returned as 1 and subsequentlypassed to iFreq
//In the Constructor I increased the nr of Columns of mat with 1
//to allow the display of UNCLASSIFIED pixels
//It required an increase of index counters for colnr on 6 other places
//
/* MatrixConfusion
   Copyright Ilwis System Development ITC
   Oct. 1996 by Jelle Wind
	Last change:  WK   12 Aug 98   12:25 pm
*/

#include "Engine\Matrix\Confmat.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\DAT.hs"

static String sSyntax() {
 return "MatrixConfusion(tbl,col1,col2,colFreq)";
}

MatrixConfusion::~MatrixConfusion()
{
}

MatrixConfusion* MatrixConfusion::create(const String& sExpr)
{
	String sFunc = MatrixObjectPtr::sParseFunc(sExpr);
	if (!fCIStrEqual(sFunc,"MatrixConfusion") )
		ExpressionError(sExpr, sSyntax());

	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms != 4)
		ExpressionError(sExpr, sSyntax());

	Table tbl(as[0]);
	Column col1 = tbl->col(as[1]);
	Column col2 = tbl->col(as[2]);
	Column colFreq = tbl->col(as[3]);
	return create(col1, col2, colFreq);
}

MatrixConfusion* MatrixConfusion::create(const Column& col1, const Column& col2, const Column& colFreq)
{
	if (0 == col1->dm()->pdsrt())
	{
		{
			String sDom = col1->dm()->sName(true, col1->fnTbl.sPath());
			WhereErrorColumn where(col1->fnTbl, col1->sName());
			ErrorSortDomain(sDom, where, errMatrixConfusion).Show(); 
		}
		return 0;
	}  
	if (0 == col2->dm()->pdsrt())
	{
		{
			String sDom = col2->dm()->sName(true, col2->fnTbl.sPath());
			WhereErrorColumn where(col2->fnTbl, col2->sName());
			ErrorSortDomain(sDom, where, errMatrixConfusion).Show(); 
		}
		return 0;
	}   
	if (!colFreq->dvrs().fValues())
	{
		{
			ErrorValueDomain(colFreq->dm()->sName(true, colFreq->fnTbl.sPath()),
			WhereErrorColumn(colFreq->fnTbl, colFreq->sName()), errMatrixConfusion+2).Show();
		}
		return 0;
	}
	if (colFreq->fUseReals())
	{
		RangeReal rr (colFreq->dvrs().rrMinMax());
		if (longConv(rr.rLo()) == iUNDEF || longConv(rr.rHi()) == iUNDEF)
		{
			{
				ErrorObject(WhatError(String("Value range too large for confusion matrix calculation."),
				errMatrixConfusion+3), WhereErrorColumn(colFreq->fnTbl, colFreq->sName())).Show();
			}
			return 0;
		}
	}
	return new MatrixConfusion(col1, col2, colFreq);
}

MatrixConfusion::MatrixConfusion(const Column& cl1, const Column& cl2, const Column& clFreq)
: MatrixObjectPtr(FileName(), 1, 1), col1(cl1), col2(cl2), colFreq(clFreq)
{
	pdsrt = cl1->dm()->pdsrt();
	pdsrt2 = cl2->dm()->pdsrt();
	
	if (cl1->dm() != cl2->dm())
	{
		fDiffDoms = true;
		mat = RealMatrix(pdsrt->iNettoSize(), pdsrt2->iNettoSize() + 1);
	}
	else
	{
		fDiffDoms = false;
		mat = RealMatrix(pdsrt->iNettoSize() + 1, pdsrt->iNettoSize() + 2);
	}
	objdep.Add(col1.ptr());
	objdep.Add(col2.ptr());
	objdep.Add(colFreq.ptr());
	sDescription = String(SDATMsgConfusionMatrix_SSS.scVal(), col1->sName(), col2->sName(), colFreq->sName());
	tmCalc = 0;
	MakeUsable();
}

String MatrixConfusion::sType() const
{
	return "Confusion Matrix";
}

String MatrixConfusion::sName(bool fExt, const String& sDirRelative) const
{
	String s = MatrixObjectPtr::sName(fExt, sDirRelative);
	if (s.length() != 0)
		return s;

	return String("MatrixConfusion(%S,%S,%S)", col1->sNameQuoted(), col2->sNameQuoted(), colFreq->sNameQuoted());
}

void MatrixConfusion::GetRowText(Array<String>& as) const
{
	as.Resize(pdsrt->iNettoSize()+1);
	for (int i = 1; i <= pdsrt->iNettoSize(); i++)
	{
		String sDomItem = pdsrt->sValue(i);
		as[i - 1] = sDomItem;
	}  
	as[pdsrt->iNettoSize()] = "RELIABILITY";
}

void MatrixConfusion::GetColumnText(Array<String>& as) const
{
	as.Resize(pdsrt2->iNettoSize() + 2);
	for (int i = 1; i <= pdsrt2->iNettoSize(); i++)
	{
		String sDomItem = pdsrt2->sValue(i).sTrimSpaces();
		as[i - 1] = sDomItem;
	}
	if (!fDiffDoms)
	{
		as[pdsrt2->iNettoSize()] = "UNCLASSIFIED";
		as[pdsrt2->iNettoSize() + 1] = "ACCURACY";
	}
	else
		as[pdsrt2->iNettoSize()] = "TOTALS";
}

String MatrixConfusion::sValue(int i, int j, int iW) const
{
	if (iW <= 0)
		iW = iWidth();
	if (!fDiffDoms)
	{
		if (i == pdsrt->iNettoSize())
		{
			if (j == pdsrt->iNettoSize() || j == pdsrt->iNettoSize()+1)
				return String();
			double r = arRel[j];
			if (r == rUNDEF)
				return sUNDEF;
			return String("%*.2f", iW, r);
		}  
		if (j == pdsrt->iNettoSize()+1)
		{
			if (i == pdsrt->iNettoSize())
				return String();
			double r = arAcc[i];
			if (r == rUNDEF)
				return sUNDEF;
			return String("%*.2f", iW, r);
		}
	}
	else
	{
		if (j == pdsrt2->iNettoSize())
		{
			if (i == pdsrt->iNettoSize())
				return String();
			long k = aiTotals[i];
			if (k == iUNDEF)
				return sUNDEF;
			return String("%*li", iW, k);
		}
	}
	return String("%*li", iW, longConv(mat(i, j)));
}

String MatrixConfusion::sSummary() const
{
	String s;
	if (fDiffDoms)
	{
		String sDom1 = col1->sName();
		String sDom2 = col2->sName();
		s = String("Confusion Matrix \r\nfor column %S against column %S.", sDom1, sDom2);
	}
	else
	{
		if (rAcc == rUNDEF) 
			s = String(SDATMsgAvgAccuracyUndef);  
		else
			s = String("%S   = %6.2f %%\r\n", SDATMsgAverageAccuracy, rAcc);
		if (rRel == rUNDEF) 
			s &= String(SDATMsgAvgReliabilityUndef);  
		else  
			s &= String("%S = %6.2f %%\r\n", SDATMsgAverageReliablity, rRel);
		if (rOverallAcc == rUNDEF) 
			s &= String(SDATMsgOverallAccurUndef);  
		else  
			s &= String("%S   = %6.2f %%\r\n", SDATMsgOverallAccuracy, rOverallAcc);
	}
	return s;
}

void MatrixConfusion::MakeUsable()
{
	long i, j;
	for (i=0; i < mat.iRows(); i++)
		for (j=0; j < mat.iCols(); j++) 
			mat(i, j) = 0;
		long iRecs = col1->iRecs(); // nr of records in cross-table
		long iOffset = col1->iOffset();  
		for (long k=iOffset; k < iOffset+iRecs; k++) {
			i = col1->iRaw(k);
			i = pdsrt->iOrd(i);
			if (i == iUNDEF) 
				continue;
			j = col2->iRaw(k);
			j = pdsrt2->iOrd(j);
			if (j == iUNDEF)
				j = mat.iCols() - 1;
			if (mat.fIndex(i-1, j-1)) {   // if element i,j belongs to cross-matrix
				if (colFreq->fUseReals())
				{
					double rFreq = colFreq->rValue(k);   // copy freq from crosstable
					if (rFreq != rUNDEF)
						mat(i-1, j-1) += rFreq;          // and put it cross matrix
				}
				else
				{
					long iFreq = colFreq->iValue(k);   // copy freq from crosstable
					if (iFreq != iUNDEF)
						mat(i-1, j-1) += iFreq;          // and put it cross matrix
				}
			}  
		} 
		
		arAcc.Resize(iRows());
		aiTotals.Resize(iRows());
		rAcc = 0;
		long iTotalRowSums = 0;
		long iUndf=0;
		long iLastRow = fDiffDoms?mat.iRows():mat.iRows()-1;
		for (i=0; i < iLastRow; i++) {
			long iSum = 0;
			for (j=0; j < mat.iCols()-1; j++)  // find total row sum
				iSum += (long)mat(i, j);
			aiTotals[i] = iSum;
			if (iSum != 0) {
				arAcc[i] = mat(i, i) / iSum;
				rAcc += arAcc[i];
			}  
			else {
				iUndf++;
				arAcc[i] = rUNDEF;
			} 
			iTotalRowSums += iSum;  // all pixels including unclassified ones
		}
		rAcc /= mat.iRows() - 1 - iUndf;
		rAcc *= 100;
		
		if (!fDiffDoms) {
			
			iUndf = 0;
			rRel = 0;
			arRel.Resize(iCols());
			for (j=0; j < mat.iCols()-2; j++) {
				long iSum = 0;
				for (i=0; i < mat.iRows()-1; i++)
					iSum += (long)mat(i, j);
				if (iSum != 0) {
					arRel[j] = mat(j, j) / iSum;
					rRel += arRel[j];
				}  
				else {
					arRel[j] = rUNDEF;
					iUndf++;
				}
			} 
			rRel /= mat.iCols() - 2 - iUndf;
			rRel *= 100;
			long iDiagSum = 0;
			for (i=0; i < mat.iRows()-1; i++)
				iDiagSum += (long)mat(i, i);
			if ( iTotalRowSums != 0)
				rOverallAcc = 100.0 * iDiagSum / iTotalRowSums;
			else  
				rOverallAcc = rUNDEF;
		}
		SetAdditionalInfoFlag(true);
		SetAdditionalInfo(sSummary());
		tmCalc = ObjectTime::timCurr();
}

bool MatrixConfusion::fUsable() const
{
	if (!objdep.fAllOlder(tmCalc))
		return false;
	return true;
}

int MatrixConfusion::iWidth() const
{
	return 8;
}
