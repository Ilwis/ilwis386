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
, data(new RasterSetData()), isThreaded(true), isThreadedBeforeOffscreen(true), sameCsy(true), fGrfLinear(true), fUsePalette(false), fPaletteOwner(false), palette(0), textureHeap(new TextureHeap()), textureHeapBeforeOffscreen(0), demTriangulator(0)
{
	setTransparency(1); // default, opaque
	//	setDrawMethod(drmNOTSET); // default
	//drawers.push_back(this); // nasty: i am my own child drawer
}

RasterLayerDrawer::RasterLayerDrawer(DrawerParameters *parms, const String& name) : LayerDrawer(parms, name), 
data(new RasterSetData()),
isThreaded(true), 
isThreadedBeforeOffscreen(true), 
sameCsy(true), 
fGrfLinear(true), 
fUsePalette(false), 
fPaletteOwner(false), 
palette(0), 
textureHeap(new TextureHeap()), 
textureHeapBeforeOffscreen(0), 
demTriangulator(0)
{
	setTransparency(1);
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

	if ( pp->type & NewDrawer::ptRENDER || pp->type & RootDrawer::ptRESTORE) {
		fUsePalette = drm != drmCOLOR;
		if ( rpr->prc()) {
			RepresentationClass *rprC = rpr->prc();
			for(int j =0 ; j < pp->filteredRaws.size(); ++j) {
				rprC->DoNotStore(true);
				int raw = pp->filteredRaws[j];
				Color clr = rprC->clrRaw(abs(raw));
				clr.m_transparency = raw > 0 ? 0 : 255;
				rprC->PutColor(abs(raw),clr);
			}
		}
		if (fPaletteOwner && fUsePalette && palette->fValid()) {
			palette->Refresh();
			getRootDrawer()->getDrawerContext()->setActivePalette(0);
		}
		textureHeap->RepresentationChanged();
		sameCsy = getRootDrawer()->getCoordinateSystem()->fnObj == csy->fnObj;
		fGrfLinear = gr()->fLinear();
	}
	if ((pp->type & pt3D) || ((pp->type & ptGEOMETRY || pp->type & ptRESTORE) && demTriangulator != 0)) {
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
	if (pp->type & ptOFFSCREENSTART) {
		isThreadedBeforeOffscreen = isThreaded;
		isThreaded = false;
		textureHeapBeforeOffscreen = textureHeap;
		DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
		textureHeap = new TextureHeap();
		setData();
		if (fPaletteOwner && fUsePalette) {
			palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);
			palette->Refresh();
			getRootDrawer()->getDrawerContext()->setActivePalette(0);
		}
	}
	if (pp->type & ptOFFSCREENEND) {
		isThreaded = isThreadedBeforeOffscreen;
		delete textureHeap;
		textureHeap = textureHeapBeforeOffscreen;
		if (fPaletteOwner && fUsePalette) {
			DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
			palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);
			palette->Refresh();
			getRootDrawer()->getDrawerContext()->setActivePalette(0);
		}
	}
	if (pp->type & ptREDRAW)
		textureHeap->ReGenerateAllTextures();
}

void RasterLayerDrawer::setData() const {
	DrawerContext* drawcontext = getRootDrawer()->getDrawerContext();
	textureHeap->SetData(rastermap, getDrawingColor(), getDrawMethod(), drawcontext->getMaxPaletteSize(), data, rrMinMax, drawcontext);
}

GeoRef RasterLayerDrawer::gr() const {
	return rastermap->gr();
}


void RasterLayerDrawer::setDrawMethod(DrawMethod method) {

	if ( method == drmINIT) {
		drm = drmRPR;
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
	if (fPaletteOwner && this->palette)
		delete this->palette;
	this->palette = palette;
	fPaletteOwner = false;
}

void RasterLayerDrawer::init() const
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
	if ( !gr()->rcSize().fUndef()) {
		data->imageWidth = gr()->rcSize().Col;
		data->imageHeight = gr()->rcSize().Row;
	} else if ( rastermap.fValid()) {
		data->imageWidth = rastermap->rcSize().Col;
		data->imageHeight = rastermap->rcSize().Row;
	}

	double log2width = log((double)data->imageWidth)/log(2.0);
	log2width = max(6, ceil(log2width)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	data->width = pow(2, log2width);
	double log2height = log((double)data->imageHeight)/log(2.0);
	log2height = max(6, ceil(log2height)); // 2^6 = 64 = the minimum texture size that OpenGL/TexImage2D supports
	data->height = pow(2, log2height);

	setData();

	if (fPaletteOwner && fUsePalette) {
		palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);
		palette->Refresh();
		getRootDrawer()->getDrawerContext()->setActivePalette(0);
	}
	data->init = true;
}

void RasterLayerDrawer::addDataSource(void *bmap, int options){
	IlwisObject *obj = (IlwisObject *)bmap;
	if ( IOTYPE((*(obj))->fnObj) == IlwisObject::iotRASMAP)
		rastermap.SetPointer(obj->pointer());
}

bool RasterLayerDrawer::draw( const CoordBounds& cbArea) const {

	//LayerDrawer::draw(cbArea);
	drawPreDrawers(cbArea);

	if (!data->init)
		init();
	if (textureHeap->fValid())
	{
		glClearColor(1.0,1.0,1.0,0.0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
		glColor4f(1, 1, 1, transparency);

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
		DisplayImagePortion(0, 0, data->width, data->height);
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

void RasterLayerDrawer::DisplayImagePortion(unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const
{
	// if patch describes the "added" portion of the map, do not display
	if (imageOffsetX > data->imageWidth || imageOffsetY > data->imageHeight)
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

	Coord b1, b2, b3, b4;
	gr()->RowCol2Coord(imageOffsetY, imageOffsetX, b1);
	gr()->RowCol2Coord(imageOffsetY, imageOffsetX + imageSizeX, b2);
	gr()->RowCol2Coord(imageOffsetY + imageSizeY, imageOffsetX + imageSizeX, b3);
	gr()->RowCol2Coord(imageOffsetY + imageSizeY, imageOffsetX, b4);
	Coord c1, c2, c3, c4;
	glFeedbackBuffer(2, GL_2D, feedbackBuffer);
	glRenderMode(GL_FEEDBACK);
	glBegin (GL_QUADS);
	if (sameCsy) {
		glVertex3d(b1.x, b1.y, 0.0);
		glVertex3d(b2.x, b2.y, 0.0);
		glVertex3d(b3.x, b3.y, 0.0);
		glVertex3d(b4.x, b4.y, 0.0);
	} else {
		c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b1);
		c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b2);
		c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b3);
		c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b4);
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
		gluProject(b1.x, b1.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[0], &m_winy[0], &m_winz[0]);
		gluProject(b2.x, b2.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[1], &m_winy[1], &m_winz[1]);
		gluProject(b3.x, b3.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[2], &m_winy[2], &m_winz[2]);
		gluProject(b4.x, b4.y, 0.0, m_modelMatrix, m_projMatrix, m_viewport, &m_winx[3], &m_winy[3], &m_winz[3]);
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
		int sizeX2 = imageSizeX / 2;
		int sizeY2 = imageSizeY / 2;
		// Q1
		DisplayImagePortion(imageOffsetX, imageOffsetY, sizeX2, sizeY2);
		// Q2
		DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY, sizeX2, sizeY2);
		// Q3
		DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY + sizeY2, sizeX2, sizeY2);
		// Q4
		DisplayImagePortion(imageOffsetX, imageOffsetY + sizeY2, sizeX2, sizeY2);
	}
	else if (xSplit)
	{
		int sizeX2 = imageSizeX / 2;
		// Q1
		DisplayImagePortion(imageOffsetX, imageOffsetY, sizeX2, imageSizeY);
		// Q2
		DisplayImagePortion(imageOffsetX + sizeX2, imageOffsetY, sizeX2, imageSizeY);
	}
	else if (ySplit)
	{
		int sizeY2 = imageSizeY / 2;
		// Q1
		DisplayImagePortion(imageOffsetX, imageOffsetY, imageSizeX, sizeY2);
		// Q2
		DisplayImagePortion(imageOffsetX, imageOffsetY + sizeY2, imageSizeX, sizeY2);
	}
	else
	{
		if (getRootDrawer()->is3D() && demTriangulator)
			DisplayTexture3D(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
		else
			DisplayTexture(c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
	}
}

void RasterLayerDrawer::DisplayTexture(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
	Texture* tex = textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor, fUsePalette ? palette : 0, isThreaded);

	if (tex != 0)
	{
		// make the quad
		glBegin (GL_QUADS);

		if (sameCsy && fGrfLinear) {
			// texture bounds
			double s1 = imageOffsetX / (double)data->width;
			double t1 = imageOffsetY / (double)data->height;
			double s2 = min(imageOffsetX + imageSizeX, data->imageWidth) / (double)data->width;
			double t2 = min(imageOffsetY + imageSizeY, data->imageHeight) / (double)data->height;

			Coord b1, b2, b3, b4;
			gr()->RowCol2Coord(imageOffsetY, imageOffsetX, b1);
			gr()->RowCol2Coord(imageOffsetY, min(imageOffsetX + imageSizeX, data->imageWidth), b2);
			gr()->RowCol2Coord(min(imageOffsetY + imageSizeY, data->imageHeight), min(imageOffsetX + imageSizeX, data->imageWidth), b3);
			gr()->RowCol2Coord(min(imageOffsetY + imageSizeY, data->imageHeight), imageOffsetX, b4);

			glTexCoord2d(s1, t1);
			glVertex3d(b1.x, b1.y, 0.0);

			glTexCoord2d(s2, t1);
			glVertex3d(b2.x, b2.y, 0.0);

			glTexCoord2d(s2, t2);
			glVertex3d(b3.x, b3.y, 0.0);

			glTexCoord2d(s1, t2);
			glVertex3d(b4.x, b4.y, 0.0);
		} else if (sameCsy) {
			const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
			// avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
			double colStep = min(imageSizeX, data->imageWidth - imageOffsetX) / (double)iSize;
			double rowStep = min(imageSizeY, data->imageHeight - imageOffsetY) / (double)iSize;

			double s1 = imageOffsetX / (double)data->width;
			for (int x = 0; x < iSize; ++x) {
				double s2 = s1 + colStep / (double)data->width;
				double t1 = imageOffsetY / (double)data->height;

				Coord b1, b2, b3, b4;
				gr()->RowCol2Coord(imageOffsetY, imageOffsetX + colStep * x, b1);
				gr()->RowCol2Coord(imageOffsetY, imageOffsetX + colStep * (x + 1), b2);

				for (int y = 1; y <= iSize ; ++y) {
					double t2 = t1 + rowStep / (double)data->height;
		
					gr()->RowCol2Coord(imageOffsetY + rowStep * y, imageOffsetX + colStep * (x + 1), b3);
					gr()->RowCol2Coord(imageOffsetY + rowStep * y, imageOffsetX + colStep * x, b4);

					glTexCoord2d(s1, t1);
					glVertex3d(b1.x, b1.y, 0.0);

					glTexCoord2d(s2, t1);
					glVertex3d(b2.x, b2.y, 0.0);

					glTexCoord2d(s2, t2);
					glVertex3d(b3.x, b3.y, 0.0);

					glTexCoord2d(s1, t2);
					glVertex3d(b4.x, b4.y, 0.0);

					t1 = t2;
					b1 = b4;
					b2 = b3;
				}
				s1 = s2;
			}
		} else {
			const unsigned int iSize = 10; // this makes 100 quads, thus 200 triangles per texture
			// avoid plotting the "added" portion of the map that was there to make the texture size a power of 2
			double colStep = min(imageSizeX, data->imageWidth - imageOffsetX) / (double)iSize;
			double rowStep = min(imageSizeY, data->imageHeight - imageOffsetY) / (double)iSize;

			double s1 = imageOffsetX / (double)data->width;
			for (int x = 0; x < iSize; ++x) {
				double s2 = s1 + colStep / (double)data->width;
				double t1 = imageOffsetY / (double)data->height;

				Coord b1, b2, b3, b4;
				gr()->RowCol2Coord(imageOffsetY, imageOffsetX + colStep * x, b1);
				gr()->RowCol2Coord(imageOffsetY, imageOffsetX + colStep * (x + 1), b2);

				c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b1);
				c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b2);
				for (int y = 1; y <= iSize ; ++y) {
					double t2 = t1 + rowStep / (double)data->height;
		
					gr()->RowCol2Coord(imageOffsetY + rowStep * y, imageOffsetX + colStep * (x + 1), b3);
					gr()->RowCol2Coord(imageOffsetY + rowStep * y, imageOffsetX + colStep * x, b4);

					c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b3);
					c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b4);

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

void RasterLayerDrawer::DisplayTexture3D(Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
	Texture* tex = textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor, fUsePalette ? palette : 0, isThreaded);

	if (tex != 0)
	{
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Coord b1, b2, b3, b4;
		gr()->RowCol2Coord(imageOffsetY, imageOffsetX, b1);
		gr()->RowCol2Coord(imageOffsetY, min(imageOffsetX + imageSizeX, data->imageWidth), b2);
		gr()->RowCol2Coord(min(imageOffsetY + imageSizeY, data->imageHeight), min(imageOffsetX + imageSizeX, data->imageWidth), b3);
		gr()->RowCol2Coord(min(imageOffsetY + imageSizeY, data->imageHeight), imageOffsetX, b4);

		if (sameCsy) {
			double clip_plane0[]={b3.y - b2.y, b2.x - b3.x, 0.0, b3.x * (b2.y - b3.y) - b3.y * (b2.x - b3.x)}; // x < x2
			double clip_plane1[]={b1.y - b4.y, b4.x - b1.x, 0.0, b1.x * (b4.y - b1.y) - b1.y * (b4.x - b1.x)}; // x > x1
			double clip_plane2[]={b4.y - b3.y, b3.x - b4.x, 0.0, b4.x * (b3.y - b4.y) - b4.y * (b3.x - b4.x)}; // y > y1
			double clip_plane3[]={b2.y - b1.y, b1.x - b2.x, 0.0, b2.x * (b1.y - b2.y) - b2.y * (b1.x - b2.x)}; // y < y2
			glClipPlane(GL_CLIP_PLANE0,clip_plane0);
			glClipPlane(GL_CLIP_PLANE1,clip_plane1);
			glClipPlane(GL_CLIP_PLANE2,clip_plane2);
			glClipPlane(GL_CLIP_PLANE3,clip_plane3);

		} else {
			c1 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b1);
			c2 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b2);
			c3 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b3);
			c4 = getRootDrawer()->getCoordinateSystem()->cConv(csy, b4);
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

Representation RasterLayerDrawer::getRepresentation() const { // avoiding copy constructotrs
	return LayerDrawer::getRepresentation();
}










