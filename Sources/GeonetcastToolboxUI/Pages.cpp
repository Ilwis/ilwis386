#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\FormElements\TreeSelector.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\objlist.h"
#include <set>
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include  "GeonetCastToolboxUI\Page.h"
#include  "GeonetCastToolboxUI\Pages.h"
#include "Engine\Base\DataObjects\RemoteXMLObject.h"
#include "Client\Base\Picture.h"
#include "Engine\Base\System\Engine.h"
#include "Client\FormElements\FieldPicture.h"
#include "engine\base\system\RegistrySettings.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Client\Editors\Utils\SBDestination.h"
#include "iniFile.h"
//#include "Engine\Base\DataObjects\RemoteObject.h"
#include "Client\ilwis.h"
//----------with the year/month/day/hour/minutes/sec format----------------------
PageType1::PageType1(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void PageType1::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	FormEntry *fe;
	if ( mainFrm->fRemote()) {
		FieldGroup *fg = new FieldGroup(this);
		foString = new FieldOneSelectString(fg,TR("Time"),&choice, timeValues);
		PushButton *pb = new PushButton(fg,"Q",(NotifyProc)&PageType1::queryServer,this);
		pb->SetHeight(foString->psn->iHeight);
		pb->Align(foString, AL_AFTER);
		pb->SetIndependentPos();
		fe= fg;
	} else {
		String s("Date (%S)", getFormat());
		FieldString *fs = new FieldString(this,s,&time);
		fs->SetWidth(EDIT_FIELD_SIZE);
		if ( noinput == false) {
			fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
			fbIn->SetWidth(EDIT_FIELD_SIZE);
			fe = fbIn;
		} else
			fe = fs;
	}
	if ( nooutput == false) {
		fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
		fbOut->Align(fe, AL_UNDER);
	}

	new FieldBlank(this);
	PushButton *pb = new PushButton(this,buttonText,(NotifyProc)&PageType1::import,this);
}

int PageType1::queryServer(Event *) {
	URL url = mainFrm->getServer();
	String command("%S?request=catalog&service=gnc&version=1.0&product=%S", url.sVal(), getId());
	RemoteObject xmlObj(command);
	String txt = xmlObj.toString();
	if ( txt != "") {
		ILWIS::XMLDocument doc(txt);
		timeValues.clear();
		doc.executeXPathExpression("//GNC_Catalog/File/ImageTime/text()",timeValues);
		sort(timeValues.begin(), timeValues.end());
        timeValues.erase(std::unique(timeValues.begin(), timeValues.end()), timeValues.end());
		foString->resetContent(timeValues);
		foString->SetVal(0);
	}
	return 1;
}


int PageType1::import(Event *ev) {
	StoreData();
	if ( mainFrm->fRemote()) {
		if ( choice >= 0) {
			String time = timeValues[choice];
			URL url = mainFrm->getServer();
			String command("%S?request=thredds&service=gnc&version=1.0&product=%S&time=%S", url.sVal(), getId(),time);
			RemoteObject xmlObj(command);
			String txt = xmlObj.toString();
		}
	}
	else {
		String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;	   
		InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
		String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
		String irfanView = mainFrm->getSpecialLocation("Irfan view");
		if ( irfanView == "")
			irfanView = "C:\\program files(x86)\\IrfanView\\i_view32.exe";

		batCmd += " " +irfanView.sQuote();
		IlwWinApp()->Execute("cd " + dirOut);
		IlwWinApp()->Execute(batCmd);
	}
	return 1;

}
//-----------------------------------
PageType2::PageType2(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void PageType2::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	String s("Julian Day (%S)", getFormat());
	FieldGroup *fg = new FieldGroup(this);
	FieldString *fs = new FieldString(fg,s,&time);
	fs->SetWidth(60);
	FieldInt *f2 = new FieldInt(fg,"",&days);
	f2->Align(fs, AL_AFTER, -80);
	f2->SetIndependentPos();
	if ( noinput == false) {
		fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
		fbIn->Align(fg,AL_UNDER);
		fbIn->SetWidth(EDIT_FIELD_SIZE);
	}
	if ( nooutput == false) {
		fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
	}
	new FieldBlank(this);
	new PushButton(this,buttonText,(NotifyProc)&PageType2::import,this);


}

int PageType2::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	batCmd = String("%S %03d", batCmd, days);
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;
}
//-------------------------------------------
PageType3::PageType3(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void PageType3::set() {
	setFolders(folderId);
	new StaticText(this,getName(),true);
	if ( getComment() != "") {
		StaticText *txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	if ( noinput == false) {
		fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
		fbIn->SetWidth(EDIT_FIELD_SIZE);
	}
	if ( nooutput == false) {
		fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
	}
	new FieldBlank(this);
	new PushButton(this,buttonText,(NotifyProc)&PageType3::import,this);


}

int PageType3::import(Event *ev) {
 	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	gdalDir = pluginDir + "\\MSGDataRetriever"; // overrule because we need a different one

	IniFile inifile;
	inifile.Open(pluginDir + "\\" + iniFile);
	String sIrfanViewDir = inifile.GetKeyValue("Special locations", "Irfanview");
	if ( sIrfanViewDir == "")
		sIrfanViewDir = "C:\\program files(x86)\\IrfanView\\i_view32.exe";
	String scriptPath = pluginDir + "\\\\" + script;
	IlwWinApp()->Execute("cd " + dirOut);
	FileName fnScript(scriptPath);
	String batCmd = String("!%S %S %S %S %S %S \"%S\"", 
					fnScript.sFullPathQuoted(),
					inputDrive,
					outputDrive,
					outputPath,
					ilwDir,
					utilDir,
					sIrfanViewDir);
	IlwWinApp()->Execute(batCmd);
	return 1;
}
//-------------------------------------------
PageType4::PageType4(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void PageType4::set() {
	setFolders(folderId);
	StaticText* txt = new StaticText(this,getName(),true);
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	FormEntry * alignField = txt;
	if (useRegion && regionMap) {
		cbRegion = new CheckBox(this, TR("Region"), useRegion);
		cbRegion->SetCallBack((NotifyProc)&DataPage::RegionChanged);
		cbRegion->SetIndependentPos();
		fmRegion = new FieldPolygonMap(cbRegion, "", regionMap);//, new MapListerDomainType(dmVALUE, false));
		fmRegion->SetCallBack((NotifyProc)&DataPage::RegionChanged);
		alignField = cbRegion;
	}
	if ( nooutput == false) {
		fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
		fbOut->SetIndependentPos();
		fbOut->Align(alignField, AL_UNDER);
	}
	new FieldBlank(this);
	new PushButton(this,buttonText,(NotifyProc)&PageType4::import,this);
}
int PageType4::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;	   
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	inputDrive = "x";
	inputPath = "x";
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String irfanView = mainFrm->getSpecialLocation("Irfan view");
	if ( irfanView == "")
		irfanView = "C:\\program files(x86)\\IrfanView\\i_view32.exe";

	batCmd += " " +irfanView.sQuote();
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	
	return 1;
}
//-------------------------------------------
PageType5::PageType5(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void PageType5::set() {
	setFolders(folderId);
	new StaticText(this,getName(),true);
	if ( getComment() != "") {
		StaticText *txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	String s("Date (%S)", getFormat());
	FieldString *fs = new FieldString(this,s,&time);
	fs->SetWidth(EDIT_FIELD_SIZE);
	if ( noinput == false) {
		fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
		fbIn->SetWidth(EDIT_FIELD_SIZE);
	}
	if ( nooutput == false) {
		fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
	}
	new FieldBlank(this);
	new PushButton(this,buttonText,(NotifyProc)&PageType5::import,this);
}

int PageType5::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;
}
//-----------------------------------
PicturePage::PicturePage(GeonetCastFrm *frm, FormEntry *e) : GNCPage(frm,e){

}

void PicturePage::setPicture(FileName fn,int width, int height, const bool resampleBicubic) {
	FieldPicture *fp = new FieldPicture(this, NULL);
	fp->SetWidth(width);
	fp->SetHeight(height);
	fp->SetBevelStyle(FormEntry::bsRAISED);
	fp->Load(fn, resampleBicubic);
}

//---------------------------------------
DirectoryPage::DirectoryPage(GeonetCastFrm *frm, FormEntry *e) : GNCPage(frm,e){
	this->SetBevelStyle(FormEntry::bsLOWERED);
	hasInput = true;
	hasOutput = true;
}

void DirectoryPage::setInput(String dir){
	inputDir = dir;
}

void DirectoryPage::setOutput(String dir){
	outputDir = dir;
}

String DirectoryPage::makePath(String p) {
	folderId = p;
	String inputoutput = p.sTail(":");
	hasOutput = (inputoutput.iPos(String("nooutput")) < 0);
	hasInput = (inputoutput.iPos(String("noinput")) < 0);

	if (inputoutput != "")
		path=p.sHead(":");
	else
		path=p;
	return path;
}

void DirectoryPage::setPages(vector<GeonetCastFrm::Page>* p) {
	pages = p;
}

void DirectoryPage::set() {
	new StaticText(this,path,true);
	if ( hasInput) {
		fb1 = new FieldBrowseDir(this,"Input location","",&inputDir);
		fb1->SetWidth(EDIT_FIELD_SIZE);
	}
	if ( hasOutput) {
		fb2 = new FieldBrowseDir(this,"Output location","",&outputDir);
		fb2->SetWidth(EDIT_FIELD_SIZE);
	}
	new FieldBlank(this);
	new PushButton(this,"Save",(NotifyProc)&DirectoryPage::save,this);
}

int DirectoryPage::save(Event *) {
	if ( hasInput)
		fb1->StoreData();
	if ( hasOutput)
		fb2->StoreData();
 	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	IniFile inifile;
	inifile.Open(pluginDir+ "\\" + iniFile);
	if ( hasInput)
		inifile.SetKeyValue(path,"InputFolder",inputDir);
	if ( hasOutput)
		inifile.SetKeyValue(path,"OutputFolder",outputDir);
	inifile.Close();
	for(int i=0; pages && i < pages->size(); ++i) {
		DataPage *page = dynamic_cast<DataPage *>(pages->at(i).page);
		if ( page) {
			if (folderId == page->getFolderId()) 
				page->updateFolders(inputDir,outputDir);
		}
	}

   return 1;

}
//----------Angle map----------------------------------------
AnglePage::AnglePage(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void AnglePage::setYearDayMonthHour(int yr, int mth, int dy,double hr) {
	year = yr;
	month = mth;
	day = dy;
	hour = hr;
}

void AnglePage::setOutput(String dir){
	dirOut= dir;
}

void AnglePage::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	ValueRange vr1(2000, 3000, 0);
	FieldBlank *fb = new FieldBlank(this, 0);
	FieldInt *fldYear = new FieldInt(this, "Year", &year, vr1);
	ValueRange vr2(1, 12, 0);
	FieldInt *fldMonth = new FieldInt(this, "Month", &month, vr2);
	ValueRange vr3(1, 31, 0);
	FieldInt *fldDay = new FieldInt(this, "Day", &day, vr3);
	ValueRange vr4(0, 24, 1e-2);
	FieldReal *fldTime = new FieldReal(this, "Time of day(UTM)", &hour, vr4);
	fbOut = new FieldBrowseDir(this, "Output Directory", "", &dirOut);
	fbOut->SetWidth(EDIT_FIELD_SIZE);
	new FieldBlank(this);
	new PushButton(this,"Create",(NotifyProc)&AnglePage::make,this);
}

int AnglePage::make(Event *ev) {
	StoreData();
	String strmonth;
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String scriptDir = script.substr(0,script.find_last_of("\\"));
	scriptDir = pluginDir + "\\" + scriptDir + "\\";
	if (month < 10)
		strmonth = String("0%li",month);
	else
		strmonth = String("%li",month);
	String sCurDir = getEngine()->sGetCurDir();
	getEngine()->Execute(String("cd %S", scriptDir));
	String batCmd = String("!generateangles.bat %li %S %li %g", year,strmonth,day,hour); 
	IlwWinApp()->Execute(batCmd);
	getEngine()->Execute(String("cd %S" ,sCurDir));

	String fromMsgName, fromSunName, scriptName;
	String toMsg_, toSun_,toMsg,toSun;
	if ( format=="zenith"){
		fromMsgName = String("satzen_%li_%S_%li_%g",year,strmonth,day,hour);
		fromSunName = String("sunzen_%li_%S_%li_%g",year,strmonth,day,hour);
		toMsg_ = "msgzen_";
		toSun_ = "solzen_";
		toMsg = "msgzen";
		toSun = "solzen";
	}
	else{
		fromMsgName = String("sataz_%li_%S_%li_%g",year,strmonth,day,hour);
		fromSunName = String("sunaz_%li_%S_%li_%g",year,strmonth,day,hour);
		toMsg_ = "msgaz_";
		toSun_ = "solaz_";
		toMsg = "msgaz";
		toSun = "solaz";
	}
	
	IlwWinApp()->Execute("'" + dirOut+"\\"+toMsg_+"':=map('" + scriptDir + fromMsgName + "',genras,Convert,378,0,Real,4,SwapBytes);");
	IlwWinApp()->Execute("setgrf '" +  dirOut+"\\"+toMsg_+"'.mpr '" +  scriptDir+"angle';");
	IlwWinApp()->Execute("'" + dirOut+"\\"+toMsg+"'{dom=value.dom;vr=-360:360:0.0001}:='" + dirOut+"\\"+toMsg_ +"';");
	IlwWinApp()->Execute("calc '" + dirOut+"\\"+ toMsg+"'.mpr;");
		
	IlwWinApp()->Execute("'" + dirOut+"\\"+toSun_+"':=map('" + scriptDir + fromSunName + "',genras,Convert,378,0,Real,4,SwapBytes);");
	IlwWinApp()->Execute("setgrf '" + dirOut+"\\"+toSun_+"'.mpr '" +  scriptDir+"angle';");
	IlwWinApp()->Execute("'" + dirOut+"\\"+toSun+"'{dom=value.dom;vr=-360:360:0.0001}:='" + dirOut+"\\"+toSun_+"';");
	IlwWinApp()->Execute("calc '" + dirOut+"\\"+ toSun+"'.mpr;");
	
	//Change to the current directory and generate angle maps
	FileName fnOut = FileName(dirOut);
	IlwWinApp()->Execute(outputDrive);
	IlwWinApp()->Execute("cd '" +  dirOut + "'");
	IlwWinApp()->Execute("copy '" +  scriptDir+"msg_angle.grf' '"+ dirOut + "';");
	IlwWinApp()->Execute("copy '" +  scriptDir+"msg_angle.csy' '"+ dirOut + "';");
	IlwWinApp()->Execute("copy '" +  scriptDir+"illum_cond.dom' '"+ dirOut + "';");
	IlwWinApp()->Execute("copy '" +  scriptDir+"illum_cond.rpr' '"+ dirOut + "';");
	IlwWinApp()->Execute("run '" + pluginDir + "\\" + script);
	IlwWinApp()->Execute("del '" + dirOut+"\\"+toMsg_+"'.mpr -force;");
	IlwWinApp()->Execute("del '" + dirOut+"\\"+ toSun_+"'.mpr -force;");
	return 1;
}

//---------------------------------------------------
StopAnimationPage::StopAnimationPage(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void StopAnimationPage::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	new PushButton(this,"Stop",(NotifyProc)&StopAnimationPage::Stop,this);
}

int StopAnimationPage::Stop(Event *ev) {
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	//String script = pluginDir + "\\\\RealTimeMSGVisualization\\StopWindow"; 
	String scriptPath = pluginDir + "\\" + script;
	FileName fnScript(scriptPath);
	IlwWinApp()->Execute("run " + fnScript.sFullPathQuoted() + " " + ilwDir);
	return 1;
}
//-----------------------------------------------------
MultiLayerHDF4::MultiLayerHDF4(GeonetCastFrm *frm, FormEntry *e) : DataPage(frm,e) {
}

void MultiLayerHDF4::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}
	type = 0;
	FieldMapList *fldList = new FieldMapList(this,"Input MapList", &sInput, false);
	RadioGroup *rg = new RadioGroup(this, "Data type", &type);
	new RadioButton(rg, "Byte(8 bits)");
	new RadioButton(rg, "Integer(16 bits)");
	new RadioButton(rg, "Unsigned integer(16 bits)");
	new RadioButton(rg, "Integer(32 bits)");
	new RadioButton(rg, "Unsigned integer(32 bits)");
	new RadioButton(rg, "Float(32 bits)");
	new RadioButton(rg, "Float(64 bits)");
 
	FieldString *fldNrBands = new FieldString(this,"Bands sequence(e.g. 3,2,1)", &bands);
	fdt = new FieldDataTypeCreate(this, "Output file", &sOutput, "", true);
	PushButton *pbs = new PushButton(this, "...", (NotifyProc)&MultiLayerHDF4::BrowseClick,this);
	pbs->psn->iPosX = fdt->psn->iPosX + fdt->psn->iMinWidth;
	pbs->psn->iMinWidth = 18;
	pbs->Align(fdt, AL_AFTER);
	pbs->SetHeight(fdt->psn->iHeight);
	pbs->SetIndependentPos();
	FieldBlank *fb = new FieldBlank(this);
	fb->Align(fdt, AL_UNDER);
	new PushButton(this, "Export", (NotifyProc)&MultiLayerHDF4::Export,this);
}

int MultiLayerHDF4::BrowseClick(Event*)
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

int MultiLayerHDF4::Export(Event *ev) {
	StoreData();
	Array<String> parts;
	Split(bands,parts,",");
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String sType = "Byte";
	switch(type) {
		case 1:
			sType = "Int16"; break;
		case 2:
			sType = "UInt32"; break;
		case 3:
			sType = "Int32"; break;
		case 4:
			sType = "UInt32"; break;
		case 5:
			sType = "Float32"; break;
		case 6:
			sType = "Float64"; break;
 	}
	String pluginDir = "\"" + ilwDir + rootPath;
	String gdalDir = pluginDir + "\\GDAL"; 
	String command = "!" + gdalDir + "\\gdal_translate\".exe -of HDF4Image -ot ";
	command += sType;
	for(int i = 0; i < parts.size(); ++i) {
		if ( parts[i].iVal() != iUNDEF)
			command += String(" -b %S",parts[i]);
	}
	FileName fnOut(sOutput,".hdf");
	command += " \"" + sInput.sUnQuote() + "\"";
	command += " \"" + fnOut.sFullPath() + "\"";

	IlwWinApp()->Execute(command);
	return 1;
}
//-----------------------------------------------
LLCornerPage::LLCornerPage(GeonetCastFrm *frm, FormEntry *parent) : DataPage(frm,parent) {
}

void LLCornerPage::set() {
	setFolders(folderId);
	StaticText *txt = new StaticText(this,getName(),true);
	txt->SetIndependentPos();
	if ( getComment() != "") {
		txt = new StaticText(this, getComment());
		txt->SetIndependentPos();
	}

	String s("Lower Left Coordinate (%S)", getFormat());
	//S10E000
	String f1, f2;
	for(int i =0; i < format.size(); ++i) {
		char c = format[i];
		if ( c >= 48 && c <= 57)
			continue;
		if ( f1 == "")
			f1 += c;
		else
			f2 += c;
	}
	s1 = setString(f1);
	s2 = setString(f2);
	StaticText *st2 = new StaticText(this,s);
	FieldGroup *fg = new FieldGroup(this);
	fg->Align(st2, AL_AFTER);
	FieldString *fi1 = new FieldString(fg,s1, &first);
	FieldString *fi2 = new FieldString(fg,s2, &second);
	fi2->Align(fi1, AL_AFTER);
	fg->SetIndependentPos();
	FieldBlank *fb2 = new FieldBlank(this,0.2);
	fb2->Align(st2, AL_UNDER);
	if ( noinput == false ) {
		FieldBrowseDir *fbIn = new FieldBrowseDir(this,"Input directory","",&dirIn);
		fbIn->SetWidth(EDIT_FIELD_SIZE);
	}
	if ( nooutput) {
		FieldBrowseDir *fbOut = new FieldBrowseDir(this,"Output directory","",&dirOut);
		fbOut->SetWidth(EDIT_FIELD_SIZE);
	}
	new FieldBlank(this);
	PushButton *pb = new PushButton(this,buttonText,(NotifyProc)&LLCornerPage::import,this);
}


String LLCornerPage::setString(const String& f1) {
	if ( f1 == "S")
		return TR("Lat");
	if ( f1 == "N")
		return TR("Lat");
	if ( f1 == "E")
		return TR("Long");
	if ( f1 == "W")
		return TR("Long");
	return "";

}

String LLCornerPage::createString(const String& part, const String& value) {

	if (TR("Lat") == part ) {
		if ( value[0] == 'S' || value[0] == 'N') {
			String rest = value.substr(1,value.size() - 1);
			int v = rest.iVal();
			if ( v != iUNDEF)
				return String("%c%02d",value[0], v);
		}
		throw ErrorObject(TR("Illegal input"));
		
	}
	if (TR("Long") == part ) {
		if ( value[0] == 'E' || value[0] == 'W') {
			String rest = value.substr(1,value.size() - 1);
			int v = rest.iVal();
			if ( v != iUNDEF)
				return String("%c%03d", value[0], v);
		}
		throw ErrorObject(TR("Illegal input"));
	
	}
	return "";
}

int LLCornerPage::import(Event *ev) {
	StoreData();
	String ilwDir,pluginDir,gdalDir, utilDir,inputDrive,inputPath,outputDrive,outputPath;	   
	InitImport(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String batCmd = createCommand(ilwDir,pluginDir,gdalDir,utilDir,inputDrive,inputPath, outputDrive, outputPath);
	String irfanView = mainFrm->getSpecialLocation("Irfan view");
	if ( irfanView == "")
		irfanView = "C:\\program files(x86)\\IrfanView\\i_view32.exe";

	batCmd += " " +irfanView.sQuote();
	IlwWinApp()->Execute("cd " + dirOut);
	IlwWinApp()->Execute(batCmd);
	return 1;

}

String LLCornerPage::createCommand(const String& ilwDir, const String& pluginDir, const String& gdalDir, const String& utilDir,const String& inputDrive, const String& inputPath, const String& outputDrive, const String& outputPath){
	String scriptPath = pluginDir + "\\" + script; 
	String ilwdir = ilwDir.substr(0, ilwDir.length() - 1);
	FileName fnScript(scriptPath);
	String sout = createString(s1,first) + createString(s2,second);
	String batCmd = String("run %S %S %S %S %S %S %S %S %S", 
					fnScript.sFullPathQuoted(),
					sout,
					inputDrive,
					inputPath,
					outputDrive,
					outputPath,
					gdalDir,
					ilwdir,
					utilDir
					);
	return batCmd;
}