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
/* BaseMap, BaseMapPtr
   Copyright Ilwis System Development ITC
   march 1995, by Wim Koolhoven
	Last change:  WK   20 Aug 97    6:27 pm
*/

#ifndef ILWBASEMAP_H
#define ILWBASEMAP_H
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\ERR.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Table\tbl.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Base\objdepen.h"
#include "Engine\Base\DataObjects\Tranq.h"

class Column;
class Feature;

class BaseMapPtr: public IlwisObjectPtr
{
friend class BaseMap;
friend class BaseMapVirtual;
friend class MapVirtual;
friend class ObjectInfo;
friend class ObjectDependency;

#define CPOLYGON(x) dynamic_cast<ILWIS::Polygon *>(x)
#define CSEGMENT(x) dynamic_cast<ILWIS::Segment *>(x)
#define CPOINT(x)	dynamic_cast<ILWIS::Point *>(x)
#define CFEATURE(x)	dynamic_cast<Feature *>(x)

public:
	enum MinMaxMethod{mmmNOCALCULATE,mmmCALCULATE, mmmSAMPLED};
    virtual _export           ~BaseMapPtr();
	
    virtual void _export Updated();
    const                CoordSystem& cs() const 
							{ return _csys; }
    const                CoordBounds& cb() const 
							{ return cbOuter; }
    const Domain&        dm() const 
							{ return dvs.dm(); }
    virtual vector<long>     	viRaw(const Coord&, double rPrx=rUNDEF) const;
    virtual long		     	iRaw(const Coord&, double rPrx=rUNDEF) const;
    virtual vector<long>     	viValue(const Coord&, double rPrx=rUNDEF) const;
    virtual long				iValue(const Coord&, double rPrx=rUNDEF) const;
    virtual vector<double>   	vrValue(const Coord&, double rPrx=rUNDEF) const;
    virtual double				rValue(const Coord&, double rPrx=rUNDEF) const;
    virtual vector<String>   	vsValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
    virtual String			 	sValue(const Coord&, short iWidth=-1, short iDec=-1, double rPrx=rUNDEF) const;
    virtual void _export 		Store();
    virtual String       		sType() const;
    virtual void _export 		SetDomainValueRangeStruct(const DomainValueRangeStruct&);
    virtual void _export 		SetValueRange(const ValueRange&); // dangerous function!
    virtual void _export 		SetCoordSystem(const CoordSystem&); // dangerous function!
    void _export         		SetCoordBounds(const CoordBounds&); // dangerous function!
    virtual void _export 		KeepOpen(bool f =true);
    virtual bool _export 		fConvertTo(const DomainValueRangeStruct& dvrs);
    virtual bool _export 		fConvertTo(const Column& col);
    virtual bool         		fConvertTo(const DomainValueRangeStruct& dvrs, const Column& col);
    Table _export        		tblAtt();
    void _export         		SetAttributeTable(const Table& tbl);
    void _export         		SetNoAttributeTable();
    String _export       		sTblAtt() const;
    bool _export         		fTblAtt();
    bool                 		fTblAttSelf() 
								{ return _fAttTable; }
    virtual StoreType    		st() const
								{ return dvs.st(); }
    bool                 		fUseReals() const     
								{ return dvs.fUseReals(); }
    bool                 		fRealValues() const   
								{ return dvs.fRealValues(); }
    bool                 		fRawAvailable() const 
								{ return dvs.fRawAvailable(); }
    bool                 		fRawIsValue() const   
								{ return dvs.fRawIsValue(); }
    bool                 		fValues() const       
								{ return dvs.fValues(); }
	RangeInt _export     		riMinMax(MinMaxMethod m=mmmNOCALCULATE);
    RangeReal _export    		rrMinMax(MinMaxMethod m=mmmNOCALCULATE);
    RangeInt  _export    		riPerc1(bool fForce=false);
    RangeReal _export    		rrPerc1(bool fForce=false);
    void _export         		SetMinMax(const RangeInt& ri);
    void _export         		SetPerc1(const RangeInt& ri);
    void _export         		SetMinMax(const RangeReal& rr);
    void _export         		SetPerc1(const RangeReal& rr);
    virtual void _export GetDataFiles(Array<FileName>& afnDat, Array<String>* asSection, Array<String>* asEntry) const;
    const DomainValueRangeStruct& dvrs() const 
									{ return dvs; }
    const ValueRange _export      &vr()  
									{ return dvs.vr(); }
    virtual DomainInfo _export    dminf() const;
	CoordBounds _export  cbGetCoordBounds() const;
    bool fErrorMap() const          { return fnErrMap.fValid(); }
    FileName fnErrorMap() const     { return fnErrMap; }
    void SetErrorMap(const FileName& fnErrorMap) { fnErrMap = fnErrorMap; }
    double rProximity() const       { return rProx; }
    void SetProximity(double rPrx)  { rProx = rPrx; }
	void _export GetObjectStructure(ObjectStructure& os);
	bool _export fDataReadOnly() const;
	virtual long _export iFeatures() const { return iUNDEF; } ;
	virtual Geometry *getFeature(long i) const { return NULL;}
	virtual vector<Geometry *> getFeatures(Coord crd, double rPrx=rUNDEF) { return vector<Geometry *>(); }
	Geometry *getTransformedFeature(long iRec, const CoordSystem& csy) const	{ return NULL;}
	virtual Feature *newFeature(geos::geom::Geometry *pnt=NULL) { return NULL; }
	virtual void removeFeature(const String& id, const vector<int>& selectedCoords=vector<int>()) {}
	
protected:
    _export BaseMapPtr();
    _export BaseMapPtr(const FileName&, bool fCreate);
    _export BaseMapPtr(const FileName& fn, const CoordSystem& cs, const CoordBounds& cb, 
						const DomainValueRangeStruct& dvs);
	
	virtual void         CalcMinMax();
	virtual RangeReal _export   rrMinMaxSampled() { return rrMinMax(mmmCALCULATE);} // overrule this when sampling makes sense
	
    RangeReal            _rrMinMax;  // for DomainValueReal
    RangeReal            _rrPerc1;
    RangeInt             _riMinMax; 
    RangeInt             _riPerc1;   // for DomainValueInt
    bool                 fKeepOpen;
    ObjectDependency     objdep;
    CoordSystem          _csys;
    CoordBounds          cbOuter;
    DomainValueRangeStruct      dvs;
	
private:
    static BaseMapPtr* create(const FileName&);
	
    Table                tblAttLoad();
    Table                _tblAtt;
    bool                 _fAttTable;
    String               sAttTable;
    double rProxDefault() const;
    FileName fnErrMap;
    double rProx;
};

class BaseMap: public IlwisObject
{
    public:
        _export BaseMap();
        _export BaseMap(const FileName& filename);
        _export BaseMap(const BaseMap& mp);

        void                         operator=(const BaseMap& mp) 
                                       { SetPointer(mp.pointer()); }
        BaseMapPtr*                  ptr() const 
                                       { return static_cast<BaseMapPtr*>(pointer()); }
        BaseMapPtr*                  operator->() const 
                                       { return ptr(); }
        static BaseMapPtr*           pGet(const FileName& fn);
        static SegmentMapPtr _export *pGetSegMap(const FileName& fn);
        static PolygonMapPtr _export *pGetPolMap(const FileName& fn);
        static PointMapPtr _export   *pGetPntMap(const FileName& fn);
        static MapPtr _export        *pGetRasMap(const FileName& fn);

    protected:
        _export BaseMap(IlwisObjectPtr* ptr);

    private:
        static IlwisObjectPtrList    listMap;
};

#define ISBASEMAP(type) (type==IlwisObject::iotRASMAP || type==IlwisObject::iotPOINTMAP || type==IlwisObject::iotPOLYGONMAP || type==IlwisObject::iotSEGMENTMAP)

#endif





