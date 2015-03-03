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

#ifndef ILWRPR_H
#define ILWRPR_H
#ifndef ILWDOMAIN_H
#error include dm.h instead of rpr.h
#endif

//class DATEXPORT RepresentationAttribute;
class RepresentationClass;
class RepresentationGradual;
class RepresentationValue;

struct _export RepresentationProperties {
	RepresentationProperties();
	RepresentationProperties(const RepresentationProperties& p);
	RepresentationProperties& operator=(const RepresentationProperties& p);
	double linewidth;
	int lineType;
	double itemAlpha;
	String symbolType;
	double symbolSize;
	String hatchName;
};

class RepresentationPtr: public IlwisObjectPtr
{
friend class Representation;

public:
  virtual ~RepresentationPtr();

  virtual void          Store();
  virtual void          GetColors(ColorBuf&) const;
  virtual void          GetColorLut(ColorBuf&) const;
  virtual void          GetPattern(long iRaw, short aPat[8]) const;
  virtual void          Rename(const FileName& fnNew);
  static void _export   GetPatternNames(Array<String>&);
  void                  SetClrBoundary(Color clr) 
                           { if (clrBoundary() != clr) { clrBoundaryColor = clr; fChanged = true; }}
  void                  SetWidthBoundary(int iW) 
                           { if (iWidthBoundary() != iW) { iBoundaryWidth = iW; fChanged = true; }}
  virtual String        sSymbolFont(long iRaw) const;
  virtual String        sType() const;
  virtual String        sName(bool fExt = false, const String& sDirRelative = "") const;
  virtual String        sNameQuoted(bool fExt = false, const String& sDirRelative = "") const;
  virtual String        sObjectSection() const;
  static String         sPattern(const byte aPat[8]);
  const Domain&         dm() const 
                          { return _dm; }

  virtual Color _export clrRaw(long iRaw) const;
  virtual Color         clrSecondRaw(long iRaw) const;
  virtual Color         clrSymbol(long iRaw) const;
  virtual Color         clrSymbolFill(long iRaw) const;
  virtual Color         clrLineFill(long iRaw) const;
  Color _export         clr(double rValue) const;            // 0..1
  virtual Color _export clr(double rValue, RangeReal) const; // stretch
  Color                 clrBoundary() const 
                          { return clrBoundaryColor; } 
  virtual int          iColor(double rValue) const;  // 0..1 
  virtual int          iColor(long iValue, RangeInt) const;
  virtual int          iColor(double rValue, RangeReal) const;
  virtual short         iPattern(long iRaw) const;
  virtual short         iSymbolType(long iRaw) const;
  virtual short         iSymbolSize(long iRaw) const;
  virtual short         iLine(long iRaw) const;
  virtual short         iSymbolWidth(long iRaw) const;
  static short _export *pPattern(const String&);
  virtual double        rSymbolRotation(long iRaw) const;
  virtual double        rLineWidth(long iRaw) const;
  virtual double        rLineDist(long iRaw) const;
  int                   iWidthBoundary() const { return iBoundaryWidth; }
  virtual               DomainInfo dminf() const;
  RepresentationClass _export   *prc() const;
  RepresentationGradual _export *prg() const;
  RepresentationValue _export   *prv() const;
	void									GetObjectStructure(ObjectStructure& os);
	virtual void getProperties(long iRaw, RepresentationProperties& props);

protected:
  _export RepresentationPtr(const FileName&);
  _export RepresentationPtr(const FileName&, const Domain& dom);

  Domain                _dm;
  Color                 clrBoundaryColor;
  int                   iBoundaryWidth;

private:
  static RepresentationPtr* create(const FileName& fn);
  static RepresentationPtr* create(const FileName& fn, const Domain& dm, bool fCreateGradual = true);
  static RepresentationPtr* create(const String& sExpression);
};

class Representation: public IlwisObject
{
public:
    _export Representation();
    _export Representation(const FileName&);
    _export Representation(const FileName&, const Domain&, bool fCreateGradual = true);
    _export Representation(const Representation&);
    Representation(const Representation&, const Table& tblAttr);
    _export Representation(const String& sExpression);
    
    void                        operator = (const Representation& rpr) 
                                    { SetPointer(rpr.pointer()); }
    RepresentationPtr*          ptr() const 
                                    { return static_cast<RepresentationPtr*>(pointer()); }
    RepresentationPtr*          operator -> () const 
                                    { return ptr(); }
    static RepresentationPtr*   pGet(const FileName& fn);
    static Domain               dmGet(const FileName& fn);
    static bool                 fHasDomainValue(const FileName& fn); 
    static bool                 fHasSameDomain(const FileName& fn, const Domain& dmCompare); 
	static Color				_export clrPrimary(int iNr, int set=0) ;

private:
	static void loadColorSets(const String& folder="");
	static void parseFile(const FileName& fn);
    static IlwisObjectPtrList   listRpr;
	static map< int, vector<Color> > colorSets;
};

#endif // ILWRPR_H




