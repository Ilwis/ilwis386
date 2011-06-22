#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Representation\Rpr.h"
#include "Engine\Spatialreference\gr.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "drawers\linedrawer.h"
#include "Engine\Drawers\OpenGLText.h"
#include "Engine\Drawers\TextDrawer.h"
#include "Drawers\AnnotationDrawers.h"
#include "Engine\Domain\dmclass.h"
#include "DrawingColor.h"
#include "Engine\Base\Round.h"

using namespace ILWIS;

AnnotationDrawer::AnnotationDrawer(DrawerParameters *parms, const String& name) : ComplexDrawer(parms,name) 
{
}

String AnnotationDrawer::store(const FileName& fnView, const String& parentSection) const{
	ComplexDrawer::store(fnView, parentSection);

	return parentSection;
}

void AnnotationDrawer::load(const FileName& fnView, const String& parentSection){
	ComplexDrawer::load(fnView, parentSection);
}

//-------------------------------------------------------
AnnotationLegendDrawer::AnnotationLegendDrawer(DrawerParameters *parms, const String& name) : 
AnnotationDrawer(parms, name),
drawOutsideBox(false),
vertical(true),
bgColor(Color(255,255,255)),
useBackground(false),
columns(1),
includeName(true),
texts(0)
{
}

void AnnotationLegendDrawer::setOrientation(bool yesno) {
	vertical = yesno;
	if (vertical) {
		CoordBounds cb = getRootDrawer()->getCoordBoundsZoom();
		cbBox.MinX() = cb.MinX() + cb.width()/ 50;
		cbBox.MinY() = cb.MinY() + cb.height()/ 50;
		double w = cb.width() / 10.0;
		double h = cb.height() / 3.0;
		cbBox.MaxX() = cbBox.MinX() + w * columns;
		cbBox.MaxY() = cbBox.MinY() + h / columns;
	} else {
		CoordBounds cb = getRootDrawer()->getCoordBoundsZoom();
		cbBox.MinX() = cb.MinX() + cb.width()/ 50;
		cbBox.MinY() = cb.MinY() + cb.height()/ 40;
		double w = cb.width() / 3.0;
		double h = cb.height() / 10.0;
		cbBox.MaxX() = cbBox.MinX() + w;
		cbBox.MaxY() = cbBox.MinY() + h;
	}
}

int AnnotationLegendDrawer::noOfColumns() const {
	return columns;
}

void AnnotationLegendDrawer::setNoOfColumns(int n) {
	columns  = n;
}

bool AnnotationLegendDrawer::getIncludeName() const {
	return includeName;
}

void AnnotationLegendDrawer::setInlcudeName(bool yesno) {
	includeName = yesno;
}


bool AnnotationLegendDrawer::getOrientation() const{
	return vertical;
}

void AnnotationLegendDrawer::setDrawBorder(bool yesno){
	drawOutsideBox = yesno;
}

bool AnnotationLegendDrawer::getDrawBorder() const{
	return drawOutsideBox;
}

CoordBounds AnnotationLegendDrawer::getBox() const {
	return cbBox;
}

void AnnotationLegendDrawer::setBox(const CoordBounds& cb) {
	cbBox = cb;
}

void AnnotationLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY) {
		setOrientation(vertical);
		DrawerParameters dp(getRootDrawer(), this);
		if ( texts != 0) {
			removeDrawer(texts->getId(), true);
		}
		texts = new TextLayerDrawer(&dp,"LegendTexts");
		texts->setFont(new OpenGLText(getRootDrawer(),"arial.ttf",10,true));
		addPostDrawer(100,texts);

		SpatialDataDrawer *spdr = (SpatialDataDrawer *)(getParentDrawer()->getParentDrawer());
		BaseMapPtr *bmp = spdr->getBaseMap();

		dm = Domain(bmp->dm()->fnObj);
		fnName = bmp->fnObj;
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		DrawerParameters dp(getRootDrawer(), texts);
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(101,ComplexDrawer::dtPOST);
		if ( !txtdr) {
			txtdr = new TextDrawer(&dp,"LegendTitleText");
			txtdr->setText(fnName.sFile);
			texts->addPostDrawer(101,txtdr);
		}
		txtdr->setText(fnName.sFile);
	}

}

bool AnnotationLegendDrawer::draw( const CoordBounds& cbArea) const{
	bool is3D = getRootDrawer()->is3D(); 

	//double z0 = cdrw->getZMaker()->getZ0(getRootDrawer()->is3D());
	double z0 = getRootDrawer()->getZMaker()->getZ0(is3D);
	if (is3D) // supporting drawers need to be slightly above the level of the "main" drawer. OpenGL won't draw them correct if they are in the same plane
		z0 +=  z0;

	double z = is3D ? z0 : 0;
	if ( includeName) {
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(101,ComplexDrawer::dtPOST);
		if ( txtdr) {
			double h = txtdr->getHeight();
			txtdr->setCoord(Coord(cbBox.MinX(),cbBox.MaxY() + h * 0.8, z));
		}
	}

	if ( useBackground) {
		glColor4d(bgColor.redP(), bgColor.greenP(), bgColor.blueP(), getTransparency());
		glBegin(GL_POLYGON);
		Coordinate c(cbBox.MinX(), cbBox.MinY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MinX(), cbBox.MaxY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MaxX(), cbBox.MaxY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MaxX(), cbBox.MinY());
		glVertex3d( c.x, c.y, z);
		glEnd();
	}

	if ( drawOutsideBox) {
		glColor4f(lproperties.drawColor.redP(),lproperties.drawColor.greenP(), lproperties.drawColor.blueP(),getTransparency() );
		glLineWidth(lproperties.thickness);
		glBegin(GL_LINE_STRIP);
		Coordinate c(cbBox.MinX(), cbBox.MinY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MinX(), cbBox.MaxY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MaxX(), cbBox.MaxY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MaxX(), cbBox.MinY());
		glVertex3d( c.x, c.y, z);
		c = Coordinate(cbBox.MinX(), cbBox.MinY());
		glVertex3d( c.x, c.y, z);
		glEnd();
	}
	return true;
}

void AnnotationLegendDrawer::setText(const vector<String>& v, int count, const Coord& c) const {
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( count);
	if ( txt) {
		txt->setCoord(c);
		txt->setText(v[count]);
		txt->setActive(true);
	}
}


String AnnotationLegendDrawer::store(const FileName& fnView, const String& parentSection) const{
	AnnotationDrawer::store(fnView, parentSection);

	return parentSection;
}

void AnnotationLegendDrawer::load(const FileName& fnView, const String& parentSection){
	AnnotationDrawer::load(fnView, parentSection);
}

bool AnnotationLegendDrawer::getUseBackBackground() const {
	return useBackground;
}

void AnnotationLegendDrawer::setUseBackground(bool yesno){
	useBackground = yesno;
}

Color AnnotationLegendDrawer::getBackgroundColor() const {
	return bgColor;
}

void AnnotationLegendDrawer::setBackgroundColor(const Color& clr){
	bgColor = clr;
}
//------------------------------------------------------
ILWIS::NewDrawer *createAnnotationClassLegendDrawer(DrawerParameters *parms) {
	return new AnnotationClassLegendDrawer(parms);
}


AnnotationClassLegendDrawer::AnnotationClassLegendDrawer(DrawerParameters *parms) : AnnotationLegendDrawer(parms,"AnnotationClassLegendDrawer"),maxw(0), cellWidth(0)
{
}

void AnnotationClassLegendDrawer::setActiveClasses(const vector<int>& rws) {
	raws.clear();
	DrawingColor dc((LayerDrawer *)getParentDrawer());
	for(int i=0; i < texts->getDrawerCount(); ++i) {
		texts->getDrawer(i)->setActive(false);
	}
	for(int i = 0; i < rws.size(); ++i) {
		long iRaw = dm->pdc()->iKey(i+1);
		if ( iRaw == iUNDEF)
			continue;
		Color clr =dc.clrRaw(iRaw,NewDrawer::drmRPR);
		raws.push_back(RawInfo(iRaw, clr));
		String txt = dm->pdc()->sValueByRaw(iRaw);
		texts->getDrawer(i)->setActive(true);
		((TextDrawer *)texts->getDrawer(i))->setText(txt);
	}
}

void AnnotationClassLegendDrawer::getActiveClasses(vector<int>& rws) const {
	rws.clear();
	for(int i = 0; i < raws.size(); ++i) {
		rws.push_back(raws[i].raw);
	}
}

void AnnotationClassLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationLegendDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY) {
		columns = 1;
		maxw = 0;
		cellWidth = 0;
		SpatialDataDrawer *spdr = (SpatialDataDrawer *)(getParentDrawer()->getParentDrawer());
		BaseMapPtr *bmp = spdr->getBaseMap();
		DrawingColor dc((LayerDrawer *)getParentDrawer());
		DrawerParameters dp(getRootDrawer(), texts);
		raws.clear();
		for(int i =  dm->pdc()->iSize(); i >= 0 ; --i) {
			long iRaw = dm->pdc()->iKey(i+1);
			if ( iRaw == iUNDEF)
				continue;
			Color clr =dc.clrRaw(iRaw,NewDrawer::drmRPR);
			String txt = dm->pdc()->sValueByRaw(iRaw);
			raws.push_back(RawInfo(iRaw,clr));
			TextDrawer *txtdr = new TextDrawer(&dp,"LegendText");
			txtdr->setText(txt);
			texts->addDrawer(txtdr);
			double h = txtdr->getHeight() * 0.5;
			maxw = max(maxw, h * txt.size());
		}
		cellWidth = cbBox.width() / 3;
	}
	if ( pp->type & NewDrawer::ptRENDER) {
		CoordBounds cb = getRootDrawer()->getCoordBoundsZoom();
		cbBox = CoordBounds(cbBox.cMin, 
			Coord(cbBox.MinX() +  (cellWidth + maxw) * columns, 
				   cbBox.MinY() + raws.size() * cb.height() / (40 * columns)));
	}

	//}
}

bool AnnotationClassLegendDrawer::draw( const CoordBounds& cbArea) const{
	if ( !isActive())
		return false;

	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cbBox))
		return false;

	AnnotationLegendDrawer::draw(cbArea);
	drawPreDrawers(cbArea);
	double yy = cbBox.height() / (raws.size() * 1.1);
	double hh = cbBox.height();
	yy *= (double)columns;
	CoordBounds cbCell(Coord(cbBox.MinX(),cbBox.MinY()),Coord(cbBox.MinX() + cellWidth, cbBox.MinY() + yy));
	double z = 0;
	double shifty = columns * cbBox.height() / raws.size();
	glPushMatrix();
	int split = raws.size() / columns;
	for(int i=0 ; i < raws.size(); ++i) {
		glColor3d(raws[i].clr.redP(), raws[i].clr.greenP(), raws[i].clr.blueP());
		glBegin(GL_POLYGON);
		glVertex3d(cbCell.MinX(), cbCell.MinY(), z);
		glVertex3d(cbCell.MinX(), cbCell.MaxY(), z);
		glVertex3d(cbCell.MaxX(), cbCell.MaxY(), z);
		glVertex3d(cbCell.MaxX(), cbCell.MinY(), z);
		TextDrawer *txtdr = (TextDrawer *)texts->getDrawer(i);
		//txtdr->setCoord(Coord(cbBox.MinX() + cbBox.width() / 4.8, cbCell.MinY() + cbCell.height() / 3.0,z));
		txtdr->setCoord(Coord(cbCell.MinX() + cellWidth * 1.1, cbCell.MinY() + cbCell.height() / 3.0,z));

		glEnd();
		glColor3d(0,0,0);
		glBegin(GL_LINE_STRIP);
			glVertex3d(cbCell.MinX(), cbCell.MinY(), z);
			glVertex3d(cbCell.MinX(), cbCell.MaxY(), z);
			glVertex3d(cbCell.MaxX(), cbCell.MaxY(), z);
			glVertex3d(cbCell.MaxX(), cbCell.MinY(), z);
			glVertex3d(cbCell.MinX(), cbCell.MinY(), z);
		glEnd();
		cbCell.MinY() += shifty;
		cbCell.MaxY() += shifty;
		if ( (i + 1) % split == 0) {
			cbCell.MinY() = cbBox.MinY();
			cbCell.MaxY() = cbBox.MinY() + yy;
			cbCell.MinX() += cellWidth + maxw;
			cbCell.MaxX() += cellWidth + maxw;
		}
	}
	

	drawPostDrawers(cbArea);

	return true;
}

String AnnotationClassLegendDrawer::store(const FileName& fnView, const String& parentSection) const{
	AnnotationLegendDrawer::store(fnView, parentSection);


	return parentSection;
}

void AnnotationClassLegendDrawer::load(const FileName& fnView, const String& parentSection){
	AnnotationLegendDrawer::load(fnView, parentSection);
}
//-------------------------------------------------------
ILWIS::NewDrawer *createAnnotationValueLegendDrawer(DrawerParameters *parms) {
	return new AnnotationValueLegendDrawer(parms);
}

AnnotationValueLegendDrawer::AnnotationValueLegendDrawer(DrawerParameters *parms) : 
AnnotationLegendDrawer(parms,"AnnotationValueLegendDrawer"),
noTicks(5)
{
}

void AnnotationValueLegendDrawer::prepare(PreparationParameters *pp) {
	AnnotationLegendDrawer::prepare(pp);
	if ( pp->type & NewDrawer::ptGEOMETRY) {
		for(int i=0; i < 10 ; ++i) {
			DrawerParameters dp(getRootDrawer(), texts);
			TextDrawer *txt = new TextDrawer(&dp,"LegendText");
			txt->setActive(false);
			texts->addDrawer(txt);
		}
	}
}

bool AnnotationValueLegendDrawer::draw( const CoordBounds& cbArea) const{

	if ( !isActive())
		return false;

	if ( !getRootDrawer()->getCoordBoundsZoom().fContains(cbBox))
		return false;

	AnnotationLegendDrawer::draw(cbArea);
	drawPreDrawers(cbArea);

	bool is3D = getRootDrawer()->is3D(); 
	double transp = getTransparency();

	//double z0 = cdrw->getZMaker()->getZ0(getRootDrawer()->is3D());
	double z0 = getRootDrawer()->getZMaker()->getZ0(is3D);
	if (is3D) // supporting drawers need to be slightly above the level of the "main" drawer. OpenGL won't draw them correct if they are in the same plane
		z0 +=  z0;

	double z = is3D ? z0 : 0;

	double noOfRect = 100.0;
	vector<String> values = makeRange((LayerDrawer *)getParentDrawer());
	RangeReal rr(values[0].rVal(), values[values.size()- 1].rVal());
	CoordBounds cbInner = cbBox;
	if ( vertical) {
		drawVertical(cbInner, rr, z, values);
	} else {
		drawHorizontal(cbInner, rr, z, values);
	}
	
	glColor4f(0,0, 0, transp );
	glBegin(GL_LINE_STRIP);
	Coordinate c(cbInner.MinX(), cbInner.MinY());
	glVertex3d( c.x, c.y, z);
	c = Coordinate(cbInner.MinX(), cbInner.MaxY());
	glVertex3d( c.x, c.y, z);
	c = Coordinate(cbInner.MaxX(), cbInner.MaxY());
	glVertex3d( c.x, c.y, z);
	c = Coordinate(cbInner.MaxX(), cbInner.MinY());
	glVertex3d( c.x, c.y, z);
	c = Coordinate(cbInner.MinX(), cbInner.MinY());
	glVertex3d( c.x, c.y, z);
	glEnd();


	drawPostDrawers(cbArea);

	return true;

}

void AnnotationValueLegendDrawer::drawVertical(CoordBounds& cbInner, const RangeReal& rr, double z, const vector<String>& values) const{
	int count = 1;
	DrawingColor dc((LayerDrawer *)getParentDrawer());
	cbInner.MaxX() = cbInner.MinX() + cbBox.width() / 3;
	double startx = cbInner.MinX();
	double starty = cbInner.MinY();
	double endx = startx + cbInner.width();
	double rStep = rr.rWidth() / 100.0;
	double rV = rr.rLo() + 0.5 * rStep;
	setText(values,0,Coord(endx + cbBox.width() / 15.0, starty + cbInner.height() / 100.0,z));
	for(int i=0; i < 100; ++i) {
		Color clr = dc.clrVal(rV);
		glColor4f(clr.redP(),clr.greenP(), clr.blueP(), getTransparency() );
		double endy = starty + cbInner.height() / 100.0;
		glBegin(GL_POLYGON);
		glVertex3d(startx,starty,z);
		glVertex3d(startx,endy,z);
		glVertex3d(endx,endy,z);
		glVertex3d(endx,starty,z);
		glEnd();
		if ( values[count].rVal() <= rV) { 
			setText(values, count, Coord(endx + cbBox.width() / 15.0, starty,z));
			glColor4f(0,0, 0, getTransparency() );
			glBegin(GL_LINE_STRIP);
			glVertex3d(endx,endy,z);
			glVertex3d(endx + cbBox.width() / 20.0,endy,z);
			glEnd();
			++count;
		}
		starty = endy;
		rV += rStep;
	}
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( values.size()-1);
	double h = txt->getHeight() * 0.9;
	setText(values,values.size()-1,Coord(cbInner.MinX() + cbInner.width() + cbBox.width() / 15.0, cbBox.MaxY() - h,z));
}

void AnnotationValueLegendDrawer::drawHorizontal(CoordBounds& cbInner, const RangeReal& rr, double z, const vector<String>& values) const{
	int count = 1;
	double shifty = 5.0;
	DrawingColor dc((LayerDrawer *)getParentDrawer());
	cbInner.MinY() = cbInner.MaxY() - cbBox.height() / 2;
	double startx = cbInner.MinX();
	double starty = cbInner.MaxY();
	double endy = starty - cbInner.height();
	double rStep = rr.rWidth() / 100.0;
	double rV = rr.rLo() + 0.5 * rStep;
	setText(values,0,Coord(startx, endy - cbBox.height() / shifty,z));
	for(int i=0; i < 100; ++i) {
		Color clr = dc.clrVal(rV);
		glColor4f(clr.redP(),clr.greenP(), clr.blueP(), getTransparency() );
		double endx = startx + cbInner.width() / 100.0;
		glBegin(GL_POLYGON);
		glVertex3d(startx,starty,z);
		glVertex3d(startx,endy,z);
		glVertex3d(endx,endy,z);
		glVertex3d(endx,starty,z);
		glEnd();
		if ( values[count].rVal() <= rV) { 
			setText(values, count, Coord( startx, endy - cbBox.height() / shifty,z));
			glColor4f(0,0, 0, getTransparency() );
			glBegin(GL_LINE_STRIP);
			glVertex3d(endx,endy,z);
			glVertex3d(endx ,endy - cbBox.height() / (shifty + 8),z);
			glEnd();
			++count;
		}
		startx = endx;
		rV += rStep;
	}
	TextDrawer *txt = (TextDrawer *)texts->getDrawer( values.size()-1);
	double h = txt->getHeight() * 0.9;
	String s(values[values.size()-1]);
	setText(values,values.size()-1,Coord(cbInner.MaxX() - s.size() * h/2, cbInner.MaxY( ) - cbInner.height() - cbBox.height() / shifty,z));
}

String AnnotationValueLegendDrawer::store(const FileName& fnView, const String& parentSection) const{
	AnnotationLegendDrawer::store(fnView, parentSection);

	return parentSection;
}

vector<String> AnnotationValueLegendDrawer::makeRange(LayerDrawer *dr) const{
	vector<String> values;
	SpatialDataDrawer *mapDrawer = dynamic_cast<SpatialDataDrawer *>(dr->getParentDrawer()); // case animation drawer

	DomainValueRangeStruct dvs = mapDrawer->getBaseMap()->dvrs();

	//SetDrawer *adrw = dynamic_cast<SetDrawer *>(drawer);
	//if ( adrw) {
	//	sdrw = (LayerDrawer *)adrw->getDrawer(0);
	//}

	if ( dr->useAttributeColumn() && dr->getAtttributeColumn().fValid()) {
		dvs = dr->getAtttributeColumn()->dvrs();
	}

	RangeReal rr = dr->getStretchRangeReal();
	RangeReal rmd = roundRange(rr.rLo(), rr.rHi());
	double rVal = rRound(rmd.rWidth()/ noTicks);
	double rStart = rRound(rmd.rLo());
	if ( rStart > rmd.rLo())
		rStart = rmd.rLo();
	double rHi = rmd.rHi();
	if (dvs.rValue(rHi) == rUNDEF)
		rHi = rmd.rHi();
	bool fImage = dvs.dm()->pdi() != 0;

	for (double v = rStart; v <= rHi; v += rVal) {
		String sName = dvs.sValue(v);
		if ( fImage && v + rVal > 255) {
			sName = "255";
		}
		values.push_back(sName);

	}	
	return values;
}

void AnnotationValueLegendDrawer::load(const FileName& fnView, const String& parentSection){
	AnnotationLegendDrawer::load(fnView, parentSection);
}

//-------------------------------------------------------
AnnotationClassElementDrawer::AnnotationClassElementDrawer(DrawerParameters *parms) : SimpleDrawer(parms, "AnnotationClassElementDrawer")
{
}
