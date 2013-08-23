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
#pragma warning( disable : 4786 )

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FieldStringList.h"
#include "Engine\Base\File\Directory.h"
#include "Engine\Base\DataObjects\URL.h"
#include "Engine\Base\System\LOGGER.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\ilwis.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\TreeSelector.h"
#include "Client\FormElements\flddat.h"
#include "Engine\Base\Tokbase.h"
#include "Engine\Base\DataObjects\Parm.h"
#include "Client\FormElements\NonIlwisObjectLister.h"
#include "Client\Forms\IMPORT.H"
#include "Client\Forms\GeneralImportForm.h" 
#include "Client\Forms\IMPRAS.H"
#include "client\forms\ImportAsciiTableWizard.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Scripting\Script.h"
#include "Client\FormElements\FieldBrowseDir.h"

FieldImportPage::FieldImportPage(FormEntry *entry) :
	FieldGroup(entry)
{
	psn->iPosY = psn->iBndDown;
	SetIndependentPos();
}

void FieldImportPage::create()
{
	FieldGroup::create();
	Hide();
}

GeneralImportForm::GeneralImportForm(CWnd* parent)
		: curPage(NULL), fInitial(true), fUseAs(false), FormWithDest(parent, "Import"), fMatch(true), isWfs(false)
{
	addModules();
	FieldGroup *fg = new FieldGroup(root);
	new StaticText(fg,"Drivers");
	//new FieldString(fg, &ss, WS_TABSTOP|WS_GROUP|ES_READONLY|WS_BORDER);
	tree = new TreeSelector(fg, false);
	tree->SetCallBack((NotifyProc)&GeneralImportForm::Fill);
	tree->SetWidth(120);
	FieldGroup *fg2 = new FieldGroup(root);
	new FieldBlank(fg2,1.0);
	if ( currentDriver.driverName != "")
		driverMessage = String("%S:%S",currentDriver.driverName, currentFormat.name);
	fsDriverDetails = new FieldString(fg2,"", &driverMessage,Domain(),true,WS_GROUP|ES_READONLY);
	fsDriverDetails->SetWidth(120);
	fsDriverDetails->SetIndependentPos();
	fsInput = new FieldString(fg2,"Input",&sInput);
	fsInput->SetCallBack((NotifyProc)&GeneralImportForm::SetDefaultOutputName);
	PushButton *pbSelectInput = new PushButton(fsInput,"...",(NotifyProc)&GeneralImportForm::ObjectSelection);
	pbSelectInput->Align(fsInput,AL_AFTER);
	pbSelectInput->SetHeight(fsInput->psn->iHeight);
	fsInput->SetWidth(100);
	fsInput->Hide();
	pbMoreOptions = new PushButton(fg2,"Options...",(NotifyProc)&GeneralImportForm::moreOptions);
	pbMoreOptions->Hide();
	pbMoreOptions->Align(fsInput, AL_UNDER);
	fsOutput = new FieldString(fg2,"Output",&sOutput);
	PushButton *pbSelectOutput = new PushButton(fsOutput,"...",(NotifyProc)&GeneralImportForm::OutputSelection);
	pbSelectOutput->Align(fsOutput,AL_AFTER);
	pbSelectOutput->SetHeight(fsOutput->psn->iHeight);
	fsOutput->Align(pbMoreOptions, AL_UNDER);
	fsOutput->SetWidth(100);
	fsOutput->Hide();
	cbMatch = new CheckBox(fg2,"Output name matches input",&fMatch);
	cbMatch->Align(fsOutput,AL_UNDER);
	cbMatch->SetCallBack((NotifyProc)&GeneralImportForm::similarNames);
	cbMatch->Hide();

	cb = new CheckBox(fg2,"Use as",&fUseAs);
	cb->Align(cbMatch, AL_UNDER);
	cb->Hide();

	fg2->Align(fg,AL_AFTER);

	for(vector<ImportDriver>::iterator cur=drivers.begin();cur != drivers.end();++cur) {
		String driverName = (*cur).driverName;
		for(vector<ImportFormat>::iterator cur2=(*cur).formats.begin(); cur2 != (*cur).formats.end(); ++cur2) {
			String leaf;
			int type = (*cur2).type;
			if ( type == ImportFormat::ifRaster)
				leaf = driverName + "#raster#" + (*cur2).name;
			else if ((type & ImportFormat::ifRaster) && ((type & ImportFormat::ifPoint) || (type & ImportFormat::ifSegment) || (type & ImportFormat::ifPolygon)))
				leaf = driverName + "#raster,vector#" + (*cur2).name;
			else if ( type > 2 && type < ImportFormat::ifUnknown)
				leaf = driverName + "#vector#" + (*cur2).name;
			else if ( type == ImportFormat::ifTable)
				leaf = driverName + "#table#" + (*cur2).name;

			pages.push_back(GeneralImportForm::Page(leaf,new FieldImportPage(root)));
		}
	}
	for(ImportPageIter cur=pages.begin(); cur != pages.end(); ++cur)
		(*cur).page->Align(tree, AL_AFTER);

	create();
}

int GeneralImportForm::similarNames(Event *ev) {
	cbMatch->StoreData();
	if ( fMatch) {
		sOutput = "[Output name matches input]";
	} else {
		sOutput = "";
	}
	fsOutput->SetVal(sOutput);

	return 1;
}

int GeneralImportForm::OutputSelection(Event *ev) {
	FileName fnO = FileName(fsOutput->sVal());
	fnO = SetExtension(fnO);
	String sExt = "*" + fnO.sExt;
	String currentDir = getEngine()->sGetCurDir();
	char path[255];
	char title[20];
	strcpy(title,"Input file");
	strcpy(path,currentDir.c_str());
	CFileDialog dlg(FALSE,NULL,fnO.sRelative().c_str(),4|2,String("ILWIS Object(%S)|%S",sExt,sExt).c_str());
	dlg.m_ofn.lpstrInitialDir = path;
	dlg.m_ofn.lpstrTitle = title;
	if ( dlg.DoModal() == IDOK) {
		String file("%s",dlg.m_ofn.lpstrFile);
		fnO = FileName(file);
		fnO = SetExtension(fnO);
		fsOutput->SetVal(fnO.sRelative());
	}

	return 1;
}

int GeneralImportForm::SetDefaultOutputName(Event *dv) {

	cbMatch->StoreData();

	String name = fsInput->sVal();
	if (URL::isUrl(name)){
		URL url(name);
		String path = url.getPath();
		Array<String> parts	;
		Split(path,parts,"/");
		if ( path.size() > 0)
			name = parts[parts.size() - 1];
		String service = url.getQueryValue("service");
		if (service == "wfs") {
			isWfs = true;
		}
	} /*else {
		int index = name.find_last_of(".");
		if ( index !=  string::npos) {
			name = name.substr(0, index);
		}
	}*/
	pbMoreOptions->Hide();
	if ( name != ""){
		if ( fMatch == false) {
			String transformedName;
			for(int i=0; i< name.size(); ++i) {
				char lastChar = name[i];
				if ( lastChar == '.' || lastChar==':' || lastChar == '/' || lastChar == '\\' || lastChar ==' ' || lastChar=='-')
					transformedName += '_';
				else
					transformedName += lastChar;
			}
			FileName fnO(transformedName);
			fnO = SetExtension(FileName(fnO.sFile + fnO.sExt));
			fsOutput->SetVal(fnO.sRelative());
		}

		pbMoreOptions->SetText("Options...");
		if ( currentFormat.ui != NULL) {
			if ( currentFormat.buttonText != "")
				pbMoreOptions->SetText(currentFormat.buttonText);
			pbMoreOptions->Show();
		} else {
			extraOptions = "";
		}
		cbMatch->Show();
	}

	return -1;
}

FileName GeneralImportForm::SetExtension(const FileName& fn) {
	FileName fnO(fn);
	if ( currentFormat.type == ImportFormat::ifRaster)
			fnO.sExt = ".mpr";
	else if ( currentFormat.type == ImportFormat::ifTable)
		fnO.sExt = ".tbt";
	else if ( currentFormat.type == ImportFormat::ifPoint)
		fnO.sExt = ".mpp";
	else if ( currentFormat.type == ImportFormat::ifSegment)
		fnO.sExt = ".mps";
	else if ( currentFormat.type == ImportFormat::ifPolygon)
		fnO.sExt = ".mpa";
	else
		fnO.sExt = ".ioc";

	return fnO;
}

class LayerForm : public FormWithDest {
public:
	LayerForm(CWnd *par, const vector<String>& layers) : FormWithDest(par, TR("Layer Selection")){
		new FieldStringList(root,&layer, layers);
	}
	String getLayer() const {
		return layer;
	}
private:
	vector<String> layers;
	String layer;
};
int GeneralImportForm::ObjectSelection(Event *ev) {

	String sExt = currentFormat.ext;
	if ( isWfs) {
		vector<String> layers;
		LayerForm form(this->wnd(), layers);
		if ( form.DoModal() == IDOK) {
			layer = form.getLayer();
		}
		return 1;
	}
	if ( sExt.find_first_of("/") != string::npos) {
		Array<String> parts;
		Split(sExt,parts,"/");
		sExt = "";
		for(int i=0; i < parts.size(); ++i) {
			if( i > 0)
				sExt += ";";
			sExt += "*." + parts[i];

		}
	} else
		sExt = "*." + sExt;

	String currentDir = getEngine()->sGetCurDir();
	char path[255];
	char title[20];
	strcpy(title,"Input file");
	strcpy(path,currentDir.c_str());
	CFileDialog dlg(TRUE,NULL,NULL,4|2,String("%S(%S)|%S",currentFormat.shortName,sExt,sExt).c_str());
	dlg.m_ofn.lpstrInitialDir = path;
	dlg.m_ofn.lpstrTitle = title;
	if ( dlg.DoModal() == IDOK) {
		String file("%s",dlg.m_ofn.lpstrFile);
		FileName fnIn(file);
		fsInput->SetVal(fnIn.sShortName());
	}

	return -1;
}

int GeneralImportForm::Fill(Event*)
{
	if (curPage)
		curPage->StoreData();
	if (tree->fValid() )
	{
		for(ImportPageIter cur=pages.begin(); cur != pages.end(); ++cur)
		{
			if ( (*cur).sName != "")
			{
				tree->Add((*cur).sName,0);
			}
		}
		fInitial=false;
	}
	for(ImportPageIter cur3=pages.begin(); cur3 != pages.end(); ++cur3)
		(*cur3).page->Hide();

	String sVal = tree->sBranchValue();

	curPage = GetPage(sVal);
	currentFormat = getFormat(sVal);
	if ( currentFormat.shortName != "" || currentFormat.name != "") {
		if ( currentFormat.shortName != "" && currentFormat.name != "")
			fsDriverDetails->SetVal(String("%S:%S",currentDriver.driverName, currentFormat.name));
		else {
			fsDriverDetails->SetVal(currentFormat.shortName != "" ? currentFormat.shortName : currentFormat.name);
		}
		fsInput->Show();
		fsOutput->Show();
	} else {
		fsDriverDetails->SetVal("");
		fsInput->Hide();
		fsOutput->Hide();
	}
	if ( currentFormat.useasSuported)
		cb->Show();
	else
		cb->Hide();
	pbMoreOptions->Hide();

	return 1;
}

int GeneralImportForm::moreOptions(Event *) {
	if ( currentFormat.ui != NULL) {
		fsInput->StoreData();
		sInput = fsInput->sVal();
		if ( currentFormat.method != "IlwisTable" && currentFormat.method != "ADO") {
			FormExtraImportOptions *frm = (FormExtraImportOptions *)currentFormat.ui;
			frm->setInput(sInput);
			frm->create();
			if ( frm->fOkClicked()) {
				extraOptions = frm->sGetExtraOptionsPart(extraOptions);
			}
		} else {
			AsciiTableWizard *wiz =   (AsciiTableWizard *)currentFormat.ui;
			wiz->SetFormat(currentFormat);
			wiz->SetInputTable(FileName(sInput));
			if ( wiz->DoModal() == ID_WIZFINISH )  {
				extraOptions = wiz->sBuildExpression();
			}
		}
	}
	return 1;
}

FormEntry *GeneralImportForm::CheckData() {
	fsInput->StoreData();
	fsOutput->StoreData();

	if ( sInput == "")
		return fsInput;
	if ( sOutput == "")
		return fsOutput;

	if ( currentFormat.ui != NULL) {
		if ( currentFormat.method != "IlwisTable" && currentFormat.method != "ADO") {
			FormExtraImportOptions *frm = (FormExtraImportOptions *)currentFormat.ui;
			if ( !frm->fValidData()) {
				frm->create();
				if ( frm->fOkClicked()) 
					extraOptions = frm->sGetExtraOptionsPart(extraOptions);
				return pbMoreOptions;
			}
		} else {
			if ( extraOptions == "") {
				AsciiTableWizard *wiz =   (AsciiTableWizard *)currentFormat.ui;
				wiz->SetFormat(currentFormat);
				wiz->SetInputTable(FileName(fsInput->sVal()));
				if ( wiz->DoModal() == ID_WIZFINISH )  {
					extraOptions = wiz->sBuildExpression();
					
				}
				return pbMoreOptions;
			}
		}
	}
	return 0;
}

int GeneralImportForm::exec() {
	FormWithDest::exec();

	String openCmd;
	if ( currentDriver.driverName== "" || sInput == "" || sOutput == "")
		return 0;

	bool containsWildcards = sInput.find_first_of("*?") != -1;
	CFileFind finder;
	int count = 0;
	BOOL b = finder.FindFile(sInput.c_str());
	while(b) {
		b = finder.FindNextFile();
		String inFile = finder.GetFileName();
		String outFile = sOutput;
		if ( outFile == "[Output name matches input]") {
			FileName fnOut(inFile);
			outFile = SetExtension(fnOut).sRelative();
		} else {
			if ( containsWildcards)
				outFile = String("%S_%d", outFile, count++);
		}

		if ( currentDriver.driverName == "ILWIS") {
			if ( currentFormat.method=="IlwisTable")
					openCmd  = FileName(outFile,".tbt").sRelativeQuoted();	
			else if ( currentFormat.method !=  "GeneralRaster")
				openCmd = String("import %S(%S, %S)", currentFormat.method, FileName(inFile).sFullPathQuoted(), FileName(outFile).sFullPathQuoted());
			else {
				openCmd = FileName(outFile).sRelativeQuoted();
				String head = extraOptions.sHead("(");
				String tail = extraOptions.sTail("(");
				openCmd += head + "(" + inFile.sQuote() + tail;
				getEngine()->Execute(openCmd);
				return 0;
			}
		} else {
			if ( currentDriver.driverName == "ActiveX Data Objects(ADO)") {
				openCmd = String("open %S -output=%S -method=%S", extraOptions, SetExtension(FileName(outFile)).sFullPathQuoted(), currentFormat.method);
				getEngine()->Execute(openCmd);
				return 0;
			}
			else if ( URL::isUrl(inFile)) {
				openCmd = String("open %S -output=%S -method=%S",inFile, SetExtension(FileName(outFile)).sFullPathQuoted(), currentFormat.method);
			}else {
				if ( currentFormat.command.find("import") == currentFormat.command.end())
					openCmd = String("open %S -output=%S -method=%S",FileName(inFile).sFullPathQuoted(), SetExtension(FileName(outFile)).sFullPathQuoted(), currentFormat.method);
				else {
					openCmd = String("%S %S %S %S", currentFormat.command["import"], currentFormat.name.sQuote(),FileName(inFile).sFullPathQuoted(),FileName(outFile).sFullPathQuoted()); 
				}
			}
		}
		openCmd += extraOptions;
		if ( !fUseAs && currentFormat.command.find("import") == currentFormat.command.end())
			openCmd += " -import";
		
		//if ( !containsWildcards)
			getEngine()->Execute(openCmd);
		//else
			//Script::Exec(openCmd);

	}
	if (!b) {
		if( !URL::isUrl(sInput) || extraOptions == "")
			return 0;
		if (extraOptions.find("wfsimportlayer") == 0)
			IlwWinApp()->Execute(extraOptions);
		else {
			String outFile = sOutput;
			if ( outFile == "[Output name matches input]") {
				int lastslash = sInput.find_last_of('/');
				outFile = sInput.substr(lastslash + 1);
				FileName fnOut(outFile);
				outFile = SetExtension(fnOut).sRelative();
			} 
			openCmd = String("open %S -output=%S -method=%S",sInput, SetExtension(FileName(outFile)).sFullPathQuoted(), currentFormat.method);
			openCmd += extraOptions;
			if ( !fUseAs && currentFormat.command.find("import") == currentFormat.command.end())
				openCmd += " -import";
			getEngine()->Execute(openCmd);
		}
	}
	return 0;
}

ImportFormat GeneralImportForm::getFormat(const String& sVal) {
	if ( sVal == "")
		return ImportFormat();

	Array<String> parts;
	Split(sVal,parts,"#");
	String provider = parts[0];
	String fname = parts[parts.size() - 1];
	String stype = parts.size() > 1 ? parts[1] : "";
	long typeMask = getTypeMask(stype);
	for(int i = 0; i < drivers.size(); ++i) {
		ImportDriver driver = drivers[i];
		if ( driver.driverName == provider) {
			for(int j = 0; j < driver.formats.size(); ++j) {
				bool nameMatch = driver.formats[j].name == fname;
				bool typeMatch = typeMask & driver.formats[j].type;
				if (  nameMatch && typeMatch) {
					currentDriver = driver;
					return driver.formats[j];
				}
			}
		}
	}
	return ImportFormat();
}

long GeneralImportForm::getTypeMask(const String& type) {
	if ( type == "table") {
		return ImportFormat::ifTable;
	} else if ( type == "raster") {
		return ImportFormat::ifRaster;
	} else if ( type == "vector") {
		return ImportFormat::ifPoint | ImportFormat::ifPolygon | ImportFormat::ifSegment;
	} else if ( type == "raster,vector") {
		return ImportFormat::ifRaster | ImportFormat::ifPoint | ImportFormat::ifPolygon | ImportFormat::ifSegment;
	} else if ( type == "point") {
		return ImportFormat::ifPoint;
	}else if ( type == "polygon") {
		return ImportFormat::ifPolygon ;
	} else if ( type == "segment") {
		return ImportFormat::ifSegment;
	}
	return ImportFormat::ifUnknown;
}
FieldImportPage *GeneralImportForm::GetPage(const String& sVal)
{
	for(ImportPageIter cur=pages.begin(); cur != pages.end(); ++cur)
		if ( (*cur).sName == sVal)
			return (*cur).page;

	return NULL;
}

void GeneralImportForm::addModule(const FileName& fnModule,vector<GetImportOptionForms>& options) {
	HMODULE hm = LoadLibrary(fnModule.sFullPath().c_str());
	if ( hm != NULL) {
		ImportDrivers dn = (ImportDrivers)GetProcAddress(hm, "getImportDriverList");
		if ( dn != NULL ) {
			dn(drivers);	
		}
		GetImportOptionForms of = (GetImportOptionForms)GetProcAddress(hm,"getImportOptionForms");
		if ( of) 
			options.push_back(of);
	}

}

void GeneralImportForm::addModules() {
	CFileFind finder;
	vector<GetImportOptionForms> options;

	AddOldStyleIlwisImports();
	String path = IlwWinApp()->Context()->sIlwDir() + "\\Extensions\\*.*";
	BOOL fFound = finder.FindFile(path.c_str());
	while(fFound) {
		fFound = finder.FindNextFile();
		if (!finder.IsDirectory())
		{
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sExt.toLower() == ".dll")
				addModule(fnNew, options);
		}
	/*	else {
			FileName fnNew (finder.GetFilePath());
			if ( fnNew.sFile != "." && fnNew.sFile != ".." && fnNew.sFile != "")
				addFolder(String(fnNew.sFullPath()),options);
		}*/
	}
	for(int i = 0; i < options.size(); ++i) {
		GetImportOptionForms opFunc = options[i];
		opFunc(wnd(),drivers);
	}
}

void GeneralImportForm::AddOldStyleIlwisImports() {
	readImportDef();

	ImportDriver id;
	id.driverName = "ILWIS";
	for(int i = 0; i < oldStyleImports.size(); ++i) {
		ImportItem item = oldStyleImports[i];
		ImportFormat frm;
		frm.ext = item.sExt.substr(1,item.sExt.size() - 1);
		frm.ext = frm.ext.sHead(".") + (frm.ext.size() > 3 ? "/" + frm.ext.sTail(".") : "");
		if ( item.sCmd == "gdal")
			continue;
		if ( item.sCmd == "e00" || item.sCmd == "lin" || item.sCmd=="smt" || item.sCmd=="dxf" || item.sCmd=="bna") {
			frm.type = (int)ImportFormat::ifPoint + (int)ImportFormat::ifSegment + (int)ImportFormat::ifPolygon;
		} else
			frm.type = ImportFormat::ifRaster;
		frm.method = item.sCmd;
		String name = item.sDescr.substr(0,item.sDescr.size() - 5);
		frm.shortName = frm.name = name;
		id.formats.push_back(frm);
	}
	ImportFormat ifm;
	ifm.ext = "*";
	ifm.method = "GeneralRaster";
	ifm.shortName = ifm.name = "General raster";
	ifm.type = ImportFormat::ifRaster;
	ifm.useasSuported = true;
	ifm.ui = new ImportRasterForm(wnd(),FileName());
	id.formats.push_back(ifm);

	ImportFormat ilwTable;
	ilwTable.ext = "dbf";
	ilwTable.method = "IlwisTable";
	ilwTable.shortName = "DBF";
	ilwTable.name = "dBase IV File Format";
	ilwTable.type = ImportFormat::ifTable;
	ilwTable.useasSuported = true;
	ilwTable.ui = new AsciiTableWizard(wnd());
	id.formats.push_back(ilwTable);

	ilwTable.ext = "tbl";
	ilwTable.method = "IlwisTable";
	ilwTable.shortName = "Ilwis 1.4";
	ilwTable.name = "Ilwis 1.4 File Format";
	ilwTable.type = ImportFormat::ifTable;
	ilwTable.useasSuported = false;
	ilwTable.ui = NULL;
	id.formats.push_back(ilwTable);

	ilwTable.ext = "*";
	ilwTable.method = "IlwisTable";
	ilwTable.shortName = "CSV";
	ilwTable.name = "Comma delimited";
	ilwTable.type = ImportFormat::ifTable;
	ilwTable.useasSuported = false;
	ilwTable.ui = new AsciiTableWizard(wnd());;
	id.formats.push_back(ilwTable);

	ilwTable.ext = "*";
	ilwTable.method = "IlwisTable";
	ilwTable.shortName = "Space delimited";
	ilwTable.name = "Space delimited";
	ilwTable.type = ImportFormat::ifTable;
	ilwTable.useasSuported = false;
	ilwTable.ui = new AsciiTableWizard(wnd());;
	id.formats.push_back(ilwTable);

	ilwTable.ext = "*";
	ilwTable.method = "IlwisTable";
	ilwTable.shortName = "Fixed format";
	ilwTable.name = "Fixed format";
	ilwTable.type = ImportFormat::ifTable;
	ilwTable.useasSuported = false;
	ilwTable.ui = new AsciiTableWizard(wnd());
	id.formats.push_back(ilwTable);

	sort(id.formats.begin(), id.formats.end());
	drivers.push_back(id);


}

void GeneralImportForm::readImportDef() {
FileName fn("import.def");
	fn.Dir(IlwWinApp()->Context()->sIlwDir() + "Resources\\Def\\");
	try {
		File fil(fn);
		fil.KeepOpen(true);
		while (!fil.fEof()) {
			String s;
			fil.ReadLnAscii(s);
			TextInput txtinp(s);
			TokenizerBase tokenizer(&txtinp);
			Token tok;
			ImportItem item;
			tok = tokenizer.tokGet();
			item.sCmd = tok.sVal();
			tok = tokenizer.tokGet();
			item.sExt = tok.sVal();
			tok = tokenizer.tokGet();
			item.sDescr = tok.sVal();
			oldStyleImports &= item;
		}
		fil.KeepOpen(false);
	}
	catch (const ErrorObject& err) {
		err.Show();
	}
}

