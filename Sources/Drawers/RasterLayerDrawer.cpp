#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\LayerDrawer.h"
#include "Drawers\RasterLayerDrawer.h"
#include "Engine\Drawers\ZValueMaker.h"
#include "Engine\Representation\Rprclass.h"
#include <GL/glu.h>
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Texture.h"
#include "DEMTriangulator.h"


using namespace ILWIS;

ILWIS::NewDrawer *createRasterLayerDrawer(DrawerParameters *parms) {
	return new RasterLayerDrawer(parms);
}

RasterLayerDrawer::RasterLayerDrawer(DrawerParameters *parms) : 
LayerDrawer(parms,"RasterLayerDrawer")
, data(new RasterSetData()), isThreaded(true), sameCsy(true), fUsePalette(false), fPaletteOwner(false), palette(0), textureHeap(new TextureHeap()), demTriangulator(0)
{
	setTransparency(1); // default, opaque
	//	setDrawMethod(drmNOTSET); // default
	//drawers.push_back(this); // nasty: i am my own child drawer
}

RasterLayerDrawer::~RasterLayerDrawer(){
	delete textureHeap;
	if (fPaletteOwner)
		delete palette;
	delete data;
	if (demTriangulator)
		delete demTriangulator;
}

void RasterLayerDrawer::setMinMax(const RangeReal & rrMinMax)
{
	this->rrMinMax = rrMinMax;
}

void RasterLayerDrawer::prepareChildDrawers(PreparationParameters *pp){
	prepare(pp);
}

void RasterLayerDrawer::prepare(PreparationParameters *pp){
	LayerDrawer::prepare(pp);

	if ( pp->type & NewDrawer::ptRENDER) {
		fUsePalette = drm != drmCOLOR;
		if ( rpr->prc()) {
			RepresentationClass *rprC = rpr->prc();
			for(int j =0 ; j < pp->filteredRaws.size(); ++j) {
				int raw = pp->filteredRaws[j];
				Color clr = rprC->clrRaw(abs(raw));
				clr.m_transparency = raw > 0 ? 0 : 255;
				rprC->PutColor(raw,clr);
			}
		}
		if (fPaletteOwner) {
			if (fUsePalette && palette->fValid()) {
				palette->Refresh();
				getRootDrawer()->getDrawerContext()->setActivePalette(0);
			}
		}
		textureHeap->RepresentationChanged();
	}
	if ( pp->type & ptGEOMETRY | pp->type & ptRESTORE) {
		sameCsy = getRootDrawer()->getCoordinateSystem()->fnObj == csy->fnObj;
	}
	if ((pp->type & pt3D) || ((pp->type & ptGEOMETRY | pp->type & ptRESTORE) && demTriangulator != 0)) {
		ZValueMaker * zMaker = getZMaker();
		bool is3DPossible = zMaker->getThreeDPossible();
		if (demTriangulator != 0) {
			delete demTriangulator;
			demTriangulator = 0;
		}
		if (is3DPossible) {
			demTriangulator = new DEMTriangulator(zMaker, rastermap.ptr(), getRootDrawer()->getCoordinateSystem(), false);
			if (!demTriangulator->fValid()) {
				delete demTriangulator;
				demTriangulator = 0;
			}
		}
	}
}

void RasterLayerDrawer::setDrawMethod(DrawMethod method) {

	if ( method == drmINIT) {
		drm = drmRPR;
		if (rastermap.fValid() ) {
			Domain _dm = rastermap->dm();
			if (0 != _dm->pdi())
				drm = drmIMAGE;
			else if (0 != _dm->pdcol())
				drm = drmCOLOR;
			else if (0 != _dm->pdid())
				drm = drmMULTIPLE;
			else if ((0 != _dm->pdbit()) || (0 != _dm->pdbool()))
				drm = drmBOOL;
			else if (0 != _dm->pdp())
				drm = drmRPR;
		}
	} else
		drm = method;
}

Palette * RasterLayerDrawer::SetPaletteOwner()
{
	if (fPaletteOwner && palette)
		delete palette;
	palette = new Palette();
	fPaletteOwner = true;
	return palette;
}

void RasterLayerDrawer::SetPalette(Palette * palette)
{
	if (fPaletteOwner && palette)
		delete palette;
	this->palette = palette;
	fPaletteOwner = false;
}

void RasterLayerDrawer::init() const
{
	// fetch the image's coordinate bounds
	if (rastermap.fValid())
	{
		data->cb = rastermap->cb();
		DrawerContext* drawcontext = (getRootDrawer())->getDrawerContext();
		data->maxTextureSize = drawcontext->getMaxTextureSize();
		int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN); // maximum X size of client area (regardless of current viewport)
		int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN); // maximum Y size of client area
		if (iXScreen < data->maxTextureSize) // prevent making textures that are larger than the screen, it is totally unnecessary and a big performance and memory hit
			data->maxTextureSize = iXScreen;
		if (iYScreen < data->maxTextureSize)
			data->maxTextureSize = iYScreen;

		data->imageWidth = rastermap->rcSize().Col;
		data->imageHeight = rastermap->rcSize().Row;

		double log2width = log((double)data->imageWidth)/log(2.0);
		log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
		data->width = pow(2, log2width);
		double log2height = log((double)data->imageHeight)/log(2.0);
		log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
		data->height = pow(2, log2height);

		textureHeap->SetData(rastermap, getDrawingColor(), getDrawMethod(), drawcontext->getMaxPaletteSize(), data->width, data->height, rrMinMax, drawcontext);
		if (fPaletteOwner)
			palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);

		if (fPaletteOwner)
			if (fUsePalette) {
				palette->Refresh();
				getRootDrawer()->getDrawerContext()->setActivePalette(0);
			}
	}
	data->init = true;
}

void RasterLayerDrawer::addDataSource(void *bmap, int options){
	rastermap.SetPointer((BaseMapPtr *)bmap);
}

bool RasterLayerDrawer::draw( const CoordBounds& cbArea) const {

	drawPreDrawers(cbArea);

	if (!data->init)
		init();
	if (textureHeap->fValid())
	{
		//glClearColor(1.0,1.0,1.0,0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, transparency);

		textureHeap->ClearQueuedTextures();

		// Extend the image so that its width and height become ^2

		// DisplayImagePortion(-1, 1, 1, -1, 0, 0, width, height);
		CoordBounds cb = data->cb;
		double minX = cb.MinX();
		double maxX = cb.MaxX();
		double minY = cb.MinY();
		double maxY = cb.MaxY();
		// Image has grown right-down
		maxX = minX + (maxX - minX) * (double)data->width / (double)data->imageWidth;
		minY = maxY + (minY - maxY) * (double)data->height / (double)data->imageHeight;

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
		DisplayImagePortion(minX, maxY, maxX, minY, 0, 0, data->width, data->height);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glDisable(GL_TEXTURE_2D);
		if (is3D)
			glPopMatrix();
		glDisable(GL_BLEND);
	}

	drawPostDrawers(cbArea);
	return true;
}

void RasterLayerDrawer::DisplayImagePortion(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const
{
	// if patch describes the "added" portion of the map, do not display
	if (x1 > data->cb.MaxX() || y1 < data->cb.MinY())
		return;

	// if patch is entirely outside viewport, do not display

	// for repetitive calls to get the feedback buffer
	GLfloat feedbackBuffer [2];

	// gl matrices
	GLdouble m_modelMatrix[16];
	GLdouble m_projMatrix[16];
	// viewport
	GLint m_viewport[4]; // x,y,width,height

	// get the matrices and the viewport
	glGetDoublev(GL_MODELVIEW_MATRIX, m_modelMatrix);
	glGetDoublev(GL_PROJECTION_MATRIX, m_projMatrix);
	glGetIntegerv(GL_VIEWPORT, m_viewport);

	Coord c1, c2, c3, c4;
	glFeedbackBuffer(2, GL_2D, feedbackBuffer);
	glRenderMode(GL_FEEDBACK);
	glBegin (GL_QUADS);
	if (sameCsy) {
		glVertex3d(x1, y1, 0.0);
		glVertex3d(x2, y1, 0.0);
		glVertex3d(x2, y2, 0.0);
		glVertex3d(x1, y2, 0.0);
	} else {
		c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1, y1, 0.0));
		c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x2, y1, 0.0));
		c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x2, y2, 0.0));
		c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1, y2, 0.0));
		glVertex3d(c1.x, c1.y, 0.0);
		glVertex3d(c2.x, c2.y, 0.0);
		glVertex3d(c3.x, c3.y, 0.0);
		glVertex3d(c4.x, c4.y, 0.0);
	}
	glEnd();
	if (0 == glRenderMode(GL_RENDER))
		return;

	// for repetitive temporary calculations, added here for performance, to avoid repetitive use of stack/heap
	// 4 x 3 doubles to project into xy screen coordinates
	GLdouble m_winx[4];
	GLdouble m_winy[4];
	GLdouble m_winz[4];

	// project the patch to 2D
	if (sameCsy) {
		gluProject(x1, y1, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
		gluProject(x2, y1, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
		gluProject(x2, y2, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
		gluProject(x1, y2, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);
	} else {
		gluProject(c1.x, c1.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
		gluProject(c2.x, c2.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
		gluProject(c3.x, c3.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
		gluProject(c4.x, c4.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);
	}

	double zoom = getMinZoom(imageSizeX, imageSizeY, m_winx, m_winy); // the minimum zoomout-factor, indicating that it is necessary to plot the patch more accurately

	double log2zoom = log(zoom)/log(2.0);
	log2zoom = floor(log2zoom);
	const unsigned int zoomFactor = min(64, max(1, pow(2, log2zoom)));
	if (0 == zoomFactor)
		return;

	// split the visible portion of the image into a number of patches, depending on the accuracy needed

	boolean xSplit = false;
	boolean ySplit = false;

	if ((imageSizeX > 1) && (imageSizeX / zoomFactor > data->maxTextureSize)) // imageSizeX / zoomFactor is the required pixels of the patch in the x-direction
		xSplit = true;
	if ((imageSizeY > 1) && (imageSizeY / zoomFactor > data->maxTextureSize)) // imageSizeY / zoomFactor is the required pixels of the patch in the y-direction
		ySplit = true;
	if (xSplit && ySplit)
	{
		double dx = (x2 - x1) / 2.0;
		double dy = (y2 - y1) / 2.0;
		int sizeX2 = imageSizeX / 2;
		int sizeY2 = imageSizeY / 2;
		// Q1
		DisplayImagePortion(x1, y1, x1 + dx, y1 + dy, imageOffsetX, imageOffsetY, sizeX2, sizeY2);
		// Q2
		DisplayImagePortion(x1 + dx, y1, x2, y1 + dy, imageOffsetX + sizeX2, imageOffsetY, sizeX2, sizeY2);
		// Q3
		DisplayImagePortion(x1 + dx, y1 + dy, x2, y2, imageOffsetX + sizeX2, imageOffsetY + sizeY2, sizeX2, sizeY2);
		// Q4
		DisplayImagePortion(x1, y1 + dy, x1 + dx, y2, imageOffsetX, imageOffsetY + sizeY2, sizeX2, sizeY2);
	}
	else if (xSplit)
	{
		double dx = (x2 - x1) / 2.0;
		int sizeX2 = imageSizeX / 2;
		// Q1
		DisplayImagePortion(x1, y1, x1 + dx, y2, imageOffsetX, imageOffsetY, sizeX2, imageSizeY);
		// Q2
		DisplayImagePortion(x1 + dx, y1, x2, y2, imageOffsetX + sizeX2, imageOffsetY, sizeX2, imageSizeY);
	}
	else if (ySplit)
	{
		double dy = (y2 - y1) / 2.0;
		int sizeY2 = imageSizeY / 2;
		// Q1
		DisplayImagePortion(x1, y1, x2, y1 + dy, imageOffsetX, imageOffsetY, imageSizeX, sizeY2);
		// Q2
		DisplayImagePortion(x1, y1 + dy, x2, y2, imageOffsetX, imageOffsetY + sizeY2, imageSizeX, sizeY2);
	}
	else
	{
		if (getRootDrawer()->is3D() && demTriangulator)
			DisplayTexture3D(x1, y1, x2, y2, c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
		else
			DisplayTexture(x1, y1, x2, y2, c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
	}
}

void RasterLayerDrawer::DisplayTexture(double x1, double y1, double x2, double y2, Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
	Texture* tex = textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, x1, y1, x2, y2, zoomFactor, palette, isThreaded);

	if (tex != 0)
	{
		// make the quad
		glBegin (GL_QUADS);

		if (sameCsy) {
			// texture bounds
			double s1 = imageOffsetX / (double)data->width;
			double t1 = imageOffsetY / (double)data->height;
			double s2 = min(imageOffsetX + imageSizeX, data->imageWidth) / (double)data->width;
			double t2 = min(imageOffsetY + imageSizeY, data->imageHeight) / (double)data->height;

			// avoid plotting the "added" portion of the map
			x2 = min(x2, data->cb.MaxX());
			y2 = max(y2, data->cb.MinY());

			glTexCoord2d(s1, t1);
			glVertex3d(x1, y1, 0.0);

			glTexCoord2d(s2, t1);
			glVertex3d(x2, y1, 0.0);

			glTexCoord2d(s2, t2);
			glVertex3d(x2, y2, 0.0);

			glTexCoord2d(s1, t2);
			glVertex3d(x1, y2, 0.0);
		} else {
			const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
			// avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
			double colStep = min(imageSizeX, data->imageWidth - imageOffsetX) / (double)iSize;
			double rowStep = min(imageSizeY, data->imageHeight - imageOffsetY) / (double)iSize;
			x2 = min(x2, data->cb.MaxX());
			y2 = max(y2, data->cb.MinY());
			double xStep = (x2 - x1) / (double)iSize;
			double yStep = (y2 - y1) / (double)iSize;

			double s1 = imageOffsetX / (double)data->width;
			for (int x = 0; x < iSize; ++x) {
				double s2 = s1 + colStep / (double)data->width;
				double t1 = imageOffsetY / (double)data->height;
				c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1 + xStep * x, y1, 0.0));
				c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1 + xStep * (x + 1), y1, 0.0));
				for (int y = 1; y <= iSize ; ++y) {
					double t2 = t1 + rowStep / (double)data->height;
					c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1 + xStep * (x + 1), y1 + yStep * y, 0.0));
					c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1 + xStep * x, y1 + yStep * y, 0.0));

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

void RasterLayerDrawer::DisplayTexture3D(double x1, double y1, double x2, double y2, Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
	Texture* tex = textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, x1, y1, x2, y2, zoomFactor, palette, isThreaded);

	if (tex != 0)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		if (sameCsy) {
			// avoid plotting the "added" portion of the map
			x2 = min(x2, data->cb.MaxX());
			y2 = max(y2, data->cb.MinY());

			double clip_plane0[]={-1.0,0.0,0.0,x2}; // x < x2
			double clip_plane1[]={1.0,0.0,0.0,-x1}; // x > x1
			double clip_plane2[]={0.0,-1.0,0.0,y1}; // y > y1
			double clip_plane3[]={0.0,1.0,0.0,-y2}; // y < y2
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);

		} else {
			// avoid plotting the "added" portion of the map
			bool fRecalculateCX2 = false;
			if (x2 > data->cb.MaxX())
			{
				x2 = data->cb.MaxX();
				fRecalculateCX2 = true;
			}
			bool fRecalculateCY2 = false;
			if (y2 < data->cb.MinY())
			{
				y2 = data->cb.MinY();
				fRecalculateCY2 = true;
			}

			//c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1, y1, 0.0));
			if (fRecalculateCX2)
				c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x2, y1, 0.0));
			if (fRecalculateCX2 || fRecalculateCY2)
				c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x2, y2, 0.0));
			if (fRecalculateCY2)
				c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, Coord(x1, y2, 0.0));
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
		}
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

double RasterLayerDrawer::getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const
{
	double screenPixelsY1 = sqrt(sqr(m_winx[1]-m_winx[0])+sqr(m_winy[1]-m_winy[0]));
	double screenPixelsX1 = sqrt(sqr(m_winx[2]-m_winx[1])+sqr(m_winy[2]-m_winy[1]));
	double screenPixelsY2 = sqrt(sqr(m_winx[3]-m_winx[2])+sqr(m_winy[3]-m_winy[2]));
	double screenPixelsX2 = sqrt(sqr(m_winx[0]-m_winx[3])+sqr(m_winy[0]-m_winy[3]));
	return min(imageSizeX/screenPixelsX1, min(imageSizeX/screenPixelsX2, min(imageSizeY/screenPixelsY1, imageSizeY/screenPixelsY2)));
}

void RasterLayerDrawer::setThreaded(bool yesno) {
	isThreaded = yesno;
}







