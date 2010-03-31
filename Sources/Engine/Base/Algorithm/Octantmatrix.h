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
#ifndef OCTANTMATRIX_H
#define OCTANTMATRIX_H
				
template <class T>  
class OctantMatrix
{
public:
	OctantMatrix(int iRadius) //this constructor calls with
		:vec(iNr(iRadius, iRadius) + 1){} // the constructor of vec, setting its size with iNr()

	// * * * 9  b=3
	// * * 5 8  b=2
	// * 2 4 7  b=1
	// 0 1 3 6  b=0
	//             // example of Octant with iRadius = 4 and iNr(4,0) = 10
	// 0 1 2 3 =a

	T& operator()   (int x, int y)  // lvalue
		{ return vec[iNr(x,y)]; }
       
	const T& operator() (int x, int y) const   // rvalue
		{ return vec[iNr(x,y)]; }

private:
	int iNr(int x, int y) const
		{
			int a = abs(x);
			int b = abs(y);
			if (a < b)
				swap(a,b);
			return (a*a + a)/2 + b;
		}
	vector<T> vec;
};

typedef OctantMatrix<double> DoubleOctantMatrix;

#endif
