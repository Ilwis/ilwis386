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
/* 
   $Log: /ILWIS 3.0/Tools/ARRAY.H $
 * 
 * 10    18-10-02 18:33 Koolhoven
 * Array::iSize() now returns an unsigned int
 * 
 * 9     22-09-00 9:40a Martin
 * made constructor of array more efficient
 * 
 * 8     20-01-00 4:37p Martin
 * uses push_back instaead of insert(end())
 * 
 * 7     15-12-99 14:01 Wind
 * improved Append, Insert and Remove
 * 
 * 6     14-12-99 13:14 Wind
 * error in Append()
 * 
 * 5     13-12-99 13:28 Wind
 * array is now derived from vector (and only zero based!)
 * 
 * 4     14-06-99 8:34a Martin
 * shorts to int where applicable
 * 
 * 3     7-06-99 11:08 Koolhoven
 * ArrayLarge is now a synonym for Array
	
	Interface for Array for ILWIS 2.0
   april 1994, by Jelle Wind
   (c) Ilwis System Development ITC
*/
#ifndef ILWARRAY_H
#define ILWARRAY_H

#include <vector>

template <class T> class Array : public vector<T>
{
public:
  Array() { Reset(); }
  Array(int iSize) : vector<T>(iSize) {}
  Array(const std::vector<T>& vec) : vector<T>(vec) {}
  void Reset() { resize(0); }
  void Resize(int iSize) { resize(iSize); }
  void Insert(int iStart, int iSize) { insert(begin()+iStart,iSize, T()); }
  void Remove(int iStart, int iSize) { erase(begin()+iStart, begin()+iStart+iSize); }
  void Append(int iSize) { insert(end(),iSize, T()); }
  unsigned int iSize() const  { return (unsigned int)size(); }
  void operator&=(const T& t)    { push_back(t); }
  void operator&=(const Array<T>& a);
  const T& ind(int index) const { return operator[](index); }
  T& ind(int index) { return operator[](index); }
  T* buf() { return &operator[](0); }
};

template <class T> 
void Array<T>::operator&=(const Array<T>& a)
{ 
  for (unsigned int i=0; i < a.iSize(); i++)
    push_back(a[i]);
}

typedef Array<char>         CharArray;
typedef Array<unsigned char> ByteArray;
typedef Array<double>       RealArray;
typedef Array<short>          IntArray;
typedef Array<long>         LongArray;
typedef Array<short>        ShortArray;

#define ArrayLarge Array

typedef ArrayLarge<unsigned char>    ByteArrayLarge;
typedef ArrayLarge<char>    CharArrayLarge;
typedef ArrayLarge<short>   ShortArrayLarge;
typedef ArrayLarge<int>     IntArrayLarge;
typedef ArrayLarge<long>    LongArrayLarge;
typedef ArrayLarge<double>  RealArrayLarge;


#endif







