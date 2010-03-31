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

#include "Client\Headers\AppFormsPCH.h"
#include "Client\MainWindow\ACTION.H"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\ilwis.h"
#include "Client\Base\WinThread.h"
#include "Client\Editors\Stereoscopy\StereoscopeWindow.h"
#include "Client\MainWindow\CommandHandlerUI.h"
#include "Headers\Hs\Mainwind.hs"

void OpenAnaglyph(CWnd *parent);
String sDataBaseName(const String& sDb);
void OpenBaseMap(CWnd *parent);
LRESULT Cmdstereoscope(CWnd *parent, const String& sCmd);
LRESULT Cmdanaglyph(CWnd* parent, const String& sCmd);
LRESULT Cmdopen(CWnd *parent, const String& sCmd);
LRESULT Cmdedit(CWnd *parent, const String& sCmd);
LRESULT Cmdmakestereopair(CWnd *parent, const String& sCmd);
LRESULT Cmdmplslideshow(CWnd *parent, const String& sCmd);
void EditBaseMap(CWnd *parent);
//int OpenMapListSlideShow(CWnd *parent);
LRESULT Cmdcreatemap(CWnd* parent, const String& sParm);
LRESULT Cmdcreateseg(CWnd* parent, const String& sParm);
LRESULT Cmdcreatepol(CWnd* parent, const String& sParm);
LRESULT Cmdcreatepnt(CWnd* parent, const String& sParm);
LRESULT Cmdcreatetbl(CWnd* parent, const String& sParm);
LRESULT Cmdcreatetb2(CWnd* parent, const String& sParm);
LRESULT Cmdcreatedom(CWnd* parent, const String& sParm);
LRESULT Cmdcreaterpr(CWnd* parent, const String& sParm);
LRESULT Cmdcreatecsy(CWnd* parent, const String& dummy);
LRESULT Cmdcreatefil(CWnd* parent, const String& sDomain);
LRESULT Cmdcreateisl(CWnd* parent, const String& dummy);
LRESULT Cmdcreatefun(CWnd* parent, const String& dummy);
LRESULT Cmdcreatempl(CWnd* parent, const String& dummy);
LRESULT Cmdcreatesms(CWnd* parent, const String& parm);
LRESULT Cmdcreategrf(CWnd* parent, const String& sCsy);
LRESULT Cmdcreateioc(CWnd* parent, const String& sN);
LRESULT Cmdlayout(CWnd *parent, const String& sCmd);
LRESULT Cmdgraph(CWnd *parent, const String& sCmd);
LRESULT Cmdrosediagram(CWnd *parent, const String& sCmd);
LRESULT Cmddisplay3d(CWnd *wndOwner, const String& sCmd);
LRESULT Cmdpropobject(CWnd *wndOwner, const String& str);

