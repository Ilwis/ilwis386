#include "Headers\toolspch.h"
#include "Engine\Map\basemap.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Drawers\ComplexDrawer.h"
#include "Engine\Drawers\SimpleDrawer.h" 
#include "Engine\Base\System\RegistrySettings.h"
#include "Engine\Drawers\RootDrawer.h"
#include "Engine\Drawers\SpatialDataDrawer.h"
#include "Drawers\LayerDrawer.h"
#include "Drawers\featuredatadrawer.h"
#include "Drawers\FeatureLayerDrawer.h"
#include "Drawers\DrawingColor.h" 
#include "SpaceTimeDrawer.h"
#include "SpaceTimeElementsDrawer.h"
#include "Drawers\PointDrawer.h"
#include "Drawers\PointFeatureDrawer.h"
#include "Engine\Representation\Rprclass.h"
#include "Engine\Drawers\DrawerContext.h"
#include "Engine\Drawers\ZValueMaker.h"

using namespace ILWIS;

SpaceTimeDrawer::SpaceTimeDrawer(DrawerParameters *parms, const String& name)
: FeatureLayerDrawer(parms, name)
, prevUseAttColumn(false)
, subDisplayLists(new map<long, GLuint>())
, nrSteps(-1)
, spaceTimeElementsDrawer(0)
, timePos(0)
, fClipTPlus(false)
, fClipTMinus(false)
{
	ppcopy = new PointProperties();
	displayList = new GLuint;
	*displayList = 0;
	fRefreshDisplayList = new bool;
	*fRefreshDisplayList = false;
	texture = new GLuint [2];
	texture[0] = 0;
	fRefreshTexture = new bool;
	*fRefreshTexture = false;
	fHatching = new bool;
	*fHatching = false;
	csDraw = new CCriticalSection ();
	DrawerParameters dp(rootDrawer, rootDrawer);
	spaceTimeElementsDrawer = (SpaceTimeElementsDrawer *)NewDrawer::getDrawer("SpaceTimeElementsDrawer", "Cube", &dp);
	spaceTimeElementsDrawer->SetSpaceTimeDrawer(this);
}

SpaceTimeDrawer::~SpaceTimeDrawer() {
	csDraw->Lock(); // wait here til drawing finishes
	csDraw->Unlock();
	delete csDraw;
	for (map<long, GLuint>::iterator mapEntry = subDisplayLists->begin(); mapEntry != subDisplayLists->end(); ++mapEntry)
			glDeleteLists(mapEntry->second, 1); // which OpenGL context?
	delete subDisplayLists;
	if (*displayList != 0) // which OpenGL context?
		glDeleteLists(*displayList, 2);
	delete displayList;
	delete fRefreshDisplayList;
	if (texture[0] != 0)
		glDeleteTextures(2, texture);
	delete [] texture;
	delete fRefreshTexture;
	delete fHatching;
	delete spaceTimeElementsDrawer;
	delete ppcopy;
}

NewDrawer *SpaceTimeDrawer::createElementDrawer(PreparationParameters *pp, ILWIS::DrawerParameters* parms) const{
	return NewDrawer::getDrawer("PointFeatureDrawer", pp,parms);
}

SpaceTimeElementsDrawer * SpaceTimeDrawer::getAdditionalElementsDrawer()
{
	return spaceTimeElementsDrawer;
}

BaseMapPtr *SpaceTimeDrawer::getBasemap()
{
	return basemap;
}

void SpaceTimeDrawer::prepareChildDrawers(PreparationParameters *pp){
	prepare(pp);
}

void SpaceTimeDrawer::prepare(PreparationParameters *parms){
	if (!isActive())
		return;
	clock_t start = clock();
	LayerDrawer::prepare(parms);
	FeatureDataDrawer *mapDrawer = (FeatureDataDrawer *)parentDrawer;
	if ( (parms->type & RootDrawer::ptGEOMETRY) || (parms->type & NewDrawer::pt3D) || (parms->type & NewDrawer::ptRESTORE)) {
		bool isAnimation = mapDrawer->getType() == "AnimationDrawer";
		if ( isAnimation ) {
			basemap = fbasemap.ptr();
		} else {
			basemap = mapDrawer->getBaseMap(parms->index);
		}

		double rMinProximity = basemap->cb().width() / 50.0; // for now use this for the 2D pixelinfo (to be properly solved with a 3D pixelinfo later)
		if (basemap->rProximity() < rMinProximity)
			basemap->SetProximity(rMinProximity); // do a serious effort to search for the pixelinfo-coordinate over the whole width of the space time path.. could this accidentally be saved back in the ODF?

		if (useAttColumn && attColumn.fValid())
			fValueMap = attColumn->dm()->pdv() != 0;
		else
			fValueMap = basemap->dm()->pdv() != 0;

		cube = rootDrawer->getMapCoordBounds();
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

		long numberOfFeatures = basemap->iFeatures();
		if (nrSteps < 1) {
			nrSteps = max(1, min(25, (5.0 - log10((double)numberOfFeatures)) * 15.0));
			if (nrSteps == 2)
				nrSteps = 1;
		}

		//Tranquilizer trq(TR("preparing data"));
		*fRefreshDisplayList = true;

	} if ( parms->type & NewDrawer::ptRENDER || parms->type & NewDrawer::pt3D || parms->type & NewDrawer::ptRESTORE) {
		PreparationParameters pp(parms);
		pp.type = pp.type & ~NewDrawer::ptGEOMETRY;

		if (useAttColumn && parms->filteredRaws.size() > 0) {
			long iRaw = attColumn->iRaw(abs(parms->filteredRaws[0]));
			bool fOn = parms->filteredRaws[0] >= 0;
			if (fOn)
				disabledRaws.erase(find(disabledRaws.begin(), disabledRaws.end(), iRaw));
			else
				disabledRaws.push_back(iRaw);
		}

		if (selectedRaws != parms->rowSelect.raws) {
			selectedRaws = parms->rowSelect.raws;
			selectedObjectIDs = getObjectIDs(selectedRaws);
			// highlight selected items
			if (rootDrawer->getDrawerContext()->TakeContext()) {
				long numberOfFeatures = basemap->iFeatures();
				bool fPrevSelected = false;
				for (map<long, GLuint>::iterator mapEntry = subDisplayLists->begin(); mapEntry != subDisplayLists->end(); ++mapEntry) {
					bool fSelected = find(selectedObjectIDs.begin(), selectedObjectIDs.end(), mapEntry->first) != selectedObjectIDs.end();
					glNewList(mapEntry->second, GL_COMPILE);
					if (fSelected && !fPrevSelected)
						glTranslated(0, 0.5, 0);
					else if (fPrevSelected && !fSelected)
						glTranslated(0, -0.5, 0);
					glEndList();
					fPrevSelected = fSelected;
				}
				rootDrawer->getDrawerContext()->ReleaseContext();
			}
		}

		*fRefreshTexture = true;

		if ( *properties != *ppcopy ) {
			*ppcopy = *properties;
			if ( properties->scaleMode != PointProperties::sNONE && properties->stretchColumn != "" && basemap->fTblAtt()) {
				Table attTable = basemap->tblAtt();
				Column col = attTable->col(properties->stretchColumn);
				if ( col.fValid() && col->dm()->pdv()) {
					SetSizeAttribute(col);
					SetSizeStretch(&(properties->stretchRange));
				}
			} else
				SetNoSize();
			*fRefreshDisplayList = true;
		}
	}
	if (spaceTimeElementsDrawer)
		spaceTimeElementsDrawer->prepare(parms);

	clock_t end = clock();
	double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	TRACE("Space Time Drawer Prepared in %2.2f seconds;\n", duration/1000);
}

String SpaceTimeDrawer::store(const FileName& fnView, const String& parentSection) const{
	FeatureLayerDrawer::store(fnView, parentSection);
	ObjectInfo::WriteElement(parentSection.c_str(), "NrEdges", fnView, nrSteps);
	storeTemporal(fnView, parentSection);
	storeSizable(fnView, parentSection);
	if (spaceTimeElementsDrawer)
		spaceTimeElementsDrawer->store(fnView, parentSection);
	return parentSection;
}

void SpaceTimeDrawer::load(const FileName& fnView, const String& currentSection){
	FeatureLayerDrawer::load(fnView, currentSection);
	ObjectInfo::ReadElement(currentSection.c_str(), "NrEdges", fnView, nrSteps);
	loadTemporal(fnView, currentSection);
	loadSizable(fnView, currentSection);
	if (spaceTimeElementsDrawer)
		spaceTimeElementsDrawer->load(fnView, currentSection);
}

RangeReal SpaceTimeDrawer::getValueRange(Column attributeColumn) const {
	RangeReal rrMinMax (0, 255);
	Domain dm = basemap->dm();
	if (dm.fValid() && (dm->pdbit() || dm->pdbool()))
		rrMinMax = RangeReal(1,2);
	else if ( basemap->dm()->pdv()) {
		rrMinMax = basemap->rrMinMax(BaseMapPtr::mmmCALCULATE); // not mmmSAMPLED here, to get a more accurate result, otherwise there's a high chance of artifacts since the sampling is only done on this one band
		if (!rrMinMax.fValid())
			rrMinMax = basemap->vr()->rrMinMax();
	} else {
		if (attributeColumn.fValid() && attributeColumn->dm()->pdv()) {
			rrMinMax = attributeColumn->rrMinMax();
			if (!rrMinMax.fValid())
				rrMinMax = attributeColumn->vr()->rrMinMax();
		}
	}
	return rrMinMax;
}

void SpaceTimeDrawer::setRepresentation(const Representation& rp) {
	// assumption: setRepresentation is always called after useAttColumn or attColumn has changed
	FeatureLayerDrawer::setRepresentation(rp);
	// set *fRefreshDisplayList = true when the texture coordinates need to change (thus when the attribute column changed)
	if ((prevUseAttColumn != useAttColumn) || (useAttColumn && (prevAttColumn != attColumn))) {
		*fRefreshDisplayList = true;
		prevUseAttColumn = useAttColumn;
		prevAttColumn = attColumn;
		disabledRaws.clear();

		if (useAttColumn && attColumn.fValid())
			fValueMap = attColumn->dm()->pdv() != 0;
		else
			fValueMap = basemap->dm()->pdv() != 0;
	}
}

void SpaceTimeDrawer::setDrawMethod(DrawMethod method) {
	if ( method == drmINIT) { 
		if ( useInternalDomain() || !rpr.fValid()) {
			setDrawMethod(drmSINGLE);
		}
		else if ( rpr.fValid()) {
			setDrawMethod(drmRPR);
		}
	}
	else
		drm = method; 
}

GeneralDrawerProperties *SpaceTimeDrawer::getProperties() {
	return properties;
}

void testError() {
	
	DWORD dw = GetLastError(); 
	if (dw != 0) {
		LPVOID lpMsgBuf;
		LPVOID lpDisplayBuf;
		FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			dw,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPTSTR) &lpMsgBuf,
			0, NULL );

		// Display the error message and exit the process

		lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT, 
			(lstrlen((LPCTSTR)lpMsgBuf)+lstrlen((LPCTSTR)TEXT("my function"))+40)*sizeof(TCHAR)); 
		sprintf((LPTSTR)lpDisplayBuf, 
			//LocalSize(lpDisplayBuf),
			TEXT("%s failed with error %d: %s"), 
			dw, lpMsgBuf); 
		MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK); 

		LocalFree(lpMsgBuf);
		LocalFree(lpDisplayBuf);
		ExitProcess(dw);
	}

}

void SpaceTimeDrawer::SetNrSteps(int steps)
{
	if (steps == 2 || steps < 1)
		nrSteps = 1;
	else if (steps > 25)
		nrSteps = 25;
	else
		nrSteps = steps;
}

int SpaceTimeDrawer::iNrSteps()
{
	return nrSteps;
}

struct sort_pair_first_value { 
	bool operator()(const std::pair<GLuint, GLuint> &left, const std::pair<GLuint, GLuint> &right) { 
        return left.first < right.first;
    } 
}; 

void SpaceTimeDrawer::SetClipTPlus(bool fClip) {
	fClipTPlus = fClip;
}

void SpaceTimeDrawer::SetClipTMinus(bool fClip) {
	fClipTMinus = fClip;
}

vector<GLuint> SpaceTimeDrawer::getSelectedObjectIDs(const CRect& rect) const
{
	vector<GLuint> selectedObjectIDs;

	bool fSpaceTimeDrawer = true;
	bool fFootprintDrawer = true;
	
	if (spaceTimeElementsDrawer) {
		fSpaceTimeDrawer = (*((PathElementProperties*)spaceTimeElementsDrawer->getProperties()))["spacetimepath"].visible;
		fFootprintDrawer = (*((PathElementProperties*)spaceTimeElementsDrawer->getProperties()))["footprint"].visible;
	}

	if (rootDrawer->getDrawerContext()->TakeContext())
	{
		if (fSpaceTimeDrawer && *displayList != 0)
		{
			const unsigned int SELECT_BUF_SIZE = 2048;
			GLuint selectBuf [SELECT_BUF_SIZE];
			GLint viewport[4];
			glGetIntegerv(GL_VIEWPORT, viewport);
			glSelectBuffer(SELECT_BUF_SIZE, selectBuf);
			glRenderMode(GL_SELECT);
			glMatrixMode(GL_PROJECTION);
			glPushMatrix();
			glLoadIdentity();
			gluPickMatrix(rect.CenterPoint().x, rect.CenterPoint().y, max(1, rect.Width()), max(1, rect.Height()), viewport);
			CoordBounds cbZoom = rootDrawer->getCoordBoundsZoom();
			if (rootDrawer->is3D()) {
				Coord eyePoint = rootDrawer->getEyePoint();
				Coord viewPoint = rootDrawer->getViewPoint();
				double windowAspectRatio = (double)viewport[2] / (double)viewport[3]; // (double)(rc.Col) / (double)(rc.Row)
				double zNear = max(abs(eyePoint.x - viewPoint.x), abs(eyePoint.y - viewPoint.y)) / 2.0;
				double zFar = max(cbZoom.width(), cbZoom.height()) * 4.0;
				gluPerspective(30.0, windowAspectRatio, zNear, zFar);
			} else {
				glOrtho(cbZoom.cMin.x,cbZoom.cMax.x,cbZoom.cMin.y,cbZoom.cMax.y,-1,1);
			}
			glMatrixMode(GL_MODELVIEW);
			CoordBounds cbArea;
			drawPreDrawers(drl3DOPAQUE, cbArea);
			glPushMatrix();
			if (!rootDrawer->is3D())
				glScaled(1,1,0);
			glMatrixMode(GL_TEXTURE);
			glPushMatrix();
			glCallList(*displayList);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			glPopMatrix();
			glMatrixMode(GL_PROJECTION);
			glPopMatrix();
			glMatrixMode(GL_MODELVIEW);
			drawPostDrawers(drl3DOPAQUE, cbArea);
			int nrObjects = glRenderMode(GL_RENDER);
			vector<std::pair<GLuint, GLuint>> sortedObjectIDs;
			for (int i = 0; i < nrObjects; ++i) {
				GLuint minZ = selectBuf[i * 4 + 1];
				GLuint objectID = selectBuf[i * 4 + 3];
				sortedObjectIDs.push_back(std::pair<GLuint, GLuint>(minZ, objectID));
			}
			std::stable_sort(sortedObjectIDs.begin(), sortedObjectIDs.end(), sort_pair_first_value());
			for(vector<std::pair<GLuint, GLuint>>::const_iterator it = sortedObjectIDs.begin(); it != sortedObjectIDs.end(); ++it)
				selectedObjectIDs.push_back((*it).second);
		}
		rootDrawer->getDrawerContext()->ReleaseContext();
	}
	return selectedObjectIDs;
}

String SpaceTimeDrawer::getInfo(const Coord& c) const
{
	if ( !hasInfo() || !isActive() )
		return "";
	String info;
	RowCol rc = rootDrawer->WorldToScreen(c);
	CRect rect (rc.Col, rc.Row, rc.Col + 1, rc.Row + 1);
	vector<GLuint> objectIDs = getSelectedObjectIDs(rect);
	int i = getNearestEnabledObjectIDIndex(objectIDs);
	if (i < objectIDs.size()) {
		GLuint objectID = objectIDs[i];
		// construct info
		Feature * feature = getFeature(objectID);
		if (feature != 0) {
			if (!useAttColumn) {
				SpatialDataDrawer *mapDrawer = (SpatialDataDrawer *)parentDrawer;
				BaseMapPtr *bmptr = mapDrawer->getBaseMap(mapDrawer->getCurrentIndex());
				if (bmptr->dvrs().fRawAvailable()) {
					long raw = feature->iValue();
					info = bmptr->dvrs().sValueByRaw(raw);
				} else {
					double val = feature->rValue();
					info = bmptr->dvrs().sValue(val);
				}
			} else if (getAtttributeColumn().fValid()) {
				long raw = feature->iValue();
				if (raw != iUNDEF)
					info = getAtttributeColumn()->sValue(raw);
				else
					info = "?";
			} else
				info = "?";
		} else
			info = "?";
	}
	return info;
}

void SpaceTimeDrawer::select(const CRect& rect, vector<long> & selectedRaws, SelectionMode selectionMode)
{
	//vector<long> raws;
	RowCol viewPort = rootDrawer->getViewPort();
	CRect rectGL (rect.left, viewPort.Row - rect.bottom, rect.right, viewPort.Row - rect.top);
	vector<GLuint> newlySelectedObjectIDs = getSelectedObjectIDs(rectGL);
	newlySelectedObjectIDs = getEnabledObjectIDs(newlySelectedObjectIDs);
	if (selectionMode == SELECTION_NEW) {
		selectedRaws.clear();
		for (int i = 0; i < newlySelectedObjectIDs.size(); ++i) {
			GLuint objectID = newlySelectedObjectIDs[i];
			getRaws(objectID, selectedRaws);
		}
		selectedObjectIDs = newlySelectedObjectIDs;
	} else if (selectionMode == SELECTION_ADD) {
		vector<long> newlySelectedRaws;
		for (int i = 0; i < newlySelectedObjectIDs.size(); ++i) {
			GLuint objectID = newlySelectedObjectIDs[i];
			getRaws(objectID, newlySelectedRaws);
		}
		sort(selectedRaws.begin(), selectedRaws.end());
		sort(newlySelectedRaws.begin(), newlySelectedRaws.end());
		vector<long> tmpRaws;
		set_union(selectedRaws.begin(), selectedRaws.end(), newlySelectedRaws.begin(), newlySelectedRaws.end(), back_inserter(tmpRaws));
		selectedRaws.swap(tmpRaws);
		sort(selectedObjectIDs.begin(), selectedObjectIDs.end());
		sort(newlySelectedObjectIDs.begin(), newlySelectedObjectIDs.end());
		vector<GLuint> tmpIDs;
		set_union(selectedObjectIDs.begin(), selectedObjectIDs.end(), newlySelectedObjectIDs.begin(), newlySelectedObjectIDs.end(), back_inserter(tmpIDs));
		selectedObjectIDs.swap(tmpIDs);
	} else if (selectionMode == SELECTION_REMOVE) {
		vector<long> newlySelectedRaws;
		for (int i = 0; i < newlySelectedObjectIDs.size(); ++i) {
			GLuint objectID = newlySelectedObjectIDs[i];
			getRaws(objectID, newlySelectedRaws);
		}
		sort(selectedRaws.begin(), selectedRaws.end());
		sort(newlySelectedRaws.begin(), newlySelectedRaws.end());
		vector<long> tmpRaws;
		set_difference(selectedRaws.begin(), selectedRaws.end(), newlySelectedRaws.begin(), newlySelectedRaws.end(), back_inserter(tmpRaws));
		selectedRaws.swap(tmpRaws);
		sort(selectedObjectIDs.begin(), selectedObjectIDs.end());
		sort(newlySelectedObjectIDs.begin(), newlySelectedObjectIDs.end());
		vector<GLuint> tmpIDs;
		set_difference(selectedObjectIDs.begin(), selectedObjectIDs.end(), newlySelectedObjectIDs.begin(), newlySelectedObjectIDs.end(), back_inserter(tmpIDs));
		selectedObjectIDs.swap(tmpIDs);
	}

	// highlight selected item

	if (rootDrawer->getDrawerContext()->TakeContext()) {
		long numberOfFeatures = basemap->iFeatures();
		bool fPrevSelected = false;
		for (map<long, GLuint>::iterator mapEntry = subDisplayLists->begin(); mapEntry != subDisplayLists->end(); ++mapEntry) {
			bool fSelected = (find(selectedObjectIDs.begin(), selectedObjectIDs.end(), mapEntry->first) != selectedObjectIDs.end());
			glNewList(mapEntry->second, GL_COMPILE);
			if (fSelected && !fPrevSelected)
				glTranslated(0, 0.5, 0);
			else if (fPrevSelected && !fSelected)
				glTranslated(0, -0.5, 0);
			glEndList();
			fPrevSelected = fSelected;
		}
		rootDrawer->getDrawerContext()->ReleaseContext();
	}
}

void SpaceTimeDrawer::getHatch(RepresentationClass * prc, long iRaw, const byte* &hatch) const {
	String hatchName = prc->sHatch(iRaw);
	if ( hatchName != sUNDEF) {
		const SVGLoader *loader = NewDrawer::getSvgLoader();
		SVGLoader::const_iterator cur = loader->find(hatchName);
		if ( cur != loader->end() && (*cur).second->getType() == IVGElement::ivgHATCH)
			hatch = (*cur).second->getHatch();
	}
}

void SpaceTimeDrawer::getHatchInverse(RepresentationClass * prc, long iRaw, const byte* &hatchInverse) const {
	String hatchName = prc->sHatch(iRaw);
	if ( hatchName != sUNDEF) {
		const SVGLoader *loader = NewDrawer::getSvgLoader();
		SVGLoader::const_iterator cur = loader->find(hatchName);
		if ( cur != loader->end() && (*cur).second->getType() == IVGElement::ivgHATCH)
			hatchInverse = (*cur).second->getHatchInverse();
	}
}

void SpaceTimeDrawer::SetTimePosVariable(double * _timePos) {
	timePos = _timePos;
}

bool SpaceTimeDrawer::draw(const DrawLoop drawLoop, const CoordBounds& cbArea) const {
	bool fSpaceTimeDrawer = true;
	bool fFootprintDrawer = true;
	
	if (spaceTimeElementsDrawer) {
		fSpaceTimeDrawer = (*((PathElementProperties*)spaceTimeElementsDrawer->getProperties()))["spacetimepath"].visible;
		fFootprintDrawer = (*((PathElementProperties*)spaceTimeElementsDrawer->getProperties()))["footprint"].visible;
	}
	if (fSpaceTimeDrawer) {
		if ((drawLoop == drl3DOPAQUE && transparency != 1.0) || (drawLoop == drl3DTRANSPARENT && transparency == 1.0))
			fSpaceTimeDrawer = false;
	}

	csDraw->Lock(); // apparently this draw became so "heavy" that we need a lock to prevent the destructor from kicking in while drawing
	if (*fRefreshDisplayList) {
		if (*displayList != 0) {
			glDeleteLists(*displayList, 2);
			*displayList = 0;
		}
		*fRefreshDisplayList = false;
		if (spaceTimeElementsDrawer)
			spaceTimeElementsDrawer->RefreshDisplayList();
	}

	if (*fRefreshTexture) {
		if (texture[0] != 0) {
			glDeleteTextures(2, texture);
			texture[0] = 0;
		}
		*fRefreshTexture = false;
	}

	drawPreDrawers(drawLoop, cbArea);

	// Following 3 lines needed for transparency to work
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glAlphaFunc(GL_GREATER, 0);
	glEnable(GL_ALPHA_TEST);

	glPushMatrix();
	bool is3D = getRootDrawer()->is3D(); 
	//double z0 = getRootDrawer()->getZMaker()->getZ0(is3D);
	//ZValueMaker *zmaker = getZMaker();

	if (is3D) {
		//glTranslated(cube.cMin.x + cube.width() / 2.0, cube.cMin.y + cube.height() / 2.0, cube.cMin.z + cube.altitude() / 2.0);
		//glScaled(cube.width() / 2.0, cube.height() / 2.0, cube.altitude() / 2.0);


		//glTranslated(0, 0, cube.cMin.z);
		//glScaled(1, 1, cube.altitude() / (timeBounds->tMax() - timeBounds->tMin()));
		//glTranslated(0, 0, -timeBounds->tMin());
		//glScaled(1,1,1);
	}
	else
		glScaled(1,1,0);

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	//if (drm == drmSINGLE)
	//	glColor4d(singleColor.redP(), singleColor.greenP(), singleColor.blueP(), transparency);
	glColor4f(1, 1, 1, transparency);
	const int steps = nrSteps;
	const bool fUseLight = is3D && steps > 1;
	if (fUseLight) {
		glEnable(GL_NORMALIZE); // otherwise light intensity changes, gets darker when objects are nearer (zoom-in), which is totally illogical
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glEnable(GL_COLOR_MATERIAL);
		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		GLfloat ambient [4] = {0.1f, 0.1f, 0.1f, 1.0f};
		GLfloat diffuse [4] = {1.0f, 1.0f, 1.0f, 1.0f};
		GLfloat specular [4] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, (GLfloat*)&ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, (GLfloat*)&diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, (GLfloat*)&specular);
		glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 64);
	}

	if (steps == 1)
		glLineWidth(properties->exaggeration);
	
	if (texture[0] == 0) {
		glGenTextures(2, texture);
		glBindTexture(GL_TEXTURE_2D, texture[0]);
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fValueMap ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fValueMap ? GL_LINEAR : GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		GLboolean oldVal;
		glGetBooleanv(GL_MAP_COLOR, &oldVal);
		glPixelTransferf(GL_MAP_COLOR, false);
		const int iTextureSize = PALETTE_TEXTURE_SIZE;
		long texture_data [iTextureSize * 2];
		if (drm == drmSINGLE) {
			for (int i = 0; i < iTextureSize; ++i) {
				texture_data[i] = singleColor;
				texture_data[i] |= (255 << 24);
				Color clr (texture_data[i]);
				clr.SetHSI(clr.hue(), clr.sat(), min(255, (clr.intens() * 2)));
				texture_data[i + iTextureSize] = clr.iVal();
			}
		} else {
			Column attributeColumnColors;
			if (useAttributeColumn())
				attributeColumnColors = getAtttributeColumn();
			RangeReal rrMinMax = getValueRange(attributeColumnColors);
			double width = rrMinMax.rWidth();
			double minMapVal = rrMinMax.rLo();
			const DrawingColor * drawColor = getDrawingColor();

			if (fValueMap) {
				double * buf = new double [iTextureSize];
				for (int i = 0; i < iTextureSize; ++i)
					buf[i] = minMapVal + i * width / iTextureSize;
				drawColor->clrVal(buf, texture_data, iTextureSize);
				for (int i = 0; i < iTextureSize; ++i) {
					texture_data[i] |= (255 << 24);
					Color clr (texture_data[i]);
					clr.SetHSI(clr.hue(), clr.sat(), min(255, (clr.intens() * 2)));
					texture_data[i + iTextureSize] = clr.iVal();
				}
				delete [] buf;
			} else {
				long * buf = new long [iTextureSize];
				for (int i = 0; i < iTextureSize; ++i)
					buf[i] = minMapVal + round(i * width / iTextureSize);
				drawColor->clrRaw(buf, texture_data, iTextureSize, drm);
				if (useAttributeColumn() && disabledRaws.size() > 0) {
					for (int i = 0; i < iTextureSize; ++i) {
						if (find(disabledRaws.begin(), disabledRaws.end(), buf[i]) != disabledRaws.end())
							texture_data[i] &= (0 << 24);
						else
							texture_data[i] |= (255 << 24);
						Color clr (texture_data[i]);
						clr.SetHSI(clr.hue(), clr.sat(), min(255, (clr.intens() * 2)));
						texture_data[i + iTextureSize] = clr.iVal();
					}
				} else {
					for (int i = 0; i < iTextureSize; ++i) {
						texture_data[i] |= (255 << 24);
						Color clr (texture_data[i]);
						clr.SetHSI(clr.hue(), clr.sat(), min(255, (clr.intens() * 2)));
						texture_data[i + iTextureSize] = clr.iVal();
					}
				}
				delete [] buf;
			}
		}
		glTexImage2D( GL_TEXTURE_2D, 0, 4, iTextureSize, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data);
		if (!fValueMap && useAttributeColumn() && getAtttributeColumn()->dm()->rpr()->prc()) { // 2nd texture for class maps, just in case uses hatching with background color
			glBindTexture(GL_TEXTURE_2D, texture[1]);
			glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, fValueMap ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, fValueMap ? GL_LINEAR : GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			Column attributeColumnColors;
			if (useAttributeColumn())
				attributeColumnColors = getAtttributeColumn();
			RangeReal rrMinMax = getValueRange(attributeColumnColors);
			double width = rrMinMax.rWidth();
			double minMapVal = rrMinMax.rLo();
			RepresentationClass * prc = attributeColumnColors->dm()->rpr()->prc();
			const long transparent = Color(-2); // in the old days this was the transparent value
			long * buf = new long [iTextureSize];
			for (int i = 0; i < iTextureSize; ++i)
				buf[i] = minMapVal + round(i * width / iTextureSize);
			for (int i = 0; i < iTextureSize; ++i) {
				long iRaw = buf[i];
				Color backgroundColor = prc->clrSecondRaw(iRaw);
				if (backgroundColor.iVal() == transparent) 
					backgroundColor = colorUNDEF;
				texture_data[i] = backgroundColor;
			}
			if (disabledRaws.size() > 0) {
				for (int i = 0; i < iTextureSize; ++i) {
					if (find(disabledRaws.begin(), disabledRaws.end(), buf[i]) != disabledRaws.end())
						texture_data[i] &= (0 << 24);
					else
						texture_data[i] |= (255 << 24);
					texture_data[i + iTextureSize] = texture_data[i];
				}
			} else {
				for (int i = 0; i < iTextureSize; ++i) {
					texture_data[i] |= (255 << 24);
					texture_data[i + iTextureSize] = texture_data[i];
				}
			}
			delete [] buf;

			glTexImage2D( GL_TEXTURE_2D, 0, 4, iTextureSize, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture_data);
			glBindTexture(GL_TEXTURE_2D, texture[0]);
		}
		glPixelTransferf(GL_MAP_COLOR, oldVal);
	} else
		glBindTexture(GL_TEXTURE_2D, texture[0]);

	if (fSpaceTimeDrawer) {
		if (fClipTPlus) {
			bool is3D = getRootDrawer()->is3D(); 
			double z0 = getRootDrawer()->getZMaker()->getZ0(is3D) + getRootDrawer()->getFakeZ(); // one more step, so that it is visible
			double clip_plane4[]={0.0, 0.0, -1.0, z0 + cube.altitude() * *timePos};
			glClipPlane(GL_CLIP_PLANE4,clip_plane4);
			glEnable(GL_CLIP_PLANE4);
		}
		if (fClipTMinus) {
			double clip_plane5[]={0.0, 0.0, 1.0, -cube.altitude() * *timePos};
			glClipPlane(GL_CLIP_PLANE5,clip_plane5);
			glEnable(GL_CLIP_PLANE5);
		}
		if (*displayList != 0) {
			glCallList(*displayList);
			if (*fHatching) {
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glCallList(*displayList + 1);
				glBindTexture(GL_TEXTURE_2D, texture[0]);
			}
		}
		else
		{
			*displayList = glGenLists(2);
			glNewList(*displayList, GL_COMPILE_AND_EXECUTE);

			CoordBounds cbMap = getRootDrawer()->getMapCoordBounds();
			CoordBounds cbBaseMap = basemap->cb();
			bool fZoomed = cbBaseMap.MinX() < cbMap.MinX() || cbBaseMap.MinY() < cbMap.MinY() || cbBaseMap.MaxX() > cbMap.MaxX() || cbBaseMap.MaxY() > cbMap.MaxY();

			if (fZoomed) {
				double pathScale = properties->exaggeration * cbMap.width() / 50;
				double clip_plane0[]={-1.0, 0.0, 0.0, cbMap.cMax.x + pathScale};
				double clip_plane1[]={1.0, 0.0, 0.0, -cbMap.cMin.x - pathScale};
				double clip_plane2[]={0.0, -1.0, 0.0, cbMap.cMax.y + pathScale};
				double clip_plane3[]={0.0, 1.0, 0.0, -cbMap.cMin.y - pathScale};
				glClipPlane(GL_CLIP_PLANE0,clip_plane0);
				glClipPlane(GL_CLIP_PLANE1,clip_plane1);
				glClipPlane(GL_CLIP_PLANE2,clip_plane2);
				glClipPlane(GL_CLIP_PLANE3,clip_plane3);
				glEnable(GL_CLIP_PLANE0);
				glEnable(GL_CLIP_PLANE1);
				glEnable(GL_CLIP_PLANE2);
				glEnable(GL_CLIP_PLANE3);
			}
			drawObjects(steps, (GetHatchFunc)&SpaceTimeDrawer::getHatch);
			if (fZoomed) {
				glDisable(GL_CLIP_PLANE0);
				glDisable(GL_CLIP_PLANE1);
				glDisable(GL_CLIP_PLANE2);
				glDisable(GL_CLIP_PLANE3);
			}
			glEndList();

			if (*fHatching) {
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				glNewList(*displayList + 1, GL_COMPILE_AND_EXECUTE);
				if (fZoomed) {
					glEnable(GL_CLIP_PLANE0);
					glEnable(GL_CLIP_PLANE1);
					glEnable(GL_CLIP_PLANE2);
					glEnable(GL_CLIP_PLANE3);
				}
				drawObjects(steps, (GetHatchFunc)&SpaceTimeDrawer::getHatchInverse);
				if (fZoomed) {
					glDisable(GL_CLIP_PLANE0);
					glDisable(GL_CLIP_PLANE1);
					glDisable(GL_CLIP_PLANE2);
					glDisable(GL_CLIP_PLANE3);
				}
				glEndList();
				glBindTexture(GL_TEXTURE_2D, texture[0]);
			}
		}
		if (fClipTPlus) {
			glDisable(GL_CLIP_PLANE4);
		}
		if (fClipTMinus) {
			glDisable(GL_CLIP_PLANE5);
		}
	}

	if (steps == 1)
		glLineWidth(1);

	if (fUseLight) {
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHTING);
		glDisable(GL_NORMALIZE);
	}

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

	drawPostDrawers(drawLoop, cbArea);

	if (fFootprintDrawer && spaceTimeElementsDrawer)
		spaceTimeElementsDrawer->draw(drawLoop, cbArea);

	csDraw->Unlock();

	return true;
}

Coord SpaceTimeDrawer::projectOnCircle(Coord AB, double r, double f) const
{
	Coord C;
	if ((AB.x != 0) || (AB.y != 0))
	{
		Coord U = normalize(AB);
		Coord V = normalize(Coord(-U.y, U.x, 0));
		Coord W = normalize(cross(U,V)); // new Vector3d(U.y*V.z-U.z*V.y, U.z*V.x-U.x*V.z, U.x*V.y-U.y*V.x);
		
		C.x = r * (V.x*cos(f) + W.x*sin(f));
		C.y = r * (V.y*cos(f) + W.y*sin(f));
		C.z = r * (V.z*cos(f) + W.z*sin(f));
	}
	else if (AB.z != 0) // circle belongs to cylinder parallel to z-axis
	{
		C.x = r*cos(f);
		C.y = r*sin(f);
		C.z = 0;
	}
	else // circle belongs to cylinder with 0-size, thus unknown direction
	{
		C.x = 0;
		C.y = 0;
		C.z = 0;
	}

	return C;
}

Coord SpaceTimeDrawer::normalize(Coord c) const
{
	double norm = 1.0/sqrt(c.x*c.x + c.y*c.y + c.z*c.z);
	return Coord(c.x * norm, c.y * norm, c.z * norm);
}

Coord SpaceTimeDrawer::cross(Coord c1, Coord c2) const
{ 
    double x = c1.y*c2.z - c1.z*c2.y;
    double y = c2.x*c1.z - c2.z*c1.x;
	return Coord(x, y, c1.x*c2.y - c1.y*c2.x);
}


//-----------------------------------------------------------------





