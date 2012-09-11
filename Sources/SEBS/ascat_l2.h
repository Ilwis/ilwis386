/////////////////////////////////////////
// header file for ascat level 12 reader
// ASAC_som_02 product 
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

#define SMO 0
#define SMR 1
#define SMF 2
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
 double fland0,fland1,fland2;
 double sm, sm_error;
 double sigma40,sigma40_error;
 double slope40,slope40_error;
 double sm_sensitivity, dry_backscatter,wet_backscatter;
 double sm_mean;
 int fprocessing,fcorrection;
 int frainfall,fquality,fsnow,ffrozen,fwetland,ftcomplexity;
}
ascat_node;

/****************************

class to read ascat level 2
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

class ascat_l2_file
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

 ascat_l2_file();

 ~ascat_l2_file();

 int check( string fname, string *ans );
 // basic check that file is ok

 int open( string fname, int *nr, int *nn );
 // open a file for reading

 void read_mdr();
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

