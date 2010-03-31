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
/* Imptbl.c
  by Willem Nieuwenhuis 21 feb 1996
	Last change:  WN   29 Feb 96    2:41 pm
*/

#include "Headers\toolspch.h"
#include "Engine\DataExchange\Convloc.h"
#include "Headers\Hs\CONV.hs"
#include "Engine\DataExchange\Imptbl.h"
#include "Engine\Table\tblstore.h"
#include "Engine\Table\TBLBIN.H"
//#include "Applications\Table\Tblcomma.h"

class ErrorImportTable {
public:
  ErrorImportTable(const String& ss) { s=ss; sExcError(); }
private:
  String s;
  String& sExcError() { return s; }
};

void ImpExp::ImpTbl(const FileName& fnFile, const FileName& fnObject) {
/*  MessageHandling* mh = getEngine()->getContext()->mh();
  try {
    mh->Error( "not implemented yet, sorry" );
    File FileIn( fnFile );
    if ( FileIn.err() )
      throw ErrorImportTable( FileIn.sError() );
    FileName fnOut=fnObject;
  }
  catch (ErrorImportTable err) {
    mh->Error( err.sExcError() );
  }*/
}

void ImportTable(const Array<String>& as)
{
  TablePtr* p;
  TableStore* ts;
  FileName fnIn, fnTbt;
  int iSkip;
  int iNrCols;
  bool fUseAs;
  FileName fnBinary;
  try {
  
    

  ////////////////  trq.SetTitle(SCVTitleImportASCII);
    /* Argument sequence in input string
    [0]   input ASCII data file
    [1]	  output ILWIS table
		[2]	  UseAs | Complete
		[3]	  Comma | Space | Fixed 
		[4]	  [skiplines]
		[4 or 5]	  TableDomain
				  And then for each column
		[5 or 6..] [width,] [UNDEFvalue,]
				  columname (domain [min:max[:precision]])
    ******************************************/
    Array<String> asUndefValue;
    Array<String> asColumnName;
    Array<DomainValueRangeStruct> aDVSDomain;
    Array<short> aiWidth;
    Array<int> aeDomTyp;
    enum {eComma,eSpace,eFixed} eInputFormat;
    String sErrMsg;
    short iTemp,iTemp1;	
    int iNrArg = as.iSize();
    if (iNrArg < 5) throw ErrorObject(String(SCVImpAsErrNotEnougPar));
    fnIn = as[0];
    fnTbt = as[1];
    fnBinary = fnTbt;
    fnBinary.sExt = ".tb#";


    String sInputType = as[3];
    if (sInputType =="comma")
       eInputFormat = eComma;
    else if (sInputType =="space")
       eInputFormat = eSpace;   
    else if (sInputType =="fixed")
       eInputFormat = eFixed;
    else {
    sErrMsg = SCVImpAsErrUnknownInpType;
    sErrMsg &="   ";
    sErrMsg &= sInputType;
    throw ErrorObject(sErrMsg);
    }

    fUseAs = ("UseAs"== as[2]);
    int iArgCnt = 4;
    iSkip = as[iArgCnt].iVal();	// if non-numerical input no skip lines are given
      if (iSkip ==shUNDEF)
        iSkip =0 ;
      else  
        iArgCnt++;
  
         
    String sTableDomain = as[iArgCnt];
    iArgCnt++;
    iNrCols = 0;

    do {
	    //retrieve column data
	    if (eInputFormat==eFixed)  {
	      iTemp = as[iArgCnt].iVal();
	      if (iTemp == shUNDEF)
	         throw ErrorImportTable(SCVImpAsErrNoWidth);
	      aiWidth &= iTemp;
	      iArgCnt++;
	    }

	    String sTemp = as[iArgCnt];
	    asUndefValue &= String(""); // Preset empty string for UNDEF
	    // UNDEF can be numerical or text, so we negate the check and look
	    // for an "(" then we know it is the "(" after the column name
	    iTemp = sTemp.iPos('(');
	    if (iTemp == shUNDEF)  {
	       asUndefValue[asUndefValue.iSize()-1] = sTemp;
	       iArgCnt++;
	    }
	    // look for column name and domain, count number of columns
	    sTemp = as[iArgCnt];
	    iTemp = sTemp.iPos('(');
	    // if (iTemp == shUNDEF) error
	    String sColumnName = sTemp.sSub(0,iTemp);
	    sColumnName.sTrimSpaces();
	    asColumnName &= sColumnName;
      aDVSDomain &= DomainValueRangeStruct(0.0, 1.0,0.0); //add Domain element
	    iTemp1 = sTemp.iPos(')');
	    // if (iTemp1 == shUNDEF) error
	    String sDom =  sTemp.sSub(iTemp+1,iTemp1-iTemp-1);
	    sDom.sTrimSpaces();
	    // look for domain type
        // zou moeten werken String sDomType = IlwisObjectPtr::sParseFunc(sDom);
        Array<String> asParts;
        Split(sDom,asParts,"{:}");
        if (asParts[0]=="value")  {
          aeDomTyp &= eValRS;

          // if .. check aantal argumenten !!!!
          // for the time beeing 
          // 2 arguments double low high (double step =1.0)
          // 3 arguments double low high step
          double rStep = 1.0;
          double rLow  = asParts[1].rVal();
          double rHigh = asParts[2].rVal();
          if (asParts.iSize() == 4)
            rStep = asParts[3].rVal();
          aDVSDomain[aDVSDomain.iSize()-1] = DomainValueRangeStruct(rLow, rHigh, rStep);
          
          
        }
        else if(asParts[0]=="string")  {
          aeDomTyp &= eStr;
        }
        //else error not valid domain type here
          	    
	    iArgCnt++;
	    iNrCols++;

    } while ((iArgCnt) < iNrArg);
    // end of input argument scan

    if (!File::fExist(fnIn))
       NotFoundError(fnIn);

    p = new TablePtr(fnTbt, String(), true);

    Domain dmTbl = Domain (sTableDomain);
    p->SetDomain(dmTbl);
    ts = NULL;
    // if comma delimited
//    ts = new TableCommaDelimited (fnIn, fnTbt, *p, fnBinary, iSkip, iNrCols, 
//                                          asColumnName, aiWidth, aeDomTyp, aDVSDomain, asUndefValue);

    }
  catch (AbortedByUser&) {  // just stop, no message
  }
  catch (ErrorObject& err) {
    err.Show();
  }

  if (0 == ts) {
    delete p;
    ////return 0;
  }
  if (fUseAs) {
    p->pts = ts;
  }
  else {
    ts->TableStore::Store();
    ts->StoreAsBinary(fnBinary);
    p->pts = new TableBinary(fnTbt, *p);
    delete ts;
  }
  p->Store(); 
  delete p;

}




