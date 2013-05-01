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
/* ui/fentbase.h
   Interface for FormEntry and derived classes
   by Wim Koolhoven, aug. 1993
   modified by Jelle Wind, dec 1993
   (c) Computer Department ITC
	Last change:  WK   10 Jul 97    2:35 pm
*/



#ifndef FENTBASE_H
#define FENTBASE_H

#define FLDNAMEWIDTH    ((int)XDIALOG(60)*frm()->rFontRatio)
#define FLDNAMEOUTPUTWIDTH  ( 2 * FLDNAMEWIDTH )
#define FLDINTWIDTH     (FLDNAMEWIDTH/2)
#define FLDREALWIDTH    (FLDNAMEWIDTH*3/4)
#define FLDSTRINGWIDTH  FLDNAMEWIDTH
#define FLDSEP          YDIALOG(2)

class _export FormEntry;
class FormBase;

#include "Client\Editors\Utils\bevel.h"

enum enumAlign { AL_UNDER, AL_AFTER };

class _export FormEntry : public CCmdTarget
{
friend class FormBase;
friend class FormBaseDialog;
friend class FormBaseView;

public:
  enum BevelStyle{bsNONE, bsRAISED, bsLOWERED};
	 
  virtual                 ~FormEntry();
  virtual void            Align(FormEntry*, enumAlign, int iOffSet = 0); // align entry under or after other
  virtual void            create();
  virtual void            show(int sw);           // set show or hide sw = SW_SHOW, SW_HIDE
  virtual void            SetFocus();             // sets focus of zApp control
  virtual void            StoreData();            // store result in destination
  virtual void            Enable();
  virtual void            Disable();
  virtual void            setHelpItem(const HelpTopic& htp);
  virtual void            SetCallBack(NotifyProc np);
  virtual void            SetCallBack(NotifyProc np, CallBackHandler* cb);
  virtual void            SetWidth(short iWid);
  virtual bool            fRadioButton() const;
  virtual bool            fCheckBox()    const;
  virtual bool            fGroup()       const;
  virtual void            DrawItem(Event* dis); // draw for class _export NameEdit
  virtual void            CheckDrawItem(DRAWITEMSTRUCT* dis);
  virtual String          sGetText();             // returns value and info of control as String
  virtual FormEntry*      CheckData();            // validate entry value
  virtual void            ClearData();
  String                  sChildrensText();	    // returns the sText of all childeren
  void                    Show();
  void                    Hide();
  bool                    fShow() 
                              { return _fShow; }
  void                    SetFieldWidth(short iWid);  // in dialog units
  void                    SetHeight(short iHeight);
  void                    ShowChildren();          // show all children
  void                    HideChildren();          // hide all children
  void                    CreateChildren();        // create all children
  void                    EnableChildren();           // enable all children
  void                    DisableChildren();          // disable all children
  void                    CallCallBacks();
	void                    DoCallBack(Event *ev=0);
  void                    SetIndependentPos()
                              { psn->fIndependentPos = true; }
  void                    SetCallBackForAll(NotifyProc np);  // sets call back for form entry and children (if no callback set yet)
  int                     Id()                     // Id of entry
                              { return id; }   
	void                    SetBevelStyle(BevelStyle bsStyle);
	virtual void setLabel(const String& s) {}

  String              htp();
  FormEntry*              parent() 
                              { return _par; }        // parent
  const NotifyProc&       npChanged() const           // call back when form entry changed
                              { return _npChanged; } 
  CallBackHandler*        cb() const 
                              { return _cb; }
  FormBase*               frm()                       // form in which entry is defined
                              { return _frm;  }  
  Parm*                   prm()                       // parameter for command line
                              { return _prm;}       
  int                     Help();                     // check to draw for class _export NameEdit
  zDimension              Dim(const String& s, CFont* fnt = 0);
  Array<FormEntry*>&      childlist() 
                              { return children; }   // list of children

  FormEntryPositioner     *psn;

protected:
  FormEntry(FormEntry*, Parm *prm = 0, bool fAutoAlign = false);

  Array<FormEntry*>       children;                   // list of children
  Parm*                   _prm;                       // parameter for command line
  FormBase*               _frm;                       // form in which entry is defined
  FormEntry*              _par;                       // parent
  NotifyProc              _npChanged;                 // call back when form entry changed
  CallBackHandler*        _cb;
  bool                    _fShow;                     // true: show children; false: hide
  bool					  _enabled;
  bool					  stopCascade;

private:
    FormEntry(FormBase*);                               // constructor for the creation of

    void                    Init();
    void                    MaxX(int *iMaxX, int iMaxHeight);// same, but restricted to form entries
                                    // with position < iMaxHeight
    int                     id;                            // actual identifier
    HelpTopic               _htp;
		BevelStyle		  				bsStyle;
		Bevel										bevel;
};

#endif // FENTBASE_H




