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
/* TableCommaDelimited
   A table stored as comma delimited ascii file
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   18 May 99    3:58 pm
*/
#include "Applications\Table\Tblcomma.h"

TableCommaDelimited::TableCommaDelimited(const FileName& fnObj, TablePtr& p)
: TableStore(fnObj, p)
{
  ObjectInfo::ReadElement(ptr.sSection("TableCommaDelimited").c_str(),
                          "SkipLines", ptr.fnObj, iSkipLines);
  // read undef info per column
  for (int c = 0; c < iCols(); ++c) {
    String sEntry("Undef%i", c);
    String sUndef;
    ObjectInfo::ReadElement(ptr.sSection("TableCommaDelimited").c_str(),
       sEntry.c_str(), ptr.fnObj, sUndef);
    asUndef[c] = sUndef;
  }
  Load();
}

TableCommaDelimited::TableCommaDelimited(
    const FileName& _FileIn,
    const FileName& fnObj, TablePtr& p,
    const FileName& fnDat,
    int iSkip, int iiNrCols,
    const Array<String>& asNames,
    const Array<short>& aiWidth,
    const Array<int>& _aeDomTyp,
    const Array<DomainValueRangeStruct>& advrs,
    const Array<String>& asUndefVals)
: TableStore(fnObj, p, fnDat),
  FileIn(_FileIn),
  iSkipLines(iSkip),
  iNrCols(iiNrCols)
  /////aeDomTyp(_aeDomTyp)
  ////asUndef(asUndefVals)  Array of String gaat zo niet ???
{
  dmString = Domain("string");

  if (iNrCols > asNames.iSize())
    iNrCols = asNames.iSize();
  if (iNrCols > advrs.iSize())
    iNrCols = advrs.iSize();
  for (int i = 0; i < iNrCols; ++i)  {
    asUndef &= asUndefVals[i];
    aeDomTyp &=_aeDomTyp[i];
	    switch (aeDomTyp[i])  {
	    
  	    case eValRS:
          colNew(asNames[i], advrs[i]);
          break;  
        case eStr:
          colNew(asNames[i],dmString);
          break;
      }    
  }  
  Load();
  
}

void TableCommaDelimited::Load()
{
	// waar kiezen we voor ? Snel en zelf doen of langzamer en
	// Ilwis functies gebruiken
	const int iMaxCharLine = 10001; // maximum line length 10k
	const int iMaxCharEl   =  1001; // maximum number of char pro element
	const int iMaxNrEl     =   100; // maximum number of elements (fields) on a line
	char  *acTblEl [iMaxNrEl+1]; // for speed pointers not with new
	char *cLnIn =new char[iMaxCharLine]; // line buffer
	for (int i =0; i < iNrCols; i++)
	  acTblEl [i] =new char [iMaxCharEl]; // each element less 1k characters

	int iCc,iRc,iLL;
	int iSkip = iSkipLines;
	File FIN(FileIn); //Read Only
	while (iSkip > 0)  {
	  iLL = FIN.ReadLnAscii(cLnIn,iMaxCharLine-1);
	  iSkip--;
	} //nog: if EOF voor aantal regels gelezen  
	iRc = 0;
	while (!FIN.fEof()) {  
		iLL = FIN.ReadLnAscii(cLnIn,iMaxCharLine-1);
		if (FIN.fEof() && iLL == 0) break;  //
		long iRecNr = iRecNew();// dit is net nieuw
		// split line on comma separator
		int iCharc = 0;
		for (int iColc=0; iColc<iNrCols; iColc++)  {
		  // skip leading spaces and tab's
		  while ((cLnIn[iCharc] == ' ') || (cLnIn[iCharc] == '\t'))  {
		    if (iCharc >= (iLL-1)) break;  // Leave at least one space or tab
		    iCharc++;
		  }  
		  for (iCc = 0; iCc <= 1000; iCc++)  {
		    char c = cLnIn[iCharc++];
		    if (c == ','||c == '\000') {  
		      acTblEl[iColc][iCc] = '\000';
		      // delete trailing spaces and tabs (leave one as minimum)
		      for (int iT = iCc-1; iT > 0; iT--)  {
		        if ((acTblEl[iColc][iT] == ' ')||(acTblEl[iColc][iT] == '\t'))
		          acTblEl[iColc][iT] = '\000';  // replace space or tab by terminating NULL
		        else break;
		      }
		    if (c=='\000')
		      iCharc--;// if more columns in table as in data line others columns see the NULL char		      
		    break;
		    }

		    acTblEl[iColc][iCc] = c;
		  }
		  // if iCc=1000 error argument too long
	    // look for domain type and put in table
	    switch (aeDomTyp[iColc])  {
  	    case eValRS:
    	    if (acTblEl[iColc] == asUndef[iColc])
	          ac[iColc]->PutVal(iRecNr,rUNDEF);
	        else  
            ac[iColc]->PutVal(iRecNr, acTblEl[iColc]);
          break;  
        case eStr:
     	    if (acTblEl[iColc] == asUndef[iColc])
	          ac[iColc]->PutVal(iRecNr,sUNDEF);
	        else  
            ac[iColc]->PutVal(iRecNr, acTblEl[iColc]);
          break;
      }    
  
	  }  
		iRc++;  
	}

	// delete the new's !!!!
	for (int i =0; i < iNrCols; i++)
	  delete [] acTblEl [i];
	delete [] cLnIn;


}

void TableCommaDelimited::Store()
{
  if (ptr.fnObj.sFile.length() == 0)  // empty file name
    return;
  TableStore::Store();
  ObjectInfo::WriteElement(ptr.sSection("TableStore").c_str(), "Type", ptr.fnObj, "CommaDelimited");
  // write undef info per column
  for (int c = 0; c < iCols(); ++c) {
    String sEntry("Undef%i", c);
    String sUndef = asUndef[c];
    if (sUndef[0])
      ObjectInfo::WriteElement(ptr.sSection("TableCommaDelimited").c_str(),
         sEntry.c_str(), ptr.fnObj, sUndef);
    else
      ObjectInfo::WriteElement(ptr.sSection("TableCommaDelimited").c_str(),
         sEntry.c_str(), ptr.fnObj, (char*)0);
  }
}

