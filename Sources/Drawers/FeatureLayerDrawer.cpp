#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\FeatureDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Engine\Map\Feature.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Domain\Dmvalue.h"
#include "Headers\Hs\Drwforms.hs"

using namespace ILWIS;

FeatureLayerDrawer::FeatureLayerDrawer(DrawerParameters *parms, const String& name) : 
	LayerDrawer(parms,name),
	singleColor(Color(0,176,20)),
	useMask(false),
	managedDrawers(0)
{
	setDrawMethod(drmNOTSET); // default
	setInfo(true);
}

FeatureLayerDrawer::~FeatureLayerDrawer() {
}

void FeatureLayerDrawer::addDataSource(void *bmap,int options) {
	fbasemap = BaseMap((*(BaseMap *)bmap)->fnObj);
}

void *FeatureLayerDrawer::getDataSource() const {
	return (void *)&fbasemap;
}

void FeatureLayerDrawer::getFeatures(vector<Feature *>& features) const {
	features.clear();
	int numberOfFeatures = fbasemap->iFeatures();
	features.resize(numberOfFeatures);
	for(int i=0; i < numberOfFeatures; ++i) {
		Feature *feature = CFEATURE(fbasemap->getFeature(i));
		features.at(i) = feature;
	}
}

void FeatureLayerDrawer::prepare(PreparationParameters *parms){
	/*if ( !isActive() && !isValid())
		return;*/

	clock_t start = clock();
	LayerDrawer::prepare(parms);
	FeatureDataDrawer *mapDrawer = (FeatureDataDrawer *)parentDrawer;
	BaseMapPtr *bmp = mapDrawer->getBaseMap();
	if ( getName() == "Unknown")
		setName(bmp->sName());
	vector<Feature *> features;
	if ( parms->type & RootDrawer::ptGEOMETRY || parms->type & NewDrawer::ptRESTORE){
		bool isAnimation = mapDrawer->getType() == "AnimationDrawer";

		TextLayerDrawer *textLayer = dynamic_cast<TextLayerDrawer *>(getDrawer(223, dtPOST));
		if ( parms->type & RootDrawer::ptGEOMETRY || textLayer == 0) {
			clear();
			ILWIS::DrawerParameters dp(getRootDrawer(), this);
			textLayer = (ILWIS::TextLayerDrawer *)NewDrawer::getDrawer("TextLayerDrawer", "ilwis38",&dp);
			if (textLayer) {
				addPostDrawer(223,textLayer);
				textLayer->setActive(false);

			}
		}
		textLayer->setFont(new OpenGLText(getRootDrawer(),"arial.ttf",12 * textLayer->getFontScale() ,false));

		if ( isAnimation ) {
			getFeatures(features);
		} else {
			mapDrawer->getFeatures(features, parms->index);
		}
		drawers.resize( features.size());
		for(int i=0; i<drawers.size(); ++i)
			drawers.at(i) = 0;
		int count = 0;
		Tranquilizer trq(TR("preparing data"));
		featureMap.clear();
		for(int i=0; i < features.size(); ++i) {
			Feature *feature = features.at(i);
			NewDrawer *pdrw;
			if ( feature && feature->fValid() ){
				ILWIS::DrawerParameters dp(getRootDrawer(), this, true);
				pdrw = createElementDrawer(parms, &dp);
				pdrw->addDataSource(feature);
				PreparationParameters fp((int)parms->type, mapDrawer->getBaseMap()->cs());
				pdrw->prepare(&fp);

				DrawerParameters dp2(getRootDrawer(),textLayer);
				TextDrawer *txtdr =(ILWIS::TextDrawer *)NewDrawer::getDrawer("TextDrawer","ilwis38",&dp2);
//				textLayer->setActive(labelAttribute != "");
				textLayer->addDrawer(txtdr);
				pdrw->setLabelDrawer(txtdr);

				if ( feature->rValue() == rUNDEF)
					pdrw->setActive(false);
				setDrawer(i, pdrw);
				++count;
				if ( i % 100 == 0) {
					trq.fUpdate(i,features.size()); 
				}
			}
		}
	}
	if ( parms->type & NewDrawer::ptRENDER || parms->type & NewDrawer::pt3D || parms->type & NewDrawer::ptRESTORE) {
			PreparationParameters pp(parms);
			pp.type = pp.type & ~NewDrawer::ptGEOMETRY;
			selectedRaws = parms->rowSelect.raws;
			prepareChildDrawers(&pp);
	}
	if ( parms->type & NewDrawer::ptOFFSCREENSTART || parms->type & NewDrawer::ptOFFSCREENEND) {
		TextLayerDrawer *textLayer = dynamic_cast<TextLayerDrawer *>(getDrawer(223, dtPOST));
		if (textLayer)
			textLayer->prepare(parms);
	}

	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Prepared in %2.2f seconds;\n", duration/1000);
}

void FeatureLayerDrawer::prepareChildDrawers(PreparationParameters *parms) {
	FeatureDataDrawer *mapDrawer = (FeatureDataDrawer *)parentDrawer;
	BaseMapPtr *bmp = ((BaseMap*)getDataSource())->ptr();
	Representation rpr = bmp->dm()->rpr();
	Table attTbl = bmp->tblAtt();
	vector<Feature *> features;
	bool isSet = mapDrawer->isSet();
	if ( isSet ) {
		getFeatures(features);
	} else {
		mapDrawer->getFeatures(features, parms->index);
	}
	for(int i = 0; i < drawers.size(); ++i) {
		NewDrawer *pdrw = drawers.at(i);

		RepresentationProperties props;
		if ( pdrw) {
			if ( bmp && bmp->dm().fValid()) {
				Feature *feature = features.at(i);
				if ( labelAttribute == "" || attTbl.fValid() == false ) {
					if (bmp->dvrs().fUseReals()) {
						double val = feature->rValue();
						pdrw->getLabelDrawer()->setText(bmp->dvrs().sValue(val).sTrimSpaces());
					} else {
						long raw = feature->iValue();
						pdrw->getLabelDrawer()->setText(bmp->dvrs().sValueByRaw(raw).sTrimSpaces());
					}
				} else if ( attTbl.fValid()){
					Column col= attTbl->col(labelAttribute);
					if ( col.fValid()) {
						String sV = col->sValue(feature->iValue());
						pdrw->getLabelDrawer()->setText(sV.sTrimSpaces());
					}
				}
				if ( useRpr) {
					if (rpr.fValid()) {
						parms->props = props;
						if ( feature && feature->fValid()){
							rpr->getProperties(feature->rValue(), parms->props);
						}
					}
				}
				pdrw->prepare(parms);
			}
		}
	}
}

String FeatureLayerDrawer::getMask() const{
	return mask;
}

void FeatureLayerDrawer::setMask(const String& sm){
	mask = sm;
}

void FeatureLayerDrawer::setSingleColor(const Color& c){
	singleColor = c;
	setDrawMethod(drmSINGLE);
}

Color FeatureLayerDrawer::getSingleColor() const {
	return singleColor;
}

String FeatureLayerDrawer::store(const FileName& fnView, const String& parentSection) const{
	LayerDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.c_str(),"SingleColor",fnView, singleColor);
	ObjectInfo::WriteElement(parentSection.c_str(),"LabelAttribute",fnView, labelAttribute);
	return parentSection;
}

void FeatureLayerDrawer::load(const FileName& fnView, const String& parentSection){
	LayerDrawer::load(fnView, parentSection);
	ObjectInfo::ReadElement(parentSection.c_str(),"SingleColor",fnView, singleColor);
	ObjectInfo::ReadElement(parentSection.c_str(),"LabelAttribute",fnView, labelAttribute);
}

String FeatureLayerDrawer::getInfo(const Coord& c) const {
	if ( !hasInfo() || !isActive() || c.fUndef())
		return "";
	Coord crd = c;
	SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
	BaseMapPtr *bmptr = mapDrawer->getBaseMap(mapDrawer->getCurrentIndex());
	if (bmptr->cs() != rootDrawer->getCoordinateSystem())
	{
		crd = bmptr->cs()->cConv(rootDrawer->getCoordinateSystem(), c);
	}
	vector<String> infos = bmptr->vsValue(crd, 0);
	if (useAttColumn) {
		int idx = 0;
		vector<Geometry *> geoms = bmptr->getFeatures(crd);
		for(int i=0; i < geoms.size() && idx < infos.size(); ++i) {
			Feature *f = CFEATURE(geoms[i]);
			if ( f) {
				long raw = f->iValue();
				if ( getAtttributeColumn().fValid() && raw != iUNDEF) {
					String v = getAtttributeColumn()->sValue(raw);
					for (int iLen = v.length(); iLen && v[iLen-1] == ' '; --iLen) // trim spaces on the right
						v[iLen-1] = 0;
					infos[idx] &= ": ";
					infos[idx] &= v;
					DomainValue* dv = getAtttributeColumn()->dm()->pdv();
					if (0 != dv && dv->fUnit())
						infos[idx] &= String(" %S", dv->sUnit());
				}
			}
			++idx;
		}
	}
	String info;
	DomainValue* dv = bmptr->dm()->pdv();
	int count = 0;
	for(int i = 0; i < infos.size(); ++i) {
		String s = infos[i].sTrimSpaces(true);
		if ( s == "?")
			continue;
		info += count == 0 ? s : ";" + s;
		if (0 != dv && dv->fUnit())
			info &= String(" %S", dv->sUnit());
		++count;
	}
	return info;
}

bool FeatureLayerDrawer::useRaw() const {
	if ( fbasemap.fValid()) {
		return !fbasemap->dvrs().fUseReals();
	}
	return false;
}

void FeatureLayerDrawer::select(const CRect& rect, vector<long> & selectedRaws, SelectionMode selectionMode) {
	if (rootDrawer->is3D())
		return;

	if (featureMap.size() != getDrawerCount()) { // we have postponed this to the first time the user "selects" .. move to "prepare" if needed
		featureMap.clear();
		for(int index = 0; index < getDrawerCount(); ++index) {
			SimpleDrawer *dr = (SimpleDrawer*)getDrawer(index);
			if ( dr) {
				long v = dr->getFeature()->iValue();
				if ( v != iUNDEF)
					featureMap.insert(pair<long, SimpleDrawer*>(v, dr));
			}
		}
	}

	SpatialDataDrawer *datadrw = dynamic_cast<SpatialDataDrawer *>(getParentDrawer());
	if ( datadrw) {
		CoordBounds cbSelect = rootDrawer->getCoordBoundsZoom();
		RowCol rectWindow = rootDrawer->getViewPort();
		Coord c1,c2;
		c1.x = cbSelect.cMin.x + cbSelect.width() * rect.left / (double)rectWindow.Col; // determine zoom rectangle in GL coordinates
		c1.y = cbSelect.cMax.y - cbSelect.height() * rect.top / (double)rectWindow.Row;
		c2.x = cbSelect.cMin.x + cbSelect.width() * rect.right / (double)rectWindow.Col;
		c2.y = cbSelect.cMax.y - cbSelect.height() * rect.bottom / (double)rectWindow.Row;
		c1.z = c2.z = 0;

		cbSelect = CoordBounds (c1,c2);

		vector<Feature *> features = datadrw->getBaseMap()->getFeatures(cbSelect, false); // the "complete" parameter forces the user to make the selection box too big, causing inaccurate selection (complete is only applicable to segments and polygons)
		if (selectionMode == SELECTION_NEW) {
			selectedRaws.clear();
			ComplexDrawer::select(false); // all child drawers
			for(int i = 0; i < features.size(); ++i) {
				Feature *f = features[i];
				if (!f || f->fValid() == false)
					continue;
				selectedRaws.push_back(f->iValue());
				pair<multimap<long, SimpleDrawer*>::iterator, multimap<long, SimpleDrawer*>::iterator> range = featureMap.equal_range(f->iValue());
				for (multimap<long, SimpleDrawer*>::iterator it = range.first; it != range.second; ++it)
					it->second->select(true);
			}
		} else if (selectionMode == SELECTION_ADD) {
			vector<long> newlySelectedRaws;
			for(int i = 0; i < features.size(); ++i) {
				Feature *f = features[i];
				if (!f || f->fValid() == false)
					continue;
				newlySelectedRaws.push_back(f->iValue());
				pair<multimap<long, SimpleDrawer*>::iterator, multimap<long, SimpleDrawer*>::iterator> range = featureMap.equal_range(f->iValue());
				for (multimap<long, SimpleDrawer*>::iterator it = range.first; it != range.second; ++it)
					it->second->select(true);
			}
			sort(selectedRaws.begin(), selectedRaws.end());
			sort(newlySelectedRaws.begin(), newlySelectedRaws.end());
			vector<long> tmp;
			set_union(selectedRaws.begin(), selectedRaws.end(), newlySelectedRaws.begin(), newlySelectedRaws.end(), back_inserter(tmp));
			selectedRaws.swap(tmp);
		} else if (selectionMode == SELECTION_REMOVE) {
			vector<long> newlySelectedRaws;
			for(int i = 0; i < features.size(); ++i) {
				Feature *f = features[i];
				if (!f || f->fValid() == false)
					continue;
				newlySelectedRaws.push_back(f->iValue());
				pair<multimap<long, SimpleDrawer*>::iterator, multimap<long, SimpleDrawer*>::iterator> range = featureMap.equal_range(f->iValue());
				for (multimap<long, SimpleDrawer*>::iterator it = range.first; it != range.second; ++it)
					it->second->select(false);
			}
			sort(selectedRaws.begin(), selectedRaws.end());
			sort(newlySelectedRaws.begin(), newlySelectedRaws.end());
			vector<long> tmp;
			set_difference(selectedRaws.begin(), selectedRaws.end(), newlySelectedRaws.begin(), newlySelectedRaws.end(), back_inserter(tmp));
			selectedRaws.swap(tmp);
		}
	}
}

void FeatureLayerDrawer::setLabelAttribute(const String& col){
	labelAttribute = col;
}
String FeatureLayerDrawer::getLabelAttribute() const{
	return labelAttribute;
}


