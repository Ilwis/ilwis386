
#include "Engine\Domain\Dmvalue.h"
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
#include "ascat_l2.h"

///////////////////////////////////
// useful definitions
///////////////////////////////////

// record id

#define MPHR_ID   1
#define SPHR_ID   2
#define IPR_ID    3
#define GEADR_ID  4
#define GIADR_ID  5
#define VEADR_ID  6
#define VIADR_ID  7
#define MDR_ID    8
#define ASCAT_ID  2
#define SMR_ID    1
#define SMO_ID    2

// record size

#define GRH_SIZE     20
#define MPHR_SIZE  3307
#define IPR_SIZE     27
#define SMO_SIZE   4064
#define SMR_SIZE   7904

// number of nodes

#define NODES_SMO   21
#define NODES_SMR   41
//#define NODES_SZF  256

// throwpos function

#define THROW_ASCAT_L2 throwpos2(__FILE__,__LINE__)

///////////////////////////////////
// function to throw a message
///////////////////////////////////

void throwpos2( const char *file, int line )
{
 string msg;
 cerr << "throw: " << file << " " << line << endl;
 msg = file;
// throw msg;
}

///////////////////////////////////
// calc ascending flag from track
///////////////////////////////////

static int calc_asc( double track )
{
 if( track < 0.0 ) track += 360.0;
 if( track>90.0 && track<270.0 )
  return 0;
 return 1;
}

///////////////////////////////////
// extract integers from mdr
///////////////////////////////////

static void get_char( unsigned char *x, int pos, int *a )
{
 x += pos;
 *a = *x;
 if( *a >= 128 ) *a -= 256;
}

static void get_ushort( unsigned char *x, int pos, int *a )
{
 x += pos;
 *a = *x;
 x++;
 *a = *a*256 + (*x);
}

static void get_short( unsigned char *x, int pos, int *a )
{
 x += pos;
 *a = *x;
 if( *a >= 128 ) *a -= 256;
 x++;
 *a = *a*256 + (*x);
}

static void get_uint( unsigned char *x, int pos, int *a )
{
 x += pos;
 *a = *x;
 if( *a > 128 ) THROW_ASCAT_L2; //throw ErrorImportExport(SCVErrUnknownConversion);
 x++;
 *a = *a*256 + (*x);
 x++;
 *a = *a*256 + (*x);
 x++;
 *a = *a*256 + (*x);
}

static void get_int( unsigned char *x, int pos, int *a )
{
 x += pos;
 *a = *x;
 if( *a > 128 ) *a -= 256;
 x++;
 *a = *a*256 + (*x);
 x++;
 *a = *a*256 + (*x);
 x++;
 *a = *a*256 + (*x);
}

static void get_uchar( unsigned char *x, int pos, int *a, int *b, int *c )
{
 x += pos;
 *a = *x;
 x++;
 *b = *x;
 x++;
 *c = *x;
}
static void get_uchar( unsigned char *x, int pos, int *a)
{
 x += pos;
 *a = *x;
}


///////////////////////////////////
// extract doubles from mdr
///////////////////////////////////

static void get_time( unsigned char *x, double *t )
{
 int day,msec,sec;

 get_ushort(x,20,&day);
 get_uint(x,22,&msec);
 sec = msec/1000;
 *t = day + sec/86400.0;
}

static void get_ushort( unsigned char *x, int pos, double sf, double *ans )
{
 int a;

 get_ushort(x,pos,&a);
 *ans = a*sf;
}

static void get_int( unsigned char *x, int pos, double sf, double *ans )
{
 int a;

 get_int(x,pos,&a);
 *ans = a*sf;
}

static void get_ushort( unsigned char *x, int pos, double sf, double *a, double *b, double *c )
{
 int ans;

 get_ushort(x,pos,&ans);
 *a = ans*sf;
 get_ushort(x,pos+2,&ans);
 *b = ans*sf;
 get_ushort(x,pos+4,&ans);
 *c = ans*sf;
}

static void get_short( unsigned char *x, int pos, double sf, double *a, double *b, double *c )
{
 int ans;

 get_short(x,pos,&ans);
 *a = ans*sf;
 get_short(x,pos+2,&ans);
 *b = ans*sf;
 get_short(x,pos+4,&ans);
 *c = ans*sf;
}

static void get_int( unsigned char *x, int pos, double sf, double *a, double *b, double *c )
{
 int ans;

 get_int(x,pos,&ans);
 *a = ans*sf;
 get_int(x,pos+4,&ans);
 *b = ans*sf;
 get_int(x,pos+8,&ans);
 *c = ans*sf;
}

///////////////////////////////////
// extract string data from mdr
///////////////////////////////////

static void get_str( unsigned char *x, int n, char *a )
{
 if( n <= 0 ) THROW_ASCAT_L2;
 while( n )
  {
   *a = (char)( *x );
   x++;
   a++;
   n--;
  }
 *a = '\0';
}

static int get_str( unsigned char *x, int pos, int n, string *ans )
{
 char s[16];
 
 if( n >= 16 ) THROW_ASCAT_L2;
 x += pos;
 get_str(x,n,s);
 *ans = s;
 return 0;
}

static int get_num( unsigned char *x, int pos, int *ans )
{
 char s[16];

 x += pos;
 get_str(x,6,s);
 if( sscanf(s,"%i",ans) != 1 ) return 1;
 if( *ans < 0 ) return 1;
 return 0;
}

///////////////////////////////////
// record handling
///////////////////////////////////

static int read_rec( FILE *fp, int size, char id, unsigned char *x )
{
 int n;

 if( fread(x,1,size,fp) != size ) THROW_ASCAT_L2; //throw ErrorImportExport(SCVErrGifCode); should be like this!!!
 if( *x != id ) THROW_ASCAT_L2;
 get_uint(x,4,&n);
 if( n != size ) THROW_ASCAT_L2;
 return 0;
}

static int skip_rec( FILE *fp, char id )
{
 unsigned char x[GRH_SIZE];
 int n,size;

 if( fread(x,1,GRH_SIZE,fp) != GRH_SIZE ) THROW_ASCAT_L2;
// if( *x != id ) THROW_ASCAT_L2;
 get_uint(x,4,&n);
 if( n < GRH_SIZE ) THROW_ASCAT_L2;
 n -= GRH_SIZE;
 if( fseek(fp,n,SEEK_CUR) ) THROW_ASCAT_L2;
 return 0;
}

///////////////////////////////////
// misc methods
///////////////////////////////////

ascat_l2_file::ascat_l2_file()
{
 mdr = NULL;
 fp = NULL;
}

ascat_l2_file::~ascat_l2_file()
{
 if( mdr )
  free(mdr);
 if( fp )
  fclose(fp);
}

void ascat_l2_file::close()
{
 if( fp )
  { fclose(fp); fp = NULL; }
 if( mdr )
  { free(mdr); mdr = NULL; }
}

int ascat_l2_file::get_sat( )
{
 return sat;
}

int ascat_l2_file::get_fmt( )
{
 return fmt;
}

int ascat_l2_file::get_nn( )
{
 return nn;
}

int ascat_l2_file::get_nr( )
{
 return nr;
}

///////////////////////////////////
// check file format and return a
// string describing any problems
///////////////////////////////////

#define RETURN(x) { *ans = x; if(fp0) fclose(fp0); return 1; }

int ascat_l2_file::check( string fname, string *ans )
{
 string instr,prod;
 unsigned char x[MPHR_SIZE];
 int num_mphr,num_sphr,skip;
 int num_geadr,num_giadr;
 int num_veadr,num_viadr;
 int num_ipr,num_mdr;
 int rec_size,i,n;
 int size0,id0;
 FILE *fp0;

 *ans = "ok";
 fp0 = NULL;

 // check file

 fp0 = fopen(fname.c_str(),"rb");
 //if (fopen_s(&fp0, fname.c_str(),"rb") != 0) THROW;

 if( fp0 == NULL )
  RETURN("non existent file");

 // mphr

 if( read_rec(fp0,MPHR_SIZE,MPHR_ID,x) )
    RETURN("bad mphr");

 if( get_str(x,552,4,&instr) )
   RETURN("bad instr string");
 if( get_str(x,625,3,&prod) )
   RETURN("bad prod string");
 if( get_num(x,2714,&num_mphr) )
   RETURN("bad num_mphr");
 if( get_num(x,2753,&num_sphr) )
   RETURN("bad num_sphr");
 if( get_num(x,2792,&num_ipr) )
   RETURN("bad num_ipr");
 if( get_num(x,2831,&num_geadr) )
   RETURN("bad num_geadr");
 if( get_num(x,2870,&num_giadr) )
   RETURN("bad num_giadr");
 if( get_num(x,2909,&num_veadr) )
   RETURN("bad num_veadr");
 if( get_num(x,2948,&num_viadr) )
   RETURN("bad num_viadr");
 if( get_num(x,2987,&num_mdr) )
   RETURN("bad num_mdr");

 // check
 
 if( num_mphr != 1 )
   RETURN("bad num_mphr value");
 if( num_sphr != 0 )
   RETURN("bad num_sphr value");
 if( instr != "ASCA" )
   RETURN("bad instr value");

 
 if( prod == "SMO" )
  {
   size0 = SMO_SIZE;
   id0 = SMO_ID;
  }
 else if( prod == "SMR" )
  {
   size0 = SMR_SIZE;
   id0 = SMR_ID;
  }
 else
   RETURN("bad prod value");

 // ipr

 for(i=0;i<num_ipr;i++)
  if( skip_rec(fp0,IPR_ID) )
   RETURN("bad ipr");

 // geadr 

 for(i=0;i<num_geadr;i++)
  if( skip_rec(fp0,GEADR_ID) )
   RETURN("bad geadr");

 // giadr 

 for(i=0;i<num_giadr;i++)
  if( skip_rec(fp0,GIADR_ID) )
   RETURN("bad giadr");

 // veadr 

 for(i=0;i<num_veadr;i++)
  if( skip_rec(fp0,VEADR_ID) )
   RETURN("bad veadr");

 // viadr 

 for(i=0;i<num_viadr;i++)
  if( skip_rec(fp0,VIADR_ID) )
   RETURN("bad viadr");

 // mdr 

 skip = size0 - GRH_SIZE;
 for(i=0;i<num_mdr;i++)
  {
   if( fread(x,1,GRH_SIZE,fp0) != GRH_SIZE )
    RETURN("mdr fread");

   if( *x != MDR_ID )
    RETURN("mdr id");
 
   if( *( x + 1 ) != ASCAT_ID )
    RETURN("mdr not gome");

   if( *( x + 2 ) != id0 )
    RETURN("mdr - unexpected subclass");
    
   get_uint(x,4,&n);
   if( n != size0 )
    RETURN("mdr - unexpected record size");
    
   if( fseek(fp0,skip,SEEK_CUR) )
    RETURN("mdr fseek");
  }

 fclose(fp0);
 return 0;
}

///////////////////////////////////
// open file, read mphr and
// extract infor from mphr
///////////////////////////////////

int ascat_l2_file::open(string fname, int *nr0, int *nn0 )
{
 unsigned char x[MPHR_SIZE];
 string instr,prod,satid;
 int num_mphr,num_sphr;
 int num_geadr,num_giadr;
 int num_veadr,num_viadr;
 int num_ipr,num_mdr;
 int rec_size,i;

 // tidy old data

 if( mdr )
  { free(mdr); mdr = NULL; }
 if( fp )
  { fclose(fp); fp = NULL; }

 // open
 fp = fopen(fname.c_str(),"rb");
 if( fp == NULL ) return 1;

 // mphr

 if( read_rec(fp,MPHR_SIZE,MPHR_ID,x) ) THROW_ASCAT_L2;

 if( get_str(x,552,4,&instr) ) THROW_ASCAT_L2;
 if( get_str(x,625,3,&prod) ) THROW_ASCAT_L2;
 if( get_str(x,696,3,&satid) ) THROW_ASCAT_L2;
 if( get_num(x,2714,&num_mphr) ) THROW_ASCAT_L2;
 if( get_num(x,2753,&num_sphr) ) THROW_ASCAT_L2;
 if( get_num(x,2792,&num_ipr) ) THROW_ASCAT_L2;
 if( get_num(x,2831,&num_geadr) ) THROW_ASCAT_L2;
 if( get_num(x,2870,&num_giadr) ) THROW_ASCAT_L2;
 if( get_num(x,2909,&num_veadr) ) THROW_ASCAT_L2;
 if( get_num(x,2948,&num_viadr) ) THROW_ASCAT_L2;
 if( get_num(x,2987,&num_mdr) ) THROW_ASCAT_L2;

 // check

 if( num_mphr != 1 ) THROW_ASCAT_L2;
 if( num_sphr != 0 ) THROW_ASCAT_L2;
 if( instr != "ASCA" ) THROW_ASCAT_L2;

  if( prod == "SMO" )
  {
   fmt = SMO;
   nn = NODES_SMO;
   size = SMO_SIZE;
   id = SMO_ID;
  }
 else if( prod == "SMR" )
  {
   fmt = SMR;
   nn = NODES_SMR;
   size = SMR_SIZE;
   id = SMR_ID;
  }
 else
  THROW_ASCAT_L2;

 // skip records

 for(i=0;i<num_ipr;i++)
  if( skip_rec(fp,IPR_ID) ) THROW_ASCAT_L2;

 for(i=0;i<num_geadr;i++)
  if( skip_rec(fp,GEADR_ID) ) THROW_ASCAT_L2;

 for(i=0;i<num_giadr;i++)
  if( skip_rec(fp,GIADR_ID) ) THROW_ASCAT_L2;

 for(i=0;i<num_veadr;i++)
  if( skip_rec(fp,VEADR_ID) ) THROW_ASCAT_L2;

 for(i=0;i<num_viadr;i++)
  if( skip_rec(fp,VIADR_ID) ) THROW_ASCAT_L2;

 // misc

 if( satid == "M01" )
  sat = METOP1;
 else if( satid == "M02" )
  sat = METOP2;
 else if( satid == "M03" )
  sat = METOP3;
 else
  sat = METOPN;

 *nn0 = nn;
 *nr0 = num_mdr;
 nr = num_mdr;
 mdr = (unsigned char*)malloc( sizeof(unsigned char)*size );
 if( mdr == NULL ) THROW_ASCAT_L2;
 return 0;
}

void ascat_l2_file::read_mdr()
{
 int n;

 if( fread(mdr,1,size,fp) != size ) THROW_ASCAT_L2;
 if( *mdr != MDR_ID ) THROW_ASCAT_L2;
 if( *( mdr + 1 ) != ASCAT_ID ) THROW_ASCAT_L2;
 if( *( mdr + 2 ) != id ) THROW_ASCAT_L2;
 get_uint(mdr,4,&n);
 if( n != size ) THROW_ASCAT_L2;
}

///////////////////////////////////
// get node data
///////////////////////////////////

static void get_smo_node( unsigned char *x, int i, int swath, ascat_node *b )
{
 // check node index is ok

 if( i < 0 ) THROW_ASCAT_L2;
 if( i >= NODES_SMO ) THROW_ASCAT_L2;

 // use the node index and
 // swath value to calculate the
 // index of the data in the mdr

 if( swath )
  i += NODES_SMO;
 else
  i = NODES_SMO - 1 - i;

 // extract node data from mdr

 get_time( x, &b->tm );
 get_ushort( x, 26, 1.0e-2, &b->track );
 get_int( x, 154 + i*4, 1.0e-6, &b->lat );
 get_int( x, 322 + i*4, 1.0e-6, &b->lon );
 get_int( x, 490 + i*12, 1.0e-6, &b->s0, &b->s1, &b->s2 );
 get_ushort( x, 994 + i*6, 1.0e-4, &b->kp0, &b->kp1, &b->kp2 );
 get_ushort( x, 1246 + i*6, 1.0e-2, &b->t0, &b->t1, &b->t2 );
 get_short( x, 1498 + i*6, 1.0e-2, &b->a0, &b->a1, &b->a2 );
 get_uchar( x, 1750 + i*3, &b->fkp0, &b->fkp1, &b->fkp2 );
 get_uchar( x, 1876 + i*3, &b->fuse0, &b->fuse1, &b->fuse2 );
 get_ushort( x, 2002 + i*6, 1.0e-3, &b->fland0, &b->fland1, &b->fland2 );
 get_ushort( x, 2258 + i*2, 1.0e-2, &b->sm);
 get_ushort( x, 2342 + i*2, 1.0e-2, &b->sm_error);
 get_int( x, 2426 + i*4, 1.0e-6, &b->sigma40);
 get_int( x, 2594 + i*4, 1.0e-6, &b->sigma40_error);
 get_int( x, 2762 + i*4, 1.0e-6, &b->slope40);
 get_int( x, 2930 + i*4, 1.0e-6, &b->sigma40_error);
 //get_uint( x, 6018 + i*4, 1.0e-6, &b->sm_sensitivity);
 get_int( x, 3266 + i*4, 1.0e-6, &b->dry_backscatter);
 get_int( x, 3434 + i*4, 1.0e-6, &b->wet_backscatter);
 get_ushort( x, 3602 + i*2, 1, &b->sm_mean);
 get_uchar( x, 3686 + i, &b->frainfall);
 get_uchar( x, 3728 + i, &b->fcorrection);
 get_ushort(x, 3770 + i * 2, &b->fprocessing);
 get_uchar( x, 3854 + i, &b->fquality);
 get_uchar( x, 3896 + i, &b->fsnow);
 get_uchar( x, 3938 + i, &b->ffrozen);
 get_uchar( x, 3980 + i, &b->fwetland);
 get_uchar( x, 4022 + i, &b->ftcomplexity);
}

static void get_smr_node( unsigned char *x, int i, int swath, ascat_node *b )
{
 // check node index is ok

 if( i < 0 ) THROW_ASCAT_L2;
 if( i >= NODES_SMR ) THROW_ASCAT_L2;

 // use the node index and
 // swath value to calculate the
 // index of the data in the mdr

 if( swath )
  i += NODES_SMR;
 else
  i = NODES_SMR - 1 - i;

 // extract node data from mdr

 get_time( x, &b->tm );
 get_ushort( x, 26, 1.0e-2, &b->track );
 get_int( x, 274 + i*4, 1.0e-6, &b->lat );
 get_int( x, 602 + i*4, 1.0e-6, &b->lon );
 get_int( x, 930 + i*12, 1.0e-6, &b->s0, &b->s1, &b->s2 );
 get_ushort( x, 1914 + i*6, 1.0e-4, &b->kp0, &b->kp1, &b->kp2 );
 get_ushort( x, 2404 + i*6, 1.0e-2, &b->t0, &b->t1, &b->t2 );
 get_short( x, 2898 + i*6, 1.0e-2, &b->a0, &b->a1, &b->a2 );
 get_uchar( x, 3390 + i*3, &b->fkp0, &b->fkp1, &b->fkp2 );
 get_uchar( x, 3636 + i*3, &b->fuse0, &b->fuse1, &b->fuse2 );
 get_ushort( x, 3882 + i*6, 1.0e-3, &b->fland0, &b->fland1, &b->fland2 );
 get_ushort( x, 4378 + i*2, 1.0e-2, &b->sm);
 get_ushort( x, 4542 + i*2, 1.0e-2, &b->sm_error);
 get_int( x, 4706 + i*4, 1.0e-6, &b->sigma40);
 get_int( x, 5034 + i*4, 1.0e-6, &b->sigma40_error);
 get_int( x, 5362 + i*4, 1.0e-6, &b->slope40);
 get_int( x, 5690 + i*4, 1.0e-6, &b->sigma40_error);
 //get_uint( x, 6018 + i*4, 1.0e-6, &b->sm_sensitivity);
 get_int( x, 6346 + i*4, 1.0e-6, &b->dry_backscatter);
 get_int( x, 6674 + i*4, 1.0e-6, &b->wet_backscatter);
 get_ushort( x, 7002 + i*2, 1.0e-2, &b->sm_mean);
 get_uchar( x, 7166 + i, &b->frainfall);
 get_uchar( x, 7248 + i, &b->fcorrection);
 get_ushort(x,7330 + i*2,&b->fprocessing);
 get_uchar( x, 7494 + i,&b->fquality);
 get_uchar( x, 7576 + i,&b->fsnow);
 get_uchar( x, 7658 + i,&b->ffrozen);
 get_uchar( x, 7740 + i,&b->fwetland);
 get_uchar( x, 7822 + i,&b->ftcomplexity);
}

void ascat_l2_file::get_node( int i, int swath, ascat_node *b )
{
 switch( nn )
  {
   case NODES_SMO :
    get_smo_node(mdr,i,swath,b);
    break;
   case NODES_SMR :
    get_smr_node(mdr,i,swath,b);
    break;
   default :
    THROW_ASCAT_L2;
  }
 b->sat = sat;
 b->fmt = fmt;
 b->index = i;
 b->swath = swath;
 b->asc = calc_asc( b->track );
}
