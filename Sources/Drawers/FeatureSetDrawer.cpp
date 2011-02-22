#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldIntSlider.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Mapwindow\Drawers\SimpleDrawer.h" 
#include "Client\Ilwis.h"
#include "Client\FormElements\fldcol.h"
#include "client\formelements\fldrpr.h"
#include "client\formelements\fentvalr.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapPaneView.h"
#include "Engine\Map\basemap.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Client\FormElements\fldcolor.h"
#include "Drawers\SetDrawer.h"
#include "Drawers\FeatureSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Drawers\AnimationDrawer.h"
//#include "Client\Mapwindow\Drawers\PointMapDrawerForm.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

FeatureSetDrawer::FeatureSetDrawer(DrawerParameters *parms, const String& name) : 
	SetDrawer(parms,name),
	singleColor(Color(0,176,20)),
	useMask(false),
	colorItem(0)
{
	setDrawMethod(drmNOTSET); // default
	setInfo(false);
}

FeatureSetDrawer::~FeatureSetDrawer() {

}

void FeatureSetDrawer::addDataSource(void *bmap,int options) {
	fbasemap.SetPointer((BaseMapPtr *)bmap);
}

void *FeatureSetDrawer::getDataSource() const {
	return (void *)&fbasemap;
}

void FeatureSetDrawer::getFeatures(vector<Feature *>& features) const {
	features.clear();
	int numberOfFeatures = fbasemap->iFeatures();
	features.resize(numberOfFeatures);
	for(int i=0; i < numberOfFeatures; ++i) {
		Feature *feature = CFEATURE(fbasemap->getFeature(i));
		features.at(i) = feature;
	}
}

void FeatureSetDrawer::prepare(PreparationParameters *parms){
	if ( !isActive())
		return;

	clock_t start = clock();
	SetDrawer::prepare(parms);
	FeatureLayerDrawer *mapDrawer = (FeatureLayerDrawer *)parentDrawer;
	if ( getName() == "Unknown")
		setName(mapDrawer->getBaseMap()->sName());
	vector<Feature *> features;
	if ( parms->type & RootDrawer::ptGEOMETRY | parms->type & NewDrawer::ptRESTORE){
		bool isAnimation = mapDrawer->getType() == "AnimationDrawer";
		if ( isAnimation ) {
			getFeatures(features);
		} else {
			mapDrawer->getFeatures(features);
		}
		clear();
		drawers.resize( features.size());
		for(int i=0; i<drawers.size(); ++i)
			drawers.at(i) = 0;
		int count = 0;
		Tranquilizer trq(TR("preparing data"));
		for(int i=0; i < features.size(); ++i) {
			Feature *feature = features.at(i);
			NewDrawer *pdrw;
			if ( feature && feature->fValid()){
				ILWIS::DrawerParameters dp(getRootDrawer(), this);
				pdrw = createElementDrawer(parms, &dp);
				pdrw->addDataSource(feature);
				/*PreparationParameters fp((int)parms->type, 0);
				pp->file = parms->file;*/
				pdrw->prepare(parms);
				setDrawer(i, pdrw);
				++count;
				if ( i % 100 == 0) {
					trq.fUpdate(i,features.size()); 
				}
			}
		}
		if ( count < features.size())
			drawers.resize(count);
	} else {
		if ( parms->type & NewDrawer::ptRENDER || parms->type & NewDrawer::pt3D) {
			prepareChildDrawers(parms);
		}
	}
	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Prepared in %2.2f seconds;\n", duration/1000);
}

String FeatureSetDrawer::getMask() const{
	return mask;
}

void FeatureSetDrawer::setMask(const String& sm){
	mask = sm;
}

void FeatureSetDrawer:: setSingleColor(const Color& c){
	singleColor = c;
	setDrawMethod(drmSINGLE);
}

Color FeatureSetDrawer::getSingleColor() const {
	return singleColor;
}

String FeatureSetDrawer::store(const FileName& fnView, const String& parentSection) const{
	SetDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.scVal(),"SingleColor",fnView, singleColor);
	return parentSection;
}

void FeatureSetDrawer::load(const FileName& fnView, const String& parentSection){
	SetDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.scVal(),"SingleColor",fnView, singleColor);

}

//-------------------------------------- UI --------------------------------------------------------
HTREEITEM FeatureSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent) {
	HTREEITEM hti = SetDrawer::configure(tv,parent);
	threeDItem = 0;
	DisplayOptionTreeItem *item;
	//if ( getUICode() == NewDrawer::ucALL) {
	//	item = new DisplayOptionTreeItem(tv,parent,this,(DisplayOptionItemFunc)&FeatureSetDrawer::displayOptionMask);
	//	HTREEITEM maskItem = InsertItem("Mask", "Mask", item, (int)useMask);
	//	InsertItem(tv, maskItem,String("Value : %S",mask),"Mask");
	//}
	portrayalItem = findTreeItemByName(tv,parent,"Portrayal");
	if ( portrayalItem) {
		bool useSingleColor = getDrawMethod() == NewDrawer::drmSINGLE;
		bool useRpr = getDrawMethod() == NewDrawer::drmRPR;
		colorItem = new DisplayOptionColorItem("Single color", tv,portrayalItem,this,
						(DisplayOptionItemFunc)&FeatureSetDrawer::displayOptionSingleColor,0,colorCheck,(SetCheckFunc)&FeatureSetDrawer::setSingleColorMap);
		colorItem->setColor(singleColor);
		HTREEITEM singleColorItem = InsertItem("Single Color","SingleColor",colorItem, useSingleColor & !useRpr);
		item = new DisplayOptionTreeItem(tv,portrayalItem,this, 0,0,colorCheck);									
		HTREEITEM multiColorItem = InsertItem("Multiple Colors","MultipleColors",item, !useSingleColor & !useRpr);
	}

	return hti;
}

void FeatureSetDrawer::setSingleColorMap(void *value, LayerTreeView *tree) {
	bool v = *(bool *)value;
	if ( v) {
		PreparationParameters parm(NewDrawer::ptRENDER, 0);
		prepareChildDrawers(&parm);
		getRootDrawer()->getDrawerContext()->doDraw();
	}
}

void FeatureSetDrawer::displayOptionMask(CWnd *parent) {
	new SetMaskForm(parent, this);
}

void FeatureSetDrawer::displayOptionSingleColor(CWnd *parent) {
	new SetSingleColorForm(parent, this);
}
//------------------------------------------------
SetSingleColorForm::SetSingleColorForm(CWnd *wPar, FeatureSetDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Single draw color for %S",dr->getName())),
	c(((FeatureSetDrawer *)dr)->getSingleColor())
{
	fc = new FieldColor(root, "Draw color", &c);
	create();
}

void  SetSingleColorForm::apply() {
	fc->StoreData();
	((FeatureSetDrawer *)drw)->setSingleColor(c);
	((FeatureSetDrawer *)drw)->colorItem->setColor(c);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepareChildDrawers(&parm);
	updateMapView();
}

//------------------------------------------------
SetMaskForm::SetMaskForm(CWnd *wPar, FeatureSetDrawer *dr) : 
	DisplayOptionsForm(dr, wPar,String("Mask for %S",dr->getName())),
	mask(dr->getMask())
{
	fs = new FieldString(root, "Mask", &mask);
	create();
}

void SetMaskForm::apply() {
	fs->StoreData();
	((FeatureSetDrawer *)drw)->setMask(mask);
	PreparationParameters parm(NewDrawer::ptRENDER, 0);
	drw->prepare(&parm);
	updateMapView();
}

