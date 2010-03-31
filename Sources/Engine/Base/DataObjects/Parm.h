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
// app/parm.h
// Interface for ParmList and Parm classes
// jan 1994, by Jelle Wind
// (c) Computer Department ITC

#ifndef ILWPARM_H
#define ILWPARM_H

enum ParmType { pmtFixed, pmtFlag, pmtOpt };

class ParmList;

class  Parm 
{
friend class ParmList;

public:
  _export Parm(int iPos, const String& sVal);                            // for fixed parm
  _export Parm(int iPos, double rVal);                                   // for fixed parm
  _export Parm(const String& sFlag, bool fFlag);                         // for flag
  _export Parm(const String& sOpt, const String& sVal);                  // for option
  _export Parm(const String& sOpt, double rVal);                         // for option

  String        sOpt() 
                  { return _sOpt; }                      // return option or flag
  ParmType      pmt() 
                  { return _pmt; }                       // return type of parm
  String        sVal() 
                  { return _sVal; }                      // get current value
  bool          fVal()                                     // get current value
	                { return _fVal==0 ? false : true; }
  long          iVal()   
                  { return _sVal.iVal(); }               // get current value
  double        rVal() 
                  { return _sVal.rVal(); }               // get current value
  void          Replace(const String& sVal)                      // replace current value
                  { if (_pmt != pmtFlag) _sVal = sVal; }
  void          Replace(double rVal)                            // replace current value
                  { if (_pmt != pmtFlag) _sVal = String("%g", rVal); }
  void          Replace(bool fFlag)                             // replace current value
                  { if (_pmt == pmtFlag) _fVal = fFlag; }
  void _export  Replace(ParmList *pl);     // replace current value with parm value in list pl (if it's there)

private:
  String        _sOpt;           // contains flag or option (used only by flag or option)
  String        _sVal;           // contains value (used only for option and fixed parm)
  union 
  {
                int _iPos;       // contains pos. of fixed parm (_pmt == pmtFixed)
                int _fVal;       // contains current flag value (_pmt == pmtFlag)
  };
  ParmType      _pmt;            // type of parm
};

//template class  DList<Parm*>;
//template class  DListP<Parm>;

class _export ParmList 
{
friend class Parm;

public:
    ParmList() 
        { fSemicolon = false; };
    ParmList(const String& sCmd)         // construct parm list from command line
        { int iFixedPos = 0; AddFrom(sCmd, &iFixedPos); }
                      ParmList(const ParmList& pl);       // copy constructor
    virtual    ~ParmList();
    
    ParmList       &operator=(const ParmList& pl); // assignment
    void    Fill(const String& sCmd);       // fills values parm according to sCmd
		void   Clear();
    String  sGet(int iPos);              // returns iPos-th fixed parameter
    bool    fGet(const String& sFlag);      // returns value of flag
    String  sGet(const String& sOption);  // return value after option
    String  sCmd();                      // return full command line
    bool    fExist(int iPos);              // true if iPos-th fixed parameter exists
    bool    fExist(const String& sOpt);     // true if option or flag sOpt exists
    bool           fExist(const Parm& prm)               // true if parm prm exists
                     { return (prm._pmt == pmtFixed) ? fExist(prm._iPos) : fExist(prm._sOpt); }
    int     iFixed();                       // nr. of fixed params
		int     iOptions();                     // nr of options
		int     iFlags();                       // nr of flags
		int     iSize();                        // all
    virtual void   Add(Parm *prm);        // add parameter to ParmList
    void    Remove(int iPos);              // remove iPos-th fixed parm from parmlist
    void    Remove(const String& sOpt);     // remove option/flag sOpt from ParmList
    void           Replace(Parm *prm);             // replaces value of parm with prm (if there)
    void           Replace(int iPos, const String& sVal); // replaces value of iPos-th fixed parm with sVal (if there)
    void           Replace(const String& sFlag, bool fVal); // replaces value of flag sFlag with value fVal (if there)
    void           Replace(const String& sOpt, long iVal); // replaces value of option sOpt with value iVal (if there)
    void           Replace(const String& sOpt, double rVal); // replaces value of option sOpt with value rVal (if there)
    void    Replace(const String& sOpt, const String& sVal);// replaces value of option sOpt with sVal (if there)
    bool                    fEndWithSemicolon() const 
                                { return fSemicolon; }
    DListP<Parm>   parms;

private:
    bool           fSemicolon;
    void    AddFrom(FileName fn, int *iFixedPos);  // read paarmeters from file fn and add to list
    void    AddFrom(const String& sCmdLine, int *iFixedPos); // read paarmeters from sCmdLine and add to list
};


#endif







