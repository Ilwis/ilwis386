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
/* Logger.c
   Copyright Ilwis System Development ITC
   march 1997, by Jelle Wind
	Last change:  WK   28 Aug 98    4:57 pm
*/
#define LOGGER_C

#include "Headers\toolspch.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\DataObjects\Color.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\Applications\objvirt.h"
#include "Engine\Applications\ModuleMap.h"
#include "Engine\Base\System\Engine.h"

Logger::Logger()
{
	if (getEngine()->getContext() == NULL)
		return;
	String sDirName = getEngine()->getContext()->sIlwDir() + "Log";
	Directory dir(sDirName);
	if ( !Directory::fIsExistingDir(dir)) {
		_mkdir(dir.sFullPath().scVal());
	}
}

Logger::Logger(const String& sPrf)
{
  SetPrefix(sPrf);
}

Logger::~Logger()
{
}

void Logger::init(Mode m)
{
	// Initializes the ofstream os using the log-file-name sFileName
	// The registry has already been searched (in constructor) for a value for
	// sFileName. If this didn't return anything useful, do one more attempt by
	// checking in which folder the user executed the command.
	if (getEngine()->getContext() == NULL)
		return;
	String sDirName = getEngine()->getContext()->sIlwDir() + "Log";
	if ( !osNormal.is_open())
	{
		String  loggerName = sDirName + "\\ilwis.log";
		osNormal.open(loggerName.sVal(), ios::app);
	}
	if ( m==lmERROR && !osError.is_open())
	{
		String loggerName = sDirName + "\\error.log";
		osError.open(loggerName.sVal(), ios::app);
	}
	if ( m==lmDEBUG && !osDebug.is_open())
	{
		String loggerName = sDirName + "\\debug.log";
		osDebug.open(loggerName.sVal(), ios::app);
	}
}

void Logger::LogTime(ofstream& os)
{
  Time tm(time(0));
  os << tm.sTime().sVal() << " " << tm.sDate().sVal();
}

void Logger::LogPrefix(ofstream& os)
{
  os << "  " << sPrefix.sVal();
}

void Logger::LogLine(const String& sLine, Mode m)
{
  init(m);
  if ( m == lmNORMAL)
	  LogLine(sLine, osNormal, m);
  else if ( m == lmERROR)
	  LogLine(sLine, osError, m);
  if ( m == lmDEBUG)
	  LogLine(sLine, osDebug, m);
}

void Logger::LogLine(const String& sLine, ofstream& os, Mode m) {
  LogTime(os);
  if (sPrefix.length() != 0)
    LogPrefix(os);
  os << "  " << sLine.scVal() << endl;
  os.flush();
}

void Logger::SetPrefix(const String& sPrf)
{
  sPrefix = sPrf;
}








