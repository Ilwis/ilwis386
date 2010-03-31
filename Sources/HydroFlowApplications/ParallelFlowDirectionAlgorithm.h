#ifndef ILWMAPFLOWDIRECTIONALG_H
#define ILWMAPFLOWDIRECTIONALG_H

//#include "RasterApplication\mapfmap.h"
#include "Engine\Applications\MAPFMAP.H"

class Cell {
public:
	int row;
	int col;
	byte val;
	int grd;
	Cell() {}
	Cell(int Row, int Col) {
		row = Row;
		col = Col;
	}
	void setRC(int Row, int Col) {
		row = Row;
		col = Col;
	}
};	

class FlowDirectionAlgorithm {
public:
	enum Method {slope, height};
	enum FlowDirection      {NW, N, NE, W, E, SW, S, SE};
private:
	Tranquilizer* m_trq;
	byte flatcell;
	byte flag;
	FlowDirectionAlgorithm::Method method;
	byte Location[8];
	vector<RealBuf> dem;
	vector<ByteBuf> flowdir;
	long lines, columns;
	byte increment;

	Method methodValueOf(String val);
	bool isEven(int elem);
	bool isEdge(int row, int col);
	bool isInOneEdge(const vector<FlowDirection>& listPos, FlowDirection fd1, FlowDirection fd2, FlowDirection fd3);
	bool hasFlow(byte flowdirection);
	
	double maxAdj(int row, int col, double listVal[]);
	double maxAdj(int row, int col, vector<IntBuf>& gradient, double listVal[]);
	
	void findDirection(double listA[], double val, vector<FlowDirection>& listPos);
	FlowDirection getFlowDirection(const vector<FlowDirection>& listPos);
	void locateOutlet(int row, int col, vector<Cell>& flatList, vector<Cell>& outList);
	void imposeGradient2LowerElevation( vector<Cell>& outletList, vector<Cell>& flatList, vector<IntBuf>& gradient);
	void imposeGradientFromElevation(vector<Cell>& flatList, vector<IntBuf>& gradient);
	void combineGradient(const vector<IntBuf>& grd1, vector<IntBuf>& grd2, vector<Cell>& flatList);
	void assignFlowInFlat(vector<Cell>& flatList, vector<IntBuf>& gradient);
	void iniGradient(vector<IntBuf>& grd1, vector<IntBuf>& grd2, vector<Cell>& flatList);

	double computeSlope(double h1,double h2,int pos);
	double computeHeightDifference(double h1,double h2);
	FlowDirection mapFlowLocation(int pos);

public:
	static int noflow;
	FlowDirectionAlgorithm(Tranquilizer*);
	vector<ByteBuf> calculate(const vector<RealBuf>& dem, String method);
};

#endif ILWMAPFLOWDIRECTIONALG_H
