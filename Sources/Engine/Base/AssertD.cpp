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
#include "Engine\Base\AssertD.h"
//#include "Headers\xercesc\util\Platforms\Win32\resource.h"

BEGIN_MESSAGE_MAP(AssertDialog, CDialog)
  ON_BN_CLICKED(IDIGNORE, OnIgnore)
  ON_BN_CLICKED(IDABORT, OnAbort)
  ON_BN_CLICKED(IDC_DEBUG, OnDebug)   
END_MESSAGE_MAP()  

AssertDialog::AssertDialog(CWnd *parent, const char* txt) :
  CDialog(),
  text(txt)
{
    dlgFormBase.dlg.style = WS_CAPTION | DS_MODALFRAME | WS_SYSMENU;
    dlgFormBase.dlg.dwExtendedStyle = 0;
    dlgFormBase.dlg.cdit = 0;
    dlgFormBase.dlg.x = 0;
    dlgFormBase.dlg.y = 0;
    dlgFormBase.dlg.cx = 0;
    dlgFormBase.dlg.cy = 0;
    dlgFormBase.cMenu[0] = 0;
    dlgFormBase.cClass[0] = 0;
    dlgFormBase.cCaption[0] = 0;
    InitModalIndirect(&dlgFormBase, parent);
}

AssertDialog::~AssertDialog() 
{
  delete Ignore;
  delete Abort;
  delete Debug;
  delete group;
  delete message;
  delete icon;

  DestroyWindow();
}

void AssertDialog::OnIgnore()
{ 
  EndDialog(IDIGNORE);
}

void AssertDialog::OnAbort()
{
  EndDialog(IDABORT);
}
 
void AssertDialog::OnDebug()
{
  EndDialog(IDC_DEBUG);
} 
  

BOOL AssertDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
  CDC *dc = GetDC();
  int iXCenter = dc->GetDeviceCaps(HORZRES) / 2;
  int iYCenter = dc->GetDeviceCaps(VERTRES) / 2;
  MoveWindow(iXCenter - 100, iYCenter -100, 200, 200);
  CPoint BasePoint(80,30);

  CRect rct;
  GetWindowRect(&rct);
  message=new CStatic();
  icon=new CStatic();
  icon->Create("", WS_CHILD | WS_VISIBLE | SS_ICON, CRect(30,30,52, 52), this);
  icon->SetIcon(LoadIcon(0, IDI_ERROR));
  message->Create(text, WS_CHILD | WS_VISIBLE , CRect(BasePoint,CSize(100,80)), this);
  CDC *cdc=message->GetDC();
  
  CString sLine;
  maxLine(text, sLine);
  CSize szSize = cdc->GetTextExtent(sLine) ;
  int iXSize = max(szSize.cx  + BasePoint.x + 52, rct.Width());
  int iYSize = max(szSize.cy * 12  + BasePoint.x, rct.Height()) ; // 9 lines
  MoveWindow(CRect(rct.TopLeft(), CSize(iXSize, iYSize)));
  GetWindowRect(&rct); 
  message->MoveWindow(BasePoint.x, BasePoint.y, szSize.cx, szSize.cy * 9);
  group=new CButton();
  group->Create("",WS_CHILD | WS_VISIBLE | WS_GROUP | BS_GROUPBOX, CRect(15,15, rct.Width()-20, rct.Height()-100), this, 101); 
  group->ShowWindow(SW_SHOW);
  Abort = new CButton();
  Ignore = new CButton();
  Debug = new CButton();
  int iYBut=rct.Height() - 70;
  int iXBut=0;
  Abort->Create("Abort",WS_CHILD | WS_VISIBLE | WS_GROUP | BS_PUSHBUTTON, CRect(CPoint(iXBut+20,iYBut), CSize(70, 30)), this, IDABORT);
  Ignore->Create("Ignore",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(iXBut + 100, iYBut), CSize(70,30)), this, IDIGNORE);
  Debug->Create("Debug",WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, CRect(CPoint(iXBut + 180, iYBut), CSize(70, 30)), this, IDC_DEBUG);
  Abort->ShowWindow(SW_SHOW);
  Ignore->ShowWindow(SW_SHOW);
  Debug->ShowWindow(SW_SHOW);
  SetWindowText("Assertion failed");

  return TRUE;
}

void AssertDialog::maxLine(const string& text, CString& sLine)
{
  int iBegin=0;
  int iEnd=0;
  string sDummy;
  while(iBegin!=text.size())
  {
    iEnd=text.find('\n', iBegin);
    if ( iEnd == -1 ) break;
    sDummy=text.substr(iBegin, iEnd - iBegin);
    if ( iEnd - iBegin > sLine.GetLength())
      sLine=CString(text.substr(iBegin, iEnd-iBegin).c_str());
    iBegin=iEnd+1;
  }

}

/*void Assert(bool v2, string sv1, string type, char *file, int line) 
{
    if ( v2) return ;

    char buf[200];
    strstream a(buf, 200);
 
    a << type << " failed : "<< "\n" <<
         sv1 << " == "   << " false\n\n" <<
         "In File : " << file << "\n" <<
         "At Line : " << line << '\0';
 
    char *s=a.str();     
    AssertDialog dlg(0, s);
    int iResult = dlg.DoModal();
    if ( iResult == IDABORT)
    {
      throw exception();
    }
	  if ( iResult == IDOK) 
    {
      _CrtDbgBreak(); // go to debugger
      return;
    }
    if (iResult == IDIGNORE )
      return;

}*/

string sCondition(const string& sType)
{
  if ( sType == "fILess") return "<";
  if ( sType == "fIEqual") return "==";
  if ( sType == "fINotEqual") return "!=";
  if ( sType == "fIGreaterEqual") return ">=";
  if ( sType == "fILessEqual") return "<=";
  if ( sType == "fIGreater") return ">";
  return "??";
}  

void Assert1(bool val)
{}










