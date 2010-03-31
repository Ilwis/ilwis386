/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/

class GradientBand
{
public:
	_export GradientBand();
	_export ~GradientBand();

	bool _export Create(CWnd *par, const Representation& rpr, CRect& rct);
	void _export Draw(CDC*);
	CRect _export SetOuterRect(CRect rct);
	void _export SetHorizontal(bool fHorz);
	bool _export fHorizontal() const;
	CSize _export GetSizeNeeded();
	void _export SetFont(CFont *fnt);
									
private:					
	void CalcStepValues(RangeReal rr, double& rLowerLim, double& rStep);
	// SetText() draws tickmark and optionally text at position rPerc (0..1)
	void SetText(CDC* dc, double rPerc, const String& sText, bool fDrawText);
	String sVal(double rVal) const;
	// get color to draw at position rPerc (0..1)
	Color clr(double rPerc) const;
	CSize szMaxTextSize();
	void FillNumbersArray();
									
	bool m_fHorizontal;
	double rLowerVal;				
	double rUpperVal;
	struct ShowStruct 
	{
		double rVal;
		bool fShow;
	};
	vector<ShowStruct> vssShow;
	Representation rpr;				
	CRect	rectOuter;
	CRect rectInner;
	CWnd* wndPar;
	CFont font;
};
