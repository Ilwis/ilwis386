#include "Headers\toolspch.h"
#include "Engine\Map\Raster\Map.h" // for Map in Texture.h, otherwise Texture.h can't compile (Texture.h can't be used standalone yet)
#include "DEMTriangulator.h"
#include "Texture.h"
#include "Engine\Drawers\ZValueMaker.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace ILWIS;

DEMTriangulator::DEMTriangulator(double* rHeights, int iWidth, int iHeight,
			double rMinX, double rMinY, double rMaxX, double rMaxY, bool fSmooth)
: iMinGlobalRes(12) // min global res., the higher the more vertices
, D2K ((double) iMinGlobalRes / (2.0 * (iMinGlobalRes - 1.0))) // factor for neighboring D2-values, between 0.5 and 1
, iSizeX(iWidth)
, iSizeY(iHeight)
, rHeights(rHeights)
, width(pow(2, max(6, ceil(log((double)iSizeX)/log(2.0)))))
, height(pow(2, max(6, ceil(log((double)iSizeY)/log(2.0)))))
, fSmooth(fSmooth)
, rStepX((double) ((rMaxX - rMinX) / iSizeX))
, rStepY((double) ((rMaxY - rMinY) / iSizeY))
, rMinX(rMinX + (rStepX / 2.0)) // Provided: corner of corners; convert to centre of corners
, rMinY(rMinY + (rStepY / 2.0))
, rDesiredResolution(0.1 / 2500.0) // (22.0) //(0.001) // ((double) iWidth * (double) rStepX * 4) // The desired resolution (calculate based on the DEM properties). Formula may need improvement.
, iNrVertices(0)
, iVertexArrayIncrement(1024 * 1024)
, valid(false)
, fSameCsy(true)
{
	if (iWidth < iHeight)
		iSize2 = (int) pow(2.0, (int) (log((double)iWidth) / log(2.0)));
	else
		iSize2 = (int) pow(2.0, (int) (log((double)iHeight) / log(2.0)));	
	// iSize2 is the largest ^2 that is smaller or equal to both width and height. Any bigger value is a waste. 
	rFactors = (double*)malloc(iSizeX * iSizeY * sizeof(double));
	for (int i = 0; i < iSizeX * iSizeY; ++i)
		if (rHeights[i] == rUNDEF)
			rHeights[i] = 0;
	DoTriangulate();
	valid = true;
}

DEMTriangulator::DEMTriangulator(ZValueMaker * zMaker, BaseMapPtr * drapeMapPtr, CoordSystem & csyDest, bool fSmooth)
: iMinGlobalRes(12) // min global res., the higher the more vertices
, D2K ((double) iMinGlobalRes / (2.0 * (iMinGlobalRes - 1.0))) // factor for neighboring D2-values, between 0.5 and 1
, fSmooth(fSmooth)
, iNrVertices(0)
, zMaker(zMaker)
, csyDest(csyDest)
, iVertexArrayIncrement(1024 * 1024)
, valid(false)
, fSelfDrape(true)
{
	if (zMaker != 0 && zMaker->getSourceRasterMap() != 0)
	{
		mp.SetPointer(zMaker->getSourceRasterMap());
		if (drapeMapPtr != 0) {
			drapemp.SetPointer(drapeMapPtr);
			if (drapemp->fnObj != mp->fnObj) {
				fSelfDrape = false;
				csyDrape = drapemp->cs();
			}
		}
		csyMap = mp->cs();
		fSameCsy = csyDest->fnObj == csyMap->fnObj;
		iSizeX = mp->rcSize().Col;
		iSizeY = mp->rcSize().Row;

		rHeights = (double*)malloc(iSizeX * iSizeY * sizeof(double));
		ReadMap();
		double minX = mp->cb().MinX();
		double maxX = mp->cb().MaxX();
		double minY = mp->cb().MinY();
		double maxY = mp->cb().MaxY();

		if (fSelfDrape) {
			width = pow(2, max(6, ceil(log((double)iSizeX)/log(2.0))));
			height = pow(2, max(6, ceil(log((double)iSizeY)/log(2.0))));
		} else {
			RowCol rc = drapemp->rcSize();
			width = (pow(2, max(6, ceil(log((double)rc.Col)/log(2.0)))));
			height = (pow(2, max(6, ceil(log((double)rc.Row)/log(2.0)))));
		}
		rStepX = (double) ((maxX - minX) / iSizeX);
		rStepY = (double) ((maxY - minY) / iSizeY);
		rMinX = minX + (rStepX / 2.0); // Provided: corner of corners; convert to centre of corners
		rMinY = minY + (rStepY / 2.0);
		rDesiredResolution = 0.1 / 2500.0; // (22.0) //(0.001) // ((double) iWidth * (double) rStepX * 4) // The desired resolution (calculate based on the DEM properties). Formula may need improvement.

		if (iSizeX < iSizeY)
			iSize2 = (int) pow(2.0, (int) (log((double)iSizeX) / log(2.0)));
		else
			iSize2 = (int) pow(2.0, (int) (log((double)iSizeY) / log(2.0)));	
		// iSize2 is the largest ^2 that is smaller or equal to both width and height. Any bigger value is a waste. 
		rFactors = (double*)malloc(iSizeX * iSizeY * sizeof(double));
		double rMin = mp->rrMinMax().rLo();
		for (int i = 0; i < iSizeX * iSizeY; ++i)
			if (rHeights[i] == rUNDEF)
				rHeights[i] = rMin;
		DoTriangulate();

		if (rHeights)
			free(rHeights);
		valid = true;
	}
}

DEMTriangulator::~DEMTriangulator(void)
{
	if (rFactors)
		free(rFactors);
	free(vertices);
}

void DEMTriangulator::ReadMap()
{
	int iWidth = mp->rcSize().Col;
	int iHeight = mp->rcSize().Row;
	bool fValue = 0 != mp->dm()->pdvi() || 0 != mp->dm()->pdvr();
	bool fAttTable = false;
	ValueRange vr = mp->vr();
	if (mp->dm()->pdbool())
		vr = ValueRange();
	bool fRealMap;
	if (vr.fValid()) // when integers are not good enough to represent the map treat it as a real map
		fRealMap = (vr->rStep() < 1) || (vr->stUsed() == stREAL);
	else
		fRealMap = false;
//		if (*fDrawStop)
//			return false;
	mp->KeepOpen(true);
	if (fRealMap) 
	{
		RealBuf bufIn(iWidth);
		double * ptrBufIn = bufIn.buf();
		memset(ptrBufIn, 0, iWidth * sizeof(double)); // to prevent NAN values in bufIn.
		for (long iYPos = 0; iYPos < iHeight; ++iYPos)
		{
//				if (*fDrawStop) {
//					mp->KeepOpen(false);
//					return false;
//				}
			mp->GetLineVal(iYPos, bufIn, 0, iWidth);
			memcpy(&rHeights[(iHeight - iYPos - 1) * iWidth], ptrBufIn, iWidth * sizeof(double));
		}
	}
	else 
	{ // !fRealMap
		LongBuf bufIn(iWidth);
		long * ptrBufIn = bufIn.buf();
		for (long iYPos = 0; iYPos < iHeight; ++iYPos) 
		{
//				if (*fDrawStop) {
//					mp->KeepOpen(false);
//					return false;
//				}
			if (fValue && !fAttTable)
				mp->GetLineVal(iYPos, bufIn, 0, iWidth);
			else
				mp->GetLineRaw(iYPos, bufIn, 0, iWidth);
			for (long iXPos = 0; iXPos < iWidth; ++iXPos)
				rHeights[(iHeight - iYPos - 1) * iWidth + iXPos] = doubleConv(ptrBufIn[iXPos]);
		}
	}                                                                         
	mp->KeepOpen(false);
}

void DEMTriangulator::DoTriangulate()
{
	bool success = false;
	while (!success) {
		TRACE("Trying with rDesiredResolution = %f\n", rDesiredResolution);
		iNrVertices = 0;
		iVerticesArraySize = iVertexArrayIncrement;
		vertices = (Vertex*)malloc(iVerticesArraySize * sizeof(Vertex));
		memset(rFactors, 0, iSizeX * iSizeY * sizeof(double));
		// --- set initial D2-Values -----------
		CalcD2ErrorMatrix();
		// --- ensure max level difference of 1 (restricted quadtree property) -
		PropagateD2Errors();
		TriangulateMesh();
		// --- create Terrain Mesh ---------------------------------------------
		success = fRenderMesh();
		if (!success) {
			free(vertices);
			rDesiredResolution /= 10.0;
			TRACE("realloc failed, requested %d megabytes of memory ... retrying\n", iVerticesArraySize * sizeof(Vertex) / (1024 * 1024));
			// if (smooth) then re-read heights
			if (fSmooth)
				ReadMap();
		}
	}
	vertices = (Vertex*)realloc(vertices, iNrVertices * sizeof(Vertex));
	TRACE("Success!! memory = %d megabytes, NrVertices = %d\n", iNrVertices * sizeof(Vertex) / (1024 * 1024), iNrVertices);
}

// === ErrorMatrix calculations ============================================

void DEMTriangulator::CalcD2ErrorMatrix()
{
	for (int iSize = 2; iSize <= iSize2; iSize *= 2) {
		int s2 = iSize / 2;
		for (int centerX = s2; centerX < iSizeX - s2; centerX += iSize) { // start left (west), and move right (east)
			for (int centerY = s2; centerY < iSizeY - s2; centerY += iSize) { // start up (north) and move down (south)
				// --- north, east, south, west height errors --------------------------
				double nhErr = abs(rHeights[centerX + iSizeX * (centerY - s2)] - (rHeights[centerX - s2 + iSizeX * (centerY - s2)] + rHeights[centerX + s2 + iSizeX * (centerY - s2)]) / 2.0);
				double ehErr = abs(rHeights[centerX + s2 + iSizeX * (centerY)] - (rHeights[centerX + s2 + iSizeX * (centerY - s2)] + rHeights[centerX + s2 + iSizeX * (centerY + s2)]) / 2.0);
				double shErr = abs(rHeights[centerX + iSizeX * (centerY + s2)] - (rHeights[centerX + s2 + iSizeX * (centerY + s2)] + rHeights[centerX - s2 + iSizeX * (centerY + s2)]) / 2.0);
				double whErr = abs(rHeights[centerX - s2 + iSizeX * (centerY)] - (rHeights[centerX - s2 + iSizeX * (centerY + s2)] + rHeights[centerX - s2 + iSizeX * (centerY - s2)]) / 2.0);

				// --- 1. and 2. diagonal height error ---------------------------------
				double d1hErr = abs(rHeights[centerX + iSizeX * (centerY)] - (rHeights[centerX - s2 + iSizeX * (centerY - s2)] + rHeights[centerX + s2 + iSizeX * (centerY + s2)]) / 2.0);
				double d2hErr = abs(rHeights[centerX + iSizeX * (centerY)] - (rHeights[centerX + s2 + iSizeX * (centerY - s2)] + rHeights[centerX - s2 + iSizeX * (centerY + s2)]) / 2.0);

				// --- determine max of the 6 errors -----------------------------------
				double rMaxhErr = max(max(nhErr, ehErr), max(max(shErr, whErr), max(d1hErr, d2hErr)));

				rFactors[centerX + iSizeX * (centerY)] = rMaxhErr / ((double) iSize);
			}
		}
	}
}

void DEMTriangulator::PropagateD2Errors()
{
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
	// --- iterate through all levels --------------------------------------
	for (int iSize = 2; iSize <= iSize2 / 2; iSize *= 2) { // start with 2x2 and grow
		int s2 = iSize / 2;
		for (int centerX = s2; centerX < iSizeX - s2; centerX += iSize) { // start left (west), and move right (east)
			for (int centerY = s2; centerY < iSizeY - s2; centerY += iSize) { // start up (north) and move down (south)
				// --- determine parents' indices --------------------------
				switch ((centerX / iSize) % 2 + 2 * ((centerY / iSize) % 2)) {
				case 0: // left, top of a quad
					x1 = centerX + s2;
					y1 = centerY + s2;
					x2 = centerX - iSize - s2;
					y2 = centerY + s2;
					x3 = centerX + s2;
					y3 = centerY - iSize - s2;
					break;
				case 1: // right, top of a quad
					x1 = centerX - s2;
					y1 = centerY + s2;
					x2 = centerX - s2;
					y2 = centerY - iSize - s2;
					x3 = centerX + iSize + s2;
					y3 = centerY + s2;
					break;
				case 2: // left, bottom of a quad
					x1 = centerX + s2;
					y1 = centerY - s2;
					x2 = centerX + s2;
					y2 = centerY + iSize + s2;
					x3 = centerX - iSize - s2;
					y3 = centerY - s2;
					break;
				case 3: // right, bottom of a quad
					x1 = centerX - s2;
					y1 = centerY - s2;
					x2 = centerX + iSize + s2;
					y2 = centerY - s2;
					x3 = centerX - s2;
					y3 = centerY + iSize + s2;
					break;
				}
				double d2K_error = D2K * rFactors[centerX + iSizeX * (centerY)];

				// --- propagate to 3 parents ------------------------------
				// --- to real father --------------------------------------
				rFactors[x1 + iSizeX * (y1)] = max(rFactors[x1 + iSizeX * (y1)], d2K_error);
				// --- other 2 "parents" -----------------------------------
				if (x2 >= 0 && x2 < iSizeX && y2 >= 0 && y2 < iSizeY)
					rFactors[x2 + iSizeX * (y2)] = max(rFactors[x2 + iSizeX * (y2)], d2K_error);
				if (x3 >= 0 && x3 < iSizeX && y3 >= 0 && y3 < iSizeY)
					rFactors[x3 + iSizeX * (y3)] = max(rFactors[x3 + iSizeX * (y3)], d2K_error);
			}
		}
	}
}

// === Triangulation =======================================================

void DEMTriangulator::TriangulateMesh()
{
	for (int iSize = 2; iSize <= iSize2; iSize *= 2) {
		int s2 = iSize / 2;
		for (int x = s2; x < iSizeX - s2; x += iSize) { // start left (west), and move right (east)
			for (int y = s2; y < iSizeY - s2; y += iSize) { // start up (north) and move down (south)
				//double subDiv = 16.0 / ((iSize * rStepX) * iMinGlobalRes * max(rDesiredResolution * rFactors[x + iSizeX * (y)] / rStepX, 1.0));
				//double subDiv = 16.0 / (max(iSize * iMinGlobalRes * rDesiredResolution * rFactors[x + iSizeX * (y)], iSize * iMinGlobalRes * rStepX));
				double subDiv = 1.0 / (iSize * iMinGlobalRes * rDesiredResolution * rFactors[x + iSizeX * y]);
				//double subDiv = eyeDist / ((width * VertexSpacing) * MinGlobalRes * Math.max(DesiredRes * ErrorMatrix[nodeIndex] / VertexSpacing, 1.0));

				if ((subDiv >= 1.0) && !((x + iSize + iSize >= iSizeX)||(y + iSize + iSize >= iSizeY))) {
					// the error of replacing this quad by a higher level quad is small
					// however, don't skip this quad if the higher level quad is at the edge of the image and would not fit to be displayed in its entirety.
//						deleteNode(x, y, iSize);
					rFactors[x + iSizeX * (y)] = rUNDEF;
				} else {			
					// --- leaf or parent? ---------------------------------------------
					// Calculate the blend value, that determines the contribution of the height at the current location when blending with neighbours			
					double blend = min(2 * (1.0 - subDiv), 1.0);
					rFactors[x + iSizeX * (y)] = blend;
				}
			}
		}
	}
}

//	private void deleteNode(int x, int y, int iSize) {
//		rFactors[x + iSizeX * (y)] = rUNDEF;
//		for (int iSize1 = iSize; iSize1 > 4; iSize1 /= 2) {
//			int s2 = iSize1 / 2;
//			int s4 = iSize1 / 4;
//			for (int x1 = x - s4; x1 < x + s4; x1 += s2) {
//				for (int y1 = y - s4; y1 < y + s4; y1 += s2) {
//					rFactors[x1 + iSizeX * (y1)] = rUNDEF;				
//				}			
//			}
//		}
//	}

// === Render Terrain-Mesh =================================================

bool DEMTriangulator::fRenderMesh()
{		
	for (int iSize = 2 * (int)(iSize2 / 2); iSize > 1; iSize /= 2) {
		int s2 = iSize / 2;
		for (int x = s2; x < iSizeX - s2; x += iSize) { // start left (west), and move right (east)
			for (int y = s2; y < iSizeY - s2; y += iSize) { // start up (north) and move down (south)
				if (rFactors[x + iSizeX * (y)] != rUNDEF) {
					// --- get all 9 heights -----------------------------------------------

					double rhNW = rHeights[x - s2 + iSizeX * (y - s2)];
					double rhNE = rHeights[x + s2 + iSizeX * (y - s2)];
					double rhSW = rHeights[x - s2 + iSizeX * (y + s2)];
					double rhSE = rHeights[x + s2 + iSizeX * (y + s2)];

					int dirToFather = 0;
					
					switch ((x / iSize) % 2 + 2 * ((y / iSize) % 2)) {
					case 0: // left, top
						dirToFather = NW;
						break;
					case 1: // right, top
						dirToFather = NE;
						break;
					case 2: // left, bottom
						dirToFather = SW;
						break;
					case 3: // right, bottom
						dirToFather = SE;
						break;
					}
					
					double hC;
					double hN;
					double hS;
					double hW;
					double hE;
					if (fSmooth) {
						hC = rGetHeight(x, y, iSize, dirToFather, C, rhNW, rhNE, rhSW, rhSE);
						hN = (y > s2) ? rHeights[x + iSizeX * (y - s2)] : rGetHeight(x, y, iSize, dirToFather, N, rhNW, rhNE, rhSW, rhSE);
						hS = rGetHeight(x, y, iSize, dirToFather, S, rhNW, rhNE, rhSW, rhSE);
						hW = (x > s2) ? rHeights[x - s2 + iSizeX * (y)] : rGetHeight(x, y, iSize, dirToFather, W, rhNW, rhNE, rhSW, rhSE);
						hE = rGetHeight(x, y, iSize, dirToFather, E, rhNW, rhNE, rhSW, rhSE);
						rHeights[x + iSizeX * (y)] = hC;
						if (y == s2)
							rHeights[x + iSizeX * (y - s2)] = hN;
						rHeights[x + iSizeX * (y + s2)] = hS;
						if (x == s2)
							rHeights[x - s2 + iSizeX * (y)] = hW;
						rHeights[x + s2 + iSizeX * (y)] = hE;
					} else {
						hC = rHeights[x + iSizeX * (y)];
						hN = rHeights[x + iSizeX * (y - s2)];
						hS = rHeights[x + iSizeX * (y + s2)];
						hW = rHeights[x - s2 + iSizeX * (y)];
						hE = rHeights[x + s2 + iSizeX * (y)];
					}
							
					if (!fCreateFanAround1(x, y, iSize, hC, hN, hS, hW, hE, rhNW, rhNE, rhSW, rhSE))
						return false;
				}
			}
		}
	}
	return true;
}

double DEMTriangulator::rGetHeight(int x, int y, int iSize, int dirToFather, int neswc, double rhNW, double rhNE, double rhSW, double rhSE)
{
	double heightValue;
	switch(neswc) {
	case C:
		heightValue = rHeights[x + iSizeX * (y)];
		break;
	case N:
		heightValue = rHeights[x + iSizeX * (y - iSize / 2)];
		break;
	case S:
		heightValue = rHeights[x + iSizeX * (y + iSize / 2)];
		break;
	case W:
		heightValue = rHeights[x - iSize / 2 + iSizeX * (y)];
		break;
	case E:
		heightValue = rHeights[x + iSize / 2 + iSizeX * (y)];
		break;
	default:
		heightValue = 0;				
	}

	// --- determine blend values if this is a leaf ------------------------
	
	if (fSmooth) {
		// blend the height with the height of two neighbours
		
		double blend = rFactors[x + iSizeX * (y)];
		
		switch (neswc) {

		case C:
			switch (dirToFather) {

			case SE:
			case NW:
				heightValue = (1.0 - blend) * (rhNW + rhSE) / 2.0 + blend * heightValue;
				break;
			case NE:
			case SW:
				heightValue = (1.0 - blend) * (rhNE + rhSW) / 2.0 + blend * heightValue;
				break;
			}

			break;

		case N:
			if (y - iSize >= 0)
				blend = min(blend, rFactors[x + iSizeX * (y - iSize)]);
			heightValue = (1.0 - blend) * (rhNW + rhNE) / 2.0 + blend * heightValue;
			break;

		case S:
			if (y + iSize < iSizeY)
				blend = min(blend, rFactors[x + iSizeX * (y + iSize)]);
			heightValue = (1.0 - blend) * (rhSW + rhSE) / 2.0 + blend * heightValue;
			break;

		case W:
			if (x - iSize >= 0)
				blend = min(blend, rFactors[x - iSize + iSizeX * (y)]);
			heightValue = (1.0 - blend) * (rhNW + rhSW) / 2.0 + blend * heightValue;
			break;

		case E:
			if (x + iSize < iSizeX)
				blend = min(blend, rFactors[x + iSize + iSizeX * (y)]);
			heightValue = (1.0 - blend) * (rhNE + rhSE) / 2.0 + blend * heightValue;
			break;
		}
	}

	return heightValue;
}

bool DEMTriangulator::fCreateFanAround1(int x, int y, int iSize,
		double hC, double hN, double hS, double hW, double hE,
		double hNW, double hNE, double hSW, double hSE)
{
	int s2 = iSize / 2;
	int s4 = iSize / 4;

	bool corners [8] = { true, true, true, true, true, true, true, true };		
	
	if (s2 > 1) {
		corners[NW] = (rFactors[x - s4 + iSizeX * (y - s4)] == rUNDEF);
		corners[NE] = (rFactors[x + s4 + iSizeX * (y - s4)] == rUNDEF);
		corners[SW] = (rFactors[x - s4 + iSizeX * (y + s4)] == rUNDEF);
		corners[SE] = (rFactors[x + s4 + iSizeX * (y + s4)] == rUNDEF);
	}
	
	if (!(corners[NW] || corners[SW] || corners[SE] || corners[NE]))
		return true;
	
	if (s2 > 1) {
		corners[W] = (corners[NW] || corners[SW]);
		corners[S] = (corners[SW] || corners[SE]);
		corners[E] = (corners[SE] || corners[NE]);
		corners[N] = (corners[NE] || corners[NW]);
	}

	// check array bounds and whether the neighbour exists

	if ((y - iSize >= 0) && (rFactors[x + iSizeX * (y - iSize)] == rUNDEF))
		corners[N] = false;
	if ((x + iSize < iSizeX) && (rFactors[x + iSize + iSizeX * (y)] == rUNDEF))
		corners[E] = false;
	if ((y + iSize < iSizeY) && (rFactors[x + iSizeX * (y + iSize)] == rUNDEF))
		corners[S] = false;
	if ((x - iSize >= 0) && (rFactors[x - iSize + iSizeX * (y)] == rUNDEF))
		corners[W] = false;

	if (iNrVertices + 24 >= iVerticesArraySize) {
		iVerticesArraySize += iVertexArrayIncrement;
		Vertex* newVertices = (Vertex*)realloc(vertices, iVerticesArraySize * sizeof(Vertex));
		if (newVertices == 0)
			return false;
		else
			vertices = newVertices;
	}

	// TRIANGLE_ARRAY, thus every 3 vertices form 1 triangle

	// --- check western quarter -------------------------------------------

	if (corners[NW]) {
		AddVertex(x - s2, y - s2, hNW);
		if (corners[W]) {
			AddVertex(x - s2, y, hW);
			if (corners[SW]) {
				AddVertex(x, y, hC);
				AddVertex(x - s2, y, hW);
				AddVertex(x - s2, y + s2, hSW);
			}
		} else {
			if (corners[SW]) {
				AddVertex(x - s2, y + s2, hSW);
			} else {
				AddVertex(x - s2, y, hW);
			}
		}
		AddVertex(x, y, hC);
	} else if (corners[W] || corners[SW]) {
		AddVertex(x - s2, y, hW);
		AddVertex(x - s2, y + s2, hSW);
		AddVertex(x, y, hC);
	}

	// --- check southern quarter ------------------------------------------

	if (corners[SW]) {
		AddVertex(x - s2, y + s2, hSW);
		if (corners[S]) {
			AddVertex(x, y + s2, hS);
			if (corners[SE]) {
				AddVertex(x, y, hC);
				AddVertex(x, y + s2, hS);
				AddVertex(x + s2, y + s2, hSE);
			}
		} else {
			if (corners[SE]) {
				AddVertex(x + s2, y + s2, hSE);
			} else {
				AddVertex(x, y + s2, hS);
			}
		}
		AddVertex(x, y, hC);
	} else if (corners[S] || corners[SE]) {
		AddVertex(x, y + s2, hS);
		AddVertex(x + s2, y + s2, hSE);
		AddVertex(x, y, hC);
	}

	// --- check eastern quarter -------------------------------------------

	if (corners[SE]) {
		AddVertex(x + s2, y + s2, hSE);
		if (corners[E]) {
			AddVertex(x + s2, y, hE);
			if (corners[NE]) {
				AddVertex(x, y, hC);
				AddVertex(x + s2, y, hE);
				AddVertex(x + s2, y - s2, hNE);
			}
		} else {
			if (corners[NE]) {
				AddVertex(x + s2, y - s2, hNE);
			} else {
				AddVertex(x + s2, y, hE);
			}
		}

		AddVertex(x, y, hC);
	} else if (corners[E] || corners[NE]) {
		AddVertex(x + s2, y, hE);
		AddVertex(x + s2, y - s2, hNE);
		AddVertex(x, y, hC);
	}

	// --- check northern quarter ------------------------------------------

	if (corners[NE]) {
		AddVertex(x + s2, y - s2, hNE);
		if (corners[N]) {
			AddVertex(x, y - s2, hN);
			if (corners[NW]) {
				AddVertex(x, y, hC);
				AddVertex(x, y - s2, hN);
				AddVertex(x - s2, y - s2, hNW);
			}
		} else {
			if (corners[NW]) {
				AddVertex(x - s2, y - s2, hNW);
			} else {
				AddVertex(x, y - s2, hN);
			}
		}
		AddVertex(x, y, hC);
	} else if (corners[N] || corners[NW]) {
		AddVertex(x, y - s2, hN);
		AddVertex(x - s2, y - s2, hNW);
		AddVertex(x, y, hC);
	}

	return true;
}

void DEMTriangulator::CreateFanAround2(int x, int y, int iSize,
		double hC, double hN, double hS, double hW, double hE,
		double hNW, double hNE, double hSW, double hSE)
{
	int s2 = iSize / 2;
	int s4 = iSize / 4;

	bool corners [8] = { true, true, true, true, true, true, true, true };		
	
	if (s2 > 1) {
		corners[NW] = (rFactors[x - s4 + iSizeX * (y - s4)] == rUNDEF);
		corners[NE] = (rFactors[x + s4 + iSizeX * (y - s4)] == rUNDEF);
		corners[SW] = (rFactors[x - s4 + iSizeX * (y + s4)] == rUNDEF);
		corners[SE] = (rFactors[x + s4 + iSizeX * (y + s4)] == rUNDEF);
	}
	
	if (!(corners[NW] || corners[SW] || corners[SE] || corners[NE]))
		return;
	
	if (s2 > 1) {
		corners[W] = (corners[NW] || corners[SW]);
		corners[S] = (corners[SW] || corners[SE]);
		corners[E] = (corners[SE] || corners[NE]);
		corners[N] = (corners[NE] || corners[NW]);
	}

	// check array bounds and whether the neighbour exists

	if ((y - iSize >= 0) && (rFactors[x + iSizeX * (y - iSize)] == rUNDEF))
		corners[N] = false;
	if ((x + iSize < iSizeX) && (rFactors[x + iSize + iSizeX * (y)] == rUNDEF))
		corners[E] = false;
	if ((y + iSize < iSizeY) && (rFactors[x + iSizeX * (y + iSize)] == rUNDEF))
		corners[S] = false;
	if ((x - iSize >= 0) && (rFactors[x - iSize + iSizeX * (y)] == rUNDEF))
		corners[W] = false;

	// TRIANGLE_FAN_ARRAY
	int vertexLength = 0;
	
	AddVertex(x, y, hC);
	++vertexLength;
	int last = C;
	int first = C;
	
	if (corners[NW]) {
		AddVertex(x - s2, y - s2, hNW);
		++vertexLength;
		last = NW;
		if (first == C)
			first = NW;
	}
	if (corners[W]) {
		AddVertex(x - s2, y, hW);
		++vertexLength;
		last = W;
		if (first == C)
			first = W;
	}

	if (corners[SW]) {
		AddVertex(x - s2, y + s2, hSW);
		++vertexLength;
		last = SW;
		if (first == C)
			first = SW;
	}
	if (corners[S]) {
		if (last == W) {
			AddVertex(x, y, hC);
			++vertexLength;
		}
		AddVertex(x, y + s2, hS);
		++vertexLength;
		last = S;
		if (first == C)
			first = S;
	}
	if (corners[SE]) {
		AddVertex(x + s2, y + s2, hSE);
		++vertexLength;
		last = SE;
		if (first == C)
			first = SE;
	}
	if (corners[E]) {
		if (last == S || last == W) {
			AddVertex(x, y, hC);
			++vertexLength;
		}
		AddVertex(x + s2, y, hE);
		++vertexLength;
		last = E;
		if (first == C)
			first = E;
	}
	if (corners[NE]) {
		AddVertex(x + s2, y - s2, hNE);
		++vertexLength;
		last = NE;
		if (first == C)
			first = NE;
	}
	if (corners[N]) {
		if (last == E || last == S || last == W) {
			AddVertex(x, y, hC);
			++vertexLength;
		}
		AddVertex(x, y - s2, hN);
		++vertexLength;
		last = N;
		if (first == C)
			first = N;
	}
	// if first and last point are not midpoints(N,S,E,W), then close triangle-fan
	if (!(first % 2 == 1 && last % 2 == 1)) { // first and last point are N,S,E,W
		AddVertex(x, y, hC);
		++vertexLength;
	}

	// triangle fan with length vertexLength
}

void DEMTriangulator::AddVertex(int x, int y, double rHeight)
{	
	// TRIANGLE_ARRAY, thus every 3 vertices form 1 triangle

	// Cases:
	// 1: mp to be draped over mp values, in projection mp->csy
	// 2: mp to be draped over mp values, in projection root->csy
	// 3: drapemp to be draped over mp values, in projection mp->csy (== drapemp->csy)
	// 4: drapemp to be draped over mp values, in projection drapemp->csy (!= drapemp->csy)
	// 5: drapemp to be draped over mp values, in projection root->csy (!= drapemp->csy)
	// 6: drapemp to be draped over mp values, in projection root->csy (!= drapemp->csy)
	Coord c (rMinX + x * rStepX, rMinY + y * rStepY, 0.0);
	double s, t;
	if (fSelfDrape) {
		s = x / (double)width;
		t = (iSizeY - y - 1) / (double)height;
	} else {
		Coord c1 = csyDrape->cConv(csyMap, c);
		double row;
		double col;
		drapemp->gr()->Coord2RowCol(c1, row, col);
		s = col / (double)width;
		t = row / (double)height;
	}
	if (!fSameCsy)
		c = csyDest->cConv(csyMap, c);
	vertices[iNrVertices++] = Vertex(s, t, c.x, c.y, zMaker->scaleValue(rHeight));
}

void DEMTriangulator::PlotTriangles(double xMin, double yMin, double xMax, double yMax, Texture * tex)
{
	/*
	int index = 0;
	bool fDraw = false;
	int iNr = vertices.size();
	int iCount = 0;
	for (vector<Vertex*>::iterator vertex = vertices.begin(); vertex != vertices.end(); ++vertex)
	{
		if (index % 3 == 0)
			fDraw = ((*vertex)->x >= xMin && (*vertex)->x <= xMax && (*vertex)->y >= yMin && (*vertex)->y <= yMax);
		if (fDraw)
		{
			glTexCoord2d((*vertex)->s, (*vertex)->t);
			glVertex3f((*vertex)->x, (*vertex)->y, (*vertex)->z);
			++iCount;
		}
		++index;
	}
	*/
	

	/*
	float * vert = (float*)malloc(vertices.size() * 5 * sizeof(float));
	int index = 0;
	for (vector<Vertex*>::iterator vertex = vertices.begin(); vertex != vertices.end(); ++vertex)
	{
		vert[index++] = tex->getTexCoord2dX((*vertex)->x);
		vert[index++] = tex->getTexCoord2dY((*vertex)->y);
		vert[index++] = (*vertex)->x;
		vert[index++] = (*vertex)->y;
		vert[index++] = (*vertex)->z;
	}
	glInterleavedArrays(GL_T2F_V3F, 0, vert);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	free(vert);
	*/

//	TRACE("Count = %d\n", iCount);
}

void DEMTriangulator::PlotTriangles()
{
	glInterleavedArrays(GL_T2F_V3F, sizeof(Vertex), vertices);
	glDrawArrays(GL_TRIANGLES, 0, iNrVertices);
}

bool DEMTriangulator::fValid()
{
	return valid;
}