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
/* tls/dlist.h
// Interface for Doubly Linked lists for ILWIS 2.0
// sept 1993, by Jelle Wind
// (c) Computer Department ITC
// jan 1994, Changed to Templates by Wim Koolhoven
	Last change:  WK   28 Aug 98    4:40 pm
*/

#ifndef ILWDLIST_H
#define ILWDLIST_H

#pragma warning( disable : 4786 )

template <class T> class DLNode;
template <class T> class DList;
template <class T> class DLIter;
template <class T> class DLIterC;


template <class T>
class DLNode {
friend DList<T>;
friend DLIter<T>;
friend DLIterC<T>;
  DLNode(const T& tVal): body(tVal) { next = prev = 0; }
  DLNode *next, *prev;
  T body;
};

template <class T>
class DList
{
    friend DLIter<T>;
    friend DLIterC<T>;

    public:
        _export DList();
        _export DList(const DList<T>&);
        _export ~DList();

        DList<T>&           append(const T& tVal);  // at end
        DList<T>&           insert(const T& tVal); // at beginning
        DList<T>&           append(DList<T>& dl);  // at end and clear dl
        DList<T>&           insert(DList<T>& dl);  // at start and clear dl
        T&                  first() 
                                { return root->body; }
        T&                  last()  
                                { return lst->body; }
        T&                  operator[](long iInd);
        long                size() const 
                                { return count; }
        long                iSize() const 
                                { return count; }
        void                removeall();
    private:
        long                count;
        DLNode<T> *         root;
        DLNode<T> *         lst;
};

template <class T>
class DLIter
{
    public:
        DLIter(DList<T>* _dl) 
            { dl = _dl; current = dl->root; }

        T&          operator()() const { return get(); }
        void        append(const T& tVal);  // after  current
        void        insert(const T& tVal); // before current
        T           remove();
        void        first() 
                        { current = dl->root; }
        void        last()  
                        { current = dl->lst; }
        void        operator++()    
                        { if (fValid()) current = current->next; }
        void        operator++(int) 
                        { if (fValid()) current = current->next; }
        void        operator--()    
                        { if (fValid()) current = current->prev; }
        void        operator--(int) 
                        { if (fValid()) current = current->prev; }
        bool        fValid() const 
                        { return current != 0; }

    private:
        T&          get() const 
                        { assert(("DList::get() called",fValid())); return current->body; }
        DLNode<T>*  current;
        DList<T>*   dl;
};


template <class T>
void DLIter<T>::append(const T& tVal)  // after  current
  { if (current) {
      DLNode<T>* node = new DLNode<T>(tVal);
      node->next = current->next;
      current->next = node;
      node->prev = current;
      if (0 != node->next)
        node->next->prev = node;
      dl->count++;
      if (current == dl->lst)
        dl->lst = node;
    }
    else
      dl->append(tVal);
  }
  
template <class T>
void DLIter<T>::insert(const T& tVal) // before current
  { if (current) {
      DLNode<T>* node = new DLNode<T>(tVal);
      node->next = current;
      node->prev = current->prev;
      current->prev = node;
      if (0 != node->prev)
        node->prev->next = node;
      dl->count++;
      if (current == dl->root)
        dl->root = node;
    }
    else
      dl->insert(tVal);
  }

template <class T>
T DLIter<T>::remove()
  { T tVal = get();
    if (fValid()) {
      DLNode<T>* tmp = current->next;
      if (current->prev)
        current->prev->next = current->next;
      else
        dl->root = current->next;
      if (current->next)
        current->next->prev = current->prev;
      else
        dl->lst = current->prev;
      dl->count--;
      delete current;
      current = tmp;
    }
    return tVal;
  }

template <class T>
class DLIterC
{
public:
  DLIterC(const DList<T>* _dl) { dl = _dl; current = dl->root; }
  T& operator()() const { return get(); }
  void first() { current = dl->root; }
  void last()  { current = dl->lst; }
  void operator++()    { if (fValid()) current = current->next; }
  void operator++(int) { if (fValid()) current = current->next; }
  void operator--()    { if (fValid()) current = current->prev; }
  void operator--(int) { if (fValid()) current = current->prev; }
  bool fValid() const { return current != 0; }
private:
  T& get() const { assert(("DLIterC::get() called",fValid())); return current->body; }
  DLNode<T> *current;
  const DList<T>* dl;
};


template <class T>
class DListP: public DList<T*>
{
public:
  DListP(bool fPurge = false);
  DListP(const DListP<T>& dl);
  ~DListP();
  void purge(bool p) { _fPurge = p; }
  void clear();
private:
  bool _fPurge;
};


template <class T>
class DLIterP: public DLIter<T*>
{
public:
  DLIterP(DListP<T>* _dl): DLIter<T*>(_dl) {}
};

template <class T>
class DLIterCP: public DLIterC<T*>
{
public:
  DLIterCP(const DListP<T>* _dl): DLIterC<T*>(_dl) {}
};

template <class T>
DList<T>::DList(const DList<T>& dl)
{
  root = lst = 0; count = 0;
  for (DLIterC<T> iter(&dl); iter.fValid(); ++iter)
    append(iter());
}


template <class T>
DList<T>::DList() 
{ 
  root = lst = 0; 
  count = 0; 
}

template <class T>
DList<T>::~DList()
{
  removeall();
};

template <class T>
T& DList<T>::operator[](long iInd)
{ DLIter<T> iter(this);
  for (;iInd>0;--iInd) ++iter;
  return iter();
}

template <class T>
void DList<T>::removeall() {
  DLNode<T>* node;
  while (root) {
    node = root;
    root = root->next;
    delete node;
  }
  root = lst = 0; count = 0;   
}  

template <class T>
DList<T>&  DList<T>::append(const T& tVal)  // at end
{ DLNode<T>* node = new DLNode<T>(tVal);
  if (root) {
    lst->next = node;
    node->prev = lst;
  }
  else
    root = node;
  lst = node;
  count++;
  return *this;
}

template <class T>
DList<T>&  DList<T>::insert(const T& tVal)  // at begin
{ DLNode<T>* node = new DLNode<T>(tVal);
  if (root) {
    root->prev = node;
    node->next = root;
  }
  else
    lst = node;
  root = node;
  count++;
  return *this;
}

template <class T>
DList<T>&  DList<T>::append(DList<T>& dl)  // at end and clear dl
{
  DLNode<T>* node = dl.root;
  if (root) {
    lst->next = node;
    if (0 != node)
      node->prev = lst;
  }
  else
    root = node;
  lst = dl.lst;
  count += dl.count;
  dl.count = 0;
  dl.root = 0;
  dl.lst = 0;
  return *this;
}

template <class T>
DList<T>&  DList<T>::insert(DList<T>& dl)  // at begin and clear dl
{ 
  DLNode<T>* node = dl.lst;
  if (root) {
    root->prev = node;
    if (0 != node)
      node->next = root;
  }
  else
    lst = node;
  root = dl.root;
  count += dl.count;
  dl.count = 0;
  dl.root = 0;
  dl.lst = 0;
  return *this;
}

template <class T>
DListP<T>::DListP(bool fPurge) 
{ _fPurge = fPurge; }

template <class T>
DListP<T>::DListP(const DListP<T>& dl)
  : DList<T*>(dl)
  {
    _fPurge = false;
    assert(dl._fPurge == false);
  }


template <class T>
DListP<T>::~DListP()
{ if (_fPurge)
    for (DLIterP<T> iter(this); iter.fValid(); ++iter)
      delete iter();
}

template <class T>
void DListP<T>::clear()
{ 
  if (_fPurge)
    for(DLIterP<T> current(this); current.fValid(); --current)
      delete current();
  removeall();  
}

#ifdef TLSTMPL_C
#define DLIST_SPEC __export
#else
#define DLIST_SPEC __import
#endif

//template class DLIST_SPEC DList<int>;

#endif







