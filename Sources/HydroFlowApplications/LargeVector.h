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
   This class was created to solve a memory issue with the hydroflow set of operations.
   It will attempt to fit an entire raster band in memory, for the fastest possible lookup.
   If this is not possible, the raster band is written to disk and paged from there.
*/
#ifndef LargeVector_H
#define LargeVector_H

template <class T>
class CachedRow
{
public:
	CachedRow();
	CachedRow(const CachedRow<T>& cr);
	virtual ~CachedRow();
	T & row();
	const T & row() const;
	const long index() const;
	void init(long index, long columns);
	void moveBefore(const long i, map<long,CachedRow<T>> & rows);
	void erase(map<long,CachedRow<T>> & rows);
	const long prev() const;
	void read(File * file);
	void write(File * file);
private:
	long m_index;
	long m_columns;
	T * m_row;
	long m_next;
	long m_prev;
};

template <class T>
class LargeVector
{
public:
	LargeVector();
	~LargeVector();
	void Open(long rows, long columns);
	void Close();
	T & operator[](long i);
private:
	const int iElementSize() const;
	enum StoreMethod {smVector, smHashMap}; // vector: fast but memory-greedy, hashmap: slow but swapped to disk when needed
	StoreMethod m_storeMethod;
	vector<T> m_vrows;
	map<long,CachedRow<T>> m_mrows;
	long mru_row;
	set<long> m_cached;
	unsigned long m_iMaxLines;
	long m_columns;
	File * m_file;
};

#endif 
