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
#ifndef ILWIS_H
#define ILWIS_H

#ifndef COMMAND_HANDLER_H
//#include "Engine\Base\System\commandhandler.h"
#endif

#if !defined(AFX_ILWISDOCUMENT_H__30AB4836_C0DB_11D2_B70E_00A0C9D5342F__INCLUDED_)
#include "Client\Base\IlwisDocument.h"
#endif

#ifndef COMCMDLINE_THREAD_H
#include "Engine\COMInterface\COMCmdLineThread.h"
#endif
#include "Client\Mapwindow\Drawers\Drawer_n.h"
#pragma warning( disable : 4786 )

class ActionList;
class ActionPairList;
class AssociationSelector;
class DirectoryDocument;
class Digitizer;
class IlwisDocTemplate;
class CatalogDocument;
class IlwisMDIDocTemplate;
class ForeignMultiDocTemplate;
class CommandHandlerUI;
class Action;
class URL;


enum CoordMessage {
					cmMOUSEMOVE = 1, 
					cmMOUSECLICK = 2,
					cmDIGIMOVE = 4,
					cmEDIT = 8,
					cmZOOMIN = 16,
					cmZOOMOUT = 32,
					cmSELECTAREA = 64,
					cmPAN = 128,
					cmINSERT = 256,
					cmDIGICLICK1 = 1024,
					cmDIGICLICK2 = 2048,
					cmDIGICLICK3 = 4096,
					cmDIGICLICK4 = 8192};

class CoordWithCoordSystem;
class MapCompositionDoc;

namespace ILWIS {
	class SVGElement;
	class SVGLoader;
	class BaseMapEditor;
	class BaseMapEditor;
}

typedef ILWIS::BaseMapEditor* (*BaseMapEditorCreate)(MapCompositionDoc *, const BaseMap& );

class IlwisWinApp: public CWinApp
{
public:
	friend class FormGeneralPreferences;
	
	enum sfFontType{sfFORM, sfTABLE, sfWindowMedium, sfGRAPH };
	
	enum DisplayOptionState{dosALL, dosRAS, dosRASIMG, dosRASVAL, dosRASSORT, dosRASBOOL, dosRASOTHER,
		dosSEG, dosSEGVAL, dosSEGSORT, dosSEGBOOL, dosSEGOTHER,
		dosPOL, dosPOLVAL, dosPOLSORT, dosPOLBOOL, dosPOLOTHER,
		dosPNT, dosPNTVAL, dosPNTSORT, dosPNTBOOL, dosPNTOTHER, dosEND };
	
	IlwisWinApp();
	~IlwisWinApp();
	LRESULT _export       Execute(const String& cCmd);
	LRESULT _export    ExecuteUI(const String& cCmd, CWnd *parent=0);
	vector<Action *>   CreateActions();


	CDocument          *OpenDirectory(LPCTSTR lpszFileName);
	void _export       DirChanged();
	CDocument          *OpenDocumentFile(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentFile(LPCTSTR lpszFileName, IlwisDocument::OpenType);
	CDocument          *OpenDocumentFile(const String& sFO, ParmList& pm);
	//CDocument          *OpenDocumentFile(const FileName& fnFO, ParmList& pm);
	CDocument		   *OpenDocumentFile(const URL& url, ParmList& pm);
	CDocument          *OpenADOTable(const FileName& fn, const ParmList& pm);
	CDocument _export  *OpenPixelInfo(LPCTSTR lpszFileName);
	CDocument          *OpenNewDirectory(LPCTSTR lpszFileName);
	CDocument _export  *OpenDocumentAsMap(LPCTSTR lpszFileName, bool fShow=true);
	CDocument _export  *OpenDocumentAsTable(LPCTSTR lpszFileName);
	CDocument _export  *OpenDocumentAsDomain(LPCTSTR lpszFileName);
	CDocument _export  *OpenDocumentAsRpr(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentScript(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentFunction(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentAsSlideShow(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentAsLayout(LPCTSTR lpszFileName);
	CDocument _export  *OpenDocumentAsGraph(LPCTSTR lpszFileName);
	CDocument _export  *OpenDocumentAsRoseDiagram(LPCTSTR lpszFileName);
	CDocument          *OpenDocumentFile(LPCTSTR lpszFileName, const String& sLayers);
	CDocument          *OpenDocumentAsSMCE(LPCTSTR lpszFileName);
	void _export       SendUpdateCoordMessages(int, CoordWithCoordSystem*);
	CatalogDocument    *dirDoc();
	void               SetCatalogDocument(CatalogDocument *doc);
	CImageList         ilSmall, ilLarge;
	int _export        iImage(const string& sExt);
	void _export       SetCurDir(const String& sDir);
	String _export     sGetCurDir();
	CFont _export      *GetFont(sfFontType);
	void _export       SetCommandLine(String s, bool fAddToHistory = false);
	void               EnableDigitizer(bool fConfigure = false);
	void               DisableDigitizer();
	FileName _export   fnGetSearchPath(int iIndex);
	void _export       RemoveSearchPath(const FileName& fn);
	void _export       AddSearchPath(const FileName& fn);
	void _export       ShowPopupMenu(CWnd*, CPoint, const FileName&, AssociationSelector** as=0, int iDfltOption=0);
	ActionList   *acl() { return actList; }
	const ActionPairList *apl() { return actPairList; }
	void               DeleteDirDoc(); // only to be called by MainWindow
	Digitizer          *dig() { return digitizer; }
	void               AddToCatalogHistory(const String& sDir);
	void               SetNextWindowRect(CPoint ptTopLeft, CSize sz);
	void _export       GetNextWindowRect(CPoint& ptTopLeft, CSize& sz);
	static bool        fHelpNumbers() { return fShowHelpNumbers; }
	IlwisDocTemplate * docTemplMapWindow() const { return dtMapWindow; }
	bool fUseOldStyleTranquilizers() { return fOldTranquilizers; }
	IlwisAppContext*    Context() { return ilwapp; }
	CommandHandlerUI			*getCommands() { return commandUI; }
	ILWIS::NewDrawer _export *getDrawer(const String& type, const String& subType, ILWIS::DrawerParameters *parms) ;
	ILWIS::BaseMapEditor _export *getMEditor(const String& type, const String& subtype, MapCompositionDoc *doc, BaseMap& bmp) ;
	ILWIS::NewDrawer _export *getDrawer(const String& type, ILWIS::PreparationParameters *pp=0, ILWIS::DrawerParameters *parms=0) ;
	void  addDrawer(const String& type, const String& subtype, DrawerCreate);
	void  addMEditor(const String& type, const String& subtype, BaseMapEditorCreate);
	ILWIS::SVGLoader *getSVGContainer() const { return svgContainer; }
	void getDocumentList(list<CDocument *>& opendocs);
	
	//{{AFX_MSG(IlwisWinApp)
	afx_msg void OnAbout();
	afx_msg void OnTest();
	afx_msg void OnCloseAll();
	afx_msg void OnMinimizeAll();
	afx_msg void OnRestoreAll();  
	afx_msg void OnAddDataWindow(WPARAM, LPARAM);
	afx_msg void OnRemoveDataWindow(WPARAM, LPARAM);
	afx_msg void OnHelpContents();
	afx_msg void OnHelpOperations();
	afx_msg void OnHelpMenuCmds();
	afx_msg void OnHelpSearch();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelpGlossary();
	afx_msg void OnHelpHowTo();
	afx_msg void OnHelpMapTabCalc();
	afx_msg void OnHelpIlwExpressions();
	afx_msg void OnHelpScriptSyntax();
	afx_msg void OnDigitizerSetup();
	afx_msg void OnUpdateDigitizerSetup(CCmdUI* pCmdUI);
	afx_msg void OnMapReferencing();
	afx_msg void OnUpdateMapReferencing(CCmdUI* pCmdUI);
	afx_msg void OnOperationCmd(UINT nID);
	afx_msg void OnHelpSebs();
	//}}AFX_MSG
	// members
	int iOpeningDoc; 
	bool fWindowsOpen() const 
	{ return !lhWindows.empty(); }
	bool fStartedAsCOMServer()  { return m_fEmbedded; }
	COMServerHandler *GetCOMServerHandler() { return chCOMServerHandler; }
	
private:
	BOOL InitApplication();
	BOOL InitInstance();
	int ExitInstance();
	void SendCommandLine(CWnd*) const;
	void RegisterClasses();
	void RegisterDocTemplates();
	void InitImageLists();
	void InitImage(string sExt, const char* sIcon, int iImage);
	void LoadSettings(const String& sSettingName="DefaultSettings");
	void SaveSettings(const String& sSettingName="DefaultSettings");
	void OpenLogoWindow();
	void CloseLogoWindow();
	bool fCheckBeta(const CTime & tExpiryDate);
	void SetTranquilizerStyle(bool fOld);
	void SplitUrl(const URL& url, ParmList& parms);
	
	map<string,int> mapExt;  // use iImage() !
	ActionList* actList;
	ActionPairList* actPairList;
	CatalogDocument* dirdoc;
	Digitizer  *digitizer;
	deque< CRect > dquNextDataWindowPos;
	ParmList plInitParms;
	String m_sCommand;
	bool fRemoveUserRegistry; // this var will be set if the restore defaults from the preferences is used.
	bool m_fEmbedded;
	bool m_fHideLogo;
	IlwisAppContext    *ilwapp;
	CommandHandlerUI *commandUI;
	
	CSemaphore smILWIS;
	CSemaphore smMainWindow;
	ILWISSingleLock lockILWIS;
	ILWISSingleLock lockMWCreated;
	map<sfFontType, CFont*> StandardFonts;
	list<HWND> lhWindows;
	map<String, DrawerCreate> drawers;
	map<String,BaseMapEditorCreate> meditors;
	IlwisDocTemplate *dtPixelInfo;
	CMultiDocTemplate *dtDirectory;
	IlwisDocTemplate *dtMapWindow;
	IlwisDocTemplate *dtTableWindow;
	IlwisDocTemplate *dtRprWindow;
	IlwisDocTemplate *dtDomWindow, *dtDomUnique;
	IlwisDocTemplate *dtScriptWindow;
	IlwisDocTemplate *dtFunctionWindow;
	IlwisDocTemplate *dtLayout;
	IlwisDocTemplate *dtGraph, *dtRoseDiagram;
	IlwisDocTemplate *dtSMCE;
	IlwisDocTemplate *dtAnimation;
	IlwisMDIDocTemplate *dtObjectCollection;
	IlwisMDIDocTemplate *dtMapList;
	ForeignMultiDocTemplate *dtForeign;
	COleTemplateServer otsMapComposition;
	HWND hWndLogo;
	static bool fShowHelpNumbers;
	ILWIS::SVGLoader		*svgContainer;
	
	CWinThread       *thrDdeServer;
	COMServerHandler *chCOMServerHandler;
	bool fOldTranquilizers;
	
	DECLARE_MESSAGE_MAP()
};

inline IlwisWinApp* IlwWinApp() { return dynamic_cast<IlwisWinApp*>(AfxGetApp()); }
#define ILWISAPP IlwWinApp()

#define ID_CAT_LARGE   15001
#define ID_CAT_SMALL   15002
#define ID_CAT_LIST    15003
#define ID_CAT_DETAILS 15004

#endif // ILWIS_H
