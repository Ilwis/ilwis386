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
#include "Drawers\WMSDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Representation\Rprclass.h"
#include <GL/glu.h>
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Texture.h"
#include "DEMTriangulator.h"
#include "Engine\SpatialReference\Grcornrs.h"
#include "Engine\SpatialReference\GrcWMS.h"
#include "Drawers\WMSTextureHeap.h"


using namespace ILWIS;

ILWIS::NewDrawer *createWMSDrawer(DrawerParameters *parms) {
	return new WMSDrawer(parms);
}

WMSDrawer::WMSDrawer(DrawerParameters *parms)
: RasterLayerDrawer(parms,"WMSDrawer")
, wmsData(new WMSData())
{
}

WMSDrawer::~WMSDrawer()
{
	delete wmsData;
}

void WMSDrawer::setup() {
	textureHeap = new WMSTextureHeap();
}

void WMSDrawer::setData() const {
	RasterLayerDrawer::setData();
	((WMSTextureHeap*)textureHeap)->SetCBImage(wmsData->cbFullExtent);
}

void WMSDrawer::init() const
{
	// fetch the image's coordinate bounds
	DrawerContext* drawcontext = (getRootDrawer())->getDrawerContext();
	data->maxTextureSize = drawcontext->getMaxTextureSize();
	int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN); // maximum X size of client area (regardless of current viewport)
	int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN); // maximum Y size of client area
	if (iXScreen < data->maxTextureSize) // prevent making textures that are larger than the screen, it is totally unnecessary and a big performance and memory hit
		data->maxTextureSize = iXScreen;
	if (iYScreen < data->maxTextureSize)
		data->maxTextureSize = iYScreen;

	GeoRefCornersWMS* grWMS = gr()->pgWMS();
	wmsData->cbFullExtent = grWMS->getInitialCoordBounds();
	rastermap->iRaw(RowCol()); // trigger the 2nd and final call of the WMSFormat constructor, which must be remembered in the MapStoreForeignFormat::ff member

	setData();

	data->init = true;
}

bool WMSDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	drawPreDrawers(drawLoop, cbArea);

	if (!data->init)
		init();
	if (textureHeap->fValid())
	{
		if ((drawLoop == drl2D && alpha != 0.0) || (drawLoop == drl3DOPAQUE && alpha == 1.0) || (drawLoop == drl3DTRANSPARENT && alpha != 1.0 && alpha != 0.0)) { // no palette used, so we dont need to test on Palette's alphaminmax
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
			glColor4f(1, 1, 1, alpha);

			textureHeap->ClearQueuedTextures();

			bool is3D = getRootDrawer()->is3D(); 
			if (is3D) {
				ZValueMaker *zmaker = getZMaker();
				double zscale = zmaker->getZScale();
				double zoffset = zmaker->getOffset();
				double z0 = getRootDrawer()->getZMaker()->getZ0(is3D);
				glPushMatrix();
				glScaled(1,1,zscale);
				glTranslated(0,0,zoffset + z0);
			}
			glEnable(GL_TEXTURE_2D);
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			CoordBounds cb (wmsData->cbFullExtent);
			if (cb.width() > cb.height()) {
				double deltay = cb.width() - cb.height();
				cb.cMax.y = cb.cMax.y + deltay;
			} else {
				double deltax = cb.height() - cb.width();
				cb.cMax.x = cb.cMax.x + deltax;
			}
			DisplayImagePortion(cb);
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

void WMSDrawer::DisplayImagePortion(CoordBounds& cb) const {
	// if patch describes the "added" portion of the map, do not display
	if (cb.cMin.x > wmsData->cbFullExtent.cMax.x || cb.cMin.y > wmsData->cbFullExtent.cMax.y)
		return;

	// if patch is entirely outside viewport, do not display

	GLfloat feedbackBuffer [2];
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
	if (c1.fUndef() || c2.fUndef() || c3.fUndef() || c4.fUndef())
		return;
	glFeedbackBuffer(2, GL_2D, feedbackBuffer);
	glRenderMode(GL_FEEDBACK);
	glBegin (GL_QUADS);
	glVertex3d(c1.x, c1.y, 0.0);
	glVertex3d(c2.x, c2.y, 0.0);
	glVertex3d(c3.x, c3.y, 0.0);
	glVertex3d(c4.x, c4.y, 0.0);
	glEnd();
	if (0 == glRenderMode(GL_RENDER))
		return;

	// 4 x 3 doubles to project onto xy screen coordinates
	GLdouble m_winx[4];
	GLdouble m_winy[4];
	GLdouble m_winz[4];

	// project the patch to 2D
	gluProject(c1.x, c1.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
	gluProject(c2.x, c2.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
	gluProject(c3.x, c3.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
	gluProject(c4.x, c4.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);

	double screenPixelsX1 = sqrt(sqr(m_winx[1]-m_winx[0])+sqr(m_winy[1]-m_winy[0]));
	double screenPixelsY1 = sqrt(sqr(m_winx[2]-m_winx[1])+sqr(m_winy[2]-m_winy[1]));
	double screenPixelsX2 = sqrt(sqr(m_winx[3]-m_winx[2])+sqr(m_winy[3]-m_winy[2]));
	double screenPixelsY2 = sqrt(sqr(m_winx[0]-m_winx[3])+sqr(m_winy[0]-m_winy[3]));

	// split the visible portion of the image into a number of patches, depending on the accuracy needed

	bool split = false;
	if (max(screenPixelsX1, screenPixelsX2) > data->maxTextureSize) {
		split = true;
	}
	if (max(screenPixelsY1, screenPixelsY2) > data->maxTextureSize) {
		split = true;
	}
	if (split)
	{
		double sizeX2 = cb.width() / 2.0;
		double sizeY2 = cb.height() / 2.0;
		// Q1
		DisplayImagePortion(CoordBounds(cb.cMin, Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2)));
		// Q2
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y), Coord(cb.cMax.x, cb.cMin.y + sizeY2)));
		// Q3
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x + sizeX2, cb.cMin.y + sizeY2), cb.cMax));
		// Q4
		DisplayImagePortion(CoordBounds(Coord(cb.cMin.x, cb.cMin.y + sizeY2), Coord(cb.cMin.x + sizeX2, cb.cMax.y)));
	}
	else
	{
		if (getRootDrawer()->is3D() && demTriangulator)
			DisplayTexture3D(cb);
		else
			DisplayTexture(cb);
	}
}

void WMSDrawer::DisplayTexture(CoordBounds & cb) const
{
	Texture* tex = ((WMSTextureHeap*)textureHeap)->GetTexture(cb, isThreaded);

	if (tex != 0)
	{
		CoordBounds cbImage = wmsData->cbFullExtent;
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
			const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
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
	}
}

void WMSDrawer::DisplayTexture3D(CoordBounds & cb) const
{
	Texture* tex = ((WMSTextureHeap*)textureHeap)->GetTexture(cb, isThreaded);

	if (tex != 0)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		CoordBounds cbImage = wmsData->cbFullExtent;

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
