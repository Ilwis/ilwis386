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
// ExternalCommand.h: interface for the ExternalCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTERNALCOMMAND_H__2884CF06_C71D_41B1_ADAD_FF16BAAF2E64__INCLUDED_)
#define AFX_EXTERNALCOMMAND_H__2884CF06_C71D_41B1_ADAD_FF16BAAF2E64__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ExternalCommand  
{
public:
	ExternalCommand(String &sExecutable, String &sArguments); // throws ErrorObject
	// An ErrorObject will be thrown in 2 cases:
	// 1. CreateProcess generated an error (usually "the executable file was not found")
	// 2. The executable generated an error
	// The description in the ErrorObject is the one returned by CreateProcess or by
	// the executable.
	virtual ~ExternalCommand();
	void Execute(bool fBlockWhileExecuting);
	// fBlockWhileExecuting = true means this function must block til the external
	// command ends, get the error result and "throw" an error if applicable
	// fBlockWhileExecuting = false means the external command can be left running
	// and this function should not block nor throw any error.


private:
	static bool fIsConsoleApplication(String sFile);
	static bool fReadFileBytes(HANDLE hFile, LPVOID lpBuffer, DWORD dwSize);
	static String sFindFullPathToFile(String sFile);
	String sCommandLine();
	String m_sArguments;
	String m_sExecutable;
};

#endif // !defined(AFX_EXTERNALCOMMAND_H__2884CF06_C71D_41B1_ADAD_FF16BAAF2E64__INCLUDED_)
