#include "Client\Headers\formelementspch.h"
#include "Engine\Map\basemap.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Client\Mapwindow\LayerTreeView.h"
#include "Client\Mapwindow\LayerTreeItem.h" 
#include "Drawers\RasterSetDrawer.h"
#include "Client\Mapwindow\Drawers\ZValueMaker.h"
#include <GL/glu.h>
#include "Client\Mapwindow\Drawers\AbstractMapDrawer.h"
#include "Client\Mapwindow\Drawers\DrawerContext.h"
#include "Texture.h"


using namespace ILWIS;

ILWIS::NewDrawer *createRasterSetDrawer(DrawerParameters *parms) {
	return new RasterSetDrawer(parms);
}

RasterSetDrawer::RasterSetDrawer(DrawerParameters *parms) : 
	SetDrawer(parms,"RasterSetDrawer")
	, data(new RasterSetData())
{
//	setTransparency(1); // default
//	setDrawMethod(drmNOTSET); // default
}

RasterSetDrawer::~RasterSetDrawer(){
	if (data->textureHeap)
		delete data->textureHeap;
	delete data;
}

void RasterSetDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);

	bool v1 = pp->type & RootDrawer::ptINITOPENGL;
	bool v2 = pp->type & RootDrawer::ptALL;
	if ( pp->dc && (  v1 || v2 )) {

	}

	drm = drmRPR;
	stretched = false;
	riStretch = RangeInt(0,255);
	rrStretch = RangeReal(0,100);

	BaseMapPtr *bmptr = ((AbstractMapDrawer*)getParentDrawer())->getBaseMap();
	if (bmptr != 0) {
		MapPtr* pmp = dynamic_cast<MapPtr*>(bmptr);
		if (0 != pmp) {
			mp.SetPointer(pmp);
			// The following is from MapDrawer::MapDrawer
			Domain _dm = mp->dm();
			bool fImage = 0 != _dm->pdi();
			if (fImage)
				drm = drmIMAGE;
			else if (mp->dm()->pdcol())
				drm = drmCOLOR;
			ValueRange vr = mp->vr();
			if (mp->dm()->pdbool())
				vr = ValueRange();
			if (vr.fValid() || fImage) {
				stretched = true;
				if (!fImage && vr->vrr()) {
					rrStretch = mp->rrPerc1(true);
					if (rrStretch.rLo() >= rrStretch.rHi())
						rrStretch = mp->rrMinMax();
					if (rrStretch.rLo() >= rrStretch.rHi())
						rrStretch = vr->rrMinMax();
					riStretch.iLo() = (long)(rounding(rrStretch.rLo()));
					riStretch.iHi() = (long)(rounding(rrStretch.rHi()));
				} else {
					riStretch = mp->riPerc1(true);
					if (riStretch.iLo() >= riStretch.iHi())
						riStretch = mp->riMinMax();
					if (riStretch.iLo() >= riStretch.iHi())
						if (fImage)
							riStretch = RangeInt(0,255);
						else if (vr.fValid())
							riStretch = vr->riMinMax();
						rrStretch.rLo() = doubleConv(riStretch.iLo());
						rrStretch.rHi() = doubleConv(riStretch.iHi());
				}
			}
			if (0 != _dm->pdid())
				drm = drmMULTIPLE;
			else if (0 != _dm->pdp())
				drm = drmRPR;

			String sStretchMethod;
			ObjectInfo::ReadElement("Display", "Stretching", mp->fnObj, sStretchMethod);
			if ("Linear" == sStretchMethod)
				stretchMethod = smLINEAR;
			else if ("Logarithmic" == sStretchMethod)
				stretchMethod = smLOGARITHMIC;
		}
	}
}

void RasterSetDrawer::init() const
{
	// fetch the image's coordinate bounds
	if (mp.fValid())
	{
		data->cb = mp->cb();
		DrawerContext* drawcontext = (getRootDrawer())->getDrawerContext();
		data->maxTextureSize = drawcontext->getMaxTextureSize();
		int iXScreen = GetSystemMetrics(SM_CXFULLSCREEN); // maximum X size of client area (regardless of current viewport)
		int iYScreen = GetSystemMetrics(SM_CYFULLSCREEN); // maximum Y size of client area
		if (iXScreen < data->maxTextureSize) // prevent making textures that are larger than the screen, it is totally unnecessary and a big performance and memory hit
			data->maxTextureSize = iXScreen;
		if (iYScreen < data->maxTextureSize)
			data->maxTextureSize = iYScreen;

		data->textureHeap = new TextureHeap(mp, getDrawingColor(), getDrawMethod(), drawcontext);
		data->imageWidth = mp->rcSize().Col;
		data->imageHeight = mp->rcSize().Row;
	}
	data->init = true;
}

void RasterSetDrawer::addDataSource(void *bmap, int options){
}

HTREEITEM RasterSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	return SetDrawer::configure(tv,parent);
}

bool RasterSetDrawer::draw(bool norecursion , const CoordBounds& cbArea) const{
	if (!data->init)
		init();
	if (data->textureHeap == 0)
		return false;

	data->textureHeap->ClearQueuedTextures();

	// Extend the image so that its width and height become ^2

	double log2width = log((double)data->imageWidth)/log(2.0);
	log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	const unsigned long width = pow(2, log2width);
	double log2height = log((double)data->imageHeight)/log(2.0);
	log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	const unsigned long height = pow(2, log2height);
	// DisplayImagePortion(-1, 1, 1, -1, 0, 0, width, height);
	CoordBounds cb = data->cb;
	double minX = cb.MinX();
	double maxX = cb.MaxX();
	double minY = cb.MinY();
	double maxY = cb.MaxY();
	// Image has grown right-down
	maxX = minX + (maxX - minX) * (double)width / (double)data->imageWidth;
	minY = maxY + (minY - maxY) * (double)height / (double)data->imageHeight;

	DisplayImagePortion(minX, maxY, maxX, minY, 0, 0, width, height);
	
	return true;
}

void RasterSetDrawer::DisplayImagePortion(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const
{
	// if quad describes the "added" portion of the map, do not display
	if (x1 > data->cb.MaxX() || y1 < data->cb.MinY())
		return;

	// if quad is entirely outside viewport, do not display

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

	glFeedbackBuffer(2, GL_2D, feedbackBuffer);
	glRenderMode(GL_FEEDBACK);
	glBegin (GL_QUADS);
	glVertex3d(x1, y1, 0.0);
	glVertex3d(x2, y1, 0.0);
	glVertex3d(x2, y2, 0.0);
	glVertex3d(x1, y2, 0.0);
	glEnd();
	if (0 == glRenderMode(GL_RENDER))
		return;

	// for repetitive temporary calculations, added here for performance, to avoid repetitive use of stack/heap
	// 4 x 3 doubles to project into xy screen coordinates
	GLdouble m_winx[4];
	GLdouble m_winy[4];
	GLdouble m_winz[4];

		// project the quad to 2D
	gluProject(x1, y1, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
	gluProject(x1, y2, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
	gluProject(x2, y2, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
	gluProject(x2, y1, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);

	double zoom = getMinZoom(imageSizeX, imageSizeY, m_winx, m_winy); // the minimum zoomout-factor, indicating that it is necessary to plot the quad more accurately

	double log2zoom = log(zoom)/log(2.0);
	log2zoom = floor(log2zoom);
	const unsigned int zoomFactor = min(64, max(1, pow(2, log2zoom)));

	// split the visible portion of the image into a number of quads, depending on the accuracy needed

	// Divide the image into quads, as follows:
	// can we display the image with one quad?
	// // no, if the texture would be too small or too big
	// // yes, if the image is zoomed out so far that splitting the quad is unnecessary
	// // what is the required resolution of the quad?
	// // if the current quad meets the required resolution, do not split it, unless the texture would be too big
	// if not, split the quad into 4 equal parts, and re-apply the procedure to each of the 4 quads
	// if yes, calculate the quad and display it

	boolean split = false;

	if ((imageSizeX > 1 && imageSizeY > 1) && (imageSizeX / zoomFactor > data->maxTextureSize || imageSizeY / zoomFactor > data->maxTextureSize)) // imageSizeXY / zoomFactor is the required pixels of the quad
		split = true;
	if (split)
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
	else
		DisplayTexture(x1, y1, x2, y2, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
}

void RasterSetDrawer::DisplayTexture(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
		Texture* tex = data->textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, x1, y1, x2, y2, zoomFactor);

		if (tex != 0)
		{

			// avoid plotting the "added" portion of the map
			x2 = min(x2, data->cb.MaxX());
			y2 = max(y2, data->cb.MinY());

			// make the quad
			glBegin (GL_QUADS);

			tex->TexCoord2d(x1, y1);
			glVertex3d(x1, y1, 0.0);

			tex->TexCoord2d(x2, y1);
			glVertex3d( x2, y1, 0.0);

			tex->TexCoord2d(x2, y2);
			glVertex3d( x2, y2, 0.0);

			tex->TexCoord2d(x1, y2);
			glVertex3d(x1, y2, 0.0);

			glEnd();
		}
}

double RasterSetDrawer::getMinZoom(unsigned int imageSizeX, unsigned int imageSizeY, GLdouble * m_winx, GLdouble * m_winy) const
{
	double screenPixelsY1 = sqrt(sqr(m_winx[1]-m_winx[0])+sqr(m_winy[1]-m_winy[0]));
	double screenPixelsX1 = sqrt(sqr(m_winx[2]-m_winx[1])+sqr(m_winy[2]-m_winy[1]));
	double screenPixelsY2 = sqrt(sqr(m_winx[3]-m_winx[2])+sqr(m_winy[3]-m_winy[2]));
	double screenPixelsX2 = sqrt(sqr(m_winx[0]-m_winx[3])+sqr(m_winy[0]-m_winy[3]));
	return min(imageSizeX/screenPixelsX1, min(imageSizeX/screenPixelsX2, min(imageSizeY/screenPixelsY1, imageSizeY/screenPixelsY2)));
}
