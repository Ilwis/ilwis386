#include "Client\Headers\formelementspch.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include <set>
#include  "GeonetCastToolboxUI\Page.h"
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include "Engine\Base\System\Engine.h"
#include "engine\base\system\RegistrySettings.h"
#include "iniFile.h"
#include "Client\ilwis.h"

GNCPage::GNCPage(GeonetCastFrm *frm, FormEntry *entry) :
	FieldGroup(entry), hidden(false)
{
	psn->iPosY = psn->iBndDown;
	SetIndependentPos();
	mainFrm = frm;
}

void GNCPage::create()
{
	FieldGroup::create();
	Hide();
}

void GNCPage::setRootPath(const String& path, const String& file) {
	rootPath = path;
	iniFile = file;
}

bool GNCPage::isHidden() const{
	return hidden;
}
// for the moment only before the tree is made(maybe later dynamically)
void GNCPage::setHidden(bool yesno){
	hidden = yesno;
}

DataPage::DataPage(GeonetCastFrm *frm, FormEntry *parent) : GNCPage(frm,parent), fbIn(0),fbOut(0),fsIn(0),choice(iUNDEF),useRegion(0),regionMap(0),cbRegion(0),fmRegion(0) {
	this->SetBevelStyle(FormEntry::bsLOWERED);
	nooutput = noinput = false;
	buttonText = "Import";
};

String DataPage::getName(){
	return name;
}
void DataPage::setName(const String& b){
	name = b;
}
String DataPage::getScript(){
	return script;
}
void DataPage::setScript(const String& s){
	script = s;
}
String DataPage::getFormat(){
	return format;
}
void DataPage::setFormat(const String& f){
	format = f;
}

void DataPage::updateFolders(const String& in, const String& out) {
	dirIn = in;
	dirOut = out;
	if ( fbIn!= 0)
		fbIn->SetVal(dirIn);
	if ( fbOut != 0)
		fbOut->SetVal(dirOut);
	if ( fsIn != 0)
		fsIn->SetVal(in);
}

void DataPage::setFolders(const String& type) {
	String name = type.sHead(":");
	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + rootPath;
	IniFile inifile;
	inifile.Open(pluginDir+ "\\" + iniFile);
	dirOut=inifile.GetKeyValue(name, "OutputFolder");
	dirIn=inifile.GetKeyValue(name, "InputFolder");
	inifile.Close();
}

void DataPage::InitImport(String& ilwDir, String& pluginDir, String& gdalDir, String& utilDir,String& inputDrive, String& inputPath, String& outputDrive, String& outputPath){
	ilwDir = IlwWinApp()->Context()->sIlwDir(); 
	pluginDir = ilwDir + rootPath;
	gdalDir = pluginDir + "\\GDAL";
	utilDir = pluginDir + "\\util";
	FileName fn;
	if(dirIn.length() != 0){
		fn = FileName(dirIn);
		inputDrive = fn.sDrive;
		inputPath = dirIn.substr(3, dirIn.length()-3);
	}
	else{
		inputDrive="x";
		inputPath="x";
	}
	if(time.length() == 0)
		time = "xx";
	dirOut = dirOut.sTrimSpaces();
	fn = FileName(dirOut);
	outputDrive = fn.sDrive;
	if ( dirOut.size() > 2)
		outputPath = dirOut.substr(3, dirOut.length()-3);
	else 
		outputPath = "x";
}

String DataPage::createCommand(const String& ilwDir, const String& pluginDir, const String& gdalDir, const String& utilDir,const String& inputDrive, const String& inputPath, const String& outputDrive, const String& outputPath){
	String scriptPath = pluginDir + "\\" + script; 
	String ilwdir = ilwDir.substr(0, ilwDir.length() - 1);
	FileName fnScript(scriptPath);
	String batCmd = String("run %S %S %S %S %S %S %S %S %S", 
					fnScript.sFullPathQuoted(),
					time,
					inputDrive,
					inputPath,
					outputDrive,
					outputPath,
					gdalDir,
					ilwdir,
					utilDir
					);
	if (useRegion && regionMap && *useRegion && *regionMap != "")
		SetEnvironmentVariable("ILWIS_TOOLBOX_REGION", FileName(*regionMap).sFullPathQuoted().c_str());
	else
		SetEnvironmentVariable("ILWIS_TOOLBOX_REGION", NULL);
	return batCmd;
}
void DataPage::setFolderId(const String& s)
{
	nooutput = (s.iPos(String("nooutput")) > 0);
	noinput = (s.iPos(String("noinput")) > 0);
	folderId=s.sHead(":");
}

String DataPage::getFolderId() const {
	return folderId;
}

void DataPage::setComment(const String& _comment) {
	comment = _comment;
}

void DataPage::setRegionMap(bool * _useRegion, String * _regionMap) {
	useRegion = _useRegion;
	regionMap = _regionMap;
}

String DataPage::getComment(){
	return comment;
}

void DataPage::setId(const String& iid){
	id = iid;
}

String DataPage::getId() const {
	return id;
}

void DataPage::setButtonText(const String& txt) {
	buttonText = txt;
}

String DataPage::getBranch() const {
	return branch;
}
void DataPage::setBranch(const String& b){
	branch = b;
}

int DataPage::RegionChanged(Event* ev)
{
	if (cbRegion && fmRegion) {
		cbRegion->StoreData();
		fmRegion->StoreData();
	}
	return 1;
}

void DataPage::show(int sw)
{
	GNCPage::show(sw);
	if ((sw == SW_SHOW) && cbRegion && fmRegion) {
		cbRegion->SetVal(*useRegion);
		fmRegion->SetVal(*regionMap);
		if (*useRegion)
			cbRegion->ShowChildren();
		else
			cbRegion->HideChildren();
	}
}