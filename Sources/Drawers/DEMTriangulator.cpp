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

DEMTriangulator::DEMTriangulator(ZValueMaker * zMaker, BaseMapPtr * drapeMapPtr, CoordSystem & csyDest, bool fSmooth)
: iNrVerticalSteps(10) // the higher the more vertices
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

		if (fSelfDrape) {
			width = pow(2, max(6, ceil(log((double)iSizeX)/log(2.0))));
			height = pow(2, max(6, ceil(log((double)iSizeY)/log(2.0))));
		} else {
			RowCol rc = drapemp->rcSize();
			width = (pow(2, max(6, ceil(log((double)rc.Col)/log(2.0)))));
			height = (pow(2, max(6, ceil(log((double)rc.Row)/log(2.0)))));
		}

		if (iSizeX < iSizeY) {
			iSize2l = (int) pow(2.0, (int) (log((double)iSizeX) / log(2.0)));
			iSize2u = (int) pow(2.0, 1 + (int) (log((double)iSizeY) / log(2.0)));
		}
		else {
			iSize2l = (int) pow(2.0, (int) (log((double)iSizeY) / log(2.0)));	
			iSize2u = (int) pow(2.0, 1 + (int) (log((double)iSizeX) / log(2.0)));
		}
		// iSize2l is the largest ^2 that is smaller or equal to both width and height. Any bigger value is a waste.
		// iSize2h is the smallest ^2 that is greater or equal to both width and height.

		iTrqMax = iSizeY + 4 * iSizeX; // ReadMap + 4 triangulation loops
		rHeights = new double [iSizeX * iSizeY];
		if (rHeights) {
			rFactors = new double [iSize2u * iSize2u];
			if (rFactors) {
				valid = fDoTriangulate();
				delete [] rFactors;
			}
			delete [] rHeights;
		}
	}
}

DEMTriangulator::~DEMTriangulator(void)
{
	if (vertices)
		free(vertices);
}

bool DEMTriangulator::fDoTriangulate()
{
	// Non-recursive Quad-Tree triangulation
	bool fSuccess = false;
	Tranquilizer trq(TR("Triangulating DEM"));
	trq.fUpdate(0, iTrqMax);
	iTrqVal = 0;
	//clock_t start = clock();
	if (!fReadMap(trq))
		return false;
	//clock_t end = clock();
	//double duration = 1000.0 * (double)(end - start) / CLOCKS_PER_SEC;
	//TRACE("Map read in %2.2f milliseconds;\n", duration);
	rHeightAccuracy = rrMinMax.rWidth() / (double)iNrVerticalSteps;
	iTrqVal = iSizeY;
	while (!fSuccess) {
		TRACE("Trying with rHeightAccuracy = %f\n", rHeightAccuracy);
		iNrVertices = 0;
		iVerticesArraySize = iVertexArrayIncrement;
		vertices = (Vertex*)malloc(iVerticesArraySize * sizeof(Vertex));
		if (!vertices) // out of memory
			break;
		// --- set initial D2-Values -----------
		if (!fCalcD2ErrorMatrix(trq))
			break;
		// --- ensure max level difference of 1 (restricted quadtree property) -
		if (!fPropagateD2Errors(trq))
			break;
		if (!fTriangulateMesh(trq))
			break;
		// --- create Terrain Mesh ---------------------------------------------
		fSuccess = fRenderMesh(trq);
		if (trq.fAborted()) // fSuccess will be false; fRenderMesh returns false for two reasons (out of memory, or user stopped)
			break;
		if (!fSuccess) {
			free(vertices);
			rHeightAccuracy *= 10.0;
			TRACE("realloc failed, requested %d megabytes of memory ... retrying\n", iVerticesArraySize * sizeof(Vertex) / (1024 * 1024));
			// if (smooth) then re-read heights
			if (fSmooth) {
				trq.fUpdate(0, iTrqMax);
				iTrqVal = 0;
				if (!fReadMap(trq))
					break;
			}
			iTrqVal = iSizeY;
		}
	}
	trq.fUpdate(iTrqMax, iTrqMax);
	fSuccess = fSuccess && (iNrVertices > 0);
	if (fSuccess) {
		Vertex* newVertices = (Vertex*)realloc(vertices, iNrVertices * sizeof(Vertex));
		if (newVertices) {
			vertices = newVertices;
			TRACE("Success!! memory = %d megabytes, NrVertices = %d\n", iNrVertices * sizeof(Vertex) / (1024 * 1024), iNrVertices);
		}
		else {
			fSuccess = false; // weird out of memory case, since in realloc we request reallocation to less memory
			TRACE("Realloc failed, could not downsize array from %d to %d megabytes, NrVertices = %d\n", iVerticesArraySize * sizeof(Vertex) / (1024 * 1024), iNrVertices * sizeof(Vertex) / (1024 * 1024), iNrVertices);
		}
	}
	return fSuccess;
}

bool DEMTriangulator::fReadMap(Tranquilizer & trq)
{
	double rMin = DBL_MAX;
	double rMax = -DBL_MAX;
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
	mp->KeepOpen(true);
	if (fRealMap) 
	{
		RealBuf bufIn(iSizeX);
		double * ptrBufIn = bufIn.buf();
		memset(ptrBufIn, 0, iSizeX * sizeof(double)); // to prevent NAN values in bufIn.
		for (long iYPos = 0; iYPos < iSizeY; ++iYPos)
		{
			if (trq.fUpdate(iYPos, iTrqMax)) {
				mp->KeepOpen(false);
				return false;
			}
			mp->GetLineVal(iYPos, bufIn, 0, iSizeX);
			memcpy(&rHeights[iYPos * iSizeX], ptrBufIn, iSizeX * sizeof(double));
			long iSeek = iYPos * iSizeX;
			for (long iXPos = 0; iXPos < iSizeX; ++iXPos) {
				if (rHeights[iSeek] != rUNDEF) {
					if (rHeights[iSeek] < rMin)
						rMin = rHeights[iSeek];
					else if (rHeights[iSeek] > rMax)
						rMax = rHeights[iSeek];
				}
				++iSeek;
			}
		}
	}
	else 
	{ // !fRealMap
		LongBuf bufIn(iSizeX);
		long * ptrBufIn = bufIn.buf();
		for (long iYPos = 0; iYPos < iSizeY; ++iYPos) 
		{
			if (trq.fUpdate(iYPos, iTrqMax)) {
				mp->KeepOpen(false);
				return false;
			}
			if (fValue && !fAttTable)
				mp->GetLineVal(iYPos, bufIn, 0, iSizeX);
			else
				mp->GetLineRaw(iYPos, bufIn, 0, iSizeX);
			long iSeek = iYPos * iSizeX;
			for (long iXPos = 0; iXPos < iSizeX; ++iXPos) {
				rHeights[iSeek] = doubleConv(ptrBufIn[iXPos]);
				if (rHeights[iSeek] != rUNDEF) {
					if (rHeights[iSeek] < rMin)
						rMin = rHeights[iSeek];
					else if (rHeights[iSeek] > rMax)
						rMax = rHeights[iSeek];
				}
				++iSeek;
			}
		}
	}                                                                         
	mp->KeepOpen(false);
	rrMinMax = RangeReal(rMin, rMax);
	for (long i = 0; i < iSizeX * iSizeY; ++i)
		if (rHeights[i] == rUNDEF)
			rHeights[i] = rMin;
	return true;
}

// === ErrorMatrix calculations ============================================

//inline double tsub(double a, double b)
//{
//	if (a != rUNDEF && b != rUNDEF)
//		return a - b;
//	else
//		return 0.0;
//}
//
//inline double tavg(double a, double b)
//{
//	if (a != rUNDEF) {
//		if (b != rUNDEF)
//			return (a + b) / 2.0;
//		else
//			return a;
//	} else
//		return b;
//}

bool DEMTriangulator::fCalcD2ErrorMatrix(Tranquilizer & trq)
{
	for (int iSize = 2; iSize <= iSize2l; iSize *= 2) {
		int s2 = iSize / 2;
		for (int centerX = s2; centerX < iSizeX - s2; centerX += iSize) { // start left (west), and move right (east)
			for (int centerY = s2; centerY < iSizeY - s2; centerY += iSize) { // start up (north) and move down (south)
				int ixNW = centerX - s2 + iSizeX * (centerY - s2);
				int ixNE = centerX + s2 + iSizeX * (centerY - s2);
				int ixSE = centerX + s2 + iSizeX * (centerY + s2);
				int ixSW = centerX - s2 + iSizeX * (centerY + s2);
				int ixC = centerX + iSizeX * centerY;
				// --- north, east, south, west height errors --------------------------
				//double nhErr = abs(tsub(rHeights[centerX + iSizeX * (centerY - s2)], tavg(rHeights[ixNW], rHeights[ixNE])));
				//double ehErr = abs(tsub(rHeights[centerX + s2 + iSizeX * centerY], tavg(rHeights[ixNE], rHeights[ixSE])));
				//double shErr = abs(tsub(rHeights[centerX + iSizeX * (centerY + s2)], tavg(rHeights[ixSE], rHeights[ixSW])));
				//double whErr = abs(tsub(rHeights[centerX - s2 + iSizeX * centerY], tavg(rHeights[ixSW], rHeights[ixNW])));

				// --- 1. and 2. diagonal height error ---------------------------------
				//double d1hErr = abs(tsub(rHeights[ixC], tavg(rHeights[ixNW], rHeights[ixSE])));
				//double d2hErr = abs(tsub(rHeights[ixC], tavg(rHeights[ixNE], rHeights[ixSW])));

				double nhErr = abs(rHeights[centerX + iSizeX * (centerY - s2)] - (rHeights[ixNW] + rHeights[ixNE]) / 2.0);
				double ehErr = abs(rHeights[centerX + s2 + iSizeX * centerY] - (rHeights[ixNE] + rHeights[ixSE]) / 2.0);
				double shErr = abs(rHeights[centerX + iSizeX * (centerY + s2)] - (rHeights[ixSE] + rHeights[ixSW]) / 2.0);
				double whErr = abs(rHeights[centerX - s2 + iSizeX * centerY] - (rHeights[ixSW] + rHeights[ixNW]) / 2.0);

				// --- 1. and 2. diagonal height error ---------------------------------
				double d1hErr = abs(rHeights[ixC] - (rHeights[ixNW] + rHeights[ixSE]) / 2.0);
				double d2hErr = abs(rHeights[ixC] - (rHeights[ixNE] + rHeights[ixSW]) / 2.0);


				// --- determine max of the 6 errors -----------------------------------
				// The maximum of the absolute values of these elevation differences is called d2
				// d2 =(1/d)*max(i=1..6)|dhi|.
				double rMaxhErr = max(max(nhErr, ehErr), max(max(shErr, whErr), max(d1hErr, d2hErr)));

				rFactors[centerX + iSize2u * centerY] = rMaxhErr / ((double) iSize);
			}
			if (trq.fUpdate(++iTrqVal, iTrqMax))
				return false;
		}
	}
	return true;
}

bool DEMTriangulator::fPropagateD2Errors(Tranquilizer & trq)
{
	int x1 = 0, y1 = 0, x2 = 0, y2 = 0, x3 = 0, y3 = 0;
	// --- iterate through all levels --------------------------------------
	for (int iSize = 2; iSize <= iSize2l; iSize *= 2) { // start with 2x2 and grow
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

				// propagate half of the error to the next level, to ensure that the relevant next level quads are split into at least half of the triangles, so that there is no crack in the result
				// (restricted quadtree property)
				double rPropagateErr = rFactors[centerX + iSize2u * centerY] / 2.0;

				// --- propagate to 3 parents ------------------------------
				// --- to real father --------------------------------------
				rFactors[x1 + iSize2u * y1] = max(rFactors[x1 + iSize2u * y1], rPropagateErr);
				// --- other 2 "parents" -----------------------------------
				if (x2 >= 0 && x2 < iSize2u && y2 >= 0 && y2 < iSize2u)
					rFactors[x2 + iSize2u * y2] = max(rFactors[x2 + iSize2u * y2], rPropagateErr);
				if (x3 >= 0 && x3 < iSize2u && y3 >= 0 && y3 < iSize2u)
					rFactors[x3 + iSize2u * y3] = max(rFactors[x3 + iSize2u * y3], rPropagateErr);
			}
			if (trq.fUpdate(++iTrqVal, iTrqMax))
				return false;
		}
	}
	return true;
}

// === Triangulation =======================================================

bool DEMTriangulator::fTriangulateMesh(Tranquilizer & trq)
{
	for (int iSize = 2; iSize <= iSize2l; iSize *= 2) {
		int s2 = iSize / 2;
		int x = s2;
		for (; x < iSizeX - s2; x += iSize) { // start left (west), and move right (east)
			int y = s2;
			for (; y < iSizeY - s2; y += iSize) { // start up (north) and move down (south)
				double rError = iSize * rFactors[x + iSize2u * y];
				if ((rError <= rHeightAccuracy)) {
					// the error of replacing this quad by a higher level quad is small
					// however, don't skip this quad if the higher level quad is at the edge of the image and would not fit to be displayed in its entirety.
					//deleteNode(x, y, iSize);
					rFactors[x + iSize2u * y] = rUNDEF;
				} else {			
					// --- leaf or parent? ---------------------------------------------
					// Calculate the blend value, that determines the contribution of the height at the current location when blending with neighbours			
					double blend = min(2 * (1.0 - rHeightAccuracy / rError), 1.0);
					rFactors[x + iSize2u * y] = blend;
				}
			}
			if (y - s2 < iSizeY) // "if (y - s2 < iSizeY)" is there to except perfect powers of 2 whereby this block is superfluous
				rFactors[x + iSize2u * y] = rFactors[x + iSize2u * (y - iSize)];
			if (trq.fUpdate(++iTrqVal, iTrqMax))
				return false;
		}
		if (x - s2 < iSizeX) { // if (x - s2 < iSizeX) is there to except perfect powers of 2 whereby this block is superfluous
			int y = s2;
			for (; y < iSizeY - s2; y += iSize)
				rFactors[x + iSize2u * y] = rFactors[x - iSize + iSize2u * y];
			if (y - s2 < iSizeY) // "if (y - s2 < iSizeY)" is there to except perfect powers of 2 whereby this block is superfluous
				rFactors[x + iSize2u * y] = rFactors[x - iSize + iSize2u * (y - iSize)];
		}
	}

	return true;
}

//void DEMTriangulator::deleteNode(int x, int y, int iSize) {
//	rFactors[x + iSize2u * y] = rUNDEF;
//	for (int iSize1 = iSize; iSize1 > 4; iSize1 /= 2) {
//		int s2 = iSize1 / 2;
//		int s4 = iSize1 / 4;
//		for (int x1 = x - s4; x1 < x + s4; x1 += s2) {
//			for (int y1 = y - s4; y1 < y + s4; y1 += s2) {
//				rFactors[x1 + iSize2u * y1] = rUNDEF;				
//			}			
//		}
//	}
//}

// === Render Terrain-Mesh =================================================

bool DEMTriangulator::fRenderMesh(Tranquilizer & trq)
{
	for (int iSize = iSize2l; iSize > 1; iSize /= 2) {
		int s2 = iSize / 2;
		int x = s2;
		for (; x < iSizeX - s2; x += iSize) { // start left (west), and move right (east)
			int y = s2;
			for (; y < iSizeY - s2; y += iSize) { // start up (north) and move down (south)
				if (rFactors[x + iSize2u * y] != rUNDEF) {
					// --- get all 9 heights -----------------------------------------------
					double rhNW = rHeights[x - s2 + iSizeX * (y - s2)];
					double rhNE = rHeights[x + s2 + iSizeX * (y - s2)];
					double rhSW = rHeights[x - s2 + iSizeX * (y + s2)];
					double rhSE = rHeights[x + s2 + iSizeX * (y + s2)];
					double hC;
					double hN;
					double hS;
					double hW;
					double hE;
					if (fSmooth) {
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
						hC = rGetHeight(x, y, iSize, dirToFather, C, rhNW, rhNE, rhSW, rhSE);
						hN = (y > s2) ? rHeights[x + iSizeX * (y - s2)] : rGetHeight(x, y, iSize, dirToFather, N, rhNW, rhNE, rhSW, rhSE);
						hS = rGetHeight(x, y, iSize, dirToFather, S, rhNW, rhNE, rhSW, rhSE);
						hW = (x > s2) ? rHeights[x - s2 + iSizeX * y] : rGetHeight(x, y, iSize, dirToFather, W, rhNW, rhNE, rhSW, rhSE);
						hE = rGetHeight(x, y, iSize, dirToFather, E, rhNW, rhNE, rhSW, rhSE);
						rHeights[x + iSizeX * y] = hC;
						if (y == s2)
							rHeights[x + iSizeX * (y - s2)] = hN;
						rHeights[x + iSizeX * (y + s2)] = hS;
						if (x == s2)
							rHeights[x - s2 + iSizeX * y] = hW;
						rHeights[x + s2 + iSizeX * y] = hE;
					} else {
						hC = rHeights[x + iSizeX * y];
						hN = rHeights[x + iSizeX * (y - s2)];
						hS = rHeights[x + iSizeX * (y + s2)];
						hW = rHeights[x - s2 + iSizeX * y];
						hE = rHeights[x + s2 + iSizeX * y];
					}
							
					if (!fCreateFanAround1(x, y, iSize, hC, hN, hS, hW, hE, rhNW, rhNE, rhSW, rhSE))
						return false; // false, 1st reason: out of memory
				}
			}

			if (y - s2 < iSizeY) { // "if (y - s2 < iSizeY)" is there to except perfect powers of 2 whereby this block is superfluous
				if (rFactors[x + iSize2u * y] != rUNDEF) {
					double rhNW = rHeights[x - s2 + iSizeX * (y - s2)];
					double rhNE = rHeights[x + s2 + iSizeX * (y - s2)];
					double hC;
					double hN = rHeights[x + iSizeX * (y - s2)];
					double hW;
					double hE;
					if (y < iSizeY) {
						hC = rHeights[x + iSizeX * y];
						hW = rHeights[x - s2 + iSizeX * y];
						hE = rHeights[x + s2 + iSizeX * y];
					} else {
						hC = hN;
						hW = rhNW;
						hE = rhNE;
					}
					double rhSW = hW;
					double rhSE = hE;
					double hS = hC;
					if (!fCreateFanAround1(x, y, iSize, hC, hN, hS, hW, hE, rhNW, rhNE, rhSW, rhSE))
						return false; // false, 1st reason: out of memory
				}
			}

			if (trq.fUpdate(++iTrqVal, iTrqMax))
				return false; // false, 2nd reason: user aborted
		}
		if (x - s2 < iSizeX) { // if (x - s2 < iSizeX) is there to except perfect powers of 2 whereby this block is superfluous
			int y = s2;
			for (; y < iSizeY - s2; y += iSize) {
				if (rFactors[x + iSize2u * y] != rUNDEF) {
					double rhNW = rHeights[x - s2 + iSizeX * (y - s2)];
					double rhSW = rHeights[x - s2 + iSizeX * (y + s2)];
					double hC;
					double hN;
					double hS;
					double hW = rHeights[x - s2 + iSizeX * y];
					if (x < iSizeX) {
						hC = rHeights[x + iSizeX * y];
						hN = rHeights[x + iSizeX * (y - s2)];
						hS = rHeights[x + iSizeX * (y + s2)];
					} else {
						hC = hW;
						hS = rhSW;
						hN = rhNW;
					}
					double rhNE = hN;
					double rhSE = hS;
					double hE = hC;
					if (!fCreateFanAround1(x, y, iSize, hC, hN, hS, hW, hE, rhNW, rhNE, rhSW, rhSE))
						return false; // false, 1st reason: out of memory
				}
			}

			if (y - s2 < iSizeY) { // "if (y - s2 < iSizeY)" is there to except perfect powers of 2 whereby this block is superfluous
				if (rFactors[x + iSize2u * y] != rUNDEF) {
					double rhNW = rHeights[x - s2 + iSizeX * (y - s2)];
					double rhNE;
					double rhSW;
					double hC;
					double hN;
					double hW;
					if (x < iSizeX) {
						hN = rHeights[x + iSizeX * (y - s2)];
						if (y < iSizeY) {
							hC = rHeights[x + iSizeX * y];
							hW = rHeights[x - s2 + iSizeX * y];
						} else {
							hC = hN;
							hW = rhNW;
						}
						rhSW = hW;
						rhNE = hN;
					} else {
						if (y < iSizeY)
							hW = rHeights[x - s2 + iSizeX * y];
						else
							hW = rhNW;
						hN = rhNW;
						hC = hW;
						rhNE = hN;
						rhSW = hW;
					}
					double hS = hC;
					double hE = hC;
					double rhSE = hC;

					if (!fCreateFanAround1(x, y, iSize, hC, hN, hS, hW, hE, rhNW, rhNE, rhSW, rhSE))
						return false; // false, 1st reason: out of memory
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
		heightValue = rHeights[x + iSizeX * y];
		break;
	case N:
		heightValue = rHeights[x + iSizeX * (y - iSize / 2)];
		break;
	case S:
		heightValue = rHeights[x + iSizeX * (y + iSize / 2)];
		break;
	case W:
		heightValue = rHeights[x - iSize / 2 + iSizeX * y];
		break;
	case E:
		heightValue = rHeights[x + iSize / 2 + iSizeX * y];
		break;
	default:
		heightValue = 0;				
	}

	// --- determine blend values if this is a leaf ------------------------
	
	if (fSmooth) {
		// blend the height with the height of two neighbours
		
		double blend = rFactors[x + iSize2u * y];
		
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
				blend = min(blend, rFactors[x + iSize2u * (y - iSize)]);
			heightValue = (1.0 - blend) * (rhNW + rhNE) / 2.0 + blend * heightValue;
			break;

		case S:
			if (y + iSize < iSizeY)
				blend = min(blend, rFactors[x + iSize2u * (y + iSize)]);
			heightValue = (1.0 - blend) * (rhSW + rhSE) / 2.0 + blend * heightValue;
			break;

		case W:
			if (x - iSize >= 0)
				blend = min(blend, rFactors[x - iSize + iSize2u * y]);
			heightValue = (1.0 - blend) * (rhNW + rhSW) / 2.0 + blend * heightValue;
			break;

		case E:
			if (x + iSize < iSizeX)
				blend = min(blend, rFactors[x + iSize + iSize2u * y]);
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
		corners[NW] = (rFactors[x - s4 + iSize2u * (y - s4)] == rUNDEF);
		corners[NE] = (rFactors[x + s4 + iSize2u * (y - s4)] == rUNDEF);
		corners[SW] = (rFactors[x - s4 + iSize2u * (y + s4)] == rUNDEF);
		corners[SE] = (rFactors[x + s4 + iSize2u * (y + s4)] == rUNDEF);
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

	if ((y - iSize >= 0) && (rFactors[x + iSize2u * (y - iSize)] == rUNDEF))
		corners[N] = false;
	if ((x + iSize < iSizeX) && (rFactors[x + iSize + iSize2u * y] == rUNDEF))
		corners[E] = false;
	if ((y + iSize < iSizeY) && (rFactors[x + iSize2u * (y + iSize)] == rUNDEF))
		corners[S] = false;
	if ((x - iSize >= 0) && (rFactors[x - iSize + iSize2u * y] == rUNDEF))
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
		corners[NW] = (rFactors[x - s4 + iSize2u * (y - s4)] == rUNDEF);
		corners[NE] = (rFactors[x + s4 + iSize2u * (y - s4)] == rUNDEF);
		corners[SW] = (rFactors[x - s4 + iSize2u * (y + s4)] == rUNDEF);
		corners[SE] = (rFactors[x + s4 + iSize2u * (y + s4)] == rUNDEF);
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

	if ((y - iSize >= 0) && (rFactors[x + iSize2u * (y - iSize)] == rUNDEF))
		corners[N] = false;
	if ((x + iSize < iSizeX) && (rFactors[x + iSize + iSize2u * y] == rUNDEF))
		corners[E] = false;
	if ((y + iSize < iSizeY) && (rFactors[x + iSize2u * (y + iSize)] == rUNDEF))
		corners[S] = false;
	if ((x - iSize >= 0) && (rFactors[x - iSize + iSize2u * y] == rUNDEF))
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
	
	Coord c;
	mp->gr()->RowCol2Coord(0.5 + y, 0.5 + x, c);
	double s, t;
	if (fSelfDrape) {
		s = x / (double)width;
		t = y / (double)height;
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

void DEMTriangulator::PlotTriangles()
{
	glInterleavedArrays(GL_T2F_V3F, sizeof(Vertex), vertices);
	glDrawArrays(GL_TRIANGLES, 0, iNrVertices);
}

bool DEMTriangulator::fValid()
{
	return valid;
}