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


#include "Applications\Raster\MAPITER.H"
#include "Applications\Raster\MAPMRROT.H"
#include "Engine\Scripting\Calc.h"
#include "Engine\Base\DataObjects\WPSMetaData.h"
#include "Headers\Htp\Ilwisapp.htp"
#include "Headers\Err\Ilwisapp.err"
#include "Headers\Hs\mapcalc.hs"

IlwisObjectPtr * createMapIterator(const FileName& fn, IlwisObjectPtr& ptr, const String& sExpr, vector<void *> parms ) {
	if ( sExpr != "")
		return (IlwisObjectPtr *)MapIterator::create(fn, (MapPtr &)ptr, sExpr);
	else
		return (IlwisObjectPtr *)new MapIterator(fn, (MapPtr &)ptr);
}

String wpsmetadataMapIter() {
	WPSMetaData metadata("MapIter");
	metadata.AddTitle("MapIter");
	metadata.AddAbstract("using the result of one calculation as input for the next. These calculations are performed line by line, pixel by pixel and take place in all directions");
	metadata.AddKeyword("spatial");
	metadata.AddKeyword("raster");
	metadata.AddKeyword("iteration");

	WPSParameter *parm1 = new WPSParameter("1","Start map",WPSParameter::pmtRASMAP);
	parm1->AddAbstract("input map which contains one or more pixels acting as the starting point for the calculation");

	WPSParameter *parm2 = new WPSParameter("2","Use propagation", WPSParameter::pmtBOOL);
	parm2->AddAbstract("use propagation");

	WPSParameter *parm3 = new WPSParameter("3","Iteration expression", WPSParameter::pmtSTRING);
	parm3->AddAbstract("defines the calculation to be performed");

	WPSParameter *parm4 = new WPSParameter("3","Number of iterations", WPSParameter::pmtINTEGER);
	parm4->AddAbstract("number of iterations to be performed");
	parm4->setOptional(true);


	metadata.AddParameter(parm1);
	metadata.AddParameter(parm2);
	metadata.AddParameter(parm3);
	metadata.AddParameter(parm4);
	WPSParameter *parmout = new WPSParameter("Result","Output Map", WPSParameter::pmtRASMAP, false);
	parmout->AddAbstract("Reference Outputmap and supporting data objects");
	metadata.AddParameter(parmout);


	return metadata.toString();
}

ApplicationMetadata metadataMapIter(ApplicationQueryData *query) {
	ApplicationMetadata md;
	if ( query->queryType == "WPSMETADATA" || query->queryType == "") {
		md.wpsxml = wpsmetadataMapIter();
	}
	if ( query->queryType == "OUTPUTTYPE" || query->queryType == "")
		md.returnType = IlwisObject::iotRASMAP;
	if ( query->queryType == "EXPERSSION" || query->queryType == "")
		md.skeletonExpression =  MapIterator::sSyntax();

	return md;
}
const char * MapIterator::sSyntax()
{ return "MapIter(startmapinit,iterexpr)\n"
"MapIterProp(startmap,iterexpr)\n"
"MapIter(startmap,iterexpr,times)\n"
"MapIterProp(startmap,iterexpr,times)"; }


MapIterator* MapIterator::create(const FileName& fn, MapPtr& p, const String& sExpr)
{
	Array<String> as;
	short iParms = IlwisObjectPtr::iParseParm(sExpr, as);
	if ((iParms < 2) || (iParms > 5))
		ExpressionError(sExpr, sSyntax());
	Map mpInit(as[0]);
	String iterExpr;
	bool fProp = false;
	long iTimes = iUNDEF;
	if ( as[1].toLower() == "true" || as[1].toLower() == "false") { // newstyle
		fProp = as[1].fVal();
		iterExpr = as[2];
		if (iParms == 4)
			iTimes = as[3].iVal();
	} else { // oldstyle
		String sFunc = IlwisObjectPtr::sParseFunc(sExpr);
		fProp = fCIStrEqual(sFunc , "MapIterProp");
		if (!fProp && (!fCIStrEqual(sFunc , "MapIter")))
			ExpressionError(sExpr, sSyntax());
		if (iParms == 3)
			iTimes = as[2].iVal();
		iterExpr = as[1];
	}
	return new MapIterator(fn, p, mpInit, iterExpr, iTimes, fProp);
}

MapIterator::MapIterator(const FileName& fn, MapPtr & p)
: MapFromMap(fn, p), inst(0)
{
	ReadElement("MapIterator", "IterExpression", sIterExpr);
	ReadElement("MapIterator", "MapInit", mpInit);
	ReadElement("MapIterator", "Times", iTimes);
	ReadElement("MapIterator", "Prop", fPropagation);
	bool fNoGeoRef;
	Calculator calc(sIterExpr, fn);
	inst = calc.instMapCalc(objdep, fNoGeoRef);
	if (0 == inst)
		DummyError();
	fNeedFreeze = true;
	Init();
}

MapIterator::MapIterator(const FileName& fn, MapPtr& p, const Map& mapInit, const String& sExpres, long iTims, bool fProp)
: MapFromMap(fn, p, mapInit), mpInit(mapInit), sIterExpr(sExpres), iTimes(iTims), inst(0), fPropagation(fProp)
{
	bool fNoGeoRef;
	Calculator calc(sIterExpr, fn);
	inst = calc.instMapCalc(objdep, fNoGeoRef);
	SetDomainValueRangeStruct(DomainValueRangeStruct(inst->dm(), inst->vr()));

	if (0 == inst)
		DummyError();
	fNeedFreeze = true;
	Init();
	if (!fnObj.fValid())
		objtime = objdep.tmNewest();
}

void MapIterator::Init()
{
	sFreezeTitle = "MapIterator";
	htpFreeze = "ilwisapp\\iteration_algorithm.htm";
}

void MapIterator::Store()
{
	MapFromMap::Store();
	WriteElement("MapFromMap", "Type", "MapIterator");
	WriteElement("MapIterator", "MapInit", mpInit);
	WriteElement("MapIterator", "IterExpression", sIterExpr);
	WriteElement("MapIterator", "Times", iTimes);
	WriteElement("MapIterator", "Prop", fPropagation);
}

MapIterator::~MapIterator()
{
	if (0 != inst)
		delete inst;
}

bool MapIterator::fDomainChangeable() const
{
	return true;
}

bool MapIterator::fValueRangeChangeable() const
{
	return true;
}

bool MapIterator::fFreezing()
{
	LongBuf iBuf(iCols());
	RealBuf rBuf(iCols());
	long iMaxIter = iTimes;
	if (/*fPropagation ||*/ iTimes == iUNDEF)
		iMaxIter = LONG_MAX;
	long iChanges = 0, iTotalChanges = 0, iTotalLastChanges = 0;
	Array<Map> amp, ampRot;
	if (fPropagation) {
		// get list of input maps (needed for rotation)
		inst->GetMapsUsed(amp);
		// remove input init map from list
		for (unsigned int i=0; i<amp.iSize(); ++i)
			if (amp[i] == mp) {
				amp[i] = Map();
				amp.Remove(i, 1);
				break;
			}
			// create rotated versions of input maps
			for (unsigned int i=0; i<amp.iSize(); ++i) {
				Map mp(FileName::fnUnique(FileName("tmpitr", ".mpr", true)), String("MapMirrorRotate(%S, Transpose)", amp[i]->sNameQuoted()));
				mp->fErase = true;
				mp->gr()->fErase = true;
				ampRot &= mp;
			}
			// calculate rotated versions
			for (unsigned int i=0; i<ampRot.iSize(); ++i) {
				ampRot[i]->Calc();
				if (!ampRot[i]->fCalculated())
					return false;
			}
	}
	FileName fnCalcMap = FileName::fnUnique(FileName("tmpitr", ".mpr", true));
	//ObjectCopierUI::CopyFiles(mpInit->fnObj, fnCalcMap, false);

	mpCalc = Map(fnCalcMap, mpInit->sName());
	mpCalc->fErase = true;
	mpCalc->SetDomainValueRangeStruct(dvrs());
	mpCalc->Calc();
	mpCalc->BreakDependency();
	mpCalc->KeepOpen(true);
	inst->ReplaceMapUsed(mpInit, mpCalc);
	Map mpCalcRot;
	if (fPropagation) {
		FileName fnCalcMapRot = FileName::fnUnique(FileName("tmpitr", ".mpr", true));

		GeoRef grRot;
		if (mpCalc->gr()->fGeoRefNone())
			grRot = GeoRef(RowCol(mpCalc->iCols(), mpCalc->iLines()));
		else {
			grRot.SetPointer(new GeoRefMirrorRotate(FileName(fnCalcMapRot, ".grf"), mpCalc->gr(), mrTRANS));
			grRot->fErase = true;
		}
		mpCalcRot = Map(fnCalcMapRot, grRot, grRot->rcSize(), mpCalc->dvrs());
		mpCalcRot->KeepOpen(true);
		mpCalcRot->fErase = true;
	}
	for (long iIter=1; iIter <= iMaxIter; ++iIter) {
		iTotalChanges=0;
		if (fPropagation) {
			inst->env.dnb = dirNbDOWN;
			trq.SetText(String(SCLCTextIterDown_ii.scVal(), iIter, iTotalLastChanges));
			for (unsigned i=0; i<amp.iSize(); ++i)
				amp[i]->KeepOpen(true);
		}
		else
			trq.SetText(String(SCLCTextIter_ii.scVal(), iIter, iChanges));
		if (!fCalcIter(iChanges, true, iLines(), iCols(), mpCalc))
			break;
		iTotalChanges += iChanges;
		if (fPropagation) {
			inst->env.dnb = dirNbUP;
			trq.SetText(String(SCLCTextIterUp_ii.scVal(), iIter, iChanges));
			if (!fCalcIter(iChanges, false, iLines(), iCols(), mpCalc))
				break;
			iTotalChanges += iChanges;

			if (!MapMirrorRotate::fCompute(mpCalc.ptr(), mpCalcRot.ptr(), mrTRANS, &trq))
				return false;
			inst->ReplaceMapUsed(mpCalc, mpCalcRot);
			// swap lines and cols
			long ii = inst->env.iLines;
			inst->env.iLines = inst->env.iCols;
			inst->env.iCols = ii;
			inst->env.dnb = dirNbRIGHT;
			for (unsigned int i=0; i<amp.iSize(); ++i) {
				inst->ReplaceMapUsed(amp[i], ampRot[i]);
				amp[i]->KeepOpen(false);
				ampRot[i]->KeepOpen(true);
			}
			trq.SetText(String(SCLCTextIterRight_ii.scVal(), iIter, iChanges));
			bool fInterrupt = false;
			if (fCalcIter(iChanges, true, iCols(), iLines(), mpCalcRot)) {
				iTotalChanges += iChanges;
				inst->env.dnb = dirNbLEFT;
				trq.SetText(String(SCLCTextIterLeft_ii.scVal(), iIter, iChanges));
				if (!fCalcIter(iChanges, false, iCols(), iLines(), mpCalcRot))
					fInterrupt = true;
			}
			else 
				fInterrupt = true;
			for (unsigned int i=0; i<ampRot.iSize(); ++i)
				ampRot[i]->KeepOpen(false);
			iTotalChanges += iChanges;
			//      if (!MapMirrorRotate::fCompute(mpCalcRot.ptr(), mpCalc.ptr(), mrTRANS, &trq))
			//       return false;
			if (fInterrupt)
				break;
			inst->ReplaceMapUsed(mpCalcRot, mpCalc);
			for (unsigned int i=0; i<amp.iSize(); ++i)
				inst->ReplaceMapUsed(ampRot[i], amp[i]);
		}
		if ((fPropagation || (iTimes == iUNDEF)) && (iTotalChanges == 0))
			break;
		iTotalLastChanges = iTotalChanges;
	}
	if (fPropagation)
		for (unsigned int i=0; i<amp.iSize(); ++i) {
			amp[i] = Map();
			ampRot[i] = Map();
		}

		trq.SetText(SCLCTextStoringResult);
		if ( dvrs().fRealValues())
			for (long i=0; i < iLines(); ++i) {
				if (trq.fUpdate(i, iLines()))
					return false;
				mpCalc->GetLineVal(i, rBuf, 0, iCols());
				pms->PutLineVal(i, rBuf, 0, iCols());
			}
		else
			for (long i=0; i < iLines(); ++i) {
				if (trq.fUpdate(i, iLines()))
					return false;
				mpCalc->GetLineRaw(i, iBuf, 0, iCols());
				pms->PutLineRaw(i, iBuf, 0, iCols());
			}
			mpCalc = Map();
			mpCalcRot = Map();
			return true;
}

String MapIterator::sExpression() const
{
	String sMapInit = mpInit->sNameQuoted(true, fnObj.sPath());
	if (fPropagation) {
		if (iTimes == iUNDEF)
			return String("MapIterProp(%S, %S)", sMapInit, sIterExpr);
		else
			return String("MapIterProp(%S, %S,%li)", sMapInit, sIterExpr, iTimes);
	}
	if (iTimes != iUNDEF)
		return String("MapIter(%S, %S,%li)", sMapInit, sIterExpr, iTimes);
	return String("MapIter(%S, %S)", sMapInit, sIterExpr);
}

bool MapIterator::fCalcIter(long& iChanges, bool fDown, long iLines, long iCols, Map& mpTmp)
{
	LongBuf iBuf(iCols), iBufPrev(iCols), iBufOld(iCols);
	RealBuf rBuf(iCols), rBufPrev(iCols), rBufOld(iCols);
	bool fReals = dvrs().fRealValues();
	long iLineStart, iLineEnd, iLineInc;
	if (fDown) {
		iLineStart = 0; iLineEnd = iLines-1; iLineInc = 1;
	}
	else {
		iLineStart = iLines-1; iLineEnd = 0; iLineInc = -1;
	}
	iChanges = 0;
	long i;
	if (fReals) {
		i = iLineStart;
		while (i != iLineEnd+iLineInc) {
			mpTmp->GetLineVal(i, rBufPrev, 0, iCols);
			if (trq.fUpdate(i, iLines)) {
				trq.ResetAborted();
				return false;
			}
			inst->MapCalcVal(i, rBuf, 0, iCols);
			if (fPropagation)
				mpTmp->PutLineVal(i, rBuf, 0, iCols);
			else if (i != iLineStart)
				mpTmp->PutLineVal(i-iLineInc, rBufOld, 0, iCols);
			for (long j=0; j < iCols; ++j) {
				if (fabs(rBufPrev[j] - rBuf[j])>1e-6)
					iChanges++;
			}
			Swap(rBuf, rBufOld);
			i += iLineInc;
		}
		if (!fPropagation)
			mpTmp->PutLineVal(iLineEnd, rBufOld, 0, iCols);
	}
	else {
		i = iLineStart;
		while (i != iLineEnd+iLineInc) {
			//      if (iTimes <= 0)
			mpTmp->GetLineRaw(i, iBufPrev, 0, iCols);
			if (trq.fUpdate(i, iLines)) {
				trq.ResetAborted();
				return false;
			}
			inst->MapCalcRaw(i, iBuf, 0, iCols);
			if (fPropagation)
				mpTmp->PutLineRaw(i, iBuf, 0, iCols);
			else if (i != iLineStart)
				mpTmp->PutLineRaw(i-iLineInc, iBufOld, 0, iCols);
			for (long j=0; j < iCols; ++j) {
				if (iBufPrev[j] != iBuf[j])
					iChanges++;
			}
			Swap(iBuf, iBufOld);
			i += iLineInc;
		}
		if (!fPropagation)
			mpTmp->PutLineRaw(iLineEnd, iBufOld, 0, iCols);
	}
	return true;
}




