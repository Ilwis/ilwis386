#include "Client\Headers\AppFormsPCH.h"
#include "Client\Forms\AboutForm.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\Base\System\module.h"
#include "Client\Base\Picture.h"
#include "Client\FormElements\FieldPicture.h"
#include "Engine\Base\DataObjects\Version.h"
#include "Client\FormElements\fldlist.h"
#include "Headers\Hs\Mainwind.hs"

AboutForm::AboutForm(CWnd* parent) : FormWithDest(parent,TR("About ILWIS"),fbsSHOWALWAYS | fbsMODAL | fbsNOCANCELBUTTON) {
	Array<String> modules;
	Array<String> persons;
	ILWIS::Module *m;
	int index = 0;
	while( (m=getEngine()->getModule(index++)) != NULL) {
		modules.push_back(m->getNameAndVersion());
	}
	persons.push_back("Community leader : Rob Lemmens");
	persons.push_back("Developers: ");
	persons.push_back("Martin Schouwenburg (main)");
	persons.push_back("Base Retsios");
	persons.push_back("SEBS & Hydroflow : Lichun Wang");
	persons.push_back(" ");
	persons.push_back("Past Developers:");
	persons.push_back("Wim Koolhoven");
	persons.push_back("Jelle Wind");
	persons.push_back("Willem Nieuwenhuis");
	persons.push_back("Petra Budde");
	persons.push_back("Jan Hendrikse");
	persons.push_back("Raymond Nijmeier");
	persons.push_back(" ");
	persons.push_back("Translations");
	persons.push_back("French: Robin Prest");
	persons.push_back("Spanish: Sandra Patricia");
	dummy = TR("http://52north.org/ilwis");


	FieldGroup *fg1 = new FieldGroup(root,true);
	FieldPicture * fp1 = new FieldPicture(fg1, NULL,NULL, true);
	fp1->LoadResource("n52");
	String ilwis("%S %S",TR("ILWIS Open"),ILWIS::Version::IlwisVersion);
	StaticText *version = new StaticText(fg1,ilwis,true);
	version->Align(fp1, AL_AFTER);
	StaticText *txt = new StaticText(root, TR("(c) 52North GmbH"));
	txt->Align(fg1, AL_UNDER);
	FieldString *fstr = new FieldString(root,"", &dummy, Domain(), true, WS_TABSTOP|WS_GROUP|ES_READONLY);
	fstr->SetWidth(140);
	new FieldBlank(root);
	txt = new StaticText(root,TR("Loaded Modules"));
	StringArrayLister *fs1 = new StringArrayLister(root, modules);
	fs1->SetHeight(90);
	fs1->SetWidth(140);
	txt = new StaticText(root,TR("Persons Involved"));
	StringArrayLister *fs2 = new StringArrayLister(root, persons);
	fs2->SetHeight(90);
	fs2->SetWidth(140);

	
	create();
}