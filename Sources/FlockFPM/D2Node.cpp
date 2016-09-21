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
Bas Retsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

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
// D2Node.cpp : implementation file
//
//////////////////////////////////////////////////////////////////////

//D2 Tree node class

#include "D2Node.h"

D2Node::D2Node(const Key & key, Disk * val)
: k(key)
, data(val)
, left(0)
, right(0)
, deleted (val == 0)
{
}

D2Node::~D2Node()
{
	if (data)
		delete data;
	if (left)
		delete left;
	if (right)
		delete right;
}

// Method ins translated from 352.ins.c of Gonnet & Baeza-Yates
bool D2Node::insert(const Key & key, Disk * value, const int lev)
{
	D2Node * next_node = 0;
	int next_lev = (lev + 1) % 2;
	if (key.keys[0] == k.keys[0] && key.keys[1] == k.keys[1]) { // equals
		if (deleted || data == 0) {
			if (data)
				delete data;
			data = value;
		} else
			return false;				
		if ((data == 0) == deleted)
			return true;
		else if (deleted) {
			deleted = false;
			return true;
		} else {
			deleted = true;
			return true;
		}
	} else if (key.keys[lev] > k.keys[lev]) {
		next_node = right;
		if (next_node == 0) {
			right = new D2Node(key, value);
			return true;
		}
	} else {
		next_node = left;
		if (next_node == 0) {
			left = new D2Node(key, value);
			return true;
		}
	}

	return next_node->insert(key, value, next_lev);
}

void D2Node::del(const Key & key, const int level) {
	if (!deleted && key.keys[0] == k.keys[0] && key.keys[1] == k.keys[1] ) // equals
		deleted = true;
	else if (key.keys[level] > k.keys[level]) {
		if (right != 0)
			right->del(key, (level + 1) % 2);
	} else {
		if (left != 0)
			left->del(key, (level + 1) % 2);
	}
}

// Method search translated from 352.range.c of Gonnet & Baeza-Yates
void D2Node::search(const Key & lowk, const Key & uppk, const int level, vector<Disk*> & v) {
	if ((lowk.keys[level] <= k.keys[level]) && (left != 0))
		left->search(lowk, uppk, (level + 1) % 2, v);
	if (!deleted) {
		int j = 0;
		while ((j < 2) && (lowk.keys[j] <= k.keys[j]) && (uppk.keys[j] >= k.keys[j]))
			j++;
		if (j == 2)
			v.push_back(data);
	}
	if ((uppk.keys[level] > k.keys[level]) && (right != 0))
		right->search(lowk, uppk, (level + 1) % 2, v);
}
