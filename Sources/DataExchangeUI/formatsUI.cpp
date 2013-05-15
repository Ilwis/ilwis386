#include "Client\Headers\formelementspch.h"
#include "Client\Forms\IMPORT.H"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Client\Forms\GeneralImportForm.h"
#include "DataExchangeUI\OptionForms.h"

void *getImportTableWizard(vector<ImportDriver>& drivers) {
	for(int i=0; i < drivers.size(); ++i) {
		ImportDriver& driver = drivers[i];
		if ( driver.driverName == "ILWIS") {
			for(int j = 0; j < driver.formats.size(); ++j) {
				ImportFormat& fmt = driver.formats[j];
				if ( fmt.shortName == "DBF") {
					return fmt.ui;
				}
			}
		}
	}
	return NULL;
}

extern "C" _export void getImportOptionForms(CWnd *parent, vector<ImportDriver>& drivers) {
	void *ui = NULL;
	for(int i=0; i < drivers.size(); ++i) {
		ImportDriver& driver = drivers[i];			 
		if ( driver.driverName == "Postgres") {
			for(int j = 0; j < driver.formats.size(); ++j) {
				ImportFormat& fmt = driver.formats[j];
				if ( fmt.name == "Database" || fmt.name == "Postgis") {
					fmt.ui = new PostGresOptions(parent);
					fmt.buttonText = "Authenticate";
				}
			}
		}
		if ( driver.driverName == "Aster") {
			driver.formats[0].ui = new AsterOptions(parent);
		}
		if ( driver.driverName == "OGC") {
			driver.formats[0].ui = new WFSOptions(parent);
		}
		if ( driver.driverName == "ADO") {
			for(int j = 0; j < driver.formats.size(); ++j) {
				ImportFormat& fmt = driver.formats[j];
				if ( ui == NULL)
					ui = getImportTableWizard(drivers);
				fmt.ui = ui;
			}
		}
	}
}