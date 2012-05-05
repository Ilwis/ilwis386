#ifndef PARETOGRAPHFUNCTION_H
#define PARETOGRAPHFUNCTION_H

#include "Engine\Base\Algorithm\SimpleFunction.h"

class ParetoGraphFunction : public SimpleFunction
{
public:
	ParetoGraphFunction();
	virtual ~ParetoGraphFunction();
	virtual double rGetFx(double x) const; // the most important function of this class that applies f to x and returns y
	virtual void SetDefaultAnchors(); // also call SolveParams in the implementation of this function
	void SetData(vector<double> dataX, vector<double> dataY);
protected:
	virtual void SolveParams();
private:
	vector <double> m_rDataX;
	vector <double> m_rDataY;
};

#endif // PARETOGRAPHFUNCTION_H