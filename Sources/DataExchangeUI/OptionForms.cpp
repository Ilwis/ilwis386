#include "Client\Headers\formelementspch.h"
#include "Client\Forms\IMPORT.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Forms\GeneralImportForm.h"
#include "DataExchangeUI\OptionForms.h"
#include "Engine/Base/XML/pugixml.hpp"
#include "DataExchange\ConnectorWFS.h"

PostGresOptions::PostGresOptions(CWnd *parent)  : FormExtraImportOptions(parent,"Postgres Options"){
	schema = "public";
	fsUserName = new FieldString(root,"User name",&userName);
	fsPassword = new FieldString(root, "Password", &passWord, Domain(), true, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_PASSWORD);
	fsSchema = new FieldString(root, "Schema", &schema);
}

String PostGresOptions::sGetExtraOptionsPart(const String& currentExp) {
	return String("%S -username=%S -password=%S -schema=%S",currentExp, userName, passWord, schema);
}

FormEntry *PostGresOptions::CheckData() {

	fsUserName->StoreData();
	fsPassword->StoreData();
	if ( userName == "")
		return fsUserName;
	if ( passWord == "")
		return fsPassword;
	return NULL;
}

bool PostGresOptions::fValidData() {
	return userName != "" && passWord != "";
}

AsterOptions::AsterOptions(CWnd *parent) : FormExtraImportOptions(parent,"Aster Options") {
	fRadiances = true;
	cb = new CheckBox(root,"Use raidiances", &fRadiances);
}

bool AsterOptions::fValidData() {
	return true;
}

String AsterOptions::sGetExtraOptionsPart(const String& currentExp) {
	if ( fRadiances == false)
		return "usednvalues";
	return "";
}

#define FIELDHWIDTH 175
#define FIELDBWIDTH 40
//------------------------------------------------------
WFSOptions::WFSOptions(CWnd *p) : FormExtraImportOptions(p,TR("WFS")), layer(-1){
	gcap = new FieldString(root,TR(""),&gcapurl);
	gcap->SetWidth(FIELDHWIDTH);
	PushButton *pb1 = new PushButton(root,TR("GetCapabilties"),(NotifyProc)&WFSOptions::doGetCapabilities);
	pb1->SetWidth(FIELDBWIDTH);
	pb1->Align(gcap, AL_AFTER);
	fldLayers = new FieldOneSelectString(root, "", &layer, layernames);
	fldLayers->SetComboWidth(FIELDHWIDTH);
	fldLayers->SetCallBack((NotifyProc)&WFSOptions::doSetAbstract);
	PushButton *pb2 = new PushButton(root,TR("GetFeature(s)"),0);
	pb2->Align(fldLayers, AL_AFTER);
	fldLayers->Align(gcap, AL_UNDER);
	pb2->SetWidth(FIELDBWIDTH);
	fldAbstract = new FieldStringMulti(root,&layerAbstract,true);
	fldAbstract->Align(fldLayers, AL_UNDER);
	fldAbstract->SetWidth(FIELDHWIDTH);
	fldAbstract->SetHeight(70);
	fldGmlType = new FieldString(root,TR("Feature type"),&featureType);
	fldGmlType->SetIndependentPos();

}
WFSOptions::~WFSOptions() {
	delete conn;
}

int WFSOptions::doSetAbstract(Event *ev) {
	fldLayers->StoreData();
	if ( layer == -1){
		fldAbstract->ClearData();
		return 0;
	}

	if ( layer >= layernames.size()) {
		fldAbstract->ClearData();
		return 1;
	}
	fldAbstract->SetVal("Retrieving information");

	String layername = layernames[layer];
	String abstractTxt = conn->getLayerAbstract(layername);
	String title = conn->getLayerTitle(layername);
	fldAbstract->SetVal(title + "\r\n"+ abstractTxt);
	featureType = conn->getGeometryType(layername);
	fldGmlType->SetVal(featureType);

	return 1;
}

int WFSOptions::doGetCapabilities(Event *ev) {
	gcap->StoreData();
	conn = new ConnectorWFS(gcapurl);
	layernames = conn->getListLayerNames();
	if ( layernames.size() == 0) {
		layer = -1;
	} else
		layer = 0;
	fldLayers->resetContent(layernames);
	fldLayers->SetVal(layer);
	doSetAbstract(0);
	return 1;
}

String WFSOptions::sGetExtraOptionsPart(const String& currentExp){
	if ( layer == -1)
		return "";
	String lyname = String(layernames[layer]).sQuote();
	String url = conn->getServerUrl();
	return "wfsimportlayer " + url + " " + lyname + " " + featureType.toLower();
}

void WFSOptions::setInput(const String& inp) {
	gcapurl = inp;
}




