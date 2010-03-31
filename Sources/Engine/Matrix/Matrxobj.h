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
/* $Log: /ILWIS 3.0/Matrix/Matrxobj.h $
 * 
 * 6     30-10-01 13:08 Koolhoven
 * made PrincCompPtr a friend of MatrixObjectPtr 
 * 
 * 5     16-08-01 19:04 Koolhoven
 * PrincipalComponents and FactorAnalysis are now virtual MatrixObjects
 * 
 * 4     8-09-00 4:25p Martin
 * added GetObjectStructure and DoNotUpdate function
 * 
 * 3     14-12-99 12:48 Koolhoven
 * Header comment
 * 
 * 2     14-12-99 11:58 Koolhoven
 * Created MatrixWindow
// Revision 1.3  1998/09/16 17:22:46  Wim
// 22beta2
//
// Revision 1.2  1997/08/12 16:35:13  Wim
// Added fDependent()
//
/* Matrix, MatrixPtr
   Copyright Ilwis System Development ITC
   march 1996, by Jelle Wind
	Last change:  WK   12 Aug 98   12:22 pm
*/

#ifndef ILWMATRIXOBJ_H
#define ILWMATRIXOBJ_H

#ifndef ILW_OBJDEP_H
#include "Engine\Base\objdepen.h"
#endif

#include "Engine\Base\Algorithm\Realmat.h"



class MatrixObjectVirtual;


class MatrixObjectPtr: public IlwisObjectPtr
{
  friend class MatrixObject;
  friend class MatrixObjectVirtual;
  friend class PrincCompPtr;
public:
  int iRows() const { return mat.iRows(); }
  int iCols() const { return mat.iCols(); }
  virtual ~MatrixObjectPtr();
  virtual String sType() const;
  virtual void Store();
  virtual void GetRowText(Array<String>& as) const;
  virtual void GetColumnText(Array<String>& as) const;
  virtual void MakeUsable();
  virtual bool fUsable() const;
  virtual String sValue(int iRow, int iCol, int iWidth) const;
  virtual int iWidth() const;
  virtual void Rename(const FileName& fnNew);
	virtual void _export GetObjectStructure(ObjectStructure& os);
	virtual void _export DoNotUpdate();	

  virtual bool _export fDependent() const; 
  virtual bool _export fCalculated() const; 
  virtual void _export Calc(bool fMakeUpToDate=false); 
  virtual void _export DeleteCalc(); 
  virtual void _export BreakDependency(); 
  virtual _export String sExpression() const;
	virtual IlwisObjectVirtual *pGetVirtualObject() const;
  
  RealMatrix mat;
protected:
  MatrixObjectPtr(const FileName&);
  MatrixObjectPtr(const FileName& fn, int iRows, int iCols);
  MatrixObjectPtr(const FileName&, const String& sExpression);
private:  
  static MatrixObjectPtr* create(const FileName&);
  static MatrixObjectPtr* create(const FileName&, const String&);
  MatrixObjectVirtual* pmov;
  ObjectDependency objdep;
};

class MatrixObject: public IlwisObject
{
  static IlwisObjectPtrList listMat;
public:
  _export MatrixObject();
  _export MatrixObject(const FileName& fn);
  MatrixObject(const FileName& fn, int iRows, int iCols);
  _export MatrixObject(const FileName& fn, const String& sExpression);
  MatrixObject(const String& sExpression, const String& sPath);
  _export MatrixObject(const MatrixObject& mat);
  void operator = (const MatrixObject& mat) { SetPointer(mat.pointer()); }
  MatrixObjectPtr* ptr() const { return static_cast<MatrixObjectPtr*>(pointer()); }
  MatrixObjectPtr* operator -> () const { return ptr(); }
  static MatrixObjectPtr* pGet(const FileName& fn);
};

#endif




