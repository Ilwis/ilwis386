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
/* $Log: /ILWIS 3.0/Table/Tblvirt.cpp $
 * 
 * 18    4/13/06 4:44p Lichun
 * Added flow path longitudinal profile routine
 * 
 * 17    6/03/05 11:46a Lichun
 * Added Horton Plot application
 * 
 * 16    6-02-03 21:02 Hendrikse
 * added TableBursaWolf in 1st create()
 * 
 * 15    5-02-03 13:52 Hendrikse
 * added TableBursaWolf 2 times and added include .h file for it
 * 
 * 14    28-11-00 19:37 Koolhoven
 * inTableVirtual::create() do not create an ObjectDependency (not used,
 * and causes problems by creating columns which survive without parent
 * table
 * 
 * 13    24-12-99 12:36 Wind
 * added TableCrossVarioGram
 * 
 * 12    20-12-99 11:57 Wind
 * forgotten stuff from port of 2.23 code
 * 
 * 11    22-11-99 12:22 Wind
 * same as previous, but different implementation
 * 
 * 10    22-11-99 12:16 Wind
 * added object name to title of report window when calculating
 * 
 * 9     17-11-99 14:00 Wind
 * use of tranquilizer
 * buf in Hash function for 8 byte maps
 * 
 * 8     10/01/99 1:37p Wind
 * support for histograms of map in maplist ODF 
 * 
 * 7     9/24/99 10:39a Wind
 * replaced calls to static funcs ObjectInfo::ReadElement and WriteElement
 * by calls to member functions
 * 
 * 6     9/10/99 1:08p Wind
 * adpated to changes in constructors of FileName
 * 
 * 4     9/08/99 10:28a Wind
 * adpated to use of quoted file names
 * 
 * 3     3/23/99 9:27a Martin
 * Case problem solved
 * 
 * 2     3/11/99 12:17p Martin
 * Added support for Case insesitive 
// Revision 1.7  1998/09/16 17:25:30  Wim
// 22beta2
//
// Revision 1.6  1997/08/15 10:11:18  Wim
// UnFreeze sets nr of records on size of domainsort
//
// Revision 1.5  1997/08/14 12:38:41  Wim
// Do not store in a filename which is empty
//
// Revision 1.4  1997-08-13 13:51:49+02  Wim
// Intialize Array<String> as always with 0 members
//
// Revision 1.3  1997-08-12 18:12:01+02  Wim
// Use UnFreeze() instead of the too simple
// delete pts; pts = 0;
//
/* TableVirtual
   Copyright Ilwis System Development ITC
   april 1995, by Wim Koolhoven, Jelle Wind
	Last change:  WK   24 Jun 98   10:07 am
*/

#include "Engine\Table\tblstore.h"
#include "Engine\Table\TBLBIN.H"
#include "Engine\Applications\TBLVIRT.H"
#include "Engine\Table\TBLHIST.H"
#include "Engine\Table\TBLHSTSG.H"
//#include "Applications\Table\tblchdom.h"
#include "Engine\Table\TBLHSTPL.H"
#include "Engine\Table\TBLHSTPT.H"
//#include "Applications\Table\AUTCSEMV.H"
//#include "Applications\Table\Spatcorr.h"
//#include "Applications\Table\SEGDIR.H"
//#include "Applications\Table\Tblcross.h"
//#include "Applications\Table\TblFlowPathLonsitudinalProfile.h"
//#include "Applications\Table\TBLTRNSP.H"
//#include "Applications\Table\TblGlue.h"
//#include "Applications\Table\Tblcrvar.h"
//#include "Applications\Table\TblBursaWolf.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Domain\dmsort.h"
#include "Headers\Hs\tbl.hs"

TableVirtual* TableVirtual::create(const FileName& fn, TablePtr& p)
{
	String sExpr;
	if (0 == ObjectInfo::ReadElement("IlwisObjectVirtual", "Expression", fn, sExpr))
		return NULL;
	String sFunc = sExpr.sHead("(");
	ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
	if ( info != NULL ) {
		vector<void *> extraParms = vector<void *>();
		return (TableVirtual *)(info->createFunction)(fn, p, "", extraParms);
	}
    return NULL;
}

TableVirtual* TableVirtual::create(const FileName& fn, TablePtr& p, const String& sExpression)
{
 	String sFunc = IlwisObjectPtr::sParseFunc(sExpression);
	ApplicationInfo * info = Engine::modules.getAppInfo(sFunc);
	vector<void *> extraParms = vector<void *>();
	if ( info != NULL ) {
		return (TableVirtual *)(info->createFunction)(fn, p, sExpression, extraParms);
	}
	  if (0 != sExpression.strchrQuoted('('))
		throw ErrorAppName(fn.sFullName(), sExpression);
	  else
		NotFoundError(FileName(sExpression, ".tbt", false));
  return 0;
}

TableVirtual::TableVirtual(const FileName& fn, TablePtr& p, bool fCreate)
: IlwisObjectVirtual(fn, p, p.objdep, true), ptr(p), pts(ptr.pts)
{
  if (fCreate)
    const_cast<FileName&>(fnObj) = fn; // for histograms with other extension than .tbt
}

void TableVirtual::Store()
{
  if (fnObj.sFile.length() == 0)  // empty file name
    return;
  IlwisObjectVirtual::Store();
  ptr.WriteElement("Table", "Type", "TableVirtual");
  // for downward compatibility to 2.02:
  ptr.WriteElement("TableVirtual", "DependentType", sType());
  ptr.WriteElement("TableVirtual", "Expression", sExpression());
}

TableVirtual::~TableVirtual()
{
}

String TableVirtual::sType() const
{
  return "Dependent Table";
}

String TableVirtual::sExpression() const
{ 
  return sUNDEF; 
}

void TableVirtual::Freeze()
{
  if (!objdep.fMakeAllUsable())
    return;
  UnFreeze();
  InitFreeze();
  CreateTableStore();
  assert(pts != 0);
  trq.Start();
  String sTitle("%S - %S", sFreezeTitle, sName(true));
  trq.SetTitle(sTitle);
  trq.SetHelpTopic(htpFreeze);
  ptr.iRef++; // to prevent deletion in fFreezing when colNew(..) is called
  bool fFrozen;
  try {
    fFrozen = fFreezing();
  }
  catch (const ErrorObject& err) {
    err.Show();
    fFrozen = false;
  }    
  ptr.iRef--; // see ptr.iRef++
  if (fFrozen) {
    ptr.Updated();
  }
  else {
    UnFreeze();
  }
  trq.SetText(STBLTextStoreTable);
  ptr.Store();
  trq.Stop();
}

void TableVirtual::UnFreeze()
{
  if (pts)
    pts->SetErase(true);
  delete pts;
  pts = 0;
  ptr._iCols = 0;
  ptr._iRecs = 0;
  DomainSort* ds = dm()->pdsrt();
  if (ds)
    ptr._iRecs = ds->iSize();
  ptr.Store();
}

bool TableVirtual::fFreezing()
{
  return false;
}

void TableVirtual::CreateTableStore()
{
  if (0 != pts) 
    UnFreeze();
  pts = new TableBinary(ptr.fnObj, ptr, ptr.fnDat);
  if (ptr.dm()->pdnone())
    pts->iRecNewWithoutRecsUpdate(iRecs());
}
