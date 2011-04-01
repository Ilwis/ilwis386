// DEMTriangulator.h: interface for the DEMTriangulator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEMTRIANGULATOR_H__CA562F44_5513_4711_8E56_EA83EE25A9DC__INCLUDED_)
#define AFX_DEMTRIANGULATOR_H__CA562F44_5513_4711_8E56_EA83EE25A9DC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace ILWIS {
	struct Vertex
	{
		Vertex(double s, double t, double x, double y, double z)
		: s(s)
		, t(t)
		, x(x)
		, y(y)
		, z(z)
		{
		}
		float s, t, x, y, z;
	};
	
	class Texture;
	class ZValueMaker;

	class DEMTriangulator
	{
	public:
		DEMTriangulator(ZValueMaker * zMaker, BaseMapPtr * drapeMapPtr, CoordSystem & csyDest, bool fSmooth);
		~DEMTriangulator(void);
		void PlotTriangles(double xMin, double yMin, double xMax, double yMax, Texture * tex);
		void PlotTriangles();
		bool fValid();

	private:
		bool fDoTriangulate();
		bool fReadMap(Tranquilizer & trq);
		bool fCalcD2ErrorMatrix(Tranquilizer & trq);
		bool fPropagateD2Errors(Tranquilizer & trq);
		bool fTriangulateMesh(Tranquilizer & trq);
		bool fRenderMesh(Tranquilizer & trq);
		double rGetHeight(int x, int y, int size, int dirToFather, int neswc, double rhNW, double rhNE, double rhSW, double rhSE);
		bool fCreateFanAround1(int x, int y, int size, double hC, double hN, double hS, double hW, double hE, double hNW, double hNE, double hSW, double hSE);
		void CreateFanAround2(int x, int y, int size, double hC, double hN, double hS, double hW, double hE, double hNW, double hNE, double hSW, double hSE);
		void AddVertex(int x, int y, double rHeight);

		static const int NW = 0, W = 1, SW = 2, S = 3, SE = 4, E = 5, NE = 6, N = 7, C = 8;
		const int iNrVerticalSteps;

		Map mp;
		Map drapemp;
		RangeReal rrMinMax;
		ZValueMaker * zMaker;
		double* rHeights; // the DEM rastermap's values
		double* rFactors; // represents quad tree (and contains heights)
		Vertex* vertices;
		unsigned long iVerticesArraySize;
		unsigned long iNrVertices;
		const unsigned long iVertexArrayIncrement;
		unsigned long iTrqVal;
		unsigned long iTrqMax;

		int iSize2l;
		int iSize2u;
		int iSizeX;
		int iSizeY;
		unsigned long width;
		unsigned long height;
		double rStepX;
		double rStepY;
		double rMinX;
		double rMaxY;
		double rHeightAccuracy;
		const bool fSmooth;
		bool valid;
		bool fSameCsy;
		bool fSelfDrape;
		CoordSystem csyMap;
		CoordSystem csyDrape;
		CoordSystem csyDest;
	};
}

#endif // !defined(AFX_DEMTRIANGULATOR_H__CA562F44_5513_4711_8E56_EA83EE25A9DC__INCLUDED_)