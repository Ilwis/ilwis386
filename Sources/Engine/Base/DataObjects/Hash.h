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
/* tls/hash.h
// Interface for Hash Table for ILWIS 2.0
// sept 1993, by Jelle Wind
// (c) Computer Department ITC
// jan 1994, Changed to Templates by Wim Koolhoven
	Last change:  WK   31 Aug 98    5:22 pm
*/

#ifndef ILWHASH_H
#define ILWHASH_H

//#include  "AdditionalHeaders\PCHeaders\app.h"
//#include  "Tools\errhand.h"

template <class T> int iHash(const T&);
//template <class T> bool operator==(String*, String*);


//--------------------- HashTable interface
template <class T>
class HashTable //: public ErrorHandling
{
    public:
        HashTable();
        HashTable(int iSize);
        ~HashTable();

        void            Resize(int iSize);
        void            Clean();
        void            add(const T& item);
        void            remove(const T& item);
        T&              get(T& item) const; 
        bool            fPresent(T& item) const;
        void            VisitAll(void(*FuncVisit)(T&));
        bool            fValid() const { return table!=NULL; }
        //private:
        SList<T>**      table;
        int             iTabSize;
};

//--------------------- HashPTable interface
template <class T>
class HashPTable //: public ErrorHandling
{
    public:
        HashPTable(int iSize);
        ~HashPTable();

        void            add(T item);
        void            remove(T item);
        T               get(T item);
        void            VisitAll(void(*FuncVisit)(T&));
    private:
        SListP<T>**     table;
        int             iTabSize;
};

//--------------------- HashPPTable
template <class T>
class HashPPTable //: public ErrorHandling
{
public:
  HashPPTable(int iSize);
  ~HashPPTable();
  void add(T* item);
  void remove(T* item);
  T* get(T* item);
  void VisitAll(void(*FuncVisit)(T&));
private:
  SListP<T> **table;
  int iTabSize;
};


//--------------------- HashTable definition


template <class T>
HashTable<T>::HashTable()
{ 
  table = 0; iTabSize = 0; 
}

template <class T>
HashTable<T>::HashTable(int iSize)
{ 
  table = 0;
  Resize(iSize);
}
  
template <class T>
HashTable<T>::~HashTable() 
{ 
  Clean();
}

template <class T>
void HashTable<T>::Resize(int iSize)
{ if (table != 0) Clean();
  table = new SList<T>*[iSize];
  if (!table) return; 
  memset(table, 0, iSize * sizeof(SList<T>*));
  iTabSize = iSize;
}

template <class T>
void HashTable<T>::Clean()
{
  if (table == 0) return;
  for (int i = 0; i < iTabSize; ++i)
    if (table[i]) delete table[i];
  delete [] table;
}


template <class T>
void HashTable<T>::add(const T& item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) {
    table[hv] = new SList<T>;
    if (!table[hv]) return; 
  }
  for (SLIter<T> iter(table[hv]); iter.fValid(); ++iter)
    if (iter() == item) return;
  table[hv]->append(item);
}

template <class T>
void HashTable<T>::remove(const T& item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) return;
  for (SLIter<T> iter(table[hv]); iter.fValid(); ++iter)
    if (iter() == item) {
      iter.remove();
      return;
    }
}
/*
template <class T>
T HashTable<T>::get(T item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (table[hv]) {
    SLIter<T> iter(table[hv]);
    for ( ;iter.fValid(); ++iter)
      if (iter() == item) return iter();
  }
  return item;   // item not found, return it
}
*/
template <class T> T& HashTable<T>::get(T& item) const
{ int hv;
  hv = iHash(item) % iTabSize;
  if (table[hv]) {
    SLIter<T> iter(table[hv]);
    for ( ;iter.fValid(); ++iter)
      if (iter() == item) return iter();
  }
  return item;   // item not found, return it
}

template <class T>
bool HashTable<T>::fPresent(T& item) const
{ int hv;
  hv = iHash(item) % iTabSize;
  if (table[hv]) {
    SLIter<T> iter(table[hv]);
    for ( ;iter.fValid(); ++iter)
      if (iter() == item) return true;
  }
  return false;   // item not found return false;
}

template <class T>
void HashTable<T>::VisitAll(void(*FuncVisit)(T&))
{ for (int i = 0; i < iTabSize; ++i)
    if (table[i])
      for (SLIter<T> iter(table[i]); iter.fValid(); ++iter)
        FuncVisit(iter());
}


//--------------------- HashPTable definition

template <class T>
HashPTable<T>::HashPTable(int iSize)
{ 
  table = new SListP<T>*[iSize];
  if (!table) return; 
  memset(table, 0, iSize * sizeof(SListP<T>*));
  iTabSize = iSize;
}

template <class T>
HashPTable<T>::~HashPTable()
{ for (int i = 0; i < iTabSize; ++i)
    if (table[i]) delete table[i];
  delete [] table;
}

template <class T>
void HashPTable<T>::add(T item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) {
    table[hv] = new SListP<T>;
    if (!table[hv]) return; 
  }
  for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
    if (*iter() == item) return;
  table[hv]->append(&item);
}

template <class T>
void HashPTable<T>::remove(T item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) return;
  for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
    if (*iter() == item) {
      iter.remove();
      return;
    }
}

template <class T>
T HashPTable<T>::get(T item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (table[hv]) {
    for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
      if (*iter() == item) return *iter();
  }
  return item;
}

template <class T>
void HashPTable<T>::VisitAll(void(*FuncVisit)(T&))
{ for (int i = 0; i < iTabSize; ++i)
    if (table[i])
      for (SLIterP<T> iter(table[i]); iter.fValid(); ++iter)
        FuncVisit(*iter());
}

//--------------------- HashPPTable definition

template <class T>
HashPPTable<T>::HashPPTable(int iSize)
{ 
  table = new SListP<T>*[iSize];
  if (!table) return; 
  memset(table, 0, iSize * sizeof(SListP<T>*));
  iTabSize = iSize;
}


template <class T>
HashPPTable<T>::~HashPPTable()
{ for (int i = 0; i < iTabSize; ++i)
    if (table[i]) delete table[i];
  delete [] table;
}

template <class T>
void HashPPTable<T>::add(T* item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) {
    table[hv] = new SListP<T>;
    if (!table[hv]) return; 
  }
  for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
    if (*iter() == *item) return;
  table[hv]->append(item);
}

template <class T>
void HashPPTable<T>::remove(T* item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (!table[hv]) return;
  for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
    if (*iter() == *item) {
      iter.remove();
      return;
    }
}

template <class T>
T* HashPPTable<T>::get(T* item)
{ unsigned hv;
  hv = iHash(item) % iTabSize;
  if (table[hv]) {
    for (SLIterP<T> iter(table[hv]); iter.fValid(); ++iter)
      if (*iter() == *item)  return iter();
  }
  return item;
}

template <class T>
void HashPPTable<T>::VisitAll(void(*FuncVisit)(T&))
{ for (int i = 0; i < iTabSize; ++i)
    if (table[i])
      for (SLIterP<T> iter(table[i]); iter.fValid(); ++iter)
        FuncVisit(*iter());
}
/*
template <class T>
bool operator==(String* ps1, String* ps2) {
  return *ps1 == *ps2;
}
	Last change:  JEL   2 Jan 96   11:20 am
*/
#endif // ILWHASH_H







