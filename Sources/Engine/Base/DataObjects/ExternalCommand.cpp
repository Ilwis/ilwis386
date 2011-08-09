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
// ExternalCommand.cpp: implementation of the ExternalCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "Headers\toolspch.h"
#include "Engine\Base\DataObjects\ExternalCommand.h"
#include "Headers\Hs\Mainwind.hs"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define BUFSIZE 4096

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define IMAGE_SIZEOF_NT_OPTIONAL32_HEADER    224
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER     IMAGE_SIZEOF_NT_OPTIONAL32_HEADER

ExternalCommand::ExternalCommand(String &sExecutable, String &sArguments)
: m_sExecutable(sExecutable)
, m_sArguments(sArguments)
{

}

ExternalCommand::~ExternalCommand()
{

}

String ExternalCommand::sCommandLine()
{
	if (m_sArguments.length() > 0)
		return m_sExecutable + " " + m_sArguments;
	else
		return m_sExecutable;
}

String ExternalCommand::sFindFullPathToFile(String sFile)
{
	// Helper function for fIsConsoleApplication
	// Given only the name of an executable, this function will return
	// the full path to the same executable that CreateProcess finds (at least, in theory)
	char cBuffer [BUFSIZE];
	LPTSTR pDummy;
	if (SearchPath(NULL, sFile.c_str(), ".exe", BUFSIZE, cBuffer, &pDummy))
		return String(cBuffer);
	else
		return sFile; // return the original by default .. we failed to find it
}

bool ExternalCommand::fReadFileBytes(HANDLE hFile, LPVOID lpBuffer, DWORD dwSize)
{
	// Helper function for fIsConsoleApplication
	// reading bytes from hFile with error capturing
	DWORD dwBytes = 0;

	if (!ReadFile (hFile, lpBuffer, dwSize, &dwBytes, NULL))
		return false; // Failed to read file!

	if (dwSize != dwBytes)
		return false; // Wrong number of bytes! expected / read: dwSize, dwBytes

	return true;
}

bool ExternalCommand::fIsConsoleApplication(String sFile)
{
	// return true if sFilePath points to an application that starts a console - false otherwise
	
	// Code taken with minor modifications from CodeGuru:
	// http://www.codeguru.com/Cpp/W-P/system/misc/article.php/c2897/
	// Title: "Determine if an Application is Console or GUI"

	HANDLE hImage;
	DWORD dwCoffHeaderOffset;
	DWORD dwNewOffset;
	DWORD dwMoreDosHeader[16];
	ULONG ulNTSignature;

	IMAGE_DOS_HEADER dos_header;
	IMAGE_FILE_HEADER file_header;
	IMAGE_OPTIONAL_HEADER optional_header;

	// Find the application file in the same search path as CreateProcess.

	String sFilePath = sFindFullPathToFile(sFile);

	// Open the application file.

	hImage = CreateFile(sFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hImage == INVALID_HANDLE_VALUE)
		return false; // Failed to open the aplication file!

	// Read MS-Dos image header.

	if (!fReadFileBytes (hImage, &dos_header, sizeof (IMAGE_DOS_HEADER)))
		return false; // Failed to read file!
	
	if (dos_header.e_magic != IMAGE_DOS_SIGNATURE)
		return false; // Application failed to classify the file type!

	// Read more MS-Dos header.

	if (!fReadFileBytes (hImage, dwMoreDosHeader, sizeof (dwMoreDosHeader)))
		return false; // Failed to read file!

	// Move the file pointer to get the actual COFF header.

	dwNewOffset = SetFilePointer(hImage, dos_header.e_lfanew, NULL, FILE_BEGIN);
	dwCoffHeaderOffset = dwNewOffset + sizeof (ULONG);

	if (dwCoffHeaderOffset == 0xFFFFFFFF)
		return false; // Failed to move file pointer!

	// Read NT signature of the file.

	if (!fReadFileBytes (hImage, &ulNTSignature, sizeof (ULONG)))
		return false; // Failed to read NT signature of file!

	if (ulNTSignature != IMAGE_NT_SIGNATURE)
		return false; // Missing NT signature!

	if (!fReadFileBytes (hImage, &file_header, IMAGE_SIZEOF_FILE_HEADER))
		return false; // "Failed to read file!

	// Read the optional header of file.

	if (!fReadFileBytes (hImage, &optional_header, IMAGE_SIZEOF_NT_OPTIONAL_HEADER))
		return false; // Failed to read file for optional header!

	CloseHandle(hImage);

	/*
	String sAppType;
	switch (optional_header.Subsystem)
	{
		case IMAGE_SUBSYSTEM_UNKNOWN:
			sAppType = "Unknown";
			break;
		case IMAGE_SUBSYSTEM_NATIVE:
			sAppType = "Native";
			break;
		case IMAGE_SUBSYSTEM_WINDOWS_GUI:
			sAppType = "Windows GUI";
			break;
		case IMAGE_SUBSYSTEM_WINDOWS_CUI:
			sAppType = "Windows Console";
			break;
		case IMAGE_SUBSYSTEM_OS2_CUI:
			sAppType = "OS//2 Console";
			break;
		case IMAGE_SUBSYSTEM_POSIX_CUI:
			sAppType = "Posix Console";
			break;
		case IMAGE_SUBSYSTEM_NATIVE_WINDOWS:
			sAppType = "Native Win9x";
			break;
		case IMAGE_SUBSYSTEM_WINDOWS_CE_GUI:
			sAppType = "Windows CE GUI";
			break;
	}
	// code taken from CodeGuru, left here for reference and future debugging
	*/

	return (optional_header.Subsystem == IMAGE_SUBSYSTEM_WINDOWS_CUI) || (optional_header.Subsystem == IMAGE_SUBSYSTEM_OS2_CUI) || (optional_header.Subsystem == IMAGE_SUBSYSTEM_POSIX_CUI);
}

void ExternalCommand::Execute(bool fBlockWhileExecuting)
{
	// Purpose of this function:
	// 1. Start the application (the "external command") indicated by the
	//    member variables.
	// 2. For console applications, display the console while they execute
	//    (and show stdout in the console)
	// 3. For console applications, capture the stderr output (if available)
	//    and throw it if there was an error at exit
	// 4. Ctrl+C or click on (x) of the console window should close (abort)
	//    the console application, but not the caller process.
	//
	// Clarification:
	// - This code runs in the caller process. The external command runs in
	//   a new process (the "child process").
	// 
	// Steps in the implementation:
	//
	// 1. Fill in STARTUPINFO structure: by default all 0's, but in case of a
	// console application use the USESTDHANDLES flag and fill in the three
	// corresponding handles (stdin, stdout, stderr). In fact we only care about
	// stderr, but the other two must be valid otherwise they don't show.
	// 
	// 1a. To be able to fill in correct handles taken by GetStdHandle(), we must
	// call AllocConsole() beforehand.
	// 
	// 1b. With CreatePipe we buffer the stderr so that we can read it back at
	// the end. Another way to do this would be to create a file and read it back
	// at the end. The pipe substitutes the need for a "memory file". The
	// "suggested size" of the pipe's buffer is set to be BUFSIZE (4KB).
	// 
	// 1c. Call FreeConsole() to hide the console attached to the caller process.
	// This console won't be used anymore. If we left it, the user would see two
	// consoles on the screen. This console was used only to get correct handles
	// from GetStdHandle().
	// 
	// 2. Declare a PROCESS_INFORMATION structure: all 0's - this is filled in
	// by CreateProcess
	// 
	// 3. Call CreateProcess. In case of a console application, pass TRUE to
	// inherit the handles we defined in STARTUPINFO. Also supply the
	// CREATE_NEW_CONSOLE flag so that the new process has its own console
	// (MSDN says that non-console applications don't create the console)
	// 
	// 4. Depending on the value of fBlockWhileExecuting, block the caller process by
	// WaitForSingleObject, and get the exit code of the child process
	// 
	// 5. Depending on the value of fBlockWhileExecuting, read back the stderr of the
	// external command that was buffered by the pipe, and throw an error if needed.

	// Notes:
	// - If you don't call AllocateConsole/FreeConsole, you will only get stderr
	//   (stdout does not come in the console that appears).
	// - If you don't use the flag CREATE_NEW_CONSOLE in CreateProcess, it seems
	//   that the console that appears belongs to the current process, with as a
	//   result that Ctrl+C stops the current process (which is ILWIS).
	//
	// Assumtions:
	// Console applications generate stdout and stderr, and return exit code != 0
	// when an error occurs
	// Windows applications don't utilize stdout or stderr, but return an
	// exit code != 0 when an error occurs


	bool fCustomHandles = false; // parts of the code can be skipped if this remains false
	HANDLE hStdErrorRd, hStdErrorWr; // read/write handles, buffered by the pipe
	// the external command writes stderr to hStdErrorWr, and at the end of this
	// function, we read back any stderr generated from hStdErrorRd

	STARTUPINFO si; // input to CreateProcess
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);

	SECURITY_ATTRIBUTES saAttr;  // for creating the pipe
	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	if (fBlockWhileExecuting && fIsConsoleApplication(m_sExecutable))
	{
		BOOL fConsoleAllocated = AllocConsole(); // purpose only to get correct handles to pass to the child process, otherwise there's no stdout

		// create a pipe to redirect stdErr of the application
		if (CreatePipe(&hStdErrorRd, &hStdErrorWr, &saAttr, BUFSIZE)) 
		{
			si.hStdError = hStdErrorWr;
			si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
			si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			if (si.hStdOutput != INVALID_HANDLE_VALUE && si.hStdInput != INVALID_HANDLE_VALUE)
			{
				si.dwFlags |= STARTF_USESTDHANDLES;
				fCustomHandles = true;
			}
			else
			{
				CloseHandle(hStdErrorRd);
				CloseHandle(hStdErrorWr);
			}
		}
		if (fConsoleAllocated)
			FreeConsole(); // It is not stated anywhere in MSDN that this is too early and handles get invalid.
										 // With this we prevent seeing two consoles, of which one is not used.
	}

	PROCESS_INFORMATION pi; // output of CreateProcess
	ZeroMemory( &pi, sizeof(pi) );

	// Start the child process. 
	if(	CreateProcess( NULL, // No module name (use command line).
			sCommandLine().sVal(),
			NULL,             // Process handle not inheritable. 
			NULL,             // Thread handle not inheritable. 
			fCustomHandles ? TRUE : FALSE,     // Set handle inheritance to TRUE/FALSE.
			CREATE_NEW_CONSOLE,                // Let the external command have its own console so that Ctrl+C does not break the calling process. 
			NULL,             // Use parent's environment block. 
			NULL,             // Use parent's starting directory. 
			&si,              // Pointer to STARTUPINFO structure.
			&pi )             // Pointer to PROCESS_INFORMATION structure.
	) 
	{ // process creation successful
		DWORD iExitCode = 0;
		// Optionally block here to get errors from child process and throw them.
		// No block implicates that errors of execution are ignored as iExitCode remains 0.
		if (fBlockWhileExecuting)
		{
			WaitForSingleObject( pi.hProcess, INFINITE );
			if (!GetExitCodeProcess(pi.hProcess, &iExitCode))
				iExitCode = 0; // reset to 0 if "get" reported to be unsuccessful
		}

		// Close process and thread handles. 
		CloseHandle( pi.hProcess );
		CloseHandle( pi.hThread );

		if (fCustomHandles) // clean-up the handles
		{
			DWORD dwRead;
			CHAR chBuf[BUFSIZE]; 

			// Close the write end of the pipe before reading from the read end of the pipe.

			if (CloseHandle(hStdErrorWr)) 
			{
				// Read output from the pipe and show user

				if((iExitCode != 0) && ReadFile( hStdErrorRd, chBuf, BUFSIZE, &dwRead, NULL) && (dwRead != 0))
				{
					chBuf[min(dwRead, BUFSIZE - 1)] = '\0'; // null-terminate the string
					String sError(chBuf);
					CloseHandle(hStdErrorRd); // close before we throw
					throw ErrorObject(sError);
				}
			}
			// Close read pipe handle
			CloseHandle(hStdErrorRd);
		}
		else if (iExitCode != 0)
		{
			String sError(TR("The external command has terminated with exit code %li.").c_str(), iExitCode);
			throw ErrorObject(sError);
		}
	}
	else 	// process creation failed .. clean up and show error if needed
	{
		if (fCustomHandles) // clean-up the handles
		{
			CloseHandle(hStdErrorWr);
			CloseHandle(hStdErrorRd);
		}
		if (fBlockWhileExecuting)
		{
			LPVOID lpMsgBuf;
			FormatMessage( 
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_SYSTEM | 
					FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					GetLastError(),
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
					(LPTSTR) &lpMsgBuf,
					0,
					NULL 
			);
			// Record the error message
			String sError ((char*)lpMsgBuf);
			// Free the buffer.
			LocalFree( lpMsgBuf );
			// Display the string.
			throw ErrorObject(String(TR("External command '%S' generated error:\n%S").c_str(), sCommandLine(), sError));
		}
	}
}
