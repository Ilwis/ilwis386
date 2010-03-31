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
/* tls/stack.h
// Interface for Stack for ILWIS 2.0
// april 1994, by Jelle Wind
// (c) Computer Department ITC
	Last change:  WK   28 Aug 98    4:49 pm
*/

#ifndef ILWSTACK_H
#define ILWSTACK_H

#include "Headers\base.h"
#include "Engine\Base\DataObjects\Dlist.h"

template <class T>
class Stack : public DList<T>
{
public:
  Stack();
  ~Stack();
  Stack<T>& push(const T& tVal);
  T pop();
  T top() const;
  bool fEmpty() const
    { return iSize() == 0; }
};

template <class T>
Stack<T>::Stack() 
: DList<T>() 
{};

template <class T>
Stack<T>::~Stack() 
{};

template <class T>
Stack<T>& Stack<T>::push(const T& tVal)
{
   insert(tVal);
   return *this;
}

template <class T>
T Stack<T>::pop()
{ 
  DLIter<T> iter(this);
  return iter.remove();
}

template <class T>
T Stack<T>::top() const
{
  DLIterC<T> iter(this);
  return iter();
}

#ifdef TLSTMPL_C
#define STACK_SPEC __export
#else
#define STACK_SPEC __import
#endif

template class STACK_SPEC Stack<int>;

#endif







