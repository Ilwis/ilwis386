#include "Headers\toolspch.h"
#include "ParetoGraphFunction.h"

ParetoGraphFunction::ParetoGraphFunction()
: SimpleFunction(0, DoubleRect(0, 0, 1, 1))
, iSelectedAnchorNr(-1)
{
	m_rDataX.resize(0);
	m_rDataY.resize(0);
}

ParetoGraphFunction::~ParetoGraphFunction()
{
}

double ParetoGraphFunction::rGetFx(double x) const
{
	int iDataSize = m_rDataX.size();
	if (iDataSize > 0)
	{
		int iX = 0;
		while (iX < iDataSize && m_rDataX[iX] < x)
			++iX;
		// iX >= iDataSize || m_rDataX[iX] >= x
		if (iX == 0)
			return m_rDataY[0];
		else if (iX >= iDataSize)
			return m_rDataY[iDataSize - 1];
		else if (m_rDataX[iX] == x)
			return m_rDataY[iX];
		else // normal case, linear interpolation
		{
			double rLeftX = m_rDataX[iX - 1];
			double rRightX = m_rDataX[iX];
			double rLeftY = m_rDataY[iX - 1];
			double rRightY = m_rDataY[iX];
			double portion = (x - rLeftX) / (rRightX - rLeftX);
			double rResult = rLeftY + portion * (rRightY - rLeftY);
			return rResult;
		}
	}
	else
		return m_Domain.bottom;
}

void ParetoGraphFunction::SetData(vector<double> dataX, vector<double> dataY)
{
	m_rDataX = dataX;
	m_rDataY = dataY;
	m_Anchors.clear();
	for (int i = 0; i < dataX.size(); ++i)
		m_Anchors.push_back(DoublePoint(dataX[i], dataY[i]));
}

void ParetoGraphFunction::SetDefaultAnchors()
{
	// no anchors to set
	SolveParams();
}

void ParetoGraphFunction::SolveParams()
{
	// nothing to solve
}

void ParetoGraphFunction::SetAnchor(DoublePoint pAnchor)
{
	if (m_Anchors.size() > 0)
	{
		iSelectedAnchorNr = 0;
		for (int i = 1; i < m_Anchors.size(); ++i)
		{
			if (rUnitDist2(pAnchor, m_Anchors[i]) < rUnitDist2(pAnchor, m_Anchors[iSelectedAnchorNr]))
				iSelectedAnchorNr = i;
		}
	}
}

int ParetoGraphFunction::iGetAnchorNr()
{
	return iSelectedAnchorNr;
}

