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
// $Log: /ILWIS 3.0/Matrix/Princcmp.cpp $
 * 
 * 22    29-01-03 9:50 Willem
 * - Added: When the matrix is calculated the time is also adjusted in the
 * [ILWIS] section (using Updated())
 * 
 * 21    10-01-03 15:02 Willem
 * - Changed: sExpression now always returns a valid expression. Before
 * this happened only when the internal maplist was loaded
 * 
 * 20    11/21/01 4:13p Lichun
 * The dependent maplist now is also opened, when the names of the bands
 * not known to the matrix yet, so they can be set in the Store() function
 * 
 * 19    11/20/01 14:32 Willem
 * - Changed order of reading from ODF in constructor
 * - Use internal variable to store band names instead of maplist. The
 * maplist may not be opened
 * 
 * 18    11/20/01 14:17 Hendrikse
 * fFactorAnal is now read earlier in the constructor in order to use the
 * Store() properly.
 * Checks now on 0x0 matrix in Freeze
 * 
 * 17    11/19/01 10:13 Willem
 * Dependency is now added before Store() is called
 * 
 * 16    11/16/01 16:16 Willem
 * - Removed lots of commented code
 * - Added code that allows a principle components matrix to be opened
 * without opening the maplist it depends on. This is allowed when the
 * matrix values already have been calculated. This allows the matrix to
 * be copied to a different directory while at the same time breaking its
 * dependencies
 * 
 * 15    30-10-01 13:09 Koolhoven
 * in constructor PrincCompPtr set ptr.pmov on this, to prevent that the
 * Store() does it wrong
 * 
 * 14    16-08-01 19:04 Koolhoven
 * PrincipalComponents and FactorAnalysis are now virtual MatrixObjects
 * 
 * 13    7-08-01 12:43 Koolhoven
 * removed domain member from MapList, because it served no purpose
 * 
 * 12    23-11-00 17:57 Koolhoven
 * added to sSyntax() optional nroutbands
 * 
 * 11    17-08-00 17:36 Hendrikse
 * Added a space in sSummary() 
 * for numberstrings after "Variance per band:\r\n");
 * to improve legibility
 * 
 * 10    1-03-00 10:45 Wind
 * added nr of output bands
 * 
 * 9     9-02-00 10:54 Wind
 * replaced \n with \r\n in additional info
 * 
 * 8     8-02-00 18:01 Wind
 * removed update catalog
 * 
 * 7     4-02-00 18:40 Hendrikse
 * modified in create() MapList constr into  MapList ml(FileName(as[0],
 * ".mpl"));
 * 
 * 6     4/01/00 9:52 Willem
 * Removed the cutting of of filenames at the 8 character when creating
 * names for the mapfiles (in GetRowText() function)
 * 
 * 5     9/29/99 10:51a Wind
 * added case insensitive string comparison
 * 
 * 4     9/29/99 10:26a Wind
 * added caseinsensitive string comparison
 * 
 * 3     9/08/99 1:01p Wind
 * changed constructor calls FileName(fn, sExt, true) to FileName(fn,
 * sExt)
 * or changed FileName(fn, sExt, false) to FileName(fn.sFullNameQuoted(),
 * seExt, false)
 * to ensure that proper constructor is called
 * 
 * 2     9/08/99 10:21a Wind
 * adpated to use of quoted file names
*/
// Revision 1.3  1998/09/16 17:30:35  Wim
// 22beta2
//
// Revision 1.2  1997/09/19 16:38:59  Wim
// Force rereading catalog after princcmp calculation
//
/* PrincCompPtr
   Copyright Ilwis System Development ITC
   August 1995, by Jelle Wind
	Last change:  WK   12 Aug 98    1:00 pm
*/

#include "Engine\Matrix\Princcmp.h"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\constant.h"
#include "Headers\Hs\DAT.hs"

PrincCompPtr::~PrincCompPtr()
{
}

static String sSyntax() {
	return "MatrixPrincComp(maplist[,nroutbands])\nMatrixFactorAnal(maplist[,nroutbands])";
}

PrincCompPtr* PrincCompPtr::create(const FileName& fn, MatrixObjectPtr& ptr, const String& sExpr)
{
	String sFunc = MatrixObjectPtr::sParseFunc(sExpr);
	if (!fCIStrEqual(sFunc,"MatrixPrincComp") && !fCIStrEqual(sFunc,"MatrixFactorAnal"))
		ExpressionError(sExpr, sSyntax());
	Array<String> as;
	int iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if (iParms > 2 || iParms <= 0)
		ExpressionError(sExpr, sSyntax());
	MapList ml(FileName(as[0], ".mpl"));
	for (int i=ml->iLower(); i<=ml->iUpper(); ++i)
		if (!ml->map(i)->fValues())
			ValueDomainError(ml->map(i)->dm()->sName(), ml->map(i)->fnObj, errMatrixPrincComp);
	bool fFacAnal = fCIStrEqual(sFunc, "MatrixFactorAnal");
	int iOutBands = ml->iSize();
	if (iParms == 2)
		iOutBands = as[1].iVal();
	return new PrincCompPtr(fn, ptr, ml, fFacAnal, iOutBands);
}

PrincCompPtr::PrincCompPtr(const FileName& fn, MatrixObjectPtr& ptr)
	: MatrixObjectVirtual(fn, ptr), m_iInputBands(0)
{
	// skip reading the maplist if the Values matrix is already stored in the ODF
	// because this is equivalent with reading only the MatrixStore object (which is non existing)
	// This "trick" allows a principle components matrix to be copied over
	// directory boundaries, when the dependencies also will be broken. In that case
	// the maplist will not be needed anymore and the links to the maplist are therefore
	// not updated, which would not allow the matrix to be opened in the new directory.
	ReadElement("Bands", "InputBands", m_iInputBands);
	String sVS;
	ReadElement("MatrixPrincComp", "MapList", m_fnMpl);
	ReadElement("MatrixPrincComp", "FactorAnalysis", m_fFactorAnal);
	ReadElement("Matrix", "Values_Size", sVS);  // check if values are already stored
	long iRows = sVS.sHead(" ").iVal();
	long iCols = sVS.sTail(" ").iVal();
	if ((m_iInputBands == iUNDEF || iRows * iCols == 0) && File::fExist(m_fnMpl)) // No values in ODF, then open maplist
		m_mpl = MapList(m_fnMpl);

	if (0==ReadElement("MatrixPrincComp", "CalcTime", m_tmCalc))
		m_tmCalc = 0;
	RealMatrix matEigVal;
	ptr.ReadElement("MatrixPrincComp", "EigenValues", matEigVal);
	if (matEigVal.iRows() > 0) 
	{
		m_vecEigVal = CVector(matEigVal.iRows());
		for (long i=0; i < matEigVal.iRows(); i++)
			m_vecEigVal(i) = matEigVal(i, 0);
	}
	ReadElement("MatrixPrincComp", "OutputBands", m_iOutBands);

	if (m_mpl.fValid())
	{
		m_iInputBands = m_mpl->iSize();
		for (int i = m_mpl->iLower(); i <= m_mpl->iUpper(); ++i)
		{
			if (!m_mpl->map(i)->fValues())
				ValueDomainError(m_mpl->map(i)->dm()->sName(), m_mpl->map(i)->fnObj, errMatrixPrincComp);
			m_asInputBandNames &= m_mpl->map(i)->fnObj.sFile;
		}
		objdep.Add(m_mpl.ptr());
		Store();  // make sure the bands information gets written
	}
	else
	{
		for (long i = 0; i < m_iInputBands; i++)
		{
			FileName fn;
			ReadElement("Bands", String("Band%d", i).c_str(), fn);
			m_asInputBandNames &= fn.sFile;
		}
	}
	if (m_mpl.fValid())
		if (m_iOutBands <= 0 || m_iOutBands > m_mpl->iSize())
			m_iOutBands = m_mpl->iSize();
}

PrincCompPtr::PrincCompPtr(const FileName& fn, MatrixObjectPtr& ptr, const MapList& maplist, bool fFacAnal, int iOutB)
	: MatrixObjectVirtual(fn, ptr, true)
	, m_mpl(maplist), m_fFactorAnal(fFacAnal), m_iOutBands(iOutB), m_iInputBands(0)
{
	m_fnMpl = m_mpl->fnObj;
	if (m_iOutBands <= 0 || m_iOutBands > m_mpl->iSize())
		m_iOutBands = m_mpl->iSize();
	m_iInputBands = m_mpl->iSize();

	objdep.Add(m_mpl.ptr());
	m_vecEigVal = CVector(ptr.iRows());
	for (int i = 0; i < ptr.iRows(); i++) 
		m_vecEigVal(i) = 0;
	m_tmCalc = 0; 
	
	ptr.pmov = this; // set before calling Store()
	ptr.Store(); // matrix needs to be present before mpl is created
	String sMplName = m_mpl->sNameQuoted(true, fnObj.sPath());
	String sExprMpl("MapListMatrixMultiply(%S,%S,%i)", sMplName, fn.sRelativeQuoted(), m_iOutBands);
	MapList mplLocal(FileName::fnUnique(FileName(fnObj, ".mpl")), sExprMpl);
	String sDescr;
	if (m_fFactorAnal)
		sDescr = String("Factors of %S", sMplName);
	else
		sDescr = String("Principal Components of %S", sMplName);
	mplLocal->SetDescription(sDescr);
	mplLocal->Store();
}

String PrincCompPtr::sType() const
{
	if (m_fFactorAnal)
		return "Factor Analysis Coefficients";
	return "Principal Components Coefficients";
}

String PrincCompPtr::sExpression() const
{
	String sName = m_fnMpl.sRelativeQuoted(false);
	if (m_mpl.fValid())
		sName = m_mpl->sNameQuoted();
	
	String sType = m_fFactorAnal ? "MatrixFactorAnal" : "MatrixPrincComp";
	String sExpr("%S(%S)", sType, sName);
	return sExpr;
}

void PrincCompPtr::Store()
{
	if (m_mpl.fValid() && ptr.sDescription == String())
		if (m_fFactorAnal)
			ptr.sDescription = String("Factor Analysis Coefficients of %S", m_mpl->sNameQuoted(true, fnObj.sPath()));
		else
			ptr.sDescription = String("Principal Components Coefficients of %S", m_mpl->sNameQuoted(true, fnObj.sPath()));

	MatrixObjectVirtual::Store();
	WriteElement("Matrix", "Type", "MatrixPrincComp");
	if (m_mpl.fValid())
	{
		WriteElement("MatrixPrincComp", "MapList", m_mpl);
		m_iInputBands = m_mpl->iSize();
		for (long i = 0; i < m_iInputBands; i++)
			m_asInputBandNames &= m_mpl->map(m_mpl->iLower() + i)->fnObj.sFile;
	}
	WriteElement("Bands", "InputBands", m_iInputBands);
	for (long i = 0; i < m_iInputBands; i++)
		WriteElement("Bands", String("Band%d", i).c_str(), m_asInputBandNames[i]);

	WriteElement("MatrixPrincComp", "FactorAnalysis", m_fFactorAnal);
	WriteElement("MatrixPrincComp", "CalcTime", m_tmCalc);
	ptr.WriteElement("MatrixPrincComp", "EigenValues", m_vecEigVal);
	WriteElement("MatrixPrincComp", "OutputBands", m_iOutBands);
}    

void PrincCompPtr::Freeze()
{
	ptr.mat = RealMatrix(m_mpl->iSize(), m_mpl->iSize());
	
	RealMatrix* matrix;
	if (m_fFactorAnal)
		matrix = m_mpl->mtCorr(true);
	else
		matrix = m_mpl->mtVarCov(true);
	if (0 != matrix && 0 != matrix->iCols() && 0 != matrix->iRows())
	{ 
		Tranquilizer trq;
		if (m_fFactorAnal)
			trq.SetTitle(TR("Calculate factors"));
		else
			trq.SetTitle(TR("Calculate principal components"));
		trq.Start();
		trq.SetText(TR("Calculating eigen vectors"));
		trq.fUpdate(0,0);

		CalcEigen(matrix);

		m_tmCalc = ObjectTime::timCurr();
		ptr.Updated();   // make sure the time is set properly!
		ptr.SetAdditionalInfoFlag(true);
		ptr.SetAdditionalInfo(sSummary());
	}
	else
		m_tmCalc = 0;
	ptr.Store();
}

static int iLargestIndex(const CVector& cv, const int i)
{
	int index = i;
	for (int j = i; j < cv.iLen(); ++j)
		if (cv(j) > cv(index)) index = j;
			return index;
}

void PrincCompPtr::CalcEigen(RealMatrix* mtInp)
{
	int i, index;
	int iSize = mtInp->iRows();
	double rDum;
	RVector   vecDum;
	RealMatrix matOrg(iSize);
	for (i = 0; i < iSize; ++i)
		matOrg.PutRowVec(i,mtInp->GetRowVec(i));
	// calculate Eigen values and Eigen vectors
	mtInp->EigenVec(ptr.mat, m_vecEigVal);
	
	// sort Eigen values and corresponding Eigen vectors
	// first transpose Eigen vector matrix to get row vectors
	ptr.mat.Transpose();
	for (i = 0; i < iSize; ++i)
	{
		index = iLargestIndex(m_vecEigVal, i);
		// swap vecEigVal(i) en vecEigVal(index):
		rDum = m_vecEigVal(i);
		m_vecEigVal(i) = m_vecEigVal(index);
		m_vecEigVal(index) = rDum;
		// swap row(i) en row(index) van de mt matrix:
		vecDum = ptr.mat.GetRowVec(i);
		ptr.mat.PutRowVec(i, ptr.mat.GetRowVec(index));
		ptr.mat.PutRowVec(index, vecDum);
	}
}

void PrincCompPtr::GetRowText(Array<String>& as) const
{
	as.Resize(m_iInputBands);
	String sPC = "PC";
	if (m_fFactorAnal)
		sPC = "Factor";
	for (int i = 0;  i < m_iInputBands; i++)
		as[i] = String("%S %i", sPC, i+1);
}

void PrincCompPtr::GetColumnText(Array<String>& as) const
{
	as = m_asInputBandNames;
}

String PrincCompPtr::sSummary() const
{
	int iSize = m_vecEigVal.iRows();
	String s("Variance per band:\r\n");
	for (int i=0; i < iSize; i++)
		s &= String("%7.2f ", m_vecEigVal(i));
	// normalize Eigen values to percentages rSum = 100%
	CVector vecEigValPerc(iSize);
	double rSum = 0;
	for (int i = 0; i < iSize; ++i)
		rSum += m_vecEigVal(i);
	vecEigValPerc = m_vecEigVal / (rSum / 100);
	s &= String("\r\nVariance percentages per band:\r\n");
	for (int i=0; i < vecEigValPerc.iRows(); i++)
		s &= String("%7.2f", vecEigValPerc(i));
	return s;
}

void PrincCompPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	objdep.Names(afnObjDep);
}

int PrincCompPtr::iWidth() const
{
	return 6;
}

String PrincCompPtr::sValue(int i, int j, int iW) const
{
	if (iW <= 0)
		iW = iWidth();
	return String("%*.3f", iW, ptr.mat(i, j));
}
