#include "Client\Headers\formelementspch.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h" 
#include "Client\Mapwindow\Drawers\AbstractObjectdrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

AbstractObjectDrawer::AbstractObjectDrawer(DrawerParameters *parms, const String& name) : 
	ComplexDrawer(parms,name),
	object(0)
{
}

AbstractObjectDrawer::~AbstractObjectDrawer() {
	delete object;
}

void AbstractObjectDrawer::prepare(PreparationParameters *pp){
	ComplexDrawer::prepare(pp);
}

IlwisObject AbstractObjectDrawer::getObject() const {
	return IlwisObject::obj(obj->fnObj);
}

void AbstractObjectDrawer::addDataSource(void *bmap, int options) {
	IlwisObject *o = (IlwisObject *)bmap;
	object = new IlwisObject(*o);
	obj = object->pointer();
	name = obj->sName();

}

//--------------------------------


DisplayOptionsForm::DisplayOptionsForm(ComplexDrawer *dr,CWnd *par, const String& title) : 
FormBaseDialog(par,title,fbsApplyButton | fbsBUTTONSUNDER | fbsOKHASCLOSETEXT | fbsSHOWALWAYS),
view((LayerTreeView *)par),
drw(dr)
{
}

void DisplayOptionsForm::OnCancel() {
	apply();
}

int DisplayOptionsForm::exec() {
	return 1;
}

void DisplayOptionsForm::apply() {
}

void DisplayOptionsForm::updateMapView() {
	MapCompositionDoc* doc = view->GetDocument();
	doc->mpvGetView()->Invalidate();
}






