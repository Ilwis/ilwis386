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
/* tls/set.h
// Interface for Sets for ILWIS 2.0
// dec. 1993, by Jelle Wind
// (c) Computer Department ITC
Last change:  WK   28 Aug 98    4:43 pm
*/
#ifndef ILWSET_H
#define ILWSET_H

#undef IMPEXP
#ifdef ILWISENGINE
#define IMPEXP __export
#else
#define IMPEXP __import
#endif

class IMPEXP  SetBaseInt
{
protected:
	SetBaseInt() {}
	virtual ~SetBaseInt() {}
public:
	virtual bool fContains(long) const = 0;  // returns true if value in set
};

class IMPEXP SetBaseReal
{
protected:
	SetBaseReal() {}
	virtual ~SetBaseReal() {}
public:
	virtual bool fContains(double) const = 0;
};

class IMPEXP SetBaseString
{
protected:
	SetBaseString() {}
	virtual ~SetBaseString() {}
public:
	virtual bool fContains(const String&) const = 0;
};

class IMPEXP RangeInt : public SetBaseInt
{
public:
	RangeInt()
	{ _iLo = 0; _iHi = -1; }
	RangeInt(long iLo, long iHi) : SetBaseInt()
		// constructor for range iLo .. iHi
	{ _iLo = iLo; _iHi = iHi; }
	RangeInt(const RangeInt& rng) {
		_iLo = rng.iLo(); _iHi = rng.iHi();
	}
	RangeInt(const String& s);
	// constructor for s with  syntax:   %li:%li
	~RangeInt() {}
	bool fContains(long iVal) const  // iVal is in range ?
	{ return (iVal >= _iLo) && (iVal <= _iHi); }
	bool fContains(const RangeInt& ri)
	{ return fContains(ri.iLo()) && fContains(ri.iHi()); }
	RangeInt& operator+=( const RangeInt& ri) {
		_iLo = min(_iLo,ri.iLo());
		_iHi = max(_iHi, ri.iHi());
		return *this;
	}
	long& iLo() { return _iLo; }        // return lower boundary
	long& iHi() { return _iHi; }        // return upper boundary
	long iLo() const { return _iLo; }   // return lower boundary
	long iHi() const { return _iHi; }   // return upper boundary
	long iWidth() const { return max(iHi() - iLo(), 1); }
	bool operator ==(const RangeInt& rng) const
	{ return (iLo() == rng.iLo()) && (iHi() == rng.iHi()); }
	bool operator !=(const RangeInt& rng) const
	{ return !(*this == rng); }
	String s() const { return String("%li:%li", iLo(), iHi()); }
	bool fValid() const { return iHi() != iUNDEF && iLo() != iUNDEF && iHi() >= iLo(); }
protected:
	long _iLo, _iHi;
};

class IMPEXP RangeReal : public SetBaseReal
{
public:
	RangeReal()
	{ _rLo = 0; _rHi = -1; _fLo = _fHi = true; }
	RangeReal(double rLo, double rHi,
		bool fLo = true, bool fHi = true) : SetBaseReal()
		// constructor for range rLo .. rHi
		// fLo and fHi : indicate if range is inclusive the boundaries
	{ _rLo = rLo; _rHi = rHi; _fLo = fLo; _fHi = fHi; }
	// The following copy constructor is commented out for the time being
	// Originally it was incorrect (_fLo and _fHi weren't copied)
	// The default copy constructor does exactly what we need (copying all members)
	/*
	RangeReal(const RangeReal& rng) {
	_rLo = rng.rLo(); _rHi = rng.rHi(); _fLo = rng._fLo; _fHi = rng._fHi;
	}
	*/
	RangeReal(const String& s);
	// constructor for s with  syntax:   %g:%g

	~RangeReal() {}
	bool fContains(double rVal) const  // rVal is in range ?
	{ return ( _fLo ? rVal >= _rLo - 1e-6 : rVal > _rLo) &&
	( _fHi ? rVal <= _rHi + 1e-6 : rVal < _rHi);
	}
	RangeReal& operator+=( const RangeReal& ri) {
		_rLo = min(_rLo,ri.rLo());
		_rHi = max(_rHi, ri.rHi());
		return *this;
	}

	RangeReal& operator+=( double v) {
		if ( v == rUNDEF || v == iUNDEF)
			return *this;

		if ( !fValid()) {
			_rLo = _rHi = v;
		}
		else if ( v > _rHi)
			_rHi = v;
		else if ( v < _rLo)
			_rLo = v;
		return *this;
	}
	bool fContains(const RangeReal& rr)
	{ return fContains(rr.rLo()) && fContains(rr.rHi()); } 	
	double& rLo() { return _rLo; }       // return lower boundary
	double& rHi() { return _rHi; }       // return upper boundary
	double rLo() const { return _rLo; }  // return lower boundary
	double rHi() const { return _rHi; }  // return upper boundary
	double rWidth() const { return rHi() - rLo(); }
	bool operator ==(const RangeReal& rng) const // the inclusive/exclusive should also match
	{ return (rLo() == rng.rLo()) && (rHi() == rng.rHi()) && (_fLo == rng._fLo) && (_fHi == rng._fHi); }
	bool operator !=(const RangeReal& rng) const
	{ return !(*this == rng); }
	String s() const { return String("%g:%g", rLo(), rHi()); }
	bool fValid() const { return rHi() != rUNDEF && rLo() != rUNDEF && rHi() >= rLo(); }
protected:
	double _rLo, _rHi;
	bool _fLo, _fHi;  // true if range is inclusive lower / upper boundary
};

//template class IMPEXP Array<RangeInt>;
//template class IMPEXP Array<RangeReal>;

#endif // ILWSET_H







