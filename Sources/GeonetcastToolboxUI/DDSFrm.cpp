#include "Client\Headers\formelementspch.h"
#include "Engine\Base\File\Directory.h"
#include "Client\FormElements\FieldBrowseDir.h"
#include "Client\FormElements\TreeSelector.h"
#include <set>
#include "Engine\Base\DataObjects\XMLDocument.h"
#include "GeonetCastToolboxUI\GeonetcCastFrm.h"
#include "DDSFrm.h"
#include  "GeonetCastToolboxUI\Page.h"
#include  "GeonetCastToolboxUI\Pages.h"
#include  "GeonetCastToolboxUI\MorePages.h"
#include "Client\Base\Picture.h"
#include "Engine\Base\System\Engine.h"
#include "Client\FormElements\FieldPicture.h"
#include "engine\base\system\RegistrySettings.h"
#include "Engine\Base\DataObjects\URL.h"
#include "GeonetCastToolboxUI\iniFile.h"
//#include "Engine\Base\DataObjects\RemoteObject.h"
#include "Client\ilwis.h"


LRESULT Cmddds(CWnd *wnd, const String& s) {
	new DDSFrm();

	return 1;
}


DDSFrm::DDSFrm() :
GeonetCastFrm("DDS Toolbox", "DDS")
{
}



//---------------------------------


//---------------------------------





