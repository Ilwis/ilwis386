#include "Headers\toolspch.h"
#include "Engine\Drawers\RootDrawer.h"
#include "CubeDrawer.h"
//#include "Engine\Drawers\SimpleDrawer.h"
//#include "Engine\Drawers\TextDrawer.h"


using namespace ILWIS;

ILWIS::NewDrawer *createCubeDrawer(DrawerParameters *parms) {
	return new CubeDrawer(parms);
}

CubeDrawer::CubeDrawer(DrawerParameters *parms)
: ComplexDrawer(parms,"CubeDrawer")
, font(0)
{
	setTransparency(1); // opaque
}

CubeDrawer::~CubeDrawer() {
	if (font)
		delete font;
}

void CubeDrawer::prepare(PreparationParameters *pp) {
	if ((pp->type & RootDrawer::ptGEOMETRY) || (pp->type & NewDrawer::ptRESTORE)) { 
		if (font == 0)
			font = new OpenGLText(rootDrawer, "arial.ttf", 28, true, 0, 0, true);
		cube = rootDrawer->getMapCoordBounds();
		CoordBounds cbMap = cube;
		if (cube.width() > cube.height()) {
			double deltay = cube.width() - cube.height();
			cube.cMin.y = cube.cMin.y - deltay / 2.0;
			cube.cMax.y = cube.cMax.y + deltay / 2.0;
			cube.cMin.z = 0;
			cube.cMax.z = cube.width();
		} else {
			double deltax = cube.height() - cube.width();
			cube.cMin.x = cube.cMin.x - deltax / 2.0;
			cube.cMax.x = cube.cMax.x + deltax / 2.0;
			cube.cMin.z = 0;
			cube.cMax.z = cube.height();
		}
		cbMap.MaxZ() = cube.cMax.z;
		cbMap.MinZ() = cube.cMin.z;
		rootDrawer->setCoordBoundsMap(cube);
		CoordSystem csy = rootDrawer->getCoordinateSystem();
		String sMin = csy->sValue(cube.cMin, 0);
		String sMax = csy->sValue(cube.cMax, 0);
		if (csy->pcsLatLon()) {
			sxMin = sMin.sTail(",").sTrimSpaces();
			syMin = sMin.sHead(",").sTrimSpaces();
			sxMax = sMax.sTail(",").sTrimSpaces();
			syMax = sMax.sHead(",").sTrimSpaces();
		} else {
			sxMin = sMin.sTail("(");
			syMin = sxMin.sTail(",").sHead(")").sTrimSpaces();
			sxMin = sxMin.sHead(",").sTrimSpaces();
			sxMax = sMax.sTail("(");
			syMax = sxMax.sTail(",").sHead(")").sTrimSpaces();
			sxMax = sxMax.sHead(",").sTrimSpaces();
		}
		if (timeBounds != 0) {
			stMin = timeBounds->tMin().toString();
			stMax = timeBounds->tMax().toString();
		}
		NewDrawer *ndrw = getRootDrawer()->getBackgroundDrawer();
		ndrw->setTransparency(0.85);
	}
}


bool CubeDrawer::draw(const CoordBounds& cbArea) const{
	if ( !isActive() || !isValid())
		return false;

	glPushMatrix();
	glTranslated(cube.cMin.x + cube.width() / 2.0, cube.cMin.y + cube.height() / 2.0, cube.cMin.z + cube.altitude() / 2.0);
	glScaled(cube.width() / 2.0, cube.height() / 2.0, cube.altitude() / 2.0);

	glColor4f(0, 1, 0, transparency);
	drawCube();
	drawTicMarks();
	drawLabels();
	drawCoords();
	drawTimes();

	glPopMatrix();
	return true;
}

void CubeDrawer::drawCube() const {	
	// Front Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(-1.0, -1.0, +1.0);
	glVertex3d(+1.0, -1.0, +1.0);
	glVertex3d(+1.0, +1.0, +1.0);
	glVertex3d(-1.0, +1.0, +1.0);
	glEnd();

	// Back Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(-1.0, -1.0, -1.0);
	glVertex3d(-1.0, +1.0, -1.0);
	glVertex3d(+1.0, +1.0, -1.0);
	glVertex3d(+1.0, -1.0, -1.0);
	glEnd();

	// Top Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(-1.0, +1.0, -1.0);
	glVertex3d(-1.0, +1.0, +1.0);
	glVertex3d(+1.0, +1.0, +1.0);
	glVertex3d(+1.0, +1.0, -1.0);
	glEnd();

	// Bottom Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(-1.0, -1.0, -1.0);
	glVertex3d(+1.0, -1.0, -1.0);
	glVertex3d(+1.0, -1.0, +1.0);
	glVertex3d(-1.0, -1.0, +1.0);
	glEnd();

	// Right Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(+1.0, -1.0, -1.0);
	glVertex3d(+1.0, +1.0, -1.0);
	glVertex3d(+1.0, +1.0, +1.0);
	glVertex3d(+1.0, -1.0, +1.0);
	glEnd();

	// Left Face
	glBegin(GL_LINE_STRIP);
	glVertex3d(-1.0, -1.0, -1.0);
	glVertex3d(-1.0, -1.0, +1.0);
	glVertex3d(-1.0, +1.0, +1.0);
	glVertex3d(-1.0, +1.0, -1.0);
	glEnd();
}

void CubeDrawer::drawTicMarks() const {
	const int nrTicMarks = 11; // use odd number so that there's a tic in the centre of an edge
	const double ticStep = 2.0 / (nrTicMarks-1); // 2.0 is the size of the cube

	for (int i=0; i<nrTicMarks; ++i)
	{
		double ticPos = -1 + i * ticStep; // position of each tic between -1 and +1
		double ticSize = ((i > 0) && (i < (nrTicMarks-1)))?0.025:0.05; // larger tics in corners 
		double ticLine = 1 + ticSize;
		// Tic marks X-axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(ticPos, -ticLine, -1.0);
		glVertex3d(ticPos, -1.0, -1.0);
		glEnd();
		// Tic marks Y-axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(-ticLine, ticPos, -1.0);
		glVertex3d(-1.0, ticPos, -1.0);
		glEnd();
		// Tic marks Z-axis
		glBegin(GL_LINE_STRIP);
		glVertex3d(-ticLine, -1.0, ticPos);
		glVertex3d(-1.0f, -1.0, ticPos);
		glEnd();
	}
}

void CubeDrawer::drawLabels() const {
	Color color (255, 0, 0);
	font->setColor(color);
	renderText(Coordinate(0.0, -1.1, -1.1), "X");
	renderText(Coordinate(-1.1, 0.0, -1.1), "Y");
	renderText(Coordinate(-1.1, -1.1, 0.0), "T");
}

void CubeDrawer::drawCoords() const {
	Color color (0, 0, 255);
	font->setColor(color);
	// minmax X
	renderText(Coordinate(-0.9, -1.1, -1.1), sxMin);
	renderText(Coordinate(0.9, -1.1, -1.1), sxMax);
	// minmax Y
	renderText(Coordinate(-1.1, -0.9, -1.1), syMin);
	renderText(Coordinate(-1.1, 0.9, -1.1), syMax);
}

void CubeDrawer::drawTimes() const {
	Color color (0, 0, 255);
	font->setColor(color);
	renderText(Coordinate(-1.1, -1.1, -0.9), stMin);
	renderText(Coordinate(-1.1, -1.1, 0.9), stMax);
}

void CubeDrawer::renderText(const Coordinate & c, const String & text) const {
	glPushMatrix();
	glTranslated(c.x, c.y, c.z);
	glScaled(2.0 / cube.width(), 2.0 / cube.height(), 2.0 / cube.altitude());
	font->renderText(Coordinate(0, 0, 0), text);
	glPopMatrix();
}
