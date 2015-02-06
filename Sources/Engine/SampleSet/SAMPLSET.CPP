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


#include "Engine\SampleSet\SAMPLSET.H"
#include "Engine\SampleSet\FSPACE.H"
#include "Engine\Map\Raster\MAPSTORE.H"
#include "Engine\Base\DataObjects\Tranq.h"
#include "Engine\Base\DataObjects\Buflist.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\System\mutex.h"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"
#include "Headers\Err\ILWISDAT.ERR"


IlwisObjectPtrList SampleSet::listSmpl;

SampleSet::SampleSet()
: IlwisObject(listSmpl)
{
}

SampleSet::SampleSet(const SampleSet& sms)
: IlwisObject(listSmpl, sms.pointer())
{
}

SampleSet::SampleSet(const FileName& fn)
: IlwisObject(listSmpl, FileName(fn, ".sms"))
{
	if (pointer())
		return;
	MutexFileName mut(fn);
	SampleSetPtr* p = SampleSet::pGet(fn);
	if (p) // if already open return it
		SetPointer(p);
	else
		SetPointer(new SampleSetPtr(FileName(fn, ".sms")));
}

SampleSet::SampleSet(IlwisObjectPtr* ptr)
: IlwisObject(listSmpl, ptr)
{
}

SampleSet::SampleSet(const FileName& fn, const String& sExpr)
: IlwisObject(listSmpl)
{
	SetPointer(SampleSetPtr::create(FileName(fn, ".sms"), sExpr));
}

SampleSet::SampleSet(const String& sExpr, const String& sPath)
: IlwisObject(listSmpl)
{
	FileName fn(sExpr, ".sms", true);
	if (0 == strchr(sExpr.c_str(), ':')) // no path set
		fn.Dir(sPath); 
	if (!File::fExist(fn))
		NotFoundError(fn);
	MutexFileName mut(fn);
	SampleSetPtr* p = SampleSet::pGet(fn);
	if (p) // if already open return it
		SetPointer(p);
	else
		SetPointer(new SampleSetPtr(fn.sFullPath()));
} 

SampleSet::SampleSet(const FileName& fn, const MapList& ml, const Domain& dm)
: IlwisObject(listSmpl)
{
	SetPointer(new SampleSetPtr(FileName(fn, ".sms"), ml, dm));
}

SampleSet::SampleSet(const FileName& fn, const MapList& ml, const Map& mapsmpl)
: IlwisObject(listSmpl)
{
	SetPointer(new SampleSetPtr(FileName(fn, ".sms"), ml, mapsmpl));
}

SampleSetPtr* SampleSet::pGet(const FileName& fn)
{
	return static_cast<SampleSetPtr*>(listSmpl.pGet(fn));
}


SampleSetPtr::~SampleSetPtr()
{
}

SampleSetPtr* SampleSetPtr::create(const FileName& fn, const String& sExpr)
{
	String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
	if (!fCIStrEqual(sFunc , "SampleSet"))
		ExpressionError(fn.sFullName(), sExpr);
	Array<String> as(2);
	if (!IlwisObjectPtr::fParseParm(sExpr, as))
		ExpressionError(fn.sFullName(), sExpr);
	MapList ml(as[0], fn.sPath());
	Domain dm(as[1], fn.sPath());
	return new SampleSetPtr(fn, ml, dm);
}

SampleSetPtr::SampleSetPtr(const FileName& fn)
: IlwisObjectPtr(fn)
{
	ReadElement("SampleSet", "MapList", _mpl);
	objdep.Add(_mpl.ptr());
	ReadElement("SampleSet", "Domain", _dm);
	objdep.Add(_dm.ptr());
	ReadElement("SampleSet", "SampleMap", smplmap);
	objdep.Add(smplmap.ptr());
	String s;
	ReadElement("SampleSet", "BackGround", s);
	if (s.length() > 0)
		ReadElement("SampleSet", "BackGround", backmap);
	if (backmap.fValid())  
		objdep.Add(backmap.ptr());
	try {
		Init();
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
}

SampleSetPtr::SampleSetPtr(const FileName& fn, const MapList& mpl, const Domain& dm)
: IlwisObjectPtr(fn, true, ".sms"), _mpl(mpl)
{
	_dm = dm;
	objdep.Add(_mpl.ptr());
	objdep.Add(_dm.ptr());
	Init();
	fChanged = true;
}

SampleSetPtr::SampleSetPtr(const FileName& fn, const MapList& mpl, const Map& smplmp)
: IlwisObjectPtr(fn, true, ".sms"), _mpl(mpl)
{
	objdep.Add(_mpl.ptr());
	smplmap = smplmp;
	objdep.Add(smplmap.ptr());
	_dm = smplmap->dm();
	Init();
	fChanged = true;
}

void SampleSetPtr::Init()
{
	_dc  = dm()->pdc();
	if (0 == _dc)
		throw ErrorClassDomain(_dc->sName(true, fnObj.sPath()), fnObj, errSampleSet);
	if (!mpl().fValid())
		return;
	iNrBands         = (short)mpl()->iSize();
	for (int i=mpl()->iLower(); i <= mpl()->iUpper(); i++)
		if (0 == mpl()[i]->dm()->pdi())
			throw ErrorImageDomain(mpl()[i]->dm()->sName(true, fnObj.sPath()), mpl()[i]->fnObj, errSampleSet+2);
	bNrClass         = (byte)dc()->iSize();
	fsl.Resize(0);
	_smplH.Allocate(dc(), _mpl);
	_smplS.Allocate(dc(), iNrBands);
	_sSum.Allocate(dc(), iNrBands);
	_sSumXY.Allocate(dc(), iNrBands);
	// empty selection-class: bKey = 0:
	RemSlct();
	fSelection = false;
	fInitStatistics = false;
}

void SampleSetPtr::Store()
{
	if (sDescription == "")
		sDescription = sTypeName();
	IlwisObjectPtr::Store();
	WriteElement("Ilwis", "Type", "SampleSet");
	WriteElement("SampleSet", "MapList", _mpl);
	WriteElement("SampleSet", "Domain", _dm);
	WriteElement("SampleSet", "SampleMap", smplmap);
	if (backmap.fValid())
		WriteElement("SampleSet", "BackGround", backmap);
	objdep.Store(this);
}    

String SampleSetPtr::sName(bool fExt) const
{
	String s = IlwisObjectPtr::sName(fExt);
	if (s.length() != 0)
		return s;
	else 
		return String("SampleSet(%S)", _mpl->sNameQuoted());
}

String SampleSetPtr::sType() const
{
	return "SampleSet";
}

void SampleSetPtr::SetMapList(const MapList& mpl)
{
	if (_mpl.fValid() && _mpl == mpl)
		return;
	_mpl = mpl;
	Updated();
	fInitStatistics = false;
}

void SampleSetPtr::SetBackground(const Map& bckmp)
{
	if (backmap.fValid() && (backmap == bckmp))
		return;
	backmap = bckmp;
	Updated();
}

void SampleSetPtr::SetSampleMap(const Map& smplmp)
{
	if (smplmap.fValid() && (smplmap == smplmp))
		return;

	if (smplmp->dm() != dm())
		throw ErrorObject(TR("Different domain of sampleset and map"));
	if (smplmp->gr() != mpl()->gr())
		throw ErrorObject(TR("Different georefs for set and map"));

	smplmap = smplmp;
	Updated();
	fInitStatistics = false;
}

bool SampleSetPtr::fInitStat()
{
	if (0 == _dc)
		return false;
	if (smplmap.fValid()) {
		if (smplmap->gr() != mpl()->gr())
			throw ErrorIncompatibleGeorefs(smplmap->gr()->sName(false, fnObj.sPath()),mpl()->gr()->sName(false, fnObj.sPath()), fnObj, errSampleSet+1);
	}  
	if (backmap.fValid()) {
		if (backmap->gr() != mpl()->gr())
			throw ErrorIncompatibleGeorefs(backmap->gr()->sName(false, fnObj.sPath()),mpl()->gr()->sName(false, fnObj.sPath()), fnObj, errSampleSet+1);
	}
	if (fInitStatistics)
		return true;
	Tranquilizer trq;
	bool fAborted = false;
	trq.SetTitle("Sample");
	trq.Start();
	if (!smplmap.fValid()) {
		trq.SetText(TR("Initiate sample set"));
		smplmap = Map(FileName::fnUnique(FileName(fnObj, ".mpr")), mpl()->gr(), mpl()[mpl()->iLower()]->rcSize(),dm());
		objdep.Add(smplmap.ptr());
		fChanged = true;
		//    MapStore* ptr = smplmap->pms();
		long iNrCols     = smplmap->iCols();
		long iNrRows     = smplmap->iLines();
		ByteBuf bb(iNrCols);
		for (int c = 0; c < iNrCols; c++)
			bb[c] = 0;
		for (int l = 0; l < iNrRows; l++) {
			if (trq.fUpdate(l, iNrRows))
			{ fAborted = true; break; }
			//      ptr->PutLineRaw(l, bb);
			smplmap->PutLineRaw(l, bb);
		}  
		fInitStatistics = !fAborted;
		return !fAborted;
	}  
	long iNrCols     = smplmap->iCols();
	long iNrRows     = smplmap->iLines();
	trq.SetText(TR("Init sample set statistics" ));
	ByteBuf            bb(iNrCols);
	ByteBufList        bbl(iNrBands, iNrCols);
	long c, r;
	short b, b2;
	byte bClass;
	//  MapStore* ptr = smplmap->pms();
	for (r = 0; r < iNrRows; r++)
	{
		if (trq.fUpdate(r, iNrRows))
		{ fAborted = true; break; }
		smplmap->GetLineRaw(r, bb);
		bool fLinesRead = false;
		for (c = 0; c < iNrCols; c++)
		{
			bClass = bb[c];  // Class number is raw value
			if (!bClass) continue;
			if (!fLinesRead) {
				for (b = 0; b < iNrBands; b++)
					mpl()[b+mpl()->iLower()]->GetLineRaw(r, bbl[b]);
				fLinesRead = true;
			}
			// the total NUMBER of pixels of
			// this class is incremented. This total
			// is stored in _sSum(this class, iNrBands).
			_sSum(bClass, iNrBands)++;
			for (b = 0; b < iNrBands; b++)
			{
				// init _smplH:
				// _smplH(this class, this band, this value) stores
				// the NUMBER of pixels in this class in this band
				// with this value. Each time it is recognised as
				// being part of the sample set in MapSample it is
				// incremented.
				_smplH(bClass, b, bbl[b][c])++;

				// init _sSum:
				// _sSum stores in _sSum(this class, this band) the sum
				// of all pixelvalues.
				byte bval = bbl[b][c];
				long sumval = _sSum(bClass, b);
				_sSum(bClass, b) += bbl[b][c];

				for (b2 = b; b2 < iNrBands; b2++)
				{
					// init _sSumXY:
					// _sSumXY stores in _sSumXY(this class, band1, band2) the
					// sum of the product of the pixelvalues in band1
					// and band2.
					if (b2 == b)
						_sSumXY(bClass, b, b) += ((double)bbl[b][c]) * ((double)bbl[b][c]);
					else
						_sSumXY(bClass, b, b2) += (double)bbl[b][c] * bbl[b2][c];
				}
			}
		}
	}
	if (trq.fUpdate(r, iNrRows))
		fAborted = true;
	if (!fAborted) {
		// init _smplS:
		// _smplS(this class, this band, CLASSMEAN) stores the
		// mean value of this band for this class. Likewise,
		// _smplS(this class, this band, CLASSSTD) stores the
		// mean value of this band for this class.
		byte bKey, bOrd;
		short b;
		double dSum, dMean, dNum, dStd;
		for (b = 0; b < iNrBands; b++)
		{
			for (bOrd = 1; bOrd <= dc()->iSize(); bOrd++)
			{
				//      bKey = dc()->iKey(bOrd);
				bKey = bOrd;
				dNum = _sSum(bKey, iNrBands);
				if (dNum == 0)
				{
					dMean = 0;
					dStd = 0;
				}
				else
				{
					dSum = _sSum(bKey, b);
					dMean = dSum / dNum;
					if (dNum <= 1)
						dStd = 0;
					else {
						double rV = _sSumXY(bKey, b, b) - dNum* dMean * dMean;
						dStd  = rV * rV / (dNum - 1);
					}
				}
				_smplS(bKey, b, CLASSMEAN) = dMean;
				_smplS.bMean(bKey, b) = byteConv(dMean);
				_smplS(bKey, b, CLASSSTD) = dStd;
			}
		}
	}
	fInitStatistics = !fAborted;
	return !fAborted;
}

byte SampleSetPtr::AppClass(const String& sClassName)
{
	long iKey = dc()->iRaw(sClassName);
	byte bKey = byteConv(iKey);
	if (_smplH.fExist(bKey))
		return bKey;
	bNrClass++;
	_smplH.AppClass(bKey);
	_sSum.AppClass(bKey);
	_sSumXY.AppClass(bKey);
	_smplS.AppClass(bKey);
	return bKey;
}

void SampleSetPtr::DelClass(byte bKey)
{
	_smplH.ResetClass(bKey);
	_sSum.ResetClass(bKey);
	_sSumXY.ResetClass(bKey);
	_smplS.ResetClass(bKey);
	//  IlwisAppContext* ilwapp;
	//  ilwapp = getEngine()->getContext();
	//  bool fUseYield = ilwapp->fUseYield();
	//  ilwapp->UseYield(true);
	Tranquilizer trq;
	byte bClass;
	//  MapStore* ptr = smplmap->pms();
	long iNrCols = smplmap->iCols();
	long iNrRows = smplmap->iLines();
	trq.SetTitle(SDATTitleDeleteFromSampleSet);
	trq.SetText(String(SDATTextDeleteClass_S.c_str(), dc()->sValueByRaw(bKey,0)));
	trq.fUpdate(0, iNrRows);
	trq.Start();
	long c, r;
	bool fLineChanged;
	ByteBuf bb(iNrCols);
	for (r = 0; r < iNrRows; r++)
	{
		if (trq.fUpdate(r, iNrRows))
			break;
		smplmap->GetLineRaw(r, bb);
		fLineChanged = false;
		for (c = 0; c < iNrCols; c++)
		{
			bClass = bb[c]; // Class number is raw value
			if (bClass == 0) continue;
			if (bClass == bKey) {
				bb[c] = 0;
				fLineChanged = true;
			}
		}
		if (fLineChanged)
			smplmap->PutLineRaw(r, bb);
	}
	for (unsigned int i = 0; i < fsl.iSize(); i++) 
		fsl[i]->Compute();
	//  ilwapp->UseYield(fUseYield);
}

byte SampleSetPtr::MrgClass(byte bKey1,
							byte bKey2)
{
	if (bKey1 == bKey2) return 0;
	_smplH.MrgClass(bKey1, bKey2);
	_sSum.MrgClass(bKey1, bKey2);
	_sSumXY.MrgClass(bKey1, bKey2);
	_smplS.MrgClass(bKey1, bKey2, _sSum, _sSumXY);
	byte bClass;
	IlwisAppContext* ilwapp = getEngine()->getContext();
	bool fUseYield = ilwapp->fUseYield();
	ilwapp->UseYield(true);
	Tranquilizer trq;
	trq.SetTitle(SDATTitleMergeSampleClasses);
	trq.SetText(String(SDATTextMergeClasses_SS.c_str(), dc()->sValueByRaw(bKey1,0), dc()->sValueByRaw(bKey2,0)));
	bool fAborted = false;
	//  MapStore* ptr = smplmap->pms();
	long iNrCols = smplmap->iCols();
	long iNrRows = smplmap->iLines();
	trq.fUpdate(0, iNrRows);
	trq.Start();
	long c, r;
	bool fLineChanged;
	ByteBuf bb(iNrCols);
	for (r = 0; r < iNrRows; r++)
	{
		if (trq.fUpdate(r, iNrRows))
		{ fAborted = true; break; }
		smplmap->GetLineRaw(r, bb);
		fLineChanged = false;
		for (c = 0; c < iNrCols; c++)
		{
			bClass = bb[c];
			if (bClass == 0) continue;
			if (bClass == bKey2) {
				bb[c] = bKey1;
				fLineChanged = true;
				RowCol rc(r, c);
			}
		}
		if (fLineChanged)
			smplmap->PutLineRaw(r, bb);
	}
	ilwapp->UseYield(fUseYield);
	for (unsigned int i = 0; i < fsl.iSize(); i++) 
		fsl[i]->Compute();
	if (fAborted)
		return 0;
	return bKey1;
}


void SampleSetPtr::StatInfo(byte   bKey,
							short  iBnd,
							double &rMean,
							double &rStd,
							byte   &bPred,
							long   &iPredNr,
							long   &iTotNr)
{
	rMean = _smplS(bKey, iBnd, CLASSMEAN);
	rStd  = _smplS(bKey, iBnd, CLASSSTD);
	bPred = 0;
	iPredNr = 0;
	int b;
	int iMax = _smplH.riRanges[iBnd].iHi();
	for (b = _smplH.riRanges[iBnd].iLo();
		b <= iMax; b++)
	{
		if (iPredNr < _smplH(bKey, iBnd, b))
		{
			iPredNr = _smplH(bKey, iBnd, b);
			bPred = b;
		}
	}
	iTotNr = _sSum(bKey, iBands());
}

void SampleSetPtr::IncrementSmplHSums(RowCol rc, byte bNew, bool fAdd)
{
	byte bGetRaw = 0;
	short b, b2;
	if (fAdd)
	{
		_sSum(bNew, iNrBands)++;
		for (b = 0; b < iNrBands; b++)
		{
			bGetRaw = (byte)(mpl()[b+mpl()->iLower()]->iRaw(rc));
			// update _smplH
			_smplH(bNew, b, bGetRaw)++;
			// update _sSum
			_sSum(bNew, b) += bGetRaw;
			// update _sSumXY
			for (b2 = b; b2 < iNrBands; b2++)
			{
				if (b2 == b)
					_sSumXY(bNew, b, b) += (long)bGetRaw * bGetRaw;
				else
					_sSumXY(bNew, b, b2) += (long)bGetRaw * mpl()[b2+mpl()->iLower()]->iRaw(rc);
			}
		}
	}
	else
	{
		_sSum(bNew, iNrBands)--;
		for (b = 0; b < iNrBands; b++)
		{
			bGetRaw = (byte)(mpl()[b+mpl()->iLower()]->iRaw(rc));
			// update _smplH
			_smplH(bNew, b, bGetRaw)--;
			// update _sSum
			_sSum(bNew, b) -= bGetRaw;
			// update _sSumXY
			for (b2 = b; b2 < iNrBands; b2++)
			{
				if (b2 == b)
					_sSumXY(bNew, b, b) -= (long)bGetRaw * bGetRaw;
				else
					_sSumXY(bNew, b, b2) -= (long)bGetRaw * mpl()[b2+mpl()->iLower()]->iRaw(rc);
			}
		}
	}
}

void SampleSetPtr::IncrementSmplS(byte bNew)
{
	short b;
	double dSum, dMean, dNum, dStd;
	for (b = 0; b < iNrBands; b++)
	{
		dNum = _sSum(bNew, iNrBands);
		if (dNum == 0)
		{
			dMean = 0;
			dStd = 0;
		}
		else
		{
			dSum = _sSum(bNew, b);
			dMean = dSum / dNum;
			// because sum over i = 1 to n of (sqr(Xi-M)) ==
			//         sum over i = 1 to n of (sqr(Xi)) - n*sqr(M):
			if (dNum == 1)
				dStd = 0;
			else
				dStd = sqrt((  _sSumXY(bNew, b, b) - dNum* dMean * dMean  ) /
				(dNum - 1));
		}
		_smplS(bNew, b, CLASSMEAN) = dMean;
		_smplS.bMean(bNew, b) = byteConv(dMean);
		_smplS(bNew, b, CLASSSTD) = dStd;
	}
}

void SampleSetPtr::SaveOldSampleMapValues(const Array<RowCol>& rca)
{
	bOldSampleMapValues.Resize(rca.iSize());
	for (unsigned long i=0; i < rca.iSize(); ++i )
		bOldSampleMapValues[i] = byteConv(smplmap->iRaw(rca[i]));
}

void SampleSetPtr::SetSlct(const Array<RowCol>& rca)
{
	RemSlct();
	if (!fSelection) {
		//  Delete Classselect if (aanwezig...)
		fSelection = true;
		//  for (i = 0; i < fsl.iSize(); i++)
		//    fsl[i]->Backup();
	}
	for (unsigned i = 0; i < rca.iSize(); i++)
		UpdateStat(rca[i], 255);
}

void SampleSetPtr::UpdateStat(const Array<RowCol>& rca)
{
	long iRaw = smplmap->iRaw(rca[0]);
	for (unsigned long i=0; i < rca.iSize(); ++i)
		UpdateStat(rca[i], iRaw, i);
	bOldSampleMapValues.Resize(0);
}

void SampleSetPtr::RemSlct()
{
	short i1, i2, b;
	for (b = 0; b < iNrBands; b++)
	{
		for (i1 = (short)smplH().riRanges[b].iLo(); i1 <= (short)smplH().riRanges[b].iHi(); i1++)
			_smplH(0, b, i1) = 0;
		smplS() (0, b, CLASSMEAN) = 0;
		smplS().bMean(0, b) = 0;
		smplS() (0, b, CLASSSTD) = 0;
		smplSum() (0, b) = 0;
		for (i2 = 0; i2 <= b; i2++)
			smplSumXY() (0, b, i2) = 0;
	}
	smplSum()(0, iNrBands) = 0;
	for (unsigned int i = 0; i < fsl.iSize(); i++)
		fsl[i]->DelSelect();
}

void SampleSetPtr::UpdateStat(RowCol rc, long iR, long iIndex)
{
	byte bOld;
	if (iIndex == iUNDEF)
		bOld = byteConv(smplmap->iRaw(rc));
	else
		bOld = bOldSampleMapValues[iIndex];
	byte bNew = byteConv(iR);

	bool add = true, del = false;
	if (bNew == 255)  // update selection statistics.
	{
		for (unsigned i = 0; i < fsl.iSize(); i++)
			fsl[i]->AddSample(rc, bNew);
		IncrementSmplHSums(rc, 0, add);
		IncrementSmplS(0);
		return;
	}
	if (fSelection)
	{
		//  for (i = 0; i < fsl.iSize(); i++)
		//  fsl[i]->Restore();
		fSelection = false;
	}
	if (bNew == 0)  // delete sample or selection stats.
	{
		if (bOld != 0) // if bOld == 0, nothing happens!!
		{
			// delete sample in featurespaces
			for (unsigned i = 0; i < fsl.iSize(); i++)
				fsl[i]->DelSample(rc);
			IncrementSmplHSums(rc, bOld, del);
			IncrementSmplS(bOld);
		}
	}
	else            // add or change sample
	{
		if (bOld == 0)  // sample yet unclassified pixel
		{
			for (unsigned i = 0; i < fsl.iSize(); i++)
				fsl[i]->AddSample(rc, bNew);
			IncrementSmplHSums(rc, bNew, add);
			IncrementSmplS(bNew);
		}
		else  // sample previously differently claasified pixel =
		{     // del sample, add sample
			for (unsigned i = 0; i < fsl.iSize(); i++)
				fsl[i]->DelSample(rc);
			IncrementSmplHSums(rc, bOld, del);
			IncrementSmplS(bOld);

			for (unsigned i = 0; i < fsl.iSize(); i++)
				fsl[i]->AddSample(rc, bNew);
			IncrementSmplHSums(rc, bNew, add);
			IncrementSmplS(bNew);
		}
	}
}

void SampleSetPtr::RegisterFS(FeatureSpace* fs)
{
	fsl.Append(1);
	fsl[fsl.iSize()-1] = fs;
}

void SampleSetPtr::UnregisterFS(FeatureSpace* fs)
{
	short i = 0;
	for (; i < fsl.iSize(); i++)
		if (fs == fsl[i]) break;
	fsl[i] = 0;
	fsl.Remove(i, 1);
}

void SampleSetPtr::GetObjectDependencies(Array<FileName>& afnObjDep)
{
	ObjectInfo::Add(afnObjDep, _mpl->fnObj, fnObj.sPath());
}

void SampleSetPtr::GetObjectStructure(ObjectStructure& os)
{
	IlwisObjectPtr::GetObjectStructure(os);
	if ( os.fGetAssociatedFiles())
	{
		os.AddFile(fnObj, "SampleSet", "MapList");
		os.AddFile(fnObj, "SampleSet", "Domain");	
		os.AddFile(fnObj, "SampleSet", "SampleMap");
	}		
}	




