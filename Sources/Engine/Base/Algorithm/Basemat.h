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
/* $Log: /ILWIS 3.0/Tools/Basemat.h $
 * 
 * 5     16-08-01 18:58 Koolhoven
 * assignment operator debugged, could cause crashes
 * 
 * 4     14-09-00 12:40 Hendrikse
 * corrected comment format at top of file
 * 
 * 3     12-09-00 20:22 Hendrikse
 */
// * 
// * 2     12-09-00 20:08 Hendrikse
// * The assignment operator:
// * void BaseMatrix<T>::operator  = (const BaseMatrix<T>& a)
// * needed an extra check:	if (&a == this)  return;
// * to allow matrices to be assigned to a matrix with the same adress
// Revision 1.3  1997/09/10 10:13:42  Wim
// do not new zero bytes in operator = of BaseMatrix
//
// Revision 1.2  1997-09-01 09:28:05+02  Wim
// Added special empty constructor to prevent uninitialized pointers in tlstmpl.c
//
// tls/basemat.h
// BaseMatrix Interface for ILWIS 2.0
// october 1994, by Jan-Willem Rensman
// (c) Computer Department ITC

#ifndef ILWBASEMATRIX_H
#define ILWBASEMATRIX_H

#include "Headers\base.h"
#include "Engine\Base\DataObjects\ERR.H"
template <class T>
class BaseMatrix
{
public:
  BaseMatrix(int iRow, int iCol = 0);
  ~BaseMatrix();
  BaseMatrix(const BaseMatrix<T>&);
#ifdef TLSTMPL_C
  BaseMatrix(): _iRows(0), _iCols(0), _iLen(0), p(0) {}
#endif
  T&        operator () (int iR, int iC)        { return tVal(iR, iC); }
  T         operator () (int iR, int iC) const  { return tVal(iR, iC); }
  void      operator  = (const BaseMatrix<T>&);

  //Inline functions
  int iRows() const                      { return _iRows; }
  int iCols() const                      { return _iCols; }
  long iLen()  const                     { return _iLen;  }
  bool fIndex (int iR, int iC)
    { return (iR>=0) && (iR < iRows()) && (iC >= 0) && (iC < iCols()); }

protected:
  T& tVal (int iR, int iC)
    { /*assert((iR>=0) && (iR < iRows()) && (iC >= 0) && (iC < iCols()));*/
      return p[iR*iCols()+iC]; }
  T  tVal (int iR, int iC) const
    { /*assert((iR>=0) && (iR < iRows()) && (iC >= 0) && (iC < iCols()));*/
      return p[iR*iCols()+iC]; }
  T& tVal (int i)
    { /*assert((i>=0) && (i < iLen()));*/
      return p[i]; }
  T  tVal (int i) const
    { /*assert((i>=0) && (i < iLen()));*/
      return p[i]; }
  int _iRows, _iCols;
  long _iLen;
  T* p;
};

template <class T>
BaseMatrix<T>::BaseMatrix(int iRow, int iCol)
{
   assert(iRow >= 0);
   assert(iCol >= 0);
   _iRows = iRow;
   _iCols = (iCol == 0 ? iRow : iCol);
   _iLen  = (long)iRows() * iCols();
   if (0 != iLen()) {
     p = (T *) ::operator new ((unsigned long)(iLen() * sizeof(T)));
     if (0 == p)
       MemAllocError();
   }
   else
     p = 0;
   if (p) 
     memset(p, 0, iLen() * sizeof(T));
}

template <class T>
BaseMatrix<T>::BaseMatrix(const BaseMatrix<T>& a)
{
   _iRows = a.iRows();
   _iCols = a.iCols();
   _iLen = a.iLen();
   p = (T *) ::operator new ((unsigned long)(iLen() * sizeof(T)));
   if ((0 != iLen()) && (0 == p))
     MemAllocError();
   if (p) 
     memcpy(p, a.p, iLen() * sizeof(T));
}

template <class T>
BaseMatrix<T>::~BaseMatrix()
{
   if (p) delete p;
}

template <class T>
void BaseMatrix<T>::operator = (const BaseMatrix<T>& a)
{
	if (&a == this)
		return;
	if (p) 
    delete p;
  p = 0;
	_iRows = a.iRows();
	_iCols = a.iCols();
	_iLen = a.iLen();
	if (iLen() == 0)
		return;
	p = (T *) ::operator new ((unsigned long)(iLen() * sizeof(T)));
	if ((iLen() != 0) && (0 == p))
		MemAllocError();
	if (p) 
	memcpy(p, a.p, iLen() * sizeof(T));
}

#ifdef TLSTMPL_C
#define MATRIX_SPEC __export
#else
#define MATRIX_SPEC __import
#endif

template class MATRIX_SPEC BaseMatrix<double>;
template class MATRIX_SPEC BaseMatrix<float>;
template class MATRIX_SPEC BaseMatrix<long>;
template class MATRIX_SPEC BaseMatrix<int>;
template class MATRIX_SPEC BaseMatrix<short>;
template class MATRIX_SPEC BaseMatrix<bool>;
template class MATRIX_SPEC BaseMatrix<byte>;
template class MATRIX_SPEC BaseMatrix<word>;

typedef BaseMatrix<float>  FloatMatrix;
typedef BaseMatrix<long>   LongMatrix;
typedef BaseMatrix<int>    IntMatrix;
typedef BaseMatrix<short>  ShortMatrix;
typedef BaseMatrix<bool>   BoolMatrix;
typedef BaseMatrix<byte>   ByteMatrix;
typedef BaseMatrix<word>   WordMatrix;


#endif







