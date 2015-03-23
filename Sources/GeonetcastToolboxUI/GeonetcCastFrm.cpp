#include "Client\Headers\formelementspch.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\FormElements\TreeSelector.h"
#include <set>
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include  "GeonetCastToolboxUI\Page.h"
#include  "GeonetCastToolboxUI\Pages.h"
#include  "GeonetCastToolboxUI\MorePages.h"
#include "Client\Base\Picture.h"
#include "Engine\Base\System\Engine.h"
#include "Client\FormElements\FieldPicture.h"
#include "engine\base\system\RegistrySettings.h"
#include "Engine\Base\DataObjects\URL.h"
#include "iniFile.h"
//#include "Engine\Base\DataObjects\RemoteObject.h"
#include "Client\ilwis.h"


LRESULT Cmdgeonetcastmanager(CWnd *wnd, const String& s) {
	new GeonetCastFrm("Geonetcast Toolbox","Geonetcast");

	return 1;
}


GeonetCastFrm::GeonetCastFrm(const String& title, const String& type) :
FormWithDest(IlwWinApp()->GetMainWnd(),title , fbsSHOWALWAYS |  fbsNOOKBUTTON | fbsBUTTONSUNDER | fbsNoCheckData | fbsCancelHasCLOSETEXT,WS_MINIMIZEBOX),
	fInitial(true),
	curPage(NULL),
	formType(type),
	useRemote(false)
{
	try{
		IlwisSettings settings("Dummy");
		workingDir = Directory(settings.sValue("WorkingDirectory"));
		//
		tree = new TreeSelector(root);   
		tree->SetHeight(450);
		tree->SetCallBack((NotifyProc)&GeonetCastFrm::Fill);
		tree->SetNotifyExpansion((NotifyProc)&GeonetCastFrm::dummy);
		tree->SetWidth(140);
		String s = getEngine()->getContext()->sIlwDir();
		s += "Extensions\\" + formType + "-Toolbox\\config.xml";

		ReadConfigFile(s);
		for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
				(*cur).page->Align(tree, AL_AFTER);
		StaticText *txt = new StaticText(root,String("Config XML version %S",xmlVersion));
		txt->Align(tree, AL_UNDER);
		StartPage();
		create();
	}catch(const ErrorObject& err) {
		err.Show();
	}

}
int GeonetCastFrm::dummy(Event *ev) {
	return 1; // else item expanding redoes the last action
}

void GeonetCastFrm::StartPage() {
	GNCPage *page = GetPage(formType);
	FieldPicture *fp = new FieldPicture(page, NULL);
	fp->SetWidth(350);
	fp->SetHeight(450);
	fp->SetBevelStyle(FormEntry::bsRAISED);
	String ilw("%SExtensions\\%S-Toolbox\\util\\%S.bmp", getEngine()->getContext()->sIlwDir(),formType,formType);
	tree->SetWidth(120);
	fp->Load(FileName(ilw));
	if ( page)
		page->Show();

}


int GeonetCastFrm::Fill(Event* ev)
{
	if (curPage)
		curPage->StoreData();
	if (tree->fValid() && fInitial)
	{
		for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		{
			if ( (*cur).sName != "" && (*cur).page->isHidden() == false)
			{
				tree->Add((*cur).sName);
			}
		}
	}
	for(PageIter cur3=pages.begin(); cur3 != pages.end(); ++cur3)
		(*cur3).page->Hide();

	String sVal = tree->sBranchValue();
	curPage = GetPage(sVal);
	if ( curPage) {
		if (!curPage->directAction()) {
			curPage->Show();
		}
	}
	if ( fInitial){
		tree->SelectNode(formType);
		fInitial=false;
		tree->ExpandTo(1);
	}
	fInitial = false;
	return 1;
}

GNCPage *GeonetCastFrm::GetPage(const String& sVal)
{
	for(PageIter cur=pages.begin(); cur != pages.end(); ++cur)
		if ( (*cur).sName == sVal)
			return (*cur).page;

	return NULL;
}

void GeonetCastFrm::ReadConfigFile(FileName fnConfig) {
	if ( fnConfig.fExist() == false)
		return;

	ILWIS::XMLDocument doc(fnConfig);
	String s = doc.toString();
	PicturePage * p = (PicturePage *)pageFactory(ptPicture);
	pages.push_back(Page(formType, p));

	vector<string> keys;
	for(map<String, vector<DataPage *> >::iterator cur = pageKeys.begin(); cur != pageKeys.end(); ++cur) {
		keys.push_back((*cur).first);
	}
	if ( keys.size() != 0) {
	fp = new FinderPage(this,root);
		pages.push_back(Page(formType + "#Toolbox Item Finder",fp ));
		fp->set();
		fp->setKeys(keys);
	}
	int count = 0;
	for(pugi::xml_node child = doc.first_child(); child; child = child.next_sibling()) {
		build(child,child.name(),"", ""); 
	}

	String ilwDir = IlwWinApp()->Context()->sIlwDir();
	String pluginDir = ilwDir + "Extensions\\" + formType + "-Toolbox";
	IniFile inifile;
	String tempName = formType;
	inifile.Open(pluginDir+ "\\" + tempName.toLower() + ".ini");
	for(set<String>::iterator cur = folders.begin(); cur != folders.end(); ++cur) {
		String folderId = (*cur);
		if ( folderId == "") 
			continue;
		DirectoryPage *p = 	(DirectoryPage *)pageFactory(ptDirectory);
		folderId = p->makePath(folderId);
		p->setInput(inifile.GetKeyValue(folderId,"InputFolder"));
		p->setOutput(inifile.GetKeyValue(folderId,"OutputFolder"));
		p->setRootPath(rootpath,iniFile);
		p->setPages(&pages);
		p->set();
		pages.push_back(Page("Configuration#Folders#" + folderId,p));
	}
	SpecialLocationsPage *sp = new SpecialLocationsPage(this, root,specialLocations);
	sp->setRootPath(rootpath,iniFile);
	sp->set();
	specialLocations["Irfan view"] =  inifile.GetKeyValue("Special locations","Irfanview");
	pages.push_back(Page("Configuration#Folders#Special locations",sp ));

	inifile.Close();

	
}

const vector<DataPage *> GeonetCastFrm::getKeyPages(const String& key) const {
	map<String, vector<DataPage *> >::const_iterator cur = pageKeys.find(key);
	if ( cur != pageKeys.end())
		return (*cur).second;

	return vector<DataPage *>();
}



void GeonetCastFrm::parseStationNode(pugi::xml_node node,StationOutputPage* spage) {
	for(pugi::xml_node cnode = node.first_child(); cnode; cnode = cnode.next_sibling()) {
		pugi::xml_node_type type = cnode.type();
		if ( type != pugi::node_element)
			continue;
		String nodeName = cnode.name();
		if ( nodeName == "Source") {
			String source = cnode.attribute("location").value();
			spage->setSource(source);
		}

	}
}

String GeonetCastFrm::parseKeywordNode(pugi::xml_node node, const String& keysUpp) {
	String keys = node.attribute( "keywords").value();
	if ( keys =="")
		return keysUpp;

	String keysUpper = keysUpp;
	if (keysUpper == "")
		keysUpper += keys;
	else
		keysUpper += ";" + keys;

	return keysUpper;
}

void GeonetCastFrm::parseSearchNode(pugi::xml_node node,StationSearchPage* spage) {
	for(pugi::xml_node cnode = node.first_child(); cnode; cnode = cnode.next_sibling()) {
		pugi::xml_node_type type = cnode.type();
		if ( type != pugi::node_element)
			continue;
		String nodeName = cnode.name();
		if ( nodeName == "Source") {
			String source = cnode.attribute("location").value();
			spage->setSource(source);
			for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
				pugi::xml_node_type type = child.type();
				if ( type != pugi::node_element)
					continue;
				String childName = child.name();
				if ( childName == "SearchDefinition") {
					SearchInfo info;
					info.keyColumn = child.attribute("key").as_int();
					info.dataColumn = child.attribute("data").as_int();
					info.tag = child.attribute("tag").value();
					spage->addSearchInfo(info);
				}
			}
		}
		 
	}
}

void GeonetCastFrm::build(pugi::xml_node node,  String current, String idPath, const String& keysUpper) {
	pugi::xml_node_type type = node.type();
	if ( type == pugi::node_element) {
		String nodeName = node.name();
		if  ( nodeName == "Version") {
			String id = node.attribute("id").value();
			xmlVersion = id;
		}
		if ( nodeName == "RemoteServer") {
			useRemote = String(node.attribute("use").value()).fVal();
			remoteServer = URL(node.attribute("url").value());
			String server= remoteServer.getHost();
			if ( useRemote && server == "localhost") {
				getEngine()->Execute("startserver");
			}
		}
		if ( nodeName == "Path" ) {
			rootpath = node.attribute("value").value();
			iniFile = node.attribute("inifile").value();
		}
		String path  = current;

		String name = node.attribute("value").value();
		if ( name != "")
			path = current + "#"+ name;
		String id = idPath == "" ? node.attribute("id").value() : String("%S:%s", idPath, node.attribute("id").value());
		if ( nodeName == "Product") {
			PageTypes type = getType(node.attribute("type").value());
			String folderid=node.attribute("folderid").value();
			String comment=node.attribute("comment").value();
			folders.insert(folderid);
			GNCPage *page = pageFactory(type);
			page->setRootPath(rootpath, iniFile);
			DataPage *pImport = dynamic_cast<DataPage *>(page);
			if ( pImport) {
				pImport->setScript(node.attribute( "script").value());
				pImport->setFormat(node.attribute( "format").value());
				pImport->setName(name);
				pImport->setId(id);
				pImport->setBranch(current + "#" + name);
				pImport->setFolderId(folderid);
				pImport->setComment(comment);
				if ( type == ptStationSearchPage) {
					parseSearchNode(node, (StationSearchPage *)pImport);
				}
				if ( type == ptStationPage) {
					parseStationNode(node, (StationOutputPage *)pImport);
				}
				String keys = parseKeywordNode(node, keysUpper);
				if ( keys != "") {
					Array<String> parts;
					Split(keys, parts,";");
					for(int index = 0; index < parts.size(); ++index) {
						pageKeys[parts[index]].push_back(pImport);
					}
				}
				pImport->set();
				pages.push_back(Page(path,pImport));
			} 
		} 
		for(pugi::xml_node child = node.first_child(); child; child = child.next_sibling()) {
			String keysUp = parseKeywordNode(node, keysUpper);
			build(child,path, id,keysUp); 
		}
	}

}

GeonetCastFrm::PageTypes GeonetCastFrm::getType(const String& type) {
	if ( type == "ymdhm") 
		return ptImportMoment;
	if ( type == "Julian day")
		return ptImportMomentJulian;
	if ( type == "Last24Hr")
		return ptLast24Hrs;
	if ( type == "InOut")
		return ptInOut;
	if ( type=="dec")
		return ptDec;
	if ( type=="angle")
		return ptAngle;
	if ( type=="Stop")
		return ptStop;
	if ( type == "MLHdf4")
		return ptMLHDF4;
	if ( type == "ExpGTiff")
		return ptExpGTiff;
	if ( type == "ExpR")
		return ptExpR;
	if ( type == "ProductNavigator")
		return ptProdNav;
	if ( type == "DataManager")
		return ptDataManager;
	if ( type == "MSGDataRetriever" )
		return ptMSGDataRetriever;
	if ( type == "Jason2")
		return ptJason2;
	if ( type == "AVHRR")
		return ptAVHRR;
	if ( type == "ImportMomentOrbit")
		return ptImportMomentOrbit;
	if (type == "ymdhmo")
		return ptImportMomentNoInput;
	if (type == "urlpage")
		return ptUrlPage;
	if ( type == "stationpage")
		return ptStationPage;
	if ( type == "stationsearch")
		return ptStationSearchPage;
	if ( type == "LLCorner")
		return ptLLCorner;

	return ptUnknown;
}

String GeonetCastFrm::removeWhiteSpace(const String& value) {
	int left = 0;
	for(left =0; left < value.size(); ++left) {
		char c = value[left];
		if ( c > 32)
			break;
	}
	int right = 0;
	for(right = value.size(); right >= 0; --right) {
		char c = value[right];
		if ( c > 32)
			break;
	}
	if ( left > right || left == right || left ==value.size() || right == -1)
		return ""; 
	return value.substr(left,right - left + 1);

}

GNCPage *GeonetCastFrm::pageFactory(PageTypes type) {
	if ( type == ptImportMoment) {
		return new PageType1(this, root); 
	}
	if ( type == ptImportMomentJulian) {
		return new PageType2(this, root);
	}
	if ( type == ptLast24Hrs) {
		return new PageType4(this, root);
	}
	if ( type == ptInOut) {
		return new PageType3(this, root);
	}
	if ( type == ptPicture) {
		return new PicturePage(this, root); 
	}
	if ( type == ptDirectory) {
		return new DirectoryPage(this, root); 
	}
	if ( type == ptDec) {
		return new PageType5(this, root);
	}
	if ( type == ptAngle) {
		return new AnglePage(this, root);
	}
	if ( type == ptStop) {
		return new StopAnimationPage(this, root);
	}
	if ( type == ptMLHDF4) {
		return new MultiLayerHDF4(this, root);
	}
	if ( type == ptExpGTiff) {
		return new ExportGeoTiff(this, root);
	}
	if ( type == ptExpR) {
		return new ExportR(this, root);
	}
	if ( type == ptProdNav) {
		return new ProductNavigator(this, root);
	}
	if ( type == ptDataManager) {
		return new DataManagerPage(this, root);
	}
	if ( type == ptMSGDataRetriever) {
		return new MSGDataRetrieverPage(this, root);
	}
	if ( type == ptJason2)
		return new Jason2Page(this, root);
	if ( type == ptAVHRR) {
		return new AVHRRPage(this, root);
	}
	if (type == ptImportMomentOrbit){
		return new TimePlusOrbit(this, root);
	}
	if (type == ptImportMomentNoInput){
		return new ImportMomentNoInput(this, root);
	}
	if ( type == ptUrlPage){
		return new URLPage(this,root);
	}
	if ( type == ptStationPage){
		return new StationOutputPage(this,root);
	}
	if ( type == ptStationSearchPage) {
		return new StationSearchPage(this,root);
	}

	if ( type == ptLLCorner) {
		return new LLCornerPage(this,root);
	}


	return 0;
}

String GeonetCastFrm::getSpecialLocation(const String& s) const {
	map<String,String>::const_iterator iter = specialLocations.find(s);
	if (  iter != specialLocations.end())
		return (*iter).second;
	return "";
}

URL GeonetCastFrm::getServer() const{
	return remoteServer;
}

bool GeonetCastFrm::fRemote() const{
	return useRemote;

}

void GeonetCastFrm::SetPage(const String& sVal, bool expand) {
	if ( curPage)
		curPage->Hide();
	curPage = GetPage(sVal);
	if ( curPage) {
		if (!curPage->directAction()) {
			curPage->Show();
			if ( expand)
				tree->SelectNode(sVal);
		}
	}
}






