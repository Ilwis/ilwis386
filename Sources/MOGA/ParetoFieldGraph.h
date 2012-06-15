#if !defined(AFX_PARETOFIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_)
#define AFX_PARETOFIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "Client\FormElements\fentbase.h"

#include "Client\FormElements\FieldGraph.h"

class ParetoGraphWindow : public SimpleGraphWindowWrapper
{
public:
	ParetoGraphWindow(FormEntry *f);
protected:
	virtual void DrawFunction(CDC* pDC, const SimpleFunction * pFunc);
};

class ParetoFieldGraph : public FieldGraph  
{
public:
	ParetoFieldGraph(FormEntry* parent);
	void create();             // overriden
};

#endif // !defined(AFX_PARETOFIELDGRAPH_H__E62D8985_1D8F_452C_815D_E3D491C4EFC0__INCLUDED_)
