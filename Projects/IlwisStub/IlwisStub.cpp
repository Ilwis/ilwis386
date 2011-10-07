// IlwisStub.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <string>
#include "IlwisStub.h"
#include "MainFrm.h"


using namespace std;


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


BEGIN_MESSAGE_MAP(CIlwisStubApp, CWinApp)
END_MESSAGE_MAP()


CIlwisStubApp::CIlwisStubApp()
{

}

CIlwisStubApp ilwisStub;


void CIlwisStubApp::copyFile(const string& rootIlwis, const string& rootUpdate,const string& f) {
	string restpath = f.substr(rootUpdate.size(), f.size() - rootUpdate.size());
	string newPath = rootIlwis + restpath;
	CopyFile(f.c_str(),newPath.c_str() , FALSE); 
}

void CIlwisStubApp::copyUpdates(const string& rootIlwis, const string& rootUpdate, const string& folder) {
	CFileFind finder;
	string pattern = folder + "*.*";
	BOOL fFound = finder.FindFile(CString(pattern.c_str()));
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			string file(finder.GetFilePath());
			copyFile(rootIlwis, rootUpdate,file);
			
		} else {
			string newfolder(finder.GetFilePath());
			bool dir = newfolder.find_last_of(".") == newfolder.size() - 1;
			dir = dir || newfolder.find_last_of("..") == newfolder.size() - 2;
			if ( !dir ) {
				copyUpdates(rootIlwis, rootUpdate, newfolder + "\\");
			}
		}
	}

}

BOOL CIlwisStubApp::InitInstance()
{
	CWinApp::InitInstance();

	//CMainFrame* pFrame = new CMainFrame;
	//if (!pFrame)
	//	return FALSE;
	//m_pMainWnd = pFrame;

	//pFrame->LoadFrame(IDR_MAINFRAME,
	//	WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
	//	NULL);

	////pFrame->ShowWindow(SW_SHOW);
	//pFrame->UpdateWindow();
	//char* szProgramm = "d:\\Projects\\drawers\\Debug\\Ilwis3\\ilwis.exe"; 
	
	char sTemp[MAX_PATH];
    ::GetModuleFileName(NULL, sTemp, MAX_PATH);
	string pathIlwis(sTemp);
	int index = pathIlwis.find_last_of("\\");
	pathIlwis = pathIlwis.substr(0, index + 1);

	char buffer[500];
	GetTempPath(500, buffer);
	string pathUpdate(buffer);
	pathUpdate += "IlwisUpdates\\";


	copyUpdates(pathIlwis,pathUpdate, pathUpdate);
	string programm("ilwisclient.exe"); 
	string cmd(m_lpCmdLine);
	//MessageBox(0,m_lpCmdLine,"test2",MB_OK);

	if ( (index = cmd.find("-server"))!=string::npos) {
		programm = "ilwisserver.exe";
		cmd.erase(index, 7);
	}
	if ( cmd != "" )
		programm += " " + cmd;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );
	char *prog = const_cast<char *>(programm.c_str());


    // Start the child process. 
    if( !CreateProcess( NULL,   // No module name (use command line)
		prog,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi )           // Pointer to PROCESS_INFORMATION structure
    ) 
    {
        TRACE( "CreateProcess failed (%d).\n", GetLastError() );
	} else {

		//WaitForSingleObject( pi.hProcess, INFINITE );

		//CloseHandle( pi.hProcess );
		//CloseHandle( pi.hThread );
	}

	return FALSE;
}




