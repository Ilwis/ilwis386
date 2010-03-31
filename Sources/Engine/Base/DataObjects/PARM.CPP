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
// $Log: /ILWIS 3.0/Tools/PARM.CPP $
 * 
 * 8     6/04/02 12:44p Martin
 * Merged from the ASTER branch
 * 
 * 9     2/15/02 2:44p Martin
 * added a clear function
 * 
 * 7     9/07/01 9:38a Martin
 * added functions to retireve the number of flag parms, options parms and
 * the total number of parms
 * 
 * 6     5/24/00 4:09p Wind
 * parsing of parameters now takes (double) quotes better into account
 * 
 * 5     29-02-00 10:13 Wind
 * flags that start with a number are now interpreted as negative values.
 * 
 * 4     9/15/99 4:24p Wind
 * debugged support for quoted parms
 * 
 * 3     9/08/99 11:41a Wind
 * comments
 * 
 * 2     9/08/99 10:32a Wind
 * adpated to use of quoted file names
// Revision 1.3  1997/08/29 16:09:38  Wim
// delete [] ps in AddFrom()
//
// Revision 1.2  1997-08-29 15:04:50+02  Wim
// purg parms in destructor of ParmList
//
// parm.c
// ParmList and Parm classes
// jan 1994, by Jelle Wind
// (c) Computer Department ITC
*/
#define PARM_C
#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\Parm.h"

///// class Parm
/*
Class Parm is used for three different parameter types:

- fixed parameters:
  - they appear in the beginning of the command line
  - they use the members _iPos and _sVal
- flags:
  - they have the form: -flag  -flag+  -flag- (-flag and -flag+ are identical)
  - they use the member _sOpt and _fVal (union with _iPos)
- options:
  - they have the form: -option value
  - they use the members _sOpt and _sVal
	Last change:  WK   28 Aug 98   11:30 am
*/


Parm::Parm(int iPos, const String& sVal)
{ _pmt = pmtFixed; Replace(sVal); _iPos = iPos; }

Parm::Parm(int iPos, double rVal)
{ _pmt = pmtFixed; Replace(rVal); _iPos = iPos; }

Parm::Parm(const String& sFlag, bool fFlag)
{ _pmt = pmtFlag; _sOpt = sFlag; Replace(fFlag); }

Parm::Parm(const String& sOpt, const String& sVal)
{ _pmt = pmtOpt; _sOpt = sOpt; Replace(sVal); }

Parm::Parm(const String& sOpt, double rVal)
{ _pmt = pmtOpt; _sOpt = sOpt; Replace(rVal); }

void Parm::Replace(ParmList *pl)
{
  DLIterP<Parm> iter(&pl->parms);
  switch (_pmt) {
    case pmtFixed:
      for (; iter.fValid(); ++iter)
        if ((iter()->_pmt == pmtFixed) && (_iPos == iter()->_iPos))
          Replace(iter()->sVal());
      break;
    case pmtFlag:
      for (; iter.fValid(); ++iter)
        if (iter()->_pmt == pmtFlag)
          if (_sOpt == iter()->_sOpt)
            Replace(iter()->fVal());
      break;
    case pmtOpt:
      for (; iter.fValid(); ++iter)
        if (_sOpt == iter()->_sOpt)
          Replace(iter()->sVal());
      break;
  }
}


///// class ParmList

ParmList::~ParmList()
{
  parms.purge(true);
}

void ParmList::Add(Parm *prm)
{ 
  if (prm) 
    parms.append(prm); 
}

void ParmList::AddFrom(FileName fn, int *iFixedPos)
{
  try {
    File f(fn);
    String s;
    while (!f.fEof()) {
      f.ReadLnAscii(s);
      AddFrom(s, iFixedPos);
    }
  }
  catch (const FileErrorObject&) {
  }
}

char * strchrQuoted(char* p, char c)
{
  bool fInSingleQuotes = false;
  bool fInDoubleQuotes = false;
  while (*p != 0) {
    if (*p == '\'' && !fInDoubleQuotes)
      fInSingleQuotes = !fInSingleQuotes;
    else if (*p == '\"' && !fInSingleQuotes)
      fInDoubleQuotes = !fInDoubleQuotes;
    if (!fInSingleQuotes && !fInDoubleQuotes)
      if (*p == c)
        return p;
    p++;
  }
  return 0;
}

void ParmList::AddFrom(const String& sCmdLine, int *iFixedPos)
{
//  fSemicolon = false;
  int iLen;
  char *ps = new char[(iLen = sCmdLine.length())+1];
  strcpy(ps, sCmdLine.scVal());
  SList<String> sl;

  char * p1 = ps, *p2 = 0;
	while (0 != *p1) {
    p2 = strchrQuoted(p1, ' ');
		if (0 == p2) {
		  sl.append(p1);
			break;
		}
		if (p1 != p2) {
			*p2 = 0;
		  sl.append(p1);
		}
		p1 = p2+1;
	}

  // check flags, options and fixed parms
// for (SLIter<char *> iter(&sl); iter.fValid(); ++iter) {
//    p1 = iter();
  for (SLIter<String> iter(&sl); iter.fValid(); ++iter) {
    p1 = &iter()[0];
    if (*p1 == '@') // response file
      AddFrom(FileName(p1+1), iFixedPos);
    else if ((*p1 == '-') && !isdigit(*(p1+1))) { // option or flag
      char *p2 = strchr(p1, '=');
      if (0 == p2) {
        // check last char for flag
        int iLast = strlen(p1) - 1;
        char c = p1[iLast];
        if ((c == '-') || (c== '+'))
          p1[iLast] = 0; // remove + or -
        Add(new Parm(p1+1, (bool)(c != '-')));
      }
      else {
        *p2 = 0;
        Add(new Parm(p1+1, String(p2+1)));
      }
    }
    else { // fixed parm
//      if (p1 == ";") {
//        fSemicolon = true;
//        break; // end of line
//      }  
      Add(new Parm((*iFixedPos)++, p1));
    }  
  }
  delete [] ps;
}

ParmList::ParmList(const ParmList& pl)
{ for (DLIterCP<Parm> iter(&pl.parms); iter.fValid(); ++iter)
    Add(iter());
}


ParmList& ParmList::operator=(const ParmList& pl)
{ for (DLIterCP<Parm> iter(&(pl.parms)); iter.fValid(); ++iter)
    Add(iter());
   return *this;
}


void ParmList::Fill(const String& sCmdLine)
{
  int iFixedPos = 0; 
  AddFrom(sCmdLine, &iFixedPos); 
}

int ParmList::iFixed()
{ int iFix = 0;
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if (iter()->_pmt == pmtFixed)
      iFix++;
   return iFix;
}

int ParmList::iFlags()
{ int iFix = 0;
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if (iter()->_pmt == pmtFlag)
      iFix++;
   return iFix;
}

int ParmList::iOptions()
{ int iFix = 0;
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if (iter()->_pmt == pmtOpt)
      iFix++;
   return iFix;
}

int ParmList::iSize()
{
	return parms.size();
}

bool ParmList::fExist(const String& sOpt)
// true if option or flag sOpt exists
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if (iter()->_sOpt == sOpt) return true;
  return false;
}

bool ParmList::fExist(int iPos)
// true if iPos-th fixed parameter exists
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtFixed) && (iter()->_iPos == iPos))
      return true;
 return false;
}


String ParmList::sGet(int iPos)
// returns iPos-th fixed parameter
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtFixed) && (iter()->_iPos == iPos))
      return iter()->sVal();
 return "";
}

bool ParmList::fGet(const String& sFlag)
// returns value of flag
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if (iter()->_pmt == pmtFlag)
      if (iter()->_sOpt == sFlag) return iter()->fVal();
  return false;
}


String ParmList::sGet(const String& sOption)
// return value of option
{
  for (DLIterP<Parm> iter(&parms);
       iter.fValid();
       ++iter)
    if ((iter()->_pmt == pmtOpt) && (iter()->_sOpt == sOption))
      return iter()->sVal();
  return "";
}

void ParmList::Remove(const String& sOpt)
{
  DLIterP<Parm> iter(&parms);
  for (; iter.fValid(); ++iter)
    if (iter()->_sOpt == sOpt) break;
  if (iter.fValid()) iter.remove();
}

void ParmList::Remove(int iPos)
{
  DLIterP<Parm> iter(&parms);
  for (; iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtFixed) && (iter()->_iPos == iPos))
      break;
  if (iter.fValid()) iter.remove();
  // adjust other positions
  for (iter.first(); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtFixed) && (iter()->_iPos > iPos))
      (iter()->_iPos)--;
}


void ParmList::Replace(Parm *prm)
{
  switch (prm->_pmt) {
    case pmtFixed:
      Replace(prm->_iPos, prm->sVal()); break;
    case pmtFlag:
      Replace(prm->_sOpt, prm->fVal()); break;
    case pmtOpt:
      Replace(prm->_sOpt, prm->sVal()); break;
  }
}

void ParmList::Replace(int iPos, const String& sVal)
// replaces iPos-th parm with value sVal (if there)
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtFixed) && (iter()->_iPos == iPos))
      iter()->Replace(sVal);
}

void ParmList::Replace(const String& sFlag, bool fVal)
// replaces flag sFlag with value fVal (if there)
{
  DLIterP<Parm> iter(&parms);
  for (; iter.fValid(); ++iter)
    if (iter()->_pmt == pmtFlag)
      if (iter()->_sOpt == sFlag) break;
  if (iter.fValid())
    iter()->Replace(fVal);
}

void ParmList::Replace(const String& sOpt, long iVal)
// replaces option sOpt with value iVal (if there)
// if option is a flag the flag is adjusted
{
  DLIterP<Parm> iter(&parms);
  for (; iter.fValid(); ++iter)
    if (iter()->_sOpt == sOpt) break;
  if (iter.fValid())
    iter()->Replace((double)iVal);
}


void ParmList::Replace(const String& sOption, double rVal)
// replaces option sOption with rVal (if there)
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtOpt) && (iter()->_sOpt == sOption))
      iter()->Replace(rVal);
}


void ParmList::Replace(const String& sOption, const String& sVal)
// replaces option sOption with sVal (if there)
{
  for (DLIterP<Parm> iter(&parms); iter.fValid(); ++iter)
    if ((iter()->_pmt == pmtOpt) && (iter()->_sOpt == sOption))
      iter()->Replace(sVal);
}

String ParmList::sCmd()
{ String s;
  // count fixed parms
  int iFixedParms = 0;
  DLIterP<Parm> iter(&parms);
  for (; iter.fValid(); ++iter)
    if (iter()->_pmt == pmtFixed) iFixedParms++;
  // put fixed parameters in order
  for (int i=0; i<iFixedParms; i++)
    for (iter.first(); iter.fValid(); ++iter)
      if ((iter()->_pmt == pmtFixed) && (iter()->_iPos == i)) {
        s &= String("%S ", iter()->_sVal);
        break;
      }
  for (iter.first(); iter.fValid(); ++iter)
    if (iter()->_pmt == pmtOpt)
      s &= String("-%S=%S ", iter()->_sOpt, iter()->_sVal);
    else if (iter()->_pmt == pmtFlag) {
      if (iter()->fVal())
        s &= String("-%S ", iter()->_sOpt);
      else
        s &= String("-%S- ", iter()->_sOpt);
    }
  return s;
}

void ParmList::Clear()
{
 parms.clear();
		
}









