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
// tls/slist.h
// Interface for Singly Linked lists for ILWIS 2.0
// sept 1993, by Jelle Wind
// (c) Computer Department ITC
// jan 1994, Changed to Templates by Wim Koolhoven

#ifndef ILWSLIST_H
#define ILWSLIST_H

//#include "Headers\base.h"

template <class T> class SLNode;
template <class T> class SList;
template <class T> class SLIter;
template <class T> class SLIterC;

template <class T>
class SLNode
{
  friend SList<T>;
  friend SLIter<T>;
  friend SLIterC<T>;
private:
  SLNode(const T& tVal): body(tVal) { next = 0; }
  SLNode *next;
  T body;
};

template <class T>
class SList
{
  friend SLIter<T>;
  friend SLIterC<T>;
public:
  SList();
  ~SList();
  SList<T>& append(const T& tVal);
  T& operator[](long iInd);
  const T& operator[](long iInd) const;
  long size() const { return count; }
  long iSize() const { return count; }
private:
  long count;
  SLNode<T> *root;
  SLNode<T> *last;
};

template <class T>
class SLIter
{
public:
  SLIter(SList<T>* slist) { sl = slist; current = sl->root; }
  T& operator()() const { return get(); }
  void append(const T& tVal);
  T remove();
  void first() { current = sl->root; }
  void last()  { current = sl->last; }
  void operator++()    { if (fValid()) current = current->next; }
  void operator++(int) { if (fValid()) current = current->next; }
  bool fValid() const { return current != 0; }
private:
  T& get() const { assert(("SList::get() called",fValid())); return current->body; }
  SLNode<T> *current;
  SList<T>* sl;
};

template <class T>
class SLIterC
{
public:
  SLIterC(const SList<T>* slist) { sl = slist; current = sl->root; }
  T& operator()() const { return get(); }
  void first() { current = sl->root; }
  void last()  { current = sl->last; }
  void operator++()    { if (fValid()) current = current->next; }
  void operator++(int) { if (fValid()) current = current->next; }
  bool fValid() const { return current != 0; }
private:
  T& get() const { assert(("SLIterC::get() called",fValid())); return current->body; }
  SLNode<T> *current;
  const SList<T>* sl;
};


template <class T>
class SListP: public SList<T*>
{
public:
  SListP(bool fPurge = false);
  ~SListP();
private:
  bool _fPurge;
};

template <class T>
SListP<T>::SListP(bool fPurge) 
{ _fPurge = fPurge; }

template <class T>
class SLIterP: public SLIter<T*>
{
public:
  SLIterP(SListP<T>* slist): SLIter<T*>(slist) {}
};

template <class T>
class SLIterCP: public SLIterC<T*>
{
public:
  SLIterCP(const SListP<T>* slist): SLIterC<T*>(slist) {}
};


template <class T>
SList<T>::SList() 
{ root = last = 0; count = 0; }

template <class T>
SList<T>::~SList()
{ SLNode<T>* node;
  while (root) {
    node = root;
    root = root->next;
    delete node;
  }
}

template <class T>
SList<T>& SList<T>::append(const T& tVal)
  { SLNode<T>* node = new SLNode<T>(tVal);
    if (root)
      last->next = node;
    else
      root = node;
    last = node;
    count++;
    return *this;
  }


template <class T>
T& SList<T>::operator[](long iInd)
{ SLIter<T> iter(this);
  for (;iInd;--iInd) ++iter;
  return iter();
}

template <class T>
const T& SList<T>::operator[](long iInd) const
{ SLIterC<T> iter(this);
  for (;iInd;--iInd) ++iter;
  return iter();
}

template <class T>
SListP<T>::~SListP()
{ if (_fPurge)
    for (SLIterP<T> iter(this); iter.fValid(); ++iter)
      delete iter();
}

template <class T>
void SLIter<T>::append(const T& tVal)
{ if (current) {
    SLNode<T>* node = new SLNode<T>(tVal);
    node->next = current->next;
    current->next = node;
    sl->count++;
  }
  else
    sl->append(tVal);
}


template <class T>
T SLIter<T>::remove()
{ T tVal = get();
  if (!fValid()) return tVal;
  SLNode<T>* tmp = sl->root;
  if (current == sl->root) {
    tmp = sl->root = current->next;
    if (sl->last == current)
      sl->last = 0;
    delete current;
    current = tmp;
  }
  else {
    SLNode<T>* sav = current->next;
    while (tmp->next != current)
      tmp = tmp->next;
    tmp->next = current->next;
    if (sl->last == current)
      sl->last = tmp;
    delete current;
    current = sav;
  }
  sl->count--;
  return tVal;
}

#endif







