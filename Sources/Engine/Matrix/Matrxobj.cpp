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
/* MatrixObject, MatrixObjectPtr
   Copyright Ilwis System Development ITC
   august 1995, by Jelle Wind
	Last change:  J    21 Oct 99   10:03 am
*/

#include "Engine\Matrix\Matrxobj.h"
#include "Engine\Matrix\Princcmp.h"
#include "Engine\Matrix\Varcovmt.h"
#include "Engine\Matrix\Confmat.h"
#include "Engine\Base\System\mutex.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"

IlwisObjectPtrList MatrixObject::listMat;

MatrixObject::MatrixObject()
	: IlwisObject(listMat)
{}

MatrixObject::MatrixObject(const FileName& fn)
	: IlwisObject(listMat)
{ 
	FileName fnMat(fn, ".mat", false);
	MutexFileName mut(fnMat);
	SetPointer(MatrixObjectPtr::create(fnMat));
}

MatrixObject::MatrixObject(const FileName& fn, int iRows, int iCols)
	: IlwisObject(listMat, new MatrixObjectPtr(fn, iRows, iCols))
{
}

MatrixObject::MatrixObject(const FileName& fn, const String& sExpression)
	: IlwisObject(listMat, MatrixObjectPtr::create(fn,sExpression))
{}

MatrixObject::MatrixObject(const String& sExpression, const String& sPath)
	: IlwisObject(listMat, MatrixObjectPtr::create(FileName::fnPathOnly(sPath),sExpression))
{}

MatrixObject::MatrixObject(const MatrixObject& mat)
	: IlwisObject(listMat, mat.pointer())
{}

MatrixObjectPtr* MatrixObject::pGet(const FileName& fn)
{
	return static_cast<MatrixObjectPtr*>(listMat.pGet(fn));
}

MatrixObjectPtr* MatrixObjectPtr::create(const FileName& fn)
{
	if (!File::fExist(fn))
		NotFoundError(fn);
	MutexFileName mut(fn);
	// see if matrix is already open (if it is in the list of open matrices)
	MatrixObjectPtr* p = MatrixObject::pGet(fn);
	if (p) // if already open return it
		return p;
	return new MatrixObjectPtr(fn);
}


MatrixObjectPtr* MatrixObjectPtr::create(const FileName& fn, const String& sExpression)
{
	if (fn.sFile.length() == 0) { // no file name
		// check if sExpression is an existing matrix on disk
		FileName fnMat(sExpression, ".mat", true);
		if (File::fExist(fnMat)) {
			MutexFileName mut(fn);
			// see if matrix is already open (if it is in the list of open matrices)
			MatrixObjectPtr* p = MatrixObject::pGet(fnMat);
			if (p) // if already open return it
				return p;
			// open map and return it
			return MatrixObjectPtr::create(fnMat);
		}
	}
	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	if (fCIStrEqual(sFunc, "MatrixVarCov"))
		return VarCovCorrMatrix::create(sExpression);
	if (fCIStrEqual(sFunc, "MatrixCorr"))
		return VarCovCorrMatrix::create(sExpression);
	if (fCIStrEqual(sFunc, "MatrixConfusion"))
		return MatrixConfusion::create(sExpression);
	
	return new MatrixObjectPtr(fn, sExpression);
}

MatrixObjectPtr::MatrixObjectPtr(const FileName& fn)
	: IlwisObjectPtr(FileName(fn, ".mat"))
	, pmov(0)
{
	if (!File::fExist(fn))
		NotFoundError(fn);
	
	String sType;
	ObjectInfo::ReadElement("Matrix", "Type", fn, sType);
	if ("MatrixPrincComp" == sType)
		pmov = new PrincCompPtr(fn, *this);
	else if ("RealMatrix" != sType)
		InvalidTypeError(fn, "Matrix", sType);
	
	ReadElement("Matrix", "Values", mat);
}

MatrixObjectPtr::MatrixObjectPtr(const FileName& fn, const String& sExpr)
	: IlwisObjectPtr(FileName(fn, ".mat"), true, ".mat")
	, pmov(0)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
	if (fCIStrEqual(sFunc, "MatrixPrincComp"))
		pmov = PrincCompPtr::create(fn, *this, sExpr);
	else if (fCIStrEqual(sFunc, "MatrixFactorAnal"))
		pmov = PrincCompPtr::create(fn, *this, sExpr);
	else
		throw ErrorAppName(fn, sFunc);
}

MatrixObjectPtr::MatrixObjectPtr(const FileName& fn, int iNrRows, int iNrCols)
	: IlwisObjectPtr(FileName(fn, ".mat"), true, ".mat")
	, pmov(0)
{
	mat = RealMatrix(iNrRows, iNrCols);
	for (int i = 0; i < iNrRows; i++)
		for (int j = 0; j < iNrCols; j++)
			mat(i,j) = 0;
}

MatrixObjectPtr::~MatrixObjectPtr()
{
}

String MatrixObjectPtr::sType() const
{
	if (pmov)
		return pmov->sType();
	return "Matrix";
}

void MatrixObjectPtr::Store()
{
	if (sDescription == "" && 0 != pmov)
		sDescription = pmov->sExpression();
	IlwisObjectPtr::Store();
	WriteElement("Ilwis", "Type", "Matrix");
	WriteElement("Matrix", "Values", mat);
	WriteElement("Matrix", "Type", "RealMatrix");
	if (0 != pmov)
		pmov->Store();
}

void MatrixObjectPtr::GetRowText(Array<String>& as) const
{
	if (pmov)
	{
		pmov->GetRowText(as);
		return;
	}
	as.Resize(iRows());
	for (int i=0; i < iRows(); i++)
		as[i] = String("%i", i+1);
}

void MatrixObjectPtr::GetColumnText(Array<String>& as) const
{
	if (pmov)
	{
		pmov->GetColumnText(as);
		return;
	} 
	as.Resize(iCols());
	for (int i=0; i < iCols(); i++)
		as[i] = String("%i", i+1);
}

void MatrixObjectPtr::MakeUsable()
{
	if (fDependent())
		IlwisObjectPtr::MakeUsable();
}

bool MatrixObjectPtr::fUsable() const
{
	return true;
}

int MatrixObjectPtr::iWidth() const
{
	if (pmov)
		return pmov->iWidth();
	return 6;
}

String MatrixObjectPtr::sValue(int iRow, int iCol, int iWidth) const
{
	if (pmov)
		return pmov->sValue(iRow, iCol, iWidth);

	String s = sUNDEF;
	if (iRow >= 0 && iRow < mat.iRows() &&
		iCol >= 0 && iCol < mat.iCols() )
		s = String("%*.3f", iWidth, mat(iRow, iCol));

	return s;
}

void MatrixObjectPtr::Rename(const FileName& fnNew)
{
	IlwisObjectPtr::Rename(fnNew);
}

bool MatrixObjectPtr::fDependent() const
{
	if (0 != pmov)
		return true;
	return false;
}

bool MatrixObjectPtr::fCalculated() const
{
	return mat.iRows() > 0;
}

void MatrixObjectPtr::Calc(bool fMakeUpToDate) 
{
	if (fMakeUpToDate)
		if (!objdep.fUpdateAll())
			return;
	if (!objdep.fMakeAllUsable())
		return;
	if (0 != pmov) 
		pmov->Freeze();
}

void MatrixObjectPtr::DeleteCalc() 
{
	mat = RealMatrix(); 
	Store();
}

void MatrixObjectPtr::BreakDependency() 
{
	if (!fCalculated())
		Calc();
	if (!fCalculated())
		return; 
	delete pmov;
	pmov = 0;
	fChanged = true;
	
	Store();
}

String MatrixObjectPtr::sExpression() const
{
	if (0 != pmov)
		return pmov->sExpression();
	return "";
}

IlwisObjectVirtual* MatrixObjectPtr::pGetVirtualObject() const
{
	return pmov;
}

void MatrixObjectPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
}

void MatrixObjectPtr::DoNotUpdate()
{
	IlwisObjectPtr::DoNotUpdate();
	
}
