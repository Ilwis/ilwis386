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
// ui/formbase.h
// Interface for FormBase
// modified by Jelle Wind, dec. 1993
// (c) Computer Department ITC
	Last change:  WK    7 Aug 98    7:07 pm
*/

#ifndef FORMBASE_H
#define FORMBASE_H

#include "Headers\toolspch.h"
#include "Engine\Base\System\SYSVAR.H"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Client\Editors\Utils\bevel.h"
#include "Headers\Htp\Ilwismen.htp"
#include "Headers\Htp\UI.HTP"

#undef IMPEXP
#ifdef ILWISCLIENT
#define IMPEXP __declspec(dllexport)
#else
#define IMPEXP __declspec(dllimport)
#endif


enum FormBaseStyle 
{ 
	fbsSHOWALWAYS = 0x01, fbsMODAL = 0x02, fbsHIDEONOK = 0x04, fbsNOOKBUTTON = 0x08, 
	fbsNOCANCELBUTTON = 0x10, fbsBUTTONSUNDER = 0x20, fbsNOPARENTRESIZE = 0x40, fbsNOBEVEL=0x80,
	fbsAPPLIC = 0x100, fbsHIDEONCLOSE = 0x200, fbsTOPMOST = 0x400, fbsCALC = 0x800,
	fbsOKHASCLOSETEXT = 0x1000, fbsNoCheckData=0x2000, fbsApplyButton=0x4000,fbsCancelHasCLOSETEXT=0x8000

};


class FormEntry;
class FormBase;
class FormBaseDialog;

class IMPEXP FormBase: public BaseZapp
{
friend class FormEntry;
friend class FormBaseDialog;

protected:
  FormBase(); // for view
  FormBase(CWnd* parent, const String& sTitle, bool fShowAlways = true, bool fModal = false, bool fHideOnOk = false);
  FormBase(CWnd* parent, const String& sTitle, int fbs);
  FormBase(CWnd* parent, const String& sTitle, ParmList *plDefault, bool fShowAlways = true, bool fModal = false, bool fHideOnOk = true);
public:
  virtual ~FormBase();
  
  bool                 fOkClicked() { return _fOkClicked; } // ? closed by clicking OK button
  virtual CWnd* wnd() = 0;
  virtual int exec()= 0;                    // executed if OK button clicked
  void OnOK();                                 // called if Ok button clicked
  virtual FormEntry *feDefaultFocus();  // overload if other default is wished.
  int                  iDefFldWidth()  
                          { return _iDefFldWidth; }  // default width of formentry
  int                  iDefFldOffSet() 
                          { return _iDefFldOffSet; } // def. placement of entry field after text
  int                  iDefFldHeight() 
                          { return _iDefFldHeight; } // default height of formentry
  void                 Add(Parm *prm);                // add prm to parm list pl
  void                 setHelpItem(const HelpTopic& htp) 
                          { _htp = htp; }
  void                 SetAppHelpTopic(unsigned long iTop)
                          { setHelpItem(HelpTopic(iTop,"ilwis.chm")); }
  void                 SetMenHelpTopic(unsigned long iTop)
                          { setHelpItem(HelpTopic(iTop,"ilwis.chm")); }
  HelpTopic&           htp() 
                          { return _htp; }
  bool                 fHideOnOk() const;
  void                 HideOnOk(bool f) 
                          { fbs |= fbsHIDEONOK; if (!f) fbs -= fbsHIDEONOK; }
  void                 EnableOK();
  void                 DisableOK();    
  void                 SetCallBack(NotifyProc np)  // sets callback for all form entries that don't have a call back defined yet
                          { _npChanged = np; }
  NotifyProc           npChanged()
                          { return _npChanged; }  // call back when form entries change
  virtual bool                 fValidData() 
                          { return CheckData() == NULL; }  
  zDisplay*            windsp() 
                          { return _windsp; };    // Display used for calc. of text dimensions
  Array<FormEntry*>&   childlist() 
                          { return root->children; } // list of children
	virtual FormEntry*   CheckData();                            // returns form entry if it contains an invalid value  
	void RemoveChildren();
	void RecalculateForm();

  String               sText();
  void                 NotShowAlways()  
                          { fbs &= ~fbsSHOWALWAYS; }
	FormEntry*  feRoot() const
	{ return root; }

  //-------- Data members
  IlwisAppContext      *ilwapp;
  FormEntry            *feLastInserted;            // last entry inserted
  double               rFontRatio;
	bool                 fEndInitialization;
  CFont								 *fnt;
  static const CFont *fntDflt;

protected:
	virtual void        create() = 0;
	virtual void        shutdown(int iReturn=IDCANCEL); 
	virtual void        Init();         // for constructors
	bool                fModal() const;
	void                Prepare();       
	bool                fShowAlways() const;
	int                 Copy(const String& sPageTitle = String());
	int                 Print(const String& sPageTitle = String());
	String              sCaption;
  
  //-------- Data members
  FormEntry*           root;                       // root entry
  CWnd*                par;                        //parent of form
  ParmList             pl;                         // parm list for forms with parm
  bool                 _fOkClicked;                // true if form closed with OK button clicked
  zDisplay*           _windsp;                   // Display used for calc. of text dimensions
  CWnd                *dummyWindow; // to be able to get text properties before dialog is created
  int                 fbs;                        // style flags

    CButton butOK, butCancel, butHelp;
	CButton butShow, butDefine; 
	Bevel               bevel;
	bool fNoCheckData;
  
private:
  //-------- Data members
  NotifyProc           _npChanged;                 // call back when form entry changed};
  int                 _iDefFldWidth, _iDefFldOffSet, _iDefFldHeight;
  ParmList            *plDflt;                    // deflt parm list for forms with parm
  HelpTopic           _htp;
};


class IMPEXP FormBaseDialog: public CDialog, public FormBase
{
public:
  FormBaseDialog(CWnd* parent, const String& sTitle, bool fShowAlways = true, bool fModal = false, bool fHideOnOk = false);
  FormBaseDialog(CWnd* parent, const String& sTitle, int fbs, DWORD extraWindowsStyles=0);
  FormBaseDialog(CWnd* parent, const String& sTitle, ParmList *plDefault, bool fShowAlways = true, bool fModal = false, bool fHideOnOk = true);
  ~FormBaseDialog();

  virtual CWnd* wnd();
  virtual void create();
  bool CreateDialogTemplate(CWnd* p, const String& title, DWORD style);
  BOOL OnInitDialog();
  afx_msg void OnOK();                                 // called if Ok button clicked
  
protected:
  void CreateDefaultPositions();               // creates and shows the form
  void shutdown(int iReturn=IDCANCEL); 
  void Copy();
  void Print();
  afx_msg virtual void OnCancel();                   // called if Cancel button clicked
  afx_msg void OnHelp();                             
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM lParam);
  afx_msg void OnDrawItem( int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct );
  afx_msg void OnMeasureItem( int nIDCtl, LPMEASUREITEMSTRUCT mi );
  afx_msg void OnSetFocus( CWnd* pOldWnd );
  afx_msg void OnSysCommand( UINT nID, LPARAM lParam );
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	int iImg; // set in derived class to set icon
private:
  struct dlgt 
  {   
      DLGTEMPLATE dlg;
      short cMenu[1];
      short cClass[1];
      short cCaption[1];
  } dlgFormBase;

  DECLARE_MESSAGE_MAP()
};

class IMPEXP FormWithDest : public FormBaseDialog
{
public:
  FormWithDest(CWnd* parent, const String& sTitle) 
    : FormBaseDialog(parent, sTitle, true, true, false) {}
  FormWithDest(CWnd* parent, const String& sTitle, int fbs, int extraWindowsStyles=0)
    : FormBaseDialog(parent, sTitle, fbs, extraWindowsStyles) {}
  FormWithDest(CWnd* parent, const String& sTitle, bool fCancelBut)
    : FormBaseDialog(parent, sTitle, (fbsSHOWALWAYS | fbsMODAL | (fCancelBut ? 0 : fbsNOCANCELBUTTON) ) & ~fbsHIDEONOK) {}
    
	~FormWithDest();

  virtual int exec();
};

class IMPEXP FormWithParm : public FormBaseDialog
{
    public:
        FormWithParm(CWnd* parent, const String& sTitle, ParmList *pl);
 

        int                 exec();
        String              sCmd() 
                                { return pl.sCmd(); }    // return command line
};

#endif // FORMBASE_H
