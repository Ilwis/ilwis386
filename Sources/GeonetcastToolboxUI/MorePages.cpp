#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\FormElements\TreeSelector.h"
#include "Client\FormElements\fldlist.h"
#include <set>
#include "Engine\Base\DataObjects\XMLDocument.h"
#include  "GeonetCastToolboxUI\Page.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include  "GeonetCastToolboxUI\Pages.h"
#include  "GeonetCastToolboxUI\MorePages.h"
#include "Client\Base\Picture.h"
#include "Engine\Base\System\Engine.h"
#include "Client\FormElements\FieldPicture.h"
#include "engine\base\system\RegistrySettings.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Client\Editors\Utils\SBDestination.h"
#include "iniFile.h"
//#include "Engine\Base\DataObjects\RemoteObject.h"
#include "Client\ilwis.h"

ExportGeoTiff::ExportGeoTiff(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent){
}

void ExportGeoTiff::set() {
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	FieldMap *fldList = new FieldMap(this,"Input Map", &sInput, false);
	fldList->SetFieldWidth(EDIT_FIELD_SIZE);
	fdt = new FieldDataTypeCreate(this, "Output file", &sOutput, "", true);
	fdt->SetWidth(EDIT_FIELD_SIZE);
	PushButton *pbs = new PushButton(this, "...", (NotifyProc)&ExportGeoTiff::BrowseClick,this);
	pbs->psn->iPosX = fdt->psn->iPosX + fdt->psn->iMinWidth;
	pbs->psn->iMinWidth = 18;
	pbs->Align(fdt, AL_AFTER);
	pbs->SetHeight(fdt->psn->iHeight);
	pbs->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fdt, AL_UNDER);
	new PushButton(this, "Export", (NotifyProc)&ExportGeoTiff::Export,this);
}


int ExportGeoTiff::Export(Event *ev){
	StoreData();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	FileName fnOut(sOutput.sUnQuote());
	fnOut.sExt = ".tif";
	String pluginDir = "\"" + ilwDir + rootPath;
	String gdalDir = pluginDir + "\\GDAL\\bin"; 
	String command = "!" + gdalDir + "\\gdal_translate\".exe -of GTiff" ;
	command += " \"" + sInput.sUnQuote() + "\"";
	command += " \"" + fnOut.sFullPath() + "\"";

	IlwWinApp()->Execute(command);
	return 1;
}

int ExportGeoTiff::BrowseClick(Event*)
{
	if ( fdt == 0)
		return 1;

	CSBDestination sb(0, "Select output folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fdt->sName());
	String sPath = fnData.sPath();
	if (sPath.length() == 0)
		sPath = IlwWinApp()->sGetCurDir();
	if (sPath[sPath.length() - 1] == '\\')
		sPath = sPath.sLeft(sPath.length() - 1);  // remove trailing backslash
	sb.SetInitialSelection(sPath.c_str());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		fnData.Dir(String(sBrowse));
		if ( fnData.sFileExt().length() < 15)
			fdt->SetVal(fnData.sFullName(false));
		else
			fdt->SetVal("");
	}
	return 1;
}

ExportR::ExportR(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e){
}

void ExportR::set() {
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	FieldMap *fldList = new FieldMap(this,"Input Map", &sInput, false);
	fldList->SetFieldWidth(EDIT_FIELD_SIZE);
	fdt = new FieldDataTypeCreate(this, "Output file", &sOutput, "", true);
	fdt->SetWidth(EDIT_FIELD_SIZE);
	PushButton *pbs = new PushButton(this, "...", (NotifyProc)&ExportR::BrowseClick,this);
	pbs->psn->iPosX = fdt->psn->iPosX + fdt->psn->iMinWidth;
	pbs->psn->iMinWidth = 18;
	pbs->Align(fdt, AL_AFTER);
	pbs->SetHeight(fdt->psn->iHeight);
	pbs->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fdt, AL_UNDER);
	new PushButton(this, "Export", (NotifyProc)&ExportR::Export,this);
}


int ExportR::Export(Event *ev){
	StoreData();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	FileName fnOut(sOutput.sUnQuote());
	fnOut.sExt = ".rda";
	String pluginDir = "\"" + ilwDir + rootPath;
	String gdalDir = pluginDir + "\\GDAL\\bin"; 
	String command = "!" + gdalDir + "\\gdal_translate\".exe -of R" ;
	command += " \"" + sInput.sUnQuote() + "\"";
	command += " \"" + fnOut.sFullPath() + "\"";

	IlwWinApp()->Execute(command);
	return 1;
}

int ExportR::BrowseClick(Event*)
{
	if ( fdt == 0)
		return 1;

	CSBDestination sb(0, "Select output folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fdt->sName());
	String sPath = fnData.sPath();
	if (sPath.length() == 0)
		sPath = IlwWinApp()->sGetCurDir();
	if (sPath[sPath.length() - 1] == '\\')
		sPath = sPath.sLeft(sPath.length() - 1);  // remove trailing backslash
	sb.SetInitialSelection(sPath.c_str());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		fnData.Dir(String(sBrowse));
		if ( fnData.sFileExt().length() < 15)
			fdt->SetVal(fnData.sFullName(false));
		else
			fdt->SetVal("");
	}
	return 1;
}


ProductNavigator::ProductNavigator(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e) {
}

void ProductNavigator::set() {
	new PushButton(this, "Start GNC Product Navigator", (NotifyProc)&ProductNavigator::start,this);
}

int ProductNavigator::start(Event *ev){
	//setFolders(folderId.sHead(":"));
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	IniFile inifile;
	inifile.Open(pluginDir+ "\\" + iniFile);
	String sProductNavigator = inifile.GetKeyValue("GNC Product Navigator", "InputFolder");
	String path("%S\\indexer.html",sProductNavigator);
	int ret = (int) ::ShellExecute(frm()->wnd()->m_hWnd,"open", path.c_str() ,"","",SW_SHOW);
	if ( ret == ERROR_PATH_NOT_FOUND || ret == ERROR_FILE_NOT_FOUND)
		throw ErrorObject(TR("Path for Product Navigator wrong was not found"));
	return 1;
}

DataManagerPage::DataManagerPage(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e) {
}

void DataManagerPage::set() {
	new PushButton(this, "Start Data Manager", (NotifyProc)&DataManagerPage::start,this);
}

int DataManagerPage::start(Event *ev){
	String curDir = IlwWinApp()->Context()->sGetCurDir();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	String extDir = pluginDir + "\\DataManager"; 
	String cdCmd = String("cd '%S'", extDir); 
	IlwWinApp()->Execute(cdCmd);
	String extCmd = String("!java -jar DataManager.jar");
    IlwWinApp()->Execute(extCmd);
	IlwWinApp()->Execute("cd '" +  curDir + "'");
	return true;
}

MSGDataRetrieverPage::MSGDataRetrieverPage(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e) {
}

void MSGDataRetrieverPage::set() {
	new PushButton(this, "Start MSG Data Retriever", (NotifyProc)&MSGDataRetrieverPage::start,this);
}

int MSGDataRetrieverPage::start(Event *ev){
	String curDir = IlwWinApp()->Context()->sGetCurDir();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	String extDir = pluginDir + "\\MSGDataRetriever"; 
	String cdCmd = String("cd '%S'", extDir); 
	IlwWinApp()->Execute(cdCmd);
	String extCmd = String("!!MSGDataRetriever.exe");
    IlwWinApp()->Execute(extCmd);
	IlwWinApp()->Execute("cd '" +  curDir + "'");
	return 1;
}

CTime GetUTCTime(int nYear, int nMonth, int nDay, int nHour, int nMin, int nSec)
{
	CTime nowInUTC (CTime::GetCurrentTime());
	struct tm* nowInUTCs = new tm;
	nowInUTC.GetGmtTm(nowInUTCs);
	CTime nowInLocal (1900 + nowInUTCs->tm_year, 1 + nowInUTCs->tm_mon, nowInUTCs->tm_mday, nowInUTCs->tm_hour, nowInUTCs->tm_min, nowInUTCs->tm_sec);
	CTimeSpan delta (nowInUTC - nowInLocal);
	CTime localTime (nYear, nMonth, nDay, nHour, nMin, nSec);
	CTime utcTime (localTime + delta);
	delete nowInUTCs;
	return utcTime;
}

CTime GetMidnightBefore(CTime time)
{
	struct tm* timeInUTC = new tm;
	time.GetGmtTm(timeInUTC);
	time_t secondsSinceMidnight = (timeInUTC->tm_hour * 60 + timeInUTC->tm_min) * 60 + timeInUTC->tm_sec;
	CTimeSpan tsSecondsSinceMidnight (secondsSinceMidnight);
	CTime utcMidnightBefore(time - tsSecondsSinceMidnight);
	return utcMidnightBefore;
}

CTime GetMidnightAfter(CTime time) // one minute to midnight!!!
{
	struct tm* timeInUTC = new tm;
	time.GetGmtTm(timeInUTC);
	time_t secondsSinceMidnight = (timeInUTC->tm_hour * 60 + timeInUTC->tm_min) * 60 + timeInUTC->tm_sec;
	time_t secondsInADay = 24 * 60 * 60;
	time_t oneSecond = 1;
	CTimeSpan tsSecondsSinceMidnight (secondsSinceMidnight);
	CTimeSpan tsSecondsInADay (secondsInADay);
	CTimeSpan tsOneSecond (oneSecond);
	CTime utcMidnightAfter(((time - tsSecondsSinceMidnight) + tsSecondsInADay) - tsOneSecond);
	return utcMidnightAfter;
}

bool fMatchDay(CString sDay) {
	if (sDay.GetLength() == 2) {
		bool fRet = true;
		for (int i = 0; i < 2; ++i)
			fRet = fRet && (0 != isdigit(sDay[i]));
		fRet = fRet && (atoi(sDay) >= 1);
		fRet = fRet && (atoi(sDay) <= 31);
		return fRet;
	} else
		return false;
}

bool fMatchMonth(CString sMonth) {
	if (sMonth.GetLength() == 2) {
		bool fRet = true;
		for (int i = 0; i < 2; ++i)
			fRet = fRet && (0 != isdigit(sMonth[i]));
		fRet = fRet && (atoi(sMonth) >= 1);
		fRet = fRet && (atoi(sMonth) <= 12);
		return fRet;
	} else
		return false;
}

bool fMatchYear(CString sYear) {
	if (sYear.GetLength() == 4) {
		bool fRet = true;
		for (int i = 0; i < 4; ++i)
			fRet = fRet && (0 != isdigit(sYear[i]));
		return fRet;
	} else
		return false;
}

void MSGDataRetrieverPage::saveDSFile(CString sLocation, CString sSatellite, CTime tFirstDay, CTime tLastDay, bool fInSameFolder, bool fDailyIncrement, bool fTilToday) {
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	String extDir = pluginDir + "\\MSGDataRetriever\\";
	CString dataFolder (extDir.c_str());

	// postprocessing of auto-detect procedure
	tFirstDay = GetMidnightBefore(tFirstDay);
	if (fTilToday)
		tLastDay = CTime::GetCurrentTime();
	else
		tLastDay = GetMidnightAfter(tLastDay);

	__time32_t firstDay = tFirstDay.GetTime(); // This is here for compatibility with MFC6.0 CTime serialization, which is a 32-bits time_t
	__time32_t lastDay = tLastDay.GetTime();

	CString sDataSourceFileName ("Data Sources.msg1");
	TRY
	{
		CFile iniFile (dataFolder + sDataSourceFileName, CFile::modeCreate|CFile::modeWrite|CFile::shareDenyWrite);

		CArchive ar (&iniFile, CArchive::store);
		ar << CString("DataSource");
		ar << sLocation;
		ar << sSatellite;
		ar << firstDay; // compatibility with MFC6.0 CTime serialization, which is a 32-bits time_t
		ar << lastDay;
		ar << CString(fInSameFolder?"This Folder":"Date Stamped Folder");
		ar << CString(fTilToday?"Until current day":"Until date mentioned");
		ar << CString(fDailyIncrement?"Daily increment":"Fixed start date");
		ar.Close();

		iniFile.Close();
	}
	CATCH (CMemoryException, pEx)
	{
	}
	CATCH (CArchiveException, pEx)
	{
	}
	CATCH (CFileException, pEx)
	{
	}
	END_CATCH
}

void MSGDataRetrieverPage::updateFolders(const String& in, const String& out) {

	DataPage::updateFolders(in, out);

	CString sLocation (in.c_str());
	if (_access(sLocation, 0) == 0) {
		if ((sLocation.GetLength() > 0) && (sLocation[sLocation.GetLength() - 1] != '\\'))
			sLocation += "\\";

		const CString satelliteItems [] = {"MSG2", "MSG1-RSS"};
		CString sMinYear, sMinMonth, sMinDay, sMaxYear, sMaxMonth, sMaxDay;
		int iSatellite = -1;
		CFileFind finder;

		// Year

		BOOL bResult = finder.FindFile(sLocation + "*.*");
		while (bResult)	{
			 bResult = finder.FindNextFile();
			 if (finder.IsDirectory() && fMatchYear(finder.GetFileName())) {
				 if (sMinYear.GetLength() > 0) {
					 if (sMinYear.Compare(finder.GetFileName()) > 0)
						 sMinYear = finder.GetFileName();
				 } else
					 sMinYear = finder.GetFileName();
				 if (sMaxYear.GetLength() > 0) {
					 if (sMaxYear.Compare(finder.GetFileName()) < 0)
						 sMaxYear = finder.GetFileName();
				 } else
					 sMaxYear = finder.GetFileName();
			 }
		}
		finder.Close();

		if ((sMinYear.GetLength() > 0) && (sMaxYear.GetLength() > 0)) {
	
			// Min Month
			
			bResult = finder.FindFile(sLocation + sMinYear + "\\*.*");
			while (bResult) {
				 bResult = finder.FindNextFile();
				 if (finder.IsDirectory() && fMatchMonth(finder.GetFileName())) {
					 if (sMinMonth.GetLength() > 0) {
						 if (sMinMonth.Compare(finder.GetFileName()) > 0)
							 sMinMonth = finder.GetFileName();
					 } else
						 sMinMonth = finder.GetFileName();
				 }
			}			
			finder.Close();

			// Max Month

			bResult = finder.FindFile(sLocation + sMaxYear + "\\*.*");
			while (bResult) {
				 bResult = finder.FindNextFile();
				 if (finder.IsDirectory() && fMatchMonth(finder.GetFileName())) {
					 if (sMaxMonth.GetLength() > 0) {
						 if (sMaxMonth.Compare(finder.GetFileName()) < 0)
							 sMaxMonth = finder.GetFileName();
					 } else
						 sMaxMonth = finder.GetFileName();
				 }
			}
			finder.Close();

			if ((sMinMonth.GetLength() > 0) && (sMaxMonth.GetLength() > 0)) {

				// Min Day

				bResult = finder.FindFile(sLocation + sMinYear + "\\" + sMinMonth + "\\*.*");
				while (bResult) {
					 bResult = finder.FindNextFile();
					 if (finder.IsDirectory() && fMatchDay(finder.GetFileName())) {
						 if (sMinDay.GetLength() > 0) {
							 if (sMinDay.Compare(finder.GetFileName()) > 0)
								 sMinDay = finder.GetFileName();
						 } else
							 sMinDay = finder.GetFileName();
					 }
				}
				finder.Close();

				// Max Day

				bResult = finder.FindFile(sLocation + sMaxYear + "\\" + sMaxMonth + "\\*.*");
				while (bResult) {
					 bResult = finder.FindNextFile();
					 if (finder.IsDirectory() && fMatchDay(finder.GetFileName())) {
						 if (sMaxDay.GetLength() > 0) {
							 if (sMaxDay.Compare(finder.GetFileName()) < 0)
								 sMaxDay = finder.GetFileName();
						 } else
							 sMaxDay = finder.GetFileName();
					 }
				}
				finder.Close();

				if ((sMinDay.GetLength() > 0) && (sMaxDay.GetLength() > 0)) {
					// Satellite
					bResult = finder.FindFile(sLocation + sMaxYear + "\\" + sMaxMonth + "\\" + sMinDay + "\\H-000-MSG?__*.*"); // look at sMinDay in case there's no data today yet
					while (bResult) {
						 bResult = finder.FindNextFile();
						 if (!(finder.IsDots() || finder.IsDirectory())) {
								CString sRSS = finder.GetFileName().Mid(18, 3);
								iSatellite = (sRSS.Compare("RSS") == 0) ? 1 : 0;
								break;
						 }
					}
					finder.Close();
				}
			}
		}

		if (sMinYear.GetLength() > 0 && sMinMonth.GetLength() > 0 && sMinDay.GetLength() > 0 &&
			sMaxYear.GetLength() > 0 && sMaxMonth.GetLength() > 0 && sMaxDay.GetLength() > 0 && iSatellite >= 0)
		{
			CTime tFirstDay = GetUTCTime(atoi(sMinYear), atoi(sMinMonth), atoi(sMinDay), 0, 0, 0);
			CTime tLastDay = GetUTCTime(atoi(sMaxYear), atoi(sMaxMonth), atoi(sMaxDay), 0, 0, 0);
			CTimeSpan ts (CTime::GetCurrentTime() - tLastDay);
			bool fTilToday = (ts.GetTotalHours() < 24);
			bool fDailyIncrement = false;
			bool fInSameFolder = false;
			CString sSatellite = satelliteItems[iSatellite];
			saveDSFile(sLocation, sSatellite, tFirstDay, tLastDay, fInSameFolder, fDailyIncrement, fTilToday);
		}
		else {
			// check if data is exactly in the indicated folder

			sMinYear = ""; sMinMonth = ""; sMinDay = ""; sMaxYear = ""; sMaxMonth = ""; sMaxDay = "";

			bResult = finder.FindFile(sLocation + "H-000-MSG?__*.*");
			while (bResult) {
				 bResult = finder.FindNextFile();
				 if (!(finder.IsDots() || finder.IsDirectory())) {
						CString sFileName = finder.GetFileName();
						CString sTimeStamp = sFileName.Mid(46, 12);
						CString sYear (sTimeStamp.Left(4));
						CString sMonth (sTimeStamp.Mid(4, 2));
						CString sDay (sTimeStamp.Mid(6, 2));
						if (iSatellite < 0) {
							CString sRSS = finder.GetFileName().Mid(18, 3);
							iSatellite = (sRSS.Compare("RSS") == 0) ? 1 : 0;
						}
						
						if (sMinYear.GetLength() > 0) {
							if (sMinYear.Compare(sYear) > 0) {
								sMinYear = sYear;
								sMinMonth = "";
								sMinDay = "";
							}
						} else
							sMinYear = sYear;

						if (sMaxYear.GetLength() > 0) {
							if (sMaxYear.Compare(sYear) < 0) {
								sMaxYear = sYear;
								sMaxMonth = "";
								sMaxDay = "";
							}
						} else
							sMaxYear = sYear;

						if (sMinYear.Compare(sYear) == 0) {
							if (sMinMonth.GetLength() > 0) {
								if (sMinMonth.Compare(sMonth) > 0) {
									sMinMonth = sMonth;
									sMinDay = "";
								}
							}
							else
								sMinMonth = sMonth;

							if (sMinMonth.Compare(sMonth) == 0) {
								if (sMinDay.GetLength() > 0) {
									if (sMinDay.Compare(sDay) > 0)
										sMinDay = sDay;
								} else
									sMinDay = sDay;
							}
						}

						if (sMaxYear.Compare(sYear) == 0) {
							if (sMaxMonth.GetLength() > 0) {
								if (sMaxMonth.Compare(sMonth) < 0) {
									sMaxMonth = sMonth;
									sMaxDay = "";
								}
							} else
								sMaxMonth = sMonth;

							if (sMaxMonth.Compare(sMonth) == 0) {
								if (sMaxDay.GetLength() > 0) {
									if (sMaxDay.Compare(sDay) < 0)
										sMaxDay = sDay;
								} else
									sMaxDay = sDay;
							}
						}
				 }
			}
			finder.Close();

			if (sMinYear.GetLength() > 0 && sMinMonth.GetLength() > 0 && sMinDay.GetLength() > 0 &&
			sMaxYear.GetLength() > 0 && sMaxMonth.GetLength() > 0 && sMaxDay.GetLength() > 0 && iSatellite >= 0)
			{
				CTime tFirstDay = GetUTCTime(atoi(sMinYear), atoi(sMinMonth), atoi(sMinDay), 0, 0, 0);
				CTime tLastDay = GetUTCTime(atoi(sMaxYear), atoi(sMaxMonth), atoi(sMaxDay), 0, 0, 0);
				CTimeSpan ts (CTime::GetCurrentTime() - tLastDay);
				bool fTilToday = (ts.GetTotalHours() < 24);
				bool fDailyIncrement = false;
				bool fInSameFolder = true;
				CString sSatellite = satelliteItems[iSatellite];
				saveDSFile(sLocation, sSatellite, tFirstDay, tLastDay, fInSameFolder, fDailyIncrement, fTilToday);
			}
			else
				MessageBox(frm()->wnd()->m_hWnd, "Failed to auto-detect min and max date folders from '" + sLocation + "'", "Error", MB_OK);
		}
	}
	else
		MessageBox (frm()->wnd()->m_hWnd, "Path '" + sLocation + "' does not exist!", "Error", MB_OK);	
}

//--------------------------
Jason2Page::Jason2Page(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e){
}

void Jason2Page::set(){
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	resolution = 0;
	fdtIn = new FieldDataTypeCreate(this, "Input file(NetCDF)", &sInput, "", true);
	fdtIn->SetWidth(EDIT_FIELD_SIZE);
	PushButton *pbsIn = new PushButton(this, "...", (NotifyProc)&Jason2Page::BrowseClickIn,this);
	pbsIn->psn->iPosX = fdtIn->psn->iPosX + fdtIn->psn->iMinWidth;
	pbsIn->psn->iMinWidth = 18;
	pbsIn->Align(fdtIn, AL_AFTER);
	pbsIn->SetHeight(fdtIn->psn->iHeight);
	pbsIn->SetIndependentPos();
	RadioGroup *rg = new RadioGroup(this,"Resolution",&resolution,true);
	new RadioButton(rg,"3");
	new RadioButton(rg,"5");
	new RadioButton(rg,"7");
	new RadioButton(rg,"9");
	rg->SetIndependentPos();
	rg->Align(fdtIn,AL_UNDER);
	fdtOut = new FieldDataTypeCreate(this, "Output file", &sOutput, "", true);
	fdtOut->SetWidth(EDIT_FIELD_SIZE);
	PushButton *pbsOut = new PushButton(this, "...", (NotifyProc)&Jason2Page::BrowseClickOut,this);
	pbsOut->psn->iPosX = fdtOut->psn->iPosX + fdtOut->psn->iMinWidth;
	pbsOut->psn->iMinWidth = 18;
	pbsOut->Align(fdtOut, AL_AFTER);
	pbsOut->SetHeight(fdtOut->psn->iHeight);
	pbsOut->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fdtOut, AL_UNDER);
	new PushButton(this, "Import", (NotifyProc)&Jason2Page::import,this);
}

FileName Jason2Page::GetPath(FieldDataTypeCreate *fdt) {
	CSBDestination sb(0, "Select folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fdt->sName());
	String sPath = fnData.sPath();
	if (sPath.length() == 0)
		sPath = IlwWinApp()->sGetCurDir();
	if (sPath[sPath.length() - 1] == '\\')
		sPath = sPath.sLeft(sPath.length() - 1);  // remove trailing backslash
	sb.SetInitialSelection(sPath.c_str());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		fnData.Dir(String(sBrowse));
		return fnData;
	}
	return FileName();
}

int Jason2Page::BrowseClickIn(Event*)
{

	String sExt = "*.nc;*.ncf";

	String currentDir = getEngine()->sGetCurDir();
	char path[255];
	char title[20];
	strcpy(title,"Input file");
	strcpy(path,currentDir.c_str());
	CFileDialog dlg(TRUE,NULL,NULL,4|2,String("NetCDF(%S)|%S|All Files (*.*)|*.*||",sExt,sExt).c_str());
	dlg.m_ofn.lpstrInitialDir = path;
	dlg.m_ofn.lpstrTitle = title;
	if ( dlg.DoModal() == IDOK) {
		String file("%s",dlg.m_ofn.lpstrFile);
		FileName fnIn(file);
		fdtIn->SetVal(fnIn.sRelative());
		fdtOut->SetVal(FileName(fnIn.sFile).sShortName());
	}

	return 1;
}

int Jason2Page::BrowseClickOut(Event*)
{
	if ( fdtOut == 0)
		return 1;
	FileName fnData = GetPath(fdtOut);
	if ( fnData.sFileExt().length() < 15)
		fdtOut->SetVal(fnData.sFullName(false));
	else
		fdtOut->SetVal("");
	return 1;
}

int Jason2Page::import(Event *ev) {
	StoreData();
	FileName fn(sOutput.sUnQuote());
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	String gdalDir = pluginDir + "\\GDAL\\bin";
	String scriptPath = pluginDir + "\\" + script;
	FileName fnScript(scriptPath);
	String command = "run " + fnScript.sFullPathQuoted(); //import\\import_jason'.isl";
	command += " " + sInput.sUnQuote();
	command += " " + sOutput.sUnQuote();
	command += String(" %d",resolution*2 + 3 );
	command += " " + gdalDir;
	command += " " + ilwDir;

	IlwWinApp()->Execute(command);

	return 1;
}

AVHRRPage::AVHRRPage(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e) {
}

void AVHRRPage::set(){
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	fdtIn = new FieldDataTypeCreate(this, "Input file(Tiff)", &sInput, "", true);
	PushButton *pbsIn = new PushButton(this, "...", (NotifyProc)&AVHRRPage::BrowseClickIn,this);
	pbsIn->psn->iPosX = fdtIn->psn->iPosX + fdtIn->psn->iMinWidth;
	pbsIn->psn->iMinWidth = 18;
	pbsIn->Align(fdtIn, AL_AFTER);
	pbsIn->SetHeight(fdtIn->psn->iHeight);
	pbsIn->SetIndependentPos();
	fdtOut = new FieldDataTypeCreate(this, "Output file", &sOutput, "", true);
	fdtOut->Align(fdtIn, AL_UNDER);
	PushButton *pbsOut = new PushButton(this, "...", (NotifyProc)&AVHRRPage::BrowseClickOut,this);
	pbsOut->psn->iPosX = fdtOut->psn->iPosX + fdtOut->psn->iMinWidth;
	pbsOut->psn->iMinWidth = 18;
	pbsOut->Align(fdtOut, AL_AFTER);
	pbsOut->SetHeight(fdtOut->psn->iHeight);
	pbsOut->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fdtOut, AL_UNDER);
	new PushButton(this, "Import", (NotifyProc)&AVHRRPage::import,this);
}

FileName AVHRRPage::GetPath(FieldDataTypeCreate *fdt) {
	CSBDestination sb(0, "Select folder");
	sb.SetFlags(BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT);
	FileName fnData(fdt->sName());
	String sPath = fnData.sPath();
	if (sPath.length() == 0)
		sPath = IlwWinApp()->sGetCurDir();
	if (sPath[sPath.length() - 1] == '\\')
		sPath = sPath.sLeft(sPath.length() - 1);  // remove trailing backslash
	sb.SetInitialSelection(sPath.c_str());
	if (sb.SelectFolder())
	{
		CString sBrowse = sb.GetSelectedFolder();
		fnData.Dir(String(sBrowse));
		return fnData;
	}
	return FileName();
}

int AVHRRPage::BrowseClickIn(Event*)
{
	String sExt = "*.tiff;*.tif";

	String currentDir = getEngine()->sGetCurDir();
	char path[255];
	char title[20];
	strcpy(title,"Input file");
	strcpy(path,currentDir.c_str());
	CFileDialog dlg(TRUE,NULL,NULL,4|2,String("GeoTiff(%S)|%S|All Files (*.*)|*.*||",sExt,sExt).c_str());
	dlg.m_ofn.lpstrInitialDir = path;
	dlg.m_ofn.lpstrTitle = title;
	if ( dlg.DoModal() == IDOK) {
		String file("%s",dlg.m_ofn.lpstrFile);
		FileName fnIn(file);
		fdtIn->SetVal(fnIn.sRelative());
		fdtOut->SetVal(FileName(fnIn.sFile + ".mpr").sShortName());
	}

	return 1;
}

int AVHRRPage::BrowseClickOut(Event*)
{
	if ( fdtOut == 0)
		return 1;
	FileName fnData = GetPath(fdtOut);
	if ( fnData.sFileExt().length() < 15)
		fdtOut->SetVal(fnData.sFullName(false));
	else
		fdtOut->SetVal("");
	return 1;
}

int AVHRRPage::import(Event *e) {
	StoreData();
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	String gdalDir = pluginDir + "\\GDAL\\bin"; 
	String command = "!" + gdalDir + "\\gdal_translate.exe -of ILWIS";
	command += " \"" + sInput.sUnQuote() + "\"";
	command += " \"" + sOutput.sUnQuote() + "\"";

	IlwWinApp()->Execute(command);

	return 1;
}

//---------------------------------------- yyyymmddhhmm + orbit
TimePlusOrbit::TimePlusOrbit(GeonetCastFrm *frm,FormEntry *parent) : DataPage(frm,parent) {
}

void TimePlusOrbit::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	String s("Date (%S)", getFormat());
	FieldGroup *grp = new FieldGroup(this);
	FieldString *fs = new FieldString(grp,s,&time);
	fs->SetWidth(EDIT_FIELD_SIZE-42);
	FieldInt *fi = new FieldInt(grp,"Orbit", &orbit,ValueRangeInt(0,100000));
	fi->Align(fs, AL_AFTER,-87);
	fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
	fbIn->Align(fs, AL_UNDER);
	fbIn->SetWidth(EDIT_FIELD_SIZE);
	fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
	fbOut->SetWidth(EDIT_FIELD_SIZE);
	new FieldBlank(this);
	PushButton *pb = new PushButton(this,"Import",(NotifyProc)&TimePlusOrbit::import,this);
}

int TimePlusOrbit::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;	   
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	batCmd += String(" %d", orbit);
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;

}

//----------with the year/month/day/hour/minutes/sec format----------------------
ImportMomentNoInput::ImportMomentNoInput(GeonetCastFrm *frm,FormEntry *parent) : DataPage(frm,parent) {
}

void ImportMomentNoInput::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	String s("Date (%S)", getFormat());
	FieldString *fs = new FieldString(this,s,&time);
	fs->SetWidth(EDIT_FIELD_SIZE);
	fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
	fbOut->SetWidth(EDIT_FIELD_SIZE);
	new FieldBlank(this);
	PushButton *pb = new PushButton(this,"Import",(NotifyProc)&ImportMomentNoInput::import,this);
}

int ImportMomentNoInput::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;	   
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;

}

//-----------------------------------------
SpecialLocationsPage::SpecialLocationsPage(GeonetCastFrm *frm, FormEntry *e, map<String, String>& _specialLocations) : 
	GNCPage(frm, e), 
	specialLocations(_specialLocations)
{

}

void SpecialLocationsPage::set() {
	StaticText *txt = new StaticText(this,"External programs",true);

	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	IniFile inifile;
	inifile.Open(pluginDir+ "\\" + iniFile);
	locationIrfanview = inifile.GetKeyValue("Special locations","Irfanview");
	locationFire = inifile.GetKeyValue("Special locations","FireService");
	inifile.Close();

	FieldGroup *fg = new FieldGroup(this);
	fs = new FieldString(fg,"Irfan view",&locationIrfanview);
	PushButton *pbLoc = new PushButton(fg,"...",(NotifyProc)&SpecialLocationsPage::select, this);
	fs->SetWidth(EDIT_FIELD_SIZE);
	pbLoc->Align(fs, AL_AFTER);
	//fg->SetIndependentPos();
	new FieldBlank(this);
	//txt = new StaticText(this,"Internet locations",true);
	//txt->Align(fs,AL_UNDER);
	//FieldString *fs2 = new FieldString(this,"Fire service",&locationFire);
	//fs2->SetWidth(EDIT_FIELD_SIZE);
	new FieldBlank(this);

	PushButton *pb = new PushButton(this,"Save",(NotifyProc)&SpecialLocationsPage::save,this);
}

int SpecialLocationsPage::select(Event *) {
	String sExt = "*.exe";
	char path[MAX_PATH];
	char title[20];
	strcpy(title,"Programs");
	SHGetSpecialFolderPath(mainFrm->m_hWnd,path,CSIDL_PROGRAM_FILESX86,FALSE);
	CFileDialog dlg(TRUE,NULL,NULL,	OFN_FILEMUSTEXIST,String("ILWIS Object(%S)|%S",sExt,sExt).c_str());
	dlg.m_ofn.lpstrInitialDir = path;
	dlg.m_ofn.lpstrTitle = title;
	if ( dlg.DoModal() == IDOK) {
		String file("%s",dlg.m_ofn.lpstrFile);
		fs->SetVal(file);
	}
	return 1;
}
int SpecialLocationsPage::save(Event *ev) {
	StoreData();
	specialLocations["Irfan view"] = locationIrfanview;
	specialLocations["Fire service"] = locationFire;
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	IniFile inifile;
	inifile.Open(pluginDir+ "\\" + iniFile);
	inifile.SetKeyValue("Special locations","Irfanview",locationIrfanview);
	inifile.SetKeyValue("Fire service","Directory",locationFire);
	inifile.Close();
	return 1;

}
//--------------------------------------
URLPage::URLPage(GeonetCastFrm *f, FormEntry *e) : DataPage(f, e) {
}

void URLPage::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	String service = getFolderId();
	String s("%S", getFormat());
	fsIn = new FieldString(this,"URL", &dirIn);
	fsIn->SetWidth(EDIT_FIELD_SIZE);
	new FieldBlank(this);
	PushButton *pb = new PushButton(this,"Open",(NotifyProc)&URLPage::save,this);
}

int URLPage::save(Event *ev) {
	StoreData();
	::ShellExecute(mainFrm->m_hWnd,"open",dirIn.c_str(),0,0,SW_SHOW);

	return 1;
}

//-----------------------------------------
StationOutputPage::StationOutputPage(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void StationOutputPage::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	FieldGroup *fg = new FieldGroup(this);
	FieldInt *fs = new FieldInt(fg,"Year",&year,ValueRange(1900,2030));
	FieldInt *f2 = new FieldInt(fg,"Station",&station);
	f2->Align(fs, AL_AFTER,-100);
	f2->SetIndependentPos();
	fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
	fbOut->Align(fs, AL_UNDER);
	fbOut->SetWidth(EDIT_FIELD_SIZE);
	new PushButton(this,"Metadata",(NotifyProc)&StationOutputPage::metadata,this);
	new FieldBlank(this);
	new PushButton(this,"Import",(NotifyProc)&StationOutputPage::import,this);
}

int StationOutputPage::metadata(Event *ev) {
	String cmd("%S%S",getEngine()->getContext()->sIlwDir(),source); 
	::ShellExecute(mainFrm->m_hWnd,"open",cmd.c_str(),0,0,SW_SHOW);

	return 1;
}

void StationOutputPage::setSource(const String& s){
	source = s;
}

int StationOutputPage::import(Event *ev) {
	StoreData();
	time = String("%d",year);
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	batCmd = String("%S %06d", batCmd, station);
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;
}

//---------------------------------------------------------
StationSearchPage::StationSearchPage(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void StationSearchPage::set() {
	index = 0;
	//if ( searchMaps.size() == 0)
	//	loadSource();
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	String s("Date (%S)", getFormat());
	FieldString *fs = new FieldString(this,s,&time);
	fs->SetWidth(EDIT_FIELD_SIZE);
	fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
	fbIn->SetWidth(EDIT_FIELD_SIZE);
	fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
	PushButton *pb = new PushButton(this,"Load code file",(NotifyProc)&StationSearchPage::load,this);
	fbOut->SetWidth(EDIT_FIELD_SIZE);
	for(int i = 0; i < infos.size(); ++i) {
		values.push_back(infos[i].tag);
	}
	FieldGroup *fg = new FieldGroup(this);
	fsTerm = new FieldOneSelectString(fg,TR("Key"),&index, values);
	fsTerm->SetCallBack((NotifyProc)&StationSearchPage::setTerm, this);
	fsKey = new FieldString(fg,"",&key);
	fsKey->Align(fsTerm, AL_AFTER,-100);
	fsKey->SetCallBack((NotifyProc)&StationSearchPage::setFilter, this);
	StaticText *st = new StaticText(this,TR("Data"));
	st->Align(fsTerm, AL_UNDER);
	lister = new StringArrayLister(this,codes);
	lister->Align(st, AL_AFTER);
	lister->SetWidth(100);
	//fnt = IlwWinApp()->GetFont(IlwisWinApp::sfTABLE);
	//lister->lbox()->SetFont(fnt);
		
	new FieldBlank(this);
	new PushButton(this,"Import",(NotifyProc)&PageType1::import,this);

}

int StationSearchPage::load(Event *ev) {
	if ( codes.size() != 0)
		return 1;

	loadSource();
	//for(int i = 0; i < values.size(); ++i)
	//	fsTerm->AddVal(values[i]);

	lister->AddStrings(codes);


	return 1;
}
void StationSearchPage::loadSource() {
	String dataDir = getEngine()->getContext()->sIlwDir();
	dataDir += "Extensions\\Geonetcast-Toolbox\\";
	dataDir += dataSource;
	ifstream in(dataDir.c_str());
	searchMaps.resize(infos.size());
	while(in.is_open() && in.good() && !in.eof()) {
		String line;
		getline(in, line);
		Array<String> parts;
		Split(line,parts,"|");
		if ( parts.size() > 1) {
			for(int i = 0 ; i < infos.size(); ++i) {
				int key = infos[i].keyColumn - 1; // not zero based
				int data = infos[i].dataColumn - 1; // note zero based
				if ( key < parts.size() && data  < parts.size())
					searchMaps[i][parts[key]] = parts[data];
			}
			int key = infos[0].keyColumn - 1; // default
			int data = infos[0].dataColumn - 1; // defualt
			String code("%S   %S",parts[data],parts[key]);
			allcodes.push_back(code);
			codes.push_back(code);	
		}
	}
	in.close();
}

void StationSearchPage::addSearchInfo(SearchInfo& inf){
	infos.push_back(inf);
}

int StationSearchPage::setFilter(Event *ev) {
	fsKey->StoreData();
	if ( key == oldKey)
		return 1;
	codes.clear();
	for(int i = 0; i < allcodes.size(); ++i) {
		size_t pos = allcodes.at(i).find(key);
		if ( pos != string::npos || key == "")
			codes.push_back(allcodes.at(i));
	}
	oldKey = key;
	lister->AddStrings(codes);

	return 1;
}

int StationSearchPage::setTerm(Event *ev) {
	long n = fsTerm->iVal();
	if ( n >= 0)
		index = n;

	return 1;
}

void StationSearchPage::setSource(const String& s){
	dataSource = s;
}

//-------------------------------------------------------------------------------
FinderPage::FinderPage(GeonetCastFrm *frm, FormEntry *parent) : GNCPage(frm, parent), fos(0) {

}

void FinderPage::set() {
	new StaticText(this,"Find products",true);

	FieldGroup *fg = new FieldGroup(this);
	fos = new FieldOneSelectString(fg,"Key words",&choice, keywords);
	fos->SetComboWidth(100);
	fos->SetCallBack((NotifyProc)&FinderPage::handleKeySelection, this);
	PushButton *pb1 = new PushButton(fg,TR("And"),(NotifyProc)&FinderPage::addAnd, this);
	pb1->SetIndependentPos();
	PushButton *pb2 = new PushButton(fg,TR("Or"),(NotifyProc)&FinderPage::addOr, this);
	pb1->Align(fos,AL_AFTER,-170);
	pb1->SetHeight(fos->psn->iHeight * 1.15);
	pb2->Align(pb1, AL_AFTER,-10);
	pb2->SetHeight(pb1->psn->iHeight);
	FieldBlank *fb2 = new FieldBlank(this);
	fb2->Align(fos, AL_UNDER);
	FieldGroup *fg2 = new FieldGroup(this);
	fsm = new  FieldString(fg2,"Query", &query);
	fsm->SetWidth(100);
	PushButton *pb3 = new PushButton(fg2,TR("Find"),(NotifyProc)&FinderPage::find, this);
	PushButton *pb4 = new PushButton(fg2,TR("Clear"),(NotifyProc)&FinderPage::clear, this);
	pb3->Align(fsm,AL_AFTER,-170);
	pb3->SetIndependentPos();
	pb3->SetHeight(fsm->psn->iHeight);
	pb4->SetHeight(pb3->psn->iHeight);
	pb4->Align(pb3, AL_AFTER,-10);
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fsm,AL_UNDER);
	FieldGroup *grp = new FieldGroup(this);
	StaticText *txt = new StaticText(grp,TR("Results"));
	lst = new StringArrayLister(grp,resultString);
	lst->SetCallBack((NotifyProc)&FinderPage::handleSelection, this);
	lst->Align(txt, AL_AFTER);
	lst->SetHeight(150);
	lst->SetWidth(180);
	//grp->SetIndependentPos();

}
int FinderPage::clear(Event *ev) {
	query = "";
	choice = -1;
	words.clear();
	fsm->SetVal("");
	lst->Clear();
	return 1;
}

int FinderPage::handleKeySelection(Event *ev) {
	if ( fos) {
		fos->StoreData();
		fsm->StoreData();
		if ( query == "" && choice >= 0 && choice < keywords.size()) {
			fsm->SetVal(keywords[choice]);
			words.push_back("!" + keywords[choice]);
		}
	}
	return 1;
}

int FinderPage::handleSelection(Event *ev) {
	fsm->StoreData();
	int index = lst->iGetSingleSelection();
	if ( index >= 0 && index < pages.size()) {
		lst->SetSel(-1);
		DataPage *page = pages[index];
		if ( page) {
			Hide();
			mainFrm->SetPage(page->getBranch(),true);
		}
	}
	return 1;
}

void FinderPage::handleAnd(vector<DataPage *>& tempPages, const String& key) {
	const vector<DataPage *> keyPages = mainFrm->getKeyPages(key);
	vector<DataPage *> foundPages;
	for(int k=0; k < tempPages.size(); ++k) {
		bool found = false;
		for( int j = 0 ; j < keyPages.size(); ++j) {
			if ( tempPages[k] == keyPages[j]) {
				found = true;
				break;
			}
			if ( found) {
				foundPages.push_back(keyPages[j]);
			}
		}
	}
	tempPages.clear();
	for(int  k =0; k < tempPages.size(); ++k) {
		tempPages.push_back(tempPages[k]);
	}
}

void FinderPage::handleOr(vector<DataPage *>& tempPages, const String& key) {
	const vector<DataPage *> keyPages = mainFrm->getKeyPages(key);
	vector<DataPage *> foundPages;
	for( int j = 0 ; j < keyPages.size(); ++j) {
		foundPages.push_back(keyPages[j]);
	}
	for(int  k =0; k < tempPages.size(); ++k) {
		tempPages.push_back(tempPages[k]);
	}
}

int FinderPage::find(Event *ev)  {
	pages.clear();
	for(int i = 0; i < words.size(); ++i) {
		String key = words[0].substr(1);
		if ( pages.size() == 0) {
			const vector<DataPage *> keyPages = mainFrm->getKeyPages(key);
			for( int j = 0 ; j < keyPages.size(); ++j) {
				pages.push_back(keyPages[j]);
			}
		} else {
			if ( words[i][0] == '!') {
				handleAnd(pages, key);
			}
			if ( words[i][0] == '?') {
				handleAnd(pages, key);
			}
		}
	}
	resultString.clear();
	for(int i=0; i < pages.size(); ++i) {
		String branch = pages[i]->getBranch();
		branch = branch.sTail("#");
		replace(branch.begin(), branch.end(), '#','>');
		resultString.push_back(branch);
	}
	lst->Clear();
	lst->AddStrings(resultString);
	return 1;
}

int FinderPage::addAnd(Event *ev) {
	int index = fos->iVal();
	if ( index >= 0) {
		fsm->StoreData();
		String s = keywords[index];
		if ( query != "")
			query += " And " + s;
		else
			query = s;
		fsm->SetVal(query);
		words.push_back("!" + s);
	}
	return 1;
}

int FinderPage::addOr(Event *ev) {
	int index = fos->iVal();
	if ( index >= 0) {
		fsm->StoreData();
		String s = keywords[index];
		if ( query != "")
			query += " Or " + s;
		else
			query = s;
		fsm->SetVal(query);
		words.push_back("?" + s);

	}
	return 1;
}

void FinderPage::setKeys(const vector<string>& keys) {
	keywords.clear();
	keywords.resize(keys.size());
	copy(keys.begin(), keys.end(), keywords.begin());
	fos->resetContent(keywords);

}
