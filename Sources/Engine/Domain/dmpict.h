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
/* DomainPicture
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  JEL   8 Jul 97    3:27 pm
*/

#ifndef ILWDOMPICTURE_H
#define ILWDOMPICTURE_H
#include "Engine\Domain\dm.h"
#include "Engine\Representation\Rpr.h"

class DomainPicture: public DomainPtr
{
  friend class DomainPtr;
protected:
  virtual void Store();
  virtual bool fEqual(const IlwisObjectPtr& ptr) const;
public:
  DomainPicture(const FileName& fn);
  DomainPicture(const FileName& fn, int iNr);
	virtual ~DomainPicture();

  virtual String sType() const;
  virtual StoreType stNeeded() const;
  virtual String sValueByRaw(long iRaw, short iWidth=-1, short=-1) const;
  void Add(const Color& col);
  long _export iRaw(const Color& col) const;
	virtual long _export iRaw(const String& str) const;
  int iColors() const { return _iColors; }
  void _export Merge(const DomainPicture* dpic);
  void GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const;
  virtual bool _export fValid(const String&) const;
private:
  int _iColors;
  Representation _rpr;
};

#endif // ILWDOMPICTURE_H





