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

 Created on: 2018-02-14
 ***************************************************************/
/* LargeVector
   Copyright Ilwis System Development ITC
   February 2018, by Bas Retsios
*/

#include "Headers\toolspch.h"
#include "LargeVector.h"

template <class T>
CachedRow<T>::CachedRow()
: m_index(-1)
, m_columns(0)
, m_row(new T())
, m_next(-1)
, m_prev(-1)
{
}

template <class T>
CachedRow<T>::CachedRow(const CachedRow<T>& cr)
: m_index(cr.m_index)
, m_columns(cr.m_columns)
, m_row(new T(*(cr.m_row)))
, m_next(cr.m_next)
, m_prev(cr.m_prev)
{
}

template <class T>
CachedRow<T>::~CachedRow()
{
	delete m_row;
}

template <class T>
void CachedRow<T>::init(long index, long columns)
{
	m_index = index;
	m_columns = columns;
}

template <class T>
const long CachedRow<T>::index() const
{
	return m_index;
}

template <class T>
const T & CachedRow<T>::row() const
{
	return *m_row;
}

template <class T>
T & CachedRow<T>::row()
{
	return *m_row;
}

template <class T>
void CachedRow<T>::moveBefore(const long i, map<long,CachedRow<T>> & rows)
{
	// link the m_prev and m_next elements of the chain with eachother
	if (m_prev != -1) {
		CachedRow<T> & cr_prev = rows[m_prev];
		cr_prev.m_next = m_next;
	}
	if (m_next != -1) {
		CachedRow<T> & cr_next = rows[m_next];
		cr_next.m_prev = m_prev;
	}
	// insert ourselves before "cr"
	if (m_index != i) {
		CachedRow<T> & cr_i = rows[i];
		m_next = i;
		m_prev = cr_i.m_prev;
		cr_i.m_prev = m_index;
		if (m_prev != -1) {
			CachedRow<T> & cr_prev = rows[m_prev];
			cr_prev.m_next = m_index;
		} else { // list has only 2 elements; finish making the list circular
			m_prev = i;
			cr_i.m_next = m_index;
		}
	}
}

template <class T>
void CachedRow<T>::erase(map<long,CachedRow<T>> & rows)
{
	// link the m_prev and m_next elements of the chain with eachother
	if (m_prev != -1) {
		CachedRow<T> & cr_prev = rows[m_prev];
		cr_prev.m_next = m_next;
	}
	if (m_next != -1) {
		CachedRow<T> & cr_next = rows[m_next];
		cr_next.m_prev = m_prev;
	}
}

template <class T>
const long CachedRow<T>::prev() const
{
	return m_prev;
}

template <>
void CachedRow<RealBuf>::read(File * file)
{
	m_row->Size(m_columns);
	file->Seek((ULONGLONG)m_index * m_columns * sizeof(double));
	file->Read(sizeof(double) * m_columns, m_row->buf());
}

template <>
void CachedRow<LongBuf>::read(File * file)
{
	m_row->Size(m_columns);
	file->Seek((ULONGLONG)m_index * m_columns * sizeof(long));
	file->Read(sizeof(long) * m_columns, m_row->buf());
}

template <>
void CachedRow<ByteBuf>::read(File * file)
{
	m_row->Size(m_columns);
	file->Seek((ULONGLONG)m_index * m_columns);
	file->Read(m_columns, m_row->buf());
}

template <>
void CachedRow<RealBuf>::write(File * file)
{
	file->Seek((ULONGLONG)m_index * m_columns * sizeof(double));
	file->Write(sizeof(double) * m_columns, m_row->buf());
}

template <>
void CachedRow<LongBuf>::write(File * file)
{
	file->Seek((ULONGLONG)m_index * m_columns * sizeof(long));
	file->Write(sizeof(long) * m_columns, m_row->buf());
}

template <>
void CachedRow<ByteBuf>::write(File * file)
{
	file->Seek((ULONGLONG)m_index * m_columns);
	file->Write(m_columns, m_row->buf());
}

/* ######################## 
*/

template <class T>
LargeVector<T>::LargeVector()
: mru_row(-1)
, m_file(0)
, m_storeMethod(smVector)
{
}

template <class T>
LargeVector<T>::~LargeVector()
{
	Close();
}

template <>
const int LargeVector<RealBuf>::iElementSize() const
{
	return sizeof(double);
}

template <>
const int LargeVector<LongBuf>::iElementSize() const
{
	return sizeof(long);
}

template <>
const int LargeVector<ByteBuf>::iElementSize() const
{
	return sizeof(byte);
}

template <class T>
void LargeVector<T>::Open(long rows, long columns)
{
	const unsigned long long iMaxByteSize = 600 * 1024 * 1024; // Max allowed: 600MB. Typically a hydroflow application will allocate 3 to 4 of these blocks
	if ((long long)rows * columns * iElementSize() > iMaxByteSize) {
		m_storeMethod = smHashMap;
		FileName fn ("hydroflow_application.tmp");
		fn = FileName::fnUnique(fn);
		m_iMaxLines = iMaxByteSize / ((long long)columns * iElementSize());
		m_columns = columns;
		m_file = new File(fn, (FileAccess)(facCRT | CFile::osRandomAccess));
		m_file->SetErase();
		m_file->KeepOpen(true); 
	} else {
		m_storeMethod = smVector;
		m_vrows.resize(rows);
	}
}

template <class T>
void LargeVector<T>::Close()
{
	if (m_file) {
		delete m_file;
		m_file = 0;
	}
	m_vrows.clear();
	m_mrows.clear();
}

template <class T>
T & LargeVector<T>::operator[](long i)
{
	if (m_storeMethod == smVector) {
		return m_vrows[i];
	} else {
		map<long, CachedRow<T>>::iterator elem = m_mrows.find(i);
		if (elem != m_mrows.end()) { // hit
			if (mru_row != -1 && mru_row != i)
				elem->second.moveBefore(mru_row, m_mrows);
			mru_row = i;
			return m_mrows[i].row();
		} else { // miss
			if (m_mrows.size() > m_iMaxLines) { // cache is full; remove least recently used element
				long last = m_mrows[mru_row].prev();
				if (last != -1) {
					CachedRow<T> & cr_last = m_mrows[last];
					cr_last.write(m_file);
					m_cached.insert(last);
					cr_last.erase(m_mrows);
					m_mrows.erase(last);
				}
			}
			// create a new empty element (buffersize == 0)
			CachedRow<T> & cr = m_mrows[i]; // allocate a new element in the map, and get its reference
			cr.init(i, m_columns);
			if (m_cached.find(i) != m_cached.end()) // element was cached before; read it back from file (buffersize == m_columns)
				cr.read(m_file);
			if (mru_row != -1 && mru_row != i)
				cr.moveBefore(mru_row, m_mrows);
			mru_row = i;
			return cr.row();
		}
	}
}

template class LargeVector<RealBuf>;
template class LargeVector<LongBuf>;
template class LargeVector<ByteBuf>;