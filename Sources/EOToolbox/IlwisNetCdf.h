#pragma once

typedef int (*FNC_create)(const char *, int , int *);
typedef int (*FNC_def_dim)(int ncid, const char *name, size_t len, int *idp);
typedef int (*FNC_def_var)(int ncid, const char *name, nc_type xtype, int ndims, const int *dimidsp, int *varidp);
typedef int (*FNC_put_att_text)(int ncid, int varid, const char *name,size_t len, const char *op);
typedef int (*FNC_enddef)(int ncid);
typedef int (*FNC_put_var_double)(int ncid, int varid, const double *op);
typedef int (*FNC_put_var_float)(int ncid, int varid, const float *op);
typedef int (*FNC_put_var_int)(int ncid, int varid, const int *op);
typedef int (*FNC_close)(int ncid);
typedef const char * (*FNC_strerror)(int ncid);
typedef int (*FNC_put_vara_int)(int ncid, int varid, const size_t start[], const size_t count[], const int *ip);


class IlwisNetCdf {
public:
	IlwisNetCdf();

	void addMapList(const MapList& mpl);
	void addMap(const Map& mp);
	bool saveToNetCdf(const FileName& fn);

private:
	void makeRasterMapEntry(const IlwisObject& mp);
	void handleResult(int err);
	vector<IlwisObject> objects;
	int ncid;
	FileName fnNC;

	//functions
	static FNC_create fnc_create;
	static FNC_def_dim fnc_def_dim;
	static FNC_def_var fnc_def_var;
	static FNC_put_att_text fnc_put_att_text;
	static FNC_put_var_double fnc_put_var_double;
	static FNC_put_var_float fnc_put_var_float;
	static FNC_put_var_int fnc_put_var_int;
	static FNC_close fnc_close;
	static FNC_enddef fnc_enddef;
	static FNC_strerror fnc_strerror;
	static FNC_put_vara_int fnc_put_vara_int;

};