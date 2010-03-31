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
#ifndef ASSERTD_H
#define ASSERTD_H

#include "Headers\toolspch.h"
#include <functional>
#include <strstream>

using namespace std;

#define IDC_DEBUG 104

#define PRECONDITION(cond, v1, v2) Assert(cond(v1, v2), v1, v2, #cond, #v1, #v2, "Precondition", __FILE__, __LINE__ )
#define POSTCONDITION(cond, v1, v2) Assert(cond(v1, v2), v1, v2, #cond, #v1, #v2, "Postcondition", __FILE__, __LINE__ )
#define INVARIANT(cond, v1, v2) Assert(cond(v1, v2), v1, v2, #cond, #v1, #v2, "Invariant", __FILE__, __LINE__ )
#define ISTRUE(cond, v1, v2) Assert(cond(v1, v2), v1, v2, #cond, #v1, #v2, "Condition", __FILE__, __LINE__ )

template<class T> bool fILess(const T a, const T b)
{ return a < b; }

template<class T> bool fILessEqual(const T a, const T b)
{ return a <= b; }

template<class T> bool fIGreater(const T a, const T b)
{ return a > b; }

template<class T> bool fIGreaterEqual(const T a, const T b)
{ return a >= b; }

template<class T> bool fIEqual(const T a, const T b)
{ return a == b; }

template<class T> bool fINotEqual(const T a, const T b)
{ return a != b; }

enum ctConditionTypes {ctLess, ctGreater, ctLessEqual, ctGreaterEqual, ctEqual, ctNotEqual };

extern string _export sCondition(const string& type);
//extern void Assert(bool v2, string sv1, string type, char *file, int line);

#define NIL (void *)0
//#define PRECONDITION(cond, v1, v2) Assert(cond, v1, v2, #v1, #v2, "Precondition", __FILE__, __LINE__)
/*#define PRECONDITION1(v1) Assert(v1,#v1, "Pre Condition", __FILE__, __LINE__ );
#define POSTCONDITION(cond, v1, v2) Assert(cond, v1, v2, #v1, #v2, "Postcondition", __FILE__, __LINE__)
#define POSTCONDITION1(v1) Assert(v1,#v1, "Postcondition", __FILE__, __LINE__ );
#define INVARIANT1(v1) Assert(v1,#v1, "Invariant", __FILE__, __LINE__ );
#define INVARIANT(cond, v1, v2) Assert(cond, v1, v2, #v1, #v2, "Invariant", __FILE__, __LINE__)*/

class AssertDialog : public CDialog
{
  public:
    _export AssertDialog(CWnd *parent, const char* txt) ;
    _export ~AssertDialog(); 
    BOOL OnInitDialog();

    void OnAbort();
    void OnIgnore();
    void OnDebug();
    void maxLine(const string& text, CString& sLine);

  private:
    const char*       text;
    CButton *group, *Ignore, *Abort, *Debug;
    CStatic *message, *icon;
    struct dlgt 
    {   
      DLGTEMPLATE dlg;
      short cMenu[1];
      short cClass[1];
      short cCaption[1];
    } dlgFormBase;

    DECLARE_MESSAGE_MAP();
};

template<class C1> void Assert(const bool fTrue, C1 v1, C1 v2,  string sCond, string sv1, string sv2,  string type, char *file, int line)
{
  if (!fTrue) // if fAssert fails make debug strings
    Out(v1, v2, sCond, sv1, sv2, type, file, line);
    
}

template<class C1> void Out(C1 v1, C1 v2, string sCond, string sv1, string sv2,  string type, char *file, int line)
{
    char buf[200];
    strstream a(buf, 200);
    string className1(typeid(C1).name());
    string className2(typeid(C1).name());

    a << type << " failed : "<< "\n" <<
         sv1 << " "   << sCondition(sCond) << " "   << sv2 << "\n\n" <<
         className1   << " "  << sv1      << " = " << v1 << "\n"    <<
         className2   << " "  << sv2      << " = " << v2 << "\n\n"  <<
         "In File : " << file << "\n" <<
         "At Line : " << line << '\0';

 
    char *s=a.str();     
    AssertDialog dlg(0, s);
    INT_PTR iResult = dlg.DoModal();
    if ( iResult == IDABORT)
    {
      throw exception();
    }
	  if ( iResult == IDC_DEBUG) 
    {
      _CrtDbgBreak(); // go to debugger
      return;
    }
    if (iResult == IDIGNORE )
      return;
  }      


#endif






