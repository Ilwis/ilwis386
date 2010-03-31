#include "Client\Headers\formelementspch.h"
#include "Client\Forms\IMPORT.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Forms\GeneralImportForm.h"
#include "DataExchangeUI\OptionForms.h"

PostGresOptions::PostGresOptions(CWnd *parent)  : FormExtraImportOptions(parent,"Postgres Options"){
	fsUserName = new FieldString(root,"User name",&userName);
	fsPassword = new FieldString(root, "Password", &passWord, Domain(), true, ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP|WS_BORDER|ES_PASSWORD);
}

String PostGresOptions::sGetExtraOptionsPart(const String& currentExp) {
	return String("%S -username=%S -password=%S",currentExp, userName, passWord);
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




