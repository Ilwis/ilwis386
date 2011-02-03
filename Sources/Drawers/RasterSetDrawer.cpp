#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\FieldList.h"
#include "Client\FormElements\selector.h"
#include "Client\FormElements\fldcolor.h"
#include "Client\Editors\Utils\MULTICOL.H"
#include "Client\FormElements\FieldIntSlider.h"
#include "Client\FormElements\FieldRealSlider.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\FldOneSelectTextOnly.h"
#include "Engine\Map\basemap.h"
#include "Client\MapWindow\Drawers\ComplexDrawer.h"
#include "Client\Ilwis.h"
#include "Engine\Map\Raster\Map.h"
#include "Engine\Base\System\RegistrySettings.h"
//#include "Client\Mapwindow\MapCompositionDoc.h"
#include "Client\Mapwindow\Drawers\RootDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "Drawers\SetDrawer.h"
#include "Drawers\ValueSlicer.h"
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
, data(new RasterSetData()), isThreaded(true), sameCsy(true), fUsePalette(false), fPaletteOwner(false), palette(0), textureHeap(new TextureHeap())
{
	setTransparency(1); // default, opaque
	//	setDrawMethod(drmNOTSET); // default
	//drawers.push_back(this); // nasty: i am my own child drawer
}

RasterSetDrawer::~RasterSetDrawer(){
	delete textureHeap;
	if (fPaletteOwner)
		delete palette;
	delete data;
}

void RasterSetDrawer::setMinMax(const RangeReal & rrMinMax)
{
	this->rrMinMax = rrMinMax;
}

void RasterSetDrawer::prepare(PreparationParameters *pp){
	SetDrawer::prepare(pp);

	if ( pp->type & NewDrawer::ptRENDER) {
		fUsePalette = drm != drmCOLOR;
		if (fPaletteOwner) {
			if (fUsePalette && palette->fValid())
				palette->Refresh();
		}
		textureHeap->PaletteChanged();
	}
	if ( pp->type & ptGEOMETRY | pp->type & ptRESTORE) {
		sameCsy = getRootDrawer()->getCoordinateSystem()->fnObj == csy->fnObj;
	}
}

void RasterSetDrawer::setDrawMethod(DrawMethod method) {

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
			else if (0 != _dm->pdp())
				drm = drmRPR;
		}
	} else
		drm = method;
}

void RasterSetDrawer::setRepresentation(const Representation& rp)
{
	SetDrawer::setRepresentation(rp);
	if (fPaletteOwner) {
		if (fUsePalette && palette->fValid())
			palette->Refresh();
	}
	textureHeap->PaletteChanged();
}

Palette * RasterSetDrawer::SetPaletteOwner()
{
	if (fPaletteOwner && palette)
		delete palette;
	palette = new Palette();
	fPaletteOwner = true;
	return palette;
}

void RasterSetDrawer::SetPalette(Palette * palette)
{
	if (fPaletteOwner && palette)
		delete palette;
	this->palette = palette;
	fPaletteOwner = false;
}

void RasterSetDrawer::init() const
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

		textureHeap->SetData(rastermap, getDrawingColor(), getDrawMethod(), drawcontext->getMaxPaletteSize(), rrMinMax, drawcontext);
		if (fPaletteOwner)
			palette->SetData(rastermap, this, drawcontext->getMaxPaletteSize(), rrMinMax);
		data->imageWidth = rastermap->rcSize().Col;
		data->imageHeight = rastermap->rcSize().Row;

		if (fPaletteOwner)
			if (fUsePalette)
				palette->Refresh();
	}
	data->init = true;
}

void RasterSetDrawer::addDataSource(void *bmap, int options){
	rastermap.SetPointer((BaseMapPtr *)bmap);
}

bool RasterSetDrawer::draw(bool norecursion , const CoordBounds& cbArea) const {

	drawPreDrawers(norecursion, cbArea);

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

		glEnable(GL_TEXTURE_2D);
		if (fUsePalette) {
			((AbstractMapDrawer*)getParentDrawer())->inactivateOtherPalettes(palette);
			palette->MakeCurrent(); // for now this is the only call .. officially it should also be called before generating textures in a separate thread, however currently the only way two palettes would interfere is with the AnimationDrawer, and there textures are generated in the current thread
		}
		DisplayImagePortion(minX, maxY, maxX, minY, 0, 0, width, height);
		glDisable(GL_TEXTURE_2D);
	}

	drawPostDrawers(norecursion, cbArea);

	return true;
}

void RasterSetDrawer::DisplayImagePortion(double x1, double y1, double x2, double y2, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY) const
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
		DisplayTexture(x1, y1, x2, y2, c1, c2, c3, c4, imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, zoomFactor);
}

void RasterSetDrawer::DisplayTexture(double x1, double y1, double x2, double y2, Coord & c1, Coord & c2, Coord & c3, Coord & c4, unsigned int imageOffsetX, unsigned int imageOffsetY, unsigned int imageSizeX, unsigned int imageSizeY, unsigned int zoomFactor) const
{
	Texture* tex = textureHeap->GetTexture(imageOffsetX, imageOffsetY, imageSizeX, imageSizeY, x1, y1, x2, y2, zoomFactor, fUsePalette, isThreaded);

	if (tex != 0)
	{
		// make the quad
		glBegin (GL_QUADS);

		if (sameCsy) {
			// avoid plotting the "added" portion of the map
			x2 = min(x2, data->cb.MaxX());
			y2 = max(y2, data->cb.MinY());

			tex->TexCoord2d(x1, y1);
			glVertex3d(x1, y1, 0.0);

			tex->TexCoord2d(x2, y1);
			glVertex3d(x2, y1, 0.0);

			tex->TexCoord2d(x2, y2);
			glVertex3d(x2, y2, 0.0);

			tex->TexCoord2d(x1, y2);
			glVertex3d(x1, y2, 0.0);
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

			tex->TexCoord2d(x1, y1);
			glVertex3d(c1.x, c1.y, 0.0);

			tex->TexCoord2d(x2, y1);
			glVertex3d(c2.x, c2.y, 0.0);

			tex->TexCoord2d(x2, y2);
			glVertex3d(c3.x, c3.y, 0.0);

			tex->TexCoord2d(x1, y2);
			glVertex3d(c4.x, c4.y, 0.0);
		}
	
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

void RasterSetDrawer::setThreaded(bool yesno) {
	isThreaded = yesno;
}

void RasterSetDrawer::addSelectionDrawers(const Representation& rpr) {
	BaseMapPtr *bmptr = ((AbstractMapDrawer *)getParentDrawer())->getBaseMap();
	BaseMap bmap;
	bmap.SetPointer(bmptr);
	RasterSetDrawer *rasterset;
	RangeReal rrmm = bmap->rrMinMax();
	NewDrawer *drwPost = getDrawer(RSELECTDRAWER,ComplexDrawer::dtPOST);
	if ( drwPost)
		removeDrawer(drwPost->getId());
	ILWIS::DrawerParameters parms(getRootDrawer(), getParentDrawer());
	rasterset = (RasterSetDrawer *)IlwWinApp()->getDrawer("RasterSetDrawer", "Ilwis38", &parms); 
	rasterset->setThreaded(false);
	rasterset->setRepresentation(rpr);
	rasterset->setMinMax(rrmm);
	rasterset->SetPalette(palette);
	PreparationParameters pp(NewDrawer::ptGEOMETRY | NewDrawer::ptRENDER);
	pp.csy = bmap->cs();
	rasterset->setName(name);
	rasterset->setRepresentation(bmptr->dm()->rpr()); //  default choice
	rasterset->getZMaker()->setSpatialSourceMap(bmap);
	rasterset->getZMaker()->setDataSourceMap(bmap);
	rasterset->addDataSource(bmptr);
	rasterset->prepare(&pp);
	Palette * palette = rasterset->SetPaletteOwner();
	addPostDrawer(RSELECTDRAWER,rasterset);

}

//-----------------------------------UI-------------------------
HTREEITEM RasterSetDrawer::configure(LayerTreeView  *tv, HTREEITEM parent){
	SetDrawer::configure(tv,parent);

	DisplayOptionTreeItem * itemSlicing = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&RasterSetDrawer::rasterSlicing);
	InsertItem(TR("Interactive Slicing"),"Slicing",itemSlicing);

	DisplayOptionTreeItem * itemSelect = new DisplayOptionTreeItem(tv, portrayalItem,this,
		0,(DisplayOptionItemFunc)&RasterSetDrawer::highLightSelection);
	InsertItem(TR("Attribute thresholds"),"SelectArea",itemSelect);

	return parent;
}


void RasterSetDrawer::rasterSlicing(CWnd *parent) {
	new InterActiveSlicing(parent,this);
}

void RasterSetDrawer::highLightSelection(CWnd *parent) {
	new HighLightDrawer(parent,this);
}

//----------------------------------------------------------
InterActiveSlicing::InterActiveSlicing(CWnd *par, RasterSetDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Slicing"))
{
	vs = new ValueSlicerSlider(root, adr);
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&InterActiveSlicing::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/3);
	FlatIconButton *fb = new FlatIconButton(fg,"Save","",(NotifyProc)&InterActiveSlicing::saveRpr,fnRpr);
	fb->Align(fldSteps, AL_AFTER);


	create();
}

int InterActiveSlicing::saveRpr(Event *ev) {
	CFileDialog filedlg (FALSE, "*.rpr", "*.rpr",OFN_HIDEREADONLY|OFN_NOREADONLYRETURN | OFN_LONGNAMES, "Ilwis Representation (*.rpr)|*.rpr||", NULL);
	if ( filedlg.DoModal() == IDOK) {
		String name(filedlg.GetPathName());
		vs->setFileNameRpr(FileName(name));
	}
	return 1;
}

int InterActiveSlicing::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"2");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
		}
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;
}

//---------------------------------------------------------------
HighLightDrawer::HighLightDrawer(CWnd *par, RasterSetDrawer *adr) 
	: DisplayOptionsForm2(adr, par, TR("Selection"))
{
	vs = new ValueSlicerSlider(root, adr);
	vs->setRprBase( ((AbstractMapDrawer *)(adr->getParentDrawer()))->getBaseMap()->dm()->rpr());
	vs->setLowColor(colorUNDEF);
	vs->setHighColor(colorUNDEF);
	vs->setNumberOfBounds(3);
	FieldGroup *fg = new FieldGroup(root);
	fldSteps = new FieldOneSelectTextOnly(fg, &steps);
	fldSteps->SetCallBack((NotifyProc)&HighLightDrawer::createSteps);
	fldSteps->Align(vs, AL_UNDER);
	fldSteps->SetWidth(vs->psn->iWidth/2);
	adr->addSelectionDrawers(vs->getRpr());

	create();

	//vs->setBoundColor(1,Color(120,230,0));
}

int HighLightDrawer::createSteps(Event*) {
	if (fldSteps->ose->GetCount() == 0) {
		for(int i = 2 ; i <= 10; ++i)
			fldSteps->AddString(String("%d",i));
		fldSteps->ose->SelectString(0,"3");
	} else {
		int mapIndex = fldSteps->ose->GetCurSel();
		if ( mapIndex != -1) {
			vs->setNumberOfBounds(mapIndex +2);
			for(int i = 0; i < mapIndex + 2; ++i) {
				if ( i % 2 == 1) {
					vs->setBoundColor(i,Color(200,0,0,0),0);
				} else {
					vs->setBoundColor(i,Color(0,0,0,255),255);
				}
			}
		}
		drw->getRootDrawer()->getDrawerContext()->doDraw();
	}
	return 1;
}


