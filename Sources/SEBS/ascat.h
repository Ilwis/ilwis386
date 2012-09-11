/////////////////////////////////////////
// header file for ascat level 1b reader
/////////////////////////////////////////

/****************************
constants to identify
satellite and format
****************************/
#include "Engine\Applications\MAPFMAP.H"
#define METOP1 0
#define METOP2 1
#define METOP3 2
#define METOPN 3

#define SZO 0
#define SZR 1
#define SZF 2

/****************************

data structure to hold
node data

tm = epoch time in days
lon,lat = long & lat in deg
s0,s1,s2 = sigma0 in dB
t0,t1,t2 = inc angle in deg
a0,a1,a2 = az angle in deg
kp0,kp1,kp2 = noise
asc = ascending flag
index = node number
sat = satellite
fmt = format
fuse0,fuse1,fuse2 = usable flags
fsyn0, fsyn1, fsyn2 = synthetic data flags
fsynq0, fsynq1, fsynq2 = synthetic data quality flags
forb0, forb1, forb2 = orbit quality flags
fsol0, fsol1, fsol2 = solar array flags
ftel0, ftel1, ftel2 = telemetry flags
fext0, fext1, fext2 = extrapolated fn flags
fland0, fland1, fland2 = land flags

****************************/

typedef struct
{
 int sat,fmt,asc;
 int swath,index;
 double lon,lat;
 double tm,track;
 double s0,s1,s2;
 double t0,t1,t2;
 double a0,a1,a2;
 double kp0,kp1,kp2;
 int fkp0,fkp1,fkp2;
 int fuse0,fuse1,fuse2;
 double fsyn0,fsyn1,fsyn2;
 double fsynq0,fsynq1,fsynq2;
 double forb0,forb1,forb2;
 double fsol0,fsol1,fsol2;
 double ftel0,ftel1,ftel2;
 double fext0,fext1,fext2;
 double fland0,fland1,fland2;
}
ascat_node;

/****************************

class to read ascat level 1b
files and return node data

fp = file pointer
nn = number of nodes
nr = number of records
mdr = mdr read from file
mdr_size = size of mdr
mdr_id = subclass of mdr
index = record index of mdr
pixsize = pixel size of data
res = resolution of data
sat = satellite
fmt = format

****************************/

class ascat_file
{
 private:

 FILE *fp;
 int nn;
 int nr;
 unsigned char *mdr;
 int size;
 int id;
 int sat;
 int fmt;

 public:
 ascat_file();
 ~ascat_file();

 int open( string fname, int *nr, int *nn );
 // open a file for reading

 int read_mdr();
 // read an mdr from the file

 void get_node( int i, int swath, ascat_node *b );
 // return szo & szr node data

 int get_sat( );
 // return sat id

 int get_fmt( );
 // return format id

 int get_nr( );
 // return number of records

 int get_nn( );
 // return number of nodes

 void close( );
 // close the file

};

