#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Drawers\OpenstreetmapDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Representation\Rprclass.h"
#include <GL/glu.h>
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Texture.h"
#include "DEMTriangulator.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GrcWMS.h"
#include "Engine\SpatialReference\GrcOSM.h"
#include "Drawers\WMSTextureHeap.h"
#include "Drawers\WMSTexture.h"

using namespace ILWIS;

ILWIS::NewDrawer *createOpenstreetmapDrawer(DrawerParameters *parms) {
	return new OpenstreetmapDrawer(parms);
}

OpenstreetmapDrawer::OpenstreetmapDrawer(DrawerParameters *parms)
: RasterLayerDrawer(parms,"OpenstreetmapDrawer")
, openstreetmapData(new OpenstreetmapData())
{
	setInfo(false);
}

OpenstreetmapDrawer::~OpenstreetmapDrawer()
{
	delete openstreetmapData;
}

void OpenstreetmapDrawer::setup() {
	textureHeap = new WMSTextureHeap();
}

void OpenstreetmapDrawer::setData() const {
	RasterLayerDrawer::setData();
	((WMSTextureHeap*)textureHeap)->SetCBImage(openstreetmapData->cbFullExtent);
}

void OpenstreetmapDrawer::init() const
{
	// fetch the image's coordinate bounds
	DrawerContext* drawcontext = (getRootDrawer())->getDrawerContext();
	data->maxTextureSize = 256; //drawcontext->getMaxTextureSize();

	GeoRefOSM* grOpenstreetmap = gr()->pgOSM();
	openstreetmapData->cbFullExtent = grOpenstreetmap->getInitialCoordBounds();
	rastermap->iRaw(RowCol()); // trigger the 2nd and final call of the OpenstreetmapFormat constructor, which must be remembered in the MapStoreForeignFormat::ff member

	setData();

	data->init = true;
}

bool OpenstreetmapDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	drawPreDrawers(drawLoop, cbArea);
	if (!data->init)
		init();
	if ((drawLoop == drl2D && alpha != 0.0) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0 && alpha != 0.0)) { // no palette used, so we dont need to test on Palette's alphaminmax
		if (textureHeap->fValid())
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
			glColor4f(1, 1, 1, alpha);

			textureHeap->ClearQueuedTextures();

			bool is3D = getRootDrawer()->is3D(); 
			if (is3D) {
				ZValueMaker *zmaker = getZMaker();
				double zscale = zmaker->getZScale();
				double zoffset = zmaker->getOffset();
				glPushMatrix();
				glScaled(1,1,zscale);
				glTranslated(0,0,zoffset);
			}
			glEnable(GL_TEXTURE_2D);
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();

			CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();

			Coord c1 (cbMap.cMin);
			Coord c2 (cbMap.cMax.x, cbMap.cMin.y);
			Coord c3 (cbMap.cMax);
			Coord c4 (cbMap.cMin.x, cbMap.cMax.y);
			double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x};
			double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x};
			double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y};
			double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y};
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);
			CoordBounds cb1(Coord(-20037508.34, -20037508.34), Coord(0, 0));
			CoordBounds cb2(Coord(0, 0), Coord(20037508.34, 20037508.34));
			CoordBounds cb3(Coord(-20037508.34, 0), Coord(0, 20037508.34));
			CoordBounds cb4(Coord(0, -20037508.34), Coord(20037508.34, 0));
			DisplayImagePortion(cb1, 1);
			DisplayImagePortion(cb2, 1);
			DisplayImagePortion(cb3, 1);
			DisplayImagePortion(cb4, 1);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glDisable(GL_TEXTURE_2D);
			if (is3D)
				glPopMatrix();
			glDisable(GL_BLEND);
		}
	}

	drawPostDrawers(drawLoop, cbArea);

	return true;
}

void OpenstreetmapDrawer::DisplayImagePortion(CoordBounds& cb, unsigned int zoomLevel) const {
	// if patch describes the "added" portion of the map, do not display
	if (cb.cMin.x > openstreetmapData->cbFullExtent.cMax.x || cb.cMin.y > openstreetmapData->cbFullExtent.cMax.y)
		return;

	// if patch is entirely outside viewport, do not display

	GLdouble m_modelMatrix[16];
	GLdouble m_projMatrix[16];
	GLint m_viewport[4]; // x,y,width,height
	// get the matrices and the viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
	glGetIntegerv(GL_VIEWPORT, m_viewport);
	Coord b4 (cb.cMin);
	Coord b3 (cb.cMax.x, cb.cMin.y);
	Coord b2 (cb.cMax);
	Coord b1 (cb.cMin.x, cb.cMax.y);
	Coord c1 (getRootDrawer()->glConv(csy, b1));
	Coord c2 (getRootDrawer()->glConv(csy, b2));
	Coord c3 (getRootDrawer()->glConv(csy, b3));
	Coord c4 (getRootDrawer()->glConv(csy, b4));

	bool fTryAlternativeComputation = (c1.fUndef() || c2.fUndef() || c3.fUndef() || c4.fUndef());

	// 4 x 3 doubles to project onto xy screen coordinates
	GLdouble m_winx[4];
	GLdouble m_winy[4];
	GLdouble m_winz[4];

	geos::geom::GeometryFactory factory;
	const vector<geos::geom::Geometry *> holes;

	if (!fTryAlternativeComputation) {
		// project the patch to 2D
		gluProject(c1.x, c1.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
		gluProject(c2.x, c2.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
		gluProject(c3.x, c3.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
		gluProject(c4.x, c4.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);
		for (int i = 0; i < 4; ++i)
			fTryAlternativeComputation |= (m_winz[i] < -10 || m_winz[i] > 10);
	}

	// convex test on projected openstreetmap tile
	if (!fTryAlternativeComputation) {
		bool fFirst = true;
		bool fPositive = false;
		bool fConvex = true;
		for (int i = 0; i < 4; ++i) {
			double dx1 = m_winx[(i + 1) % 4] - m_winx[i];
			double dy1 = m_winy[(i + 1) % 4] - m_winy[i];
			double dx2 = m_winx[(i + 2) % 4] - m_winx[(i + 1) % 4];
			double dy2 = m_winy[(i + 2) % 4] - m_winy[(i + 1) % 4];
			double cross = dx1*dy2 - dy1*dx2;
			if (fFirst) {
				fFirst = false;
				fPositive = (cross > 0);
			} else {
				fConvex &= (fPositive == (cross > 0));
			}
		}
		if (!fConvex)
			fTryAlternativeComputation = true;
	}

	if (!fTryAlternativeComputation) {
		geos::geom::CoordinateArraySequence * coordsTile = new geos::geom::CoordinateArraySequence();
		for (int i = 0; i < 4; ++i)
			coordsTile->add(Coordinate(m_winx[i], m_winy[i]));
		coordsTile->add(Coordinate(m_winx[0], m_winy[0]));
		const geos::geom::LinearRing ringTile(coordsTile, &factory);		
		geos::geom::Polygon * polyTile(factory.createPolygon(ringTile, holes));

		geos::geom::CoordinateArraySequence * coordsViewport = new geos::geom::CoordinateArraySequence();
		coordsViewport->add(Coordinate(m_viewport[0], m_viewport[1]));
		coordsViewport->add(Coordinate(m_viewport[2], m_viewport[1]));
		coordsViewport->add(Coordinate(m_viewport[2], m_viewport[3]));
		coordsViewport->add(Coordinate(m_viewport[0], m_viewport[3]));
		coordsViewport->add(Coordinate(m_viewport[0], m_viewport[1]));
		const geos::geom::LinearRing ringViewport(coordsViewport, &factory);		
		geos::geom::Polygon * polyViewport(factory.createPolygon(ringViewport, holes));

		bool fContains = !polyViewport->disjoint(polyTile);
		delete polyTile;
		delete polyViewport;
		if (!fContains)
			fTryAlternativeComputation = true;
	}

	if (fTryAlternativeComputation) {
		if (zoomLevel >= 18) // we can only "split" here, and zoomlevel 18 can't be split anymore
			return;
		if (cb.width() < 1000000 || cb.height() < 1000000) // deadlock, or very-large-computation protection: if we haven't found it by now with the regular formula, it is a really weird projection
			return;
		Coord c1, c2, c3, c4;
		double posZ;
		float winZ;
		glReadPixels( m_viewport[0], m_viewport[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
		gluUnProject( m_viewport[0], m_viewport[1], winZ, m_modelMatrix, m_projMatrix, m_viewport, &c1.x, &c1.y, &posZ);
		glReadPixels( m_viewport[2], m_viewport[1], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
		gluUnProject( m_viewport[2], m_viewport[1], winZ, m_modelMatrix, m_projMatrix, m_viewport, &c2.x, &c2.y, &posZ);
		glReadPixels( m_viewport[2], m_viewport[3], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
		gluUnProject( m_viewport[2], m_viewport[3], winZ, m_modelMatrix, m_projMatrix, m_viewport, &c3.x, &c3.y, &posZ);
		glReadPixels( m_viewport[0], m_viewport[3], 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ );
		gluUnProject( m_viewport[0], m_viewport[3], winZ, m_modelMatrix, m_projMatrix, m_viewport, &c4.x, &c4.y, &posZ);
		c1.z = 0;
		c2.z = 0;
		c3.z = 0;
		c4.z = 0;

		Coord b1 (getRootDrawer()->glToWorld(csy, c1));
		Coord b2 (getRootDrawer()->glToWorld(csy, c2));
		Coord b3 (getRootDrawer()->glToWorld(csy, c3));
		Coord b4 (getRootDrawer()->glToWorld(csy, c4));

		CoordBounds cbView; // intentionally like this: it checks for crdUNDEF on every coord added
		cbView += b1;
		cbView += b2;
		cbView += b3;
		cbView += b4;

		if (cbView.fUndef() || cbView.width() == 0 || cbView.height() == 0)
			return;

		geos::geom::CoordinateArraySequence * coordsView = new geos::geom::CoordinateArraySequence();
		coordsView->add(cbView.cMin);
		coordsView->add(Coord(cbView.cMin.x, cbView.cMax.y));
		coordsView->add(cbView.cMax);
		coordsView->add(Coord(cbView.cMax.x, cbView.cMin.y));
		coordsView->add(cbView.cMin);
		const geos::geom::LinearRing ringView(coordsView, &factory);		
		geos::geom::Polygon * polyView(factory.createPolygon(ringView, holes));

		geos::geom::CoordinateArraySequence * coordsOSM = new geos::geom::CoordinateArraySequence();
		coordsOSM->add(cb.cMin);
		coordsOSM->add(Coord(cb.cMin.x, cb.cMax.y));
		coordsOSM->add(cb.cMax);
		coordsOSM->add(Coord(cb.cMax.x, cb.cMin.y));
		coordsOSM->add(cb.cMin);
		const geos::geom::LinearRing ringOSM(coordsOSM, &factory);		
		geos::geom::Polygon * polyOSM(factory.createPolygon(ringOSM, holes));

		bool fOverlaps = !polyOSM->disjoint(polyView);
		delete polyView;
		delete polyOSM;

		if (fOverlaps) {
			unsigned int nextZoomLevel = zoomLevel + 1;
			double sizeX2 = cb.width() / 2.0;
			double sizeY2 = cb.height() / 2.0;
			// Q1
			DisplayImagePortion(CoordBounds(cb.cMin, Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2)), nextZoomLevel);
			// Q2
			DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y), Coord(cb.cMax.x, cb.cMin.y + sizeY2)), nextZoomLevel);
			// Q3
			DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2), cb.cMax), nextZoomLevel);
			// Q4
			DisplayImagePortion(CoordBounds(Coord(cb.cMin.x, cb.cMin.y + sizeY2), Coord(cb.cMin.x + sizeX2, cb.cMax.y)), nextZoomLevel);
		}
		return;
	}

	double screenPixelsX1 = sqrt(sqr(m_winx[1]-m_winx[0])+sqr(m_winy[1]-m_winy[0]));
	double screenPixelsY1 = sqrt(sqr(m_winx[2]-m_winx[1])+sqr(m_winy[2]-m_winy[1]));
	double screenPixelsX2 = sqrt(sqr(m_winx[3]-m_winx[2])+sqr(m_winy[3]-m_winy[2]));
	double screenPixelsY2 = sqrt(sqr(m_winx[0]-m_winx[3])+sqr(m_winy[0]-m_winy[3]));

	// split the visible portion of the image into a number of patches, depending on the accuracy needed

	bool split = false;
	if (min(screenPixelsX1, screenPixelsX2) > data->maxTextureSize * 2)
		split = true;
	if (min(screenPixelsY1, screenPixelsY2) > data->maxTextureSize * 2)
		split = true;
	if (zoomLevel >= 18)
		split = false;

	if (split)
	{
		double sizeX2 = cb.width() / 2.0;
		double sizeY2 = cb.height() / 2.0;
		unsigned int nextZoomLevel = zoomLevel + 1;
		// Q1
		DisplayImagePortion(CoordBounds(cb.cMin, Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2)), nextZoomLevel);
		// Q2
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y), Coord(cb.cMax.x, cb.cMin.y + sizeY2)), nextZoomLevel);
		// Q3
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2), cb.cMax), nextZoomLevel);
		// Q4
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x, cb.cMin.y + sizeY2), Coord(cb.cMin.x + sizeX2, cb.cMax.y)), nextZoomLevel);
	}
	else
	{
		CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
		if (cbMap.fContains(c1) || cbMap.fContains(c2) || cbMap.fContains(c3) || cbMap.fContains(c4)) {
			if (getRootDrawer()->is3D() && demTriangulator)
				DisplayTexture3D(cb);
			else {
				DisplayTexture(cb);
			}
		}
	}
}

void OpenstreetmapDrawer::DisplayTexture(CoordBounds & cb) const
{
	Texture* tex = ((WMSTextureHeap*)textureHeap)->GetTexture(cb, isThreaded);

	if (tex != 0)
	{
		glEnable(GL_CLIP_PLANE0);
		glEnable(GL_CLIP_PLANE1);
		glEnable(GL_CLIP_PLANE2);
		glEnable(GL_CLIP_PLANE3);

		CoordBounds cbImage = openstreetmapData->cbFullExtent;
		// make the quad
		glBegin (GL_QUADS);

		if (fLinear) {
			// texture bounds
			double s1 = (cb.cMin.x - cbImage.cMin.x) / cbImage.width();
			double t1 = max(0.0, (cbImage.cMax.y - cb.cMax.y) / cbImage.height());
			double s2 = min(1.0, (cb.cMax.x - cbImage.cMin.x) / cbImage.width());
			double t2 = min(1.0, (cbImage.cMax.y - cb.cMin.y) / cbImage.height());

			Coord b4 (cb.cMin);
			Coord b3 (min(cb.cMax.x, cbImage.cMax.x), cb.cMin.y);
			Coord b2 (min(cb.cMax.x, cbImage.cMax.x), min(cb.cMax.y, cbImage.cMax.y));
			Coord b1 (cb.cMin.x, min(cb.cMax.y, cbImage.cMax.y));
			Coord c1 = getRootDrawer()->glConv(csy, b1);
			Coord c2 = getRootDrawer()->glConv(csy, b2);
			Coord c3 = getRootDrawer()->glConv(csy, b3);
			Coord c4 = getRootDrawer()->glConv(csy, b4);

			glTexCoord2d(s1, t1);
			glVertex3d(c1.x, c1.y, 0.0);

			glTexCoord2d(s2, t1);
			glVertex3d(c2.x, c2.y, 0.0);

			glTexCoord2d(s2, t2);
			glVertex3d(c3.x, c3.y, 0.0);

			glTexCoord2d(s1, t2);
			glVertex3d(c4.x, c4.y, 0.0);
		} else {
			const unsigned int iSize = 10; // this makes 16 quads, thus 32 triangles per texture
			// avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
			double colStep = min(cb.width(), cbImage.cMax.x - cb.cMin.x) / (double)iSize;
			double rowStep = min(cb.height(), cbImage.cMax.y - cb.cMin.y) / (double)iSize;

			double s1 = (cb.cMin.x - cbImage.cMin.x) / cbImage.width();
			for (int x = 0; x < iSize; ++x) {
				double s2 = s1 + colStep / cbImage.width();
				double t1 = max(0.0, (cbImage.cMax.y - cb.cMax.y) / cbImage.height());

				Coord b1 (cb.cMin.x + colStep * x, min(cb.cMax.y, cbImage.cMax.y));
				Coord b2 (cb.cMin.x + colStep * (x + 1), min(cb.cMax.y, cbImage.cMax.y));

				Coord c1 = getRootDrawer()->glConv(csy, b1);
				Coord c2 = getRootDrawer()->glConv(csy, b2);
				for (int y = 1; y <= iSize ; ++y) {
					double t2 = t1 + rowStep / cbImage.height();
					Coord b3 (cb.cMin.x + colStep * (x + 1), min(cb.cMax.y, cbImage.cMax.y) - rowStep * y);
					Coord b4 (cb.cMin.x + colStep * x, min(cb.cMax.y, cbImage.cMax.y) - rowStep * y);

					Coord c3 = getRootDrawer()->glConv(csy, b3);
					Coord c4 = getRootDrawer()->glConv(csy, b4);

					glTexCoord2d(s1, t1);
					glVertex3d(c1.x, c1.y, 0.0);

					glTexCoord2d(s2, t1);
					glVertex3d(c2.x, c2.y, 0.0);

					glTexCoord2d(s2, t2);
					glVertex3d(c3.x, c3.y, 0.0);

					glTexCoord2d(s1, t2);
					glVertex3d(c4.x, c4.y, 0.0);

					t1 = t2;
					c1 = c4;
					c2 = c3;
				}
				s1 = s2;
			}
		}
	
		glEnd();

		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
	}
}

void OpenstreetmapDrawer::DisplayTexture3D(CoordBounds & cb) const
{
	Texture* tex = ((WMSTextureHeap*)textureHeap)->GetTexture(cb, isThreaded);

	if (tex != 0)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		CoordBounds cbImage = openstreetmapData->cbFullExtent;

		Coord b4 (cb.cMin);
		Coord b3 (min(cb.cMax.x, cbImage.cMax.x), cb.cMin.y);
		Coord b2 (min(cb.cMax.x, cbImage.cMax.x), min(cb.cMax.y, cbImage.cMax.y));
		Coord b1 (cb.cMin.x, min(cb.cMax.y, cbImage.cMax.y));

		Coord c1 = getRootDrawer()->glConv(csy, b1);
		Coord c2 = getRootDrawer()->glConv(csy, b2);
		Coord c3 = getRootDrawer()->glConv(csy, b3);
		Coord c4 = getRootDrawer()->glConv(csy, b4);
		//double clip_plane0[]={c3.y - c2.y, c2.x - c3.x, 0.0, c2.x * (c2.y - c3.y) - c2.y * (c2.x - c3.x)}; // x < x2
		//double clip_plane1[]={c1.y - c4.y, c4.x - c1.x, 0.0, c4.x * (c4.y - c1.y) - c4.y * (c4.x - c1.x)}; // x > x1
		//double clip_plane2[]={c2.y - c1.y, c1.x - c2.x, 0.0, c1.x * (c1.y - c2.y) - c1.y * (c1.x - c2.x)}; // y > y1
		//double clip_plane3[]={c4.y - c3.y, c3.x - c4.x, 0.0, c3.x * (c3.y - c4.y) - c3.y * (c3.x - c4.x)}; // y < y2
		double clip_plane0[]={c3.y - c2.y, c2.x - c3.x, 0.0, c3.x * (c2.y - c3.y) - c3.y * (c2.x - c3.x)}; // x < x2
		double clip_plane1[]={c1.y - c4.y, c4.x - c1.x, 0.0, c1.x * (c4.y - c1.y) - c1.y * (c4.x - c1.x)}; // x > x1
		double clip_plane2[]={c4.y - c3.y, c3.x - c4.x, 0.0, c4.x * (c3.y - c4.y) - c4.y * (c3.x - c4.x)}; // y > y1
		double clip_plane3[]={c2.y - c1.y, c1.x - c2.x, 0.0, c2.x * (c1.y - c2.y) - c2.y * (c1.x - c2.x)}; // y < y2
		glClipPlane(GL_CLIP_PLANE0,clip_plane0);
		glClipPlane(GL_CLIP_PLANE1,clip_plane1);
		glClipPlane(GL_CLIP_PLANE2,clip_plane2);
		glClipPlane(GL_CLIP_PLANE3,clip_plane3);
		glEnable(GL_CLIP_PLANE0);
		glEnable(GL_CLIP_PLANE1);
		glEnable(GL_CLIP_PLANE2);
		glEnable(GL_CLIP_PLANE3);
		demTriangulator->PlotTriangles();
		glDisable(GL_CLIP_PLANE0);
		glDisable(GL_CLIP_PLANE1);
		glDisable(GL_CLIP_PLANE2);
		glDisable(GL_CLIP_PLANE3);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void *OpenstreetmapDrawer::getDataSource() const{
	return 0; // no annotation-legend for colorcomposite
}

String OpenstreetmapDrawer::getInfo(const Coord& c) const {
	if ( !hasInfo() || !isActive())
		return "";
	Coord crd = c;
	if (rastermap->cs() != rootDrawer->getCoordinateSystem())
	{
		crd = rastermap->cs()->cConv(rootDrawer->getCoordinateSystem(), c);
	}
	long iRaw;
	WMSTexture* tex = (WMSTexture*)((WMSTextureHeap*)textureHeap)->GetTexture(crd);
	if (tex)
		iRaw = tex->iRaw(crd);
	else
		iRaw = iUNDEF;
	String s;
	if (iRaw == iUNDEF) {
		s = String("?");
	} else {
		Color clr = Color(iRaw);
		s = String("(%3i,%3i,%3i)", clr.red(), clr.green(), clr.blue());
	}
	return s;
}
