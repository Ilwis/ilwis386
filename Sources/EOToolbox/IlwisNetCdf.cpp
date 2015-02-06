#include "headers\Toolspch.h"
#include "Engine\base\system\engine.h"
#include "Engine\Map\Raster\MapList\maplist.h"
#include "netcdf.h"
#include "IlwisNetCdf.h"


FNC_create IlwisNetCdf::fnc_create=0;
FNC_def_dim IlwisNetCdf::fnc_def_dim=0;
FNC_def_var IlwisNetCdf::fnc_def_var=0;
FNC_put_att_text IlwisNetCdf::fnc_put_att_text=0;
FNC_put_var_double IlwisNetCdf::fnc_put_var_double=0;
FNC_put_var_float IlwisNetCdf::fnc_put_var_float=0;
FNC_put_var_int IlwisNetCdf::fnc_put_var_int=0;
FNC_put_vara_int IlwisNetCdf::fnc_put_vara_int=0;
FNC_close IlwisNetCdf::fnc_close=0;
FNC_enddef IlwisNetCdf::fnc_enddef=0;
FNC_strerror IlwisNetCdf::fnc_strerror=0;


IlwisNetCdf::IlwisNetCdf() : ncid(iUNDEF){
	if ( fnc_create == 0) {
		String base = getEngine()->getContext()->sIlwDir() + "Extensions\\EO-Toolbox\\";
		HMODULE hh = LoadLibrary(String(base + "szip.dll").c_str());
		hh = LoadLibrary(String(base + "hdf5dll.dll").c_str());
		hh = LoadLibrary(String(base + "hdf5_hldll.dll").c_str());
		hh = LoadLibrary(String(base + "netcdf.dll").c_str());

		fnc_create = (FNC_create)GetProcAddress(hh, "nc_create");
		fnc_def_dim = (FNC_def_dim)GetProcAddress(hh, "nc_def_dim");
		fnc_def_var = (FNC_def_var)GetProcAddress(hh, "nc_def_var");
		fnc_put_att_text = (FNC_put_att_text)GetProcAddress(hh, "nc_put_att_text");
		fnc_put_var_double = (FNC_put_var_double)GetProcAddress(hh, "nc_put_var_double");
		fnc_put_var_float = (FNC_put_var_float)GetProcAddress(hh, "nc_put_var_float");
		fnc_put_var_int = (FNC_put_var_int)GetProcAddress(hh, "nc_put_var_int");
		fnc_close = (FNC_close)GetProcAddress(hh, "nc_close");
		fnc_enddef = (FNC_enddef)GetProcAddress(hh, "nc_enddef");
		fnc_strerror = (FNC_strerror)GetProcAddress(hh, "nc_strerror");
		fnc_put_vara_int = (FNC_put_vara_int)GetProcAddress(hh,"nc_put_vara_int");
	}
}

void IlwisNetCdf::addMap(const Map& mp) {
	if ( mp.fValid())
		objects.push_back(mp);
}

void IlwisNetCdf::addMapList(const MapList& mpl) {
	if ( mpl.fValid())
		objects.push_back(mpl);
}

bool IlwisNetCdf::saveToNetCdf(const FileName& fn) {
	fnNC = fn;
	for(int i = 0; i < objects.size(); ++i) {
		if ( IOTYPE(objects[i]->fnObj) == IlwisObject::iotRASMAP) {
			makeRasterMapEntry(objects[i]);
		}
	}
	return true;
}

void IlwisNetCdf::handleResult(int err) {
	if ( err!= 0) {
		if ( ncid != iUNDEF)
			fnc_close(ncid);
		throw ErrorObject(fnc_strerror(err));
	}
}


void IlwisNetCdf::makeRasterMapEntry(const IlwisObject &obj) {
	if ( ncid == iUNDEF) {
		handleResult(fnc_create(fnNC.sFullPath().c_str(), NC_CLOBBER, &ncid));
	}
	Map mp;
	mp.SetPointer(obj.pointer());
	int rows = mp->rcSize().Row;
	int cols = mp->rcSize().Col;
	float *crdsX=0, *crdsY=0;
	int *vals;

	int y_dimid, x_dimid, val_id, y_varid, x_varid, val_varid;

	try{
	handleResult(fnc_def_dim(ncid, "level", 1, &val_id));
	handleResult(fnc_def_dim(ncid, "lat", rows, &y_dimid));
	handleResult(fnc_def_dim(ncid, "lon", cols, &x_dimid));

	handleResult(fnc_def_var(ncid, "lat", NC_FLOAT, 1, &y_dimid, &y_varid));
	handleResult(fnc_def_var(ncid, "lon", NC_FLOAT, 1, &x_dimid, &x_varid));

	handleResult(fnc_put_att_text(ncid, x_varid, "units" , strlen("degrees east"), "degrees east"));
	handleResult(fnc_put_att_text(ncid, y_varid, "units" , strlen("degress north"), "degress north"));

	int dimids[3];
	dimids[0] = val_id;
	dimids[1] = y_dimid;
	dimids[2] = x_dimid;

	handleResult(fnc_def_var(ncid, "value", NC_INT, 3, dimids, &val_varid));

	handleResult(fnc_enddef(ncid));


	float *crdsY = new float[rows];
	for(int y = 0; y < rows; ++y) {
		Coord c = mp->gr()->cConv(RowCol(y,0));
		//yx ll = mp->cs()->llConv(c);
		crdsY[y] = c.y;
	}

	float *crdsX = new float[cols];
	for(int x = 0; x < cols; ++x) {
		Coord c = mp->gr()->cConv(RowCol(0,x));
		//yx ll = mp->cs()->llConv(c);
		crdsX[x] = c.x;
	}

	handleResult(fnc_put_var_float(ncid, y_varid, crdsY));
	handleResult(fnc_put_var_float(ncid, x_varid, crdsX));


	size_t start[3], count[3];

    count[0] = 1;
    count[1] = rows;
    count[2] = cols;
	start[0] = 0;
    start[1] = 0;
    start[2] = 0;

	if ( mp->dvrs().fUseReals()) {

	} else {
		vals = new int[rows * cols];
		for(int y = 0; y < rows; ++y) {
			LongBuf buf(cols);
			mp->GetLineVal(y,buf);
			for( int x = 0; x < cols; ++x) {
				vals[x + y * cols] = buf[x];
			}
		}
		handleResult(fnc_put_vara_int(ncid, val_varid, start, count,vals));
		//handleResult(fnc_put_var_int(ncid, val_varid, vals));
		delete [] vals;
	}

	handleResult(fnc_close(ncid));

	delete [] crdsX;
	delete [] crdsY;
	}
	catch(const ErrorObject& err) {
		if ( crdsX)	delete [] crdsX;
		if ( crdsY) delete [] crdsY;
		if ( vals) delete [] vals;
		throw err;
	}


}
//void IlwisNetCdf::makeRasterMapEntry(const IlwisObject &obj) {
//	Map mp;
//	mp.SetPointer(obj.pointer());
//	int rows = mp->rcSize().Row;
//	int cols = mp->rcSize().Col;
//	if ( netCdfId == iUNDEF) {
//		handleResult(fnc_create(fnNC.sFullPath().c_str(), NC_CLOBBER, &netCdfId));
//	}
//
//	double *crdsY = new double[rows];
//	for(int y = 0; y < rows; ++y) {
//		crdsY[y] = mp->gr()->cConv(RowCol(y,0)).y;
//	}
//
//	double *crdsX = new double[cols];
//	for(int x = 0; x < cols; ++x) {
//		crdsX[x] = mp->gr()->cConv(RowCol(0,x)).x;
//	}
//
//    int dimids[2]; 
//	String unit1, unit2;
//	String xname, yname;
//	int x_dimid, y_dimid, y_varid, x_varid, val_varid;
//	if ( mp->cs()->pcsyx()) {
//		xname = "xgitude";
//		yname = "yitude";
//		unit1 = "degrees_north";
//		unit2 = "degrees_east";
//	} else {
//		xname = "x";
//		yname = "y";
//		unit1 = "meter";
//		unit2 = "meter";
//	}
//
//
//	handleResult(fnc_def_dim(netCdfId, yname.c_str(), rows, &y_dimid));
//	handleResult(fnc_def_dim(netCdfId, xname.c_str(), cols, &x_dimid));
//	handleResult(fnc_def_var(netCdfId, yname.c_str(), NC_DOUBLE, 1, &y_dimid, &y_varid));
//	handleResult(fnc_def_var(netCdfId, xname.c_str(), NC_DOUBLE, 1, &x_dimid, &x_varid));
//
//	handleResult(fnc_put_att_text(netCdfId, y_varid, "units" , unit1.size(), unit1.c_str()));
//	handleResult(fnc_put_att_text(netCdfId, x_varid, "units" , unit2.size(), unit2.c_str()));
//	dimids[0] = y_dimid;
//	dimids[1] = x_dimid; 
//
//
//	if ( mp->dvrs().fUseReals())
//		handleResult(fnc_def_var(netCdfId, "value", NC_DOUBLE, 2, dimids, &val_varid));
//	else
//		handleResult(fnc_def_var(netCdfId, "value", NC_INT, 2, dimids, &val_varid));
//
//	handleResult(fnc_enddef(netCdfId));
//
//	handleResult(fnc_put_var_double(netCdfId, y_varid, crdsY));
//	handleResult(fnc_put_var_double(netCdfId, x_varid, crdsX));
//
//	if ( mp->dvrs().fUseReals()) {
//		double ** vals = new double*[rows];
//		for(int y=0; y < rows; ++y) {
//			vals[y] = new double[cols];
//			RealBuf buf;
//			mp->GetLineVal(y,buf);
//			for(int x = 0; x < cols; ++x) {
//				vals[y][x] = buf[x];
//			}
//		}
//		handleResult(fnc_put_var_double(netCdfId, val_varid, &vals[0][0]));
//		for(int y=0; y < rows; ++y) {
//			delete [] vals[y];
//		}
//		delete [] vals;
//	} else {
//		int *vals = new int[rows * cols];
//		for(int y=0; y < rows; ++y) {
//			xgBuf buf(cols);
//			mp->GetLineVal(y,buf);
//			for(int x = 0; x < cols; ++x) {
//				vals[x + y * rows] = buf[x];
//			}
//		}
//		handleResult(fnc_put_var_int(netCdfId, val_varid, vals));
//		delete [] vals;
//
//	}
//
//	handleResult(fnc_close(netCdfId));
//
//	delete [] crdsX;
//	delete [] crdsY;
//}




