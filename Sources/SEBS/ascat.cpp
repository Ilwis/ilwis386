#include "Engine\Domain\Dmvalue.h"
#include <iostream>
#include <string>
#include <stdio.h>
using namespace std;
#include "ascat.h"

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
#define SZO_ID    2
#define SZR_ID    1
#define SZF_ID    3

// record size

#define GRH_SIZE     20
#define MPHR_SIZE  3307
#define IPR_SIZE     27
#define SZO_SIZE   4018
#define SZR_SIZE   7818
#define SZF_SIZE  40324

// number of nodes

#define NODES_SZO   21
#define NODES_SZR   41
#define NODES_SZF  256

// throwpos function

#define THROW throwpos(__FILE__,__LINE__)

///////////////////////////////////
// function to throw a message
///////////////////////////////////

void throwpos( const char *file, int line )
{
 string msg;
 cerr << "throw: " << file << " " << line << endl;
 msg = file;
 throw msg;
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
 if( *a > 128 ) THROW;
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
 if( n <= 0 ) THROW;
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
 
 if( n >= 16 ) THROW;
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

 if( fread(x,1,size,fp) != size ) THROW;
 if( *x != id ) THROW;
 get_uint(x,4,&n);
 if( n != size ) THROW;
 return 0;
}

static int skip_rec( FILE *fp, char id )
{
 unsigned char x[GRH_SIZE];
 int n,size;

 if( fread(x,1,GRH_SIZE,fp) != GRH_SIZE ) THROW;
 if( *x != id ) THROW;
 get_uint(x,4,&n);
 if( n < GRH_SIZE ) THROW;
 n -= GRH_SIZE;
 if( fseek(fp,n,SEEK_CUR) ) THROW;
 return 0;
}

///////////////////////////////////
// misc methods
///////////////////////////////////

ascat_file::ascat_file()
{
 mdr = NULL;
 fp = NULL;
}

ascat_file::~ascat_file()
{
 if( mdr )
  free(mdr);
 if( fp )
  fclose(fp);
}

void ascat_file::close()
{
 if( fp )
  { fclose(fp); fp = NULL; }
 if( mdr )
  { free(mdr); mdr = NULL; }
}

int ascat_file::get_sat( )
{
 return sat;
}

int ascat_file::get_fmt( )
{
 return fmt;
}

int ascat_file::get_nn( )
{
 return nn;
}

int ascat_file::get_nr( )
{
 return nr;
}

///////////////////////////////////
// open file, read mphr and
// extract infor from mphr
///////////////////////////////////

int ascat_file::open( string fname, int *nr0, int *nn0 )
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

 if( read_rec(fp,MPHR_SIZE,MPHR_ID,x) ) THROW;

 if( get_str(x,552,4,&instr) ) THROW;
 if( get_str(x,625,3,&prod) ) THROW;
 if( get_str(x,696,3,&satid) ) THROW;
 if( get_num(x,2714,&num_mphr) ) THROW;
 if( get_num(x,2753,&num_sphr) ) THROW;
 if( get_num(x,2792,&num_ipr) ) THROW;
 if( get_num(x,2831,&num_geadr) ) THROW;
 if( get_num(x,2870,&num_giadr) ) THROW;
 if( get_num(x,2909,&num_veadr) ) THROW;
 if( get_num(x,2948,&num_viadr) ) THROW;
 if( get_num(x,2987,&num_mdr) ) THROW;

 // check

 if( num_mphr != 1 ) THROW;
 //if( num_sphr != 1 ) THROW; //from 2008 onwards
 //if( num_sphr != 0 ) THROW; //for 2007 files
 if( instr != "ASCA" ) THROW;

 if( prod == "SZO" )
  {
   fmt = SZO;
   nn = NODES_SZO;
   size = SZO_SIZE;
   id = SZO_ID;
  }
 else if( prod == "SZR" )
  {
   fmt = SZR;
   nn = NODES_SZR;
   size = SZR_SIZE;
   id = SZR_ID;
  }
 else
  THROW;

 // skip records

 for(i=0;i<num_sphr;i++)
  if( skip_rec(fp,SPHR_ID) ) THROW;

 for(i=0;i<num_ipr;i++)
  if( skip_rec(fp,IPR_ID) ) THROW;

 for(i=0;i<num_geadr;i++)
  if( skip_rec(fp,GEADR_ID) ) THROW;

 for(i=0;i<num_giadr;i++)
  if( skip_rec(fp,GIADR_ID) ) THROW;

 for(i=0;i<num_veadr;i++)
  if( skip_rec(fp,VEADR_ID) ) THROW;

 for(i=0;i<num_viadr;i++)
  if( skip_rec(fp,VIADR_ID) ) THROW;

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
 if( mdr == NULL ) THROW;
 return 0;
}

int ascat_file::read_mdr()
{
 int n;

 // read grh
 if( size < 20 ) THROW;
 if( fread(mdr,1,20,fp) != 20 ) THROW;
 if( *mdr != MDR_ID ) THROW;

 // test for dummy mdr
 if( *( mdr + 1 ) == 13 )
  {
   if( *( mdr + 2 ) != 1 ) THROW;
   get_uint(mdr,4,&n);
   if( n != 21 ) THROW;
   if( fread(mdr,1,1,fp) != 1 ) THROW;
   return 1;
  }
 if( *( mdr + 1 ) != ASCAT_ID ) THROW;
 if( *( mdr + 2 ) != id ) THROW;
 get_uint(mdr,4,&n);
 if( n != size ) THROW;

 // read rest of mdr
 n = size - 20;
 if( fread(mdr+20,1,n,fp) != n ) THROW;
 return 0;
}

///////////////////////////////////
// get node data
///////////////////////////////////

static void get_szo_node( unsigned char *x, int i, int swath, ascat_node *b )
{
 // check node index is ok

 if( i < 0 ) THROW;
 if( i >= NODES_SZO ) THROW;

 // use the node index and
 // swath value to calculate the
 // index of the data in the mdr

 if( swath )
  i += NODES_SZO;
 else
  i = NODES_SZO - 1 - i;

 // extract node data from mdr

 get_time( x, &b->tm );
 get_ushort( x, 26, 1.0e-2, &b->track );
 get_int( x, 154 + i*4, 1.0e-6, &b->lat );
 get_int( x, 322 + i*4, 1.0e-6, &b->lon );
 get_int( x, 742 + i*12, 1.0e-6, &b->s0, &b->s1, &b->s2 );
 get_ushort( x, 1246 + i*6, 1.0e-4, &b->kp0, &b->kp1, &b->kp2 );
 get_ushort( x, 1498 + i*6, 1.0e-2, &b->t0, &b->t1, &b->t2 );
 get_short( x, 1750 + i*6, 1.0e-2, &b->a0, &b->a1, &b->a2 );
 get_uchar( x, 2002 + i*3, &b->fkp0, &b->fkp1, &b->fkp2 );
 get_uchar( x, 2128 + i*3, &b->fuse0, &b->fuse1, &b->fuse2 );
 get_ushort( x, 2254 + i*6, 1.0e-3, &b->fsyn0, &b->fsyn1, &b->fsyn2 );
 get_ushort( x, 2506 + i*6, 1.0e-3, &b->fsynq0, &b->fsynq1, &b->fsynq2 );
 get_ushort( x, 2758 + i*6, 1.0e-3, &b->forb0, &b->forb1, &b->forb2 );
 get_ushort( x, 3010 + i*6, 1.0e-3, &b->fsol0, &b->fsol1, &b->fsol2 );
 get_ushort( x, 3262 + i*6, 1.0e-3, &b->ftel0, &b->ftel1, &b->ftel2 );
 get_ushort( x, 3514 + i*6, 1.0e-3, &b->fext0, &b->fext1, &b->fext2 );
 get_ushort( x, 3766 + i*6, 1.0e-3, &b->fland0, &b->fland1, &b->fland2 );
}

static void get_szr_node( unsigned char *x, int i, int swath, ascat_node *b )
{
 // check node index is ok

 if( i < 0 ) THROW;
 if( i >= NODES_SZR ) THROW;

 // use the node index and
 // swath value to calculate the
 // index of the data in the mdr

 if( swath )
  i += NODES_SZR;
 else
  i = NODES_SZR - 1 - i;

 // extract node data from mdr

 get_time( x, &b->tm );
 get_ushort( x, 26, 1.0e-2, &b->track );
 get_int( x, 274 + i*4, 1.0e-6, &b->lat );
 get_int( x, 602 + i*4, 1.0e-6, &b->lon );
 get_int( x, 1422 + i*12, 1.0e-6, &b->s0, &b->s1, &b->s2 );
 get_ushort( x, 2406 + i*6, 1.0e-4, &b->kp0, &b->kp1, &b->kp2 );
 get_ushort( x, 2898 + i*6, 1.0e-2, &b->t0, &b->t1, &b->t2 );
 get_short( x, 3390 + i*6, 1.0e-2, &b->a0, &b->a1, &b->a2 );
 get_uchar( x, 3882 + i*3, &b->fkp0, &b->fkp1, &b->fkp2 );
 get_uchar( x, 4128 + i*3, &b->fuse0, &b->fuse1, &b->fuse2 );
 get_ushort( x, 4374 + i*6, 1.0e-3, &b->fsyn0, &b->fsyn1, &b->fsyn2 );
 get_ushort( x, 4866 + i*6, 1.0e-3, &b->fsynq0, &b->fsynq1, &b->fsynq2 );
 get_ushort( x, 5358 + i*6, 1.0e-3, &b->forb0, &b->forb1, &b->forb2 );
 get_ushort( x, 5850 + i*6, 1.0e-3, &b->fsol0, &b->fsol1, &b->fsol2 );
 get_ushort( x, 6342 + i*6, 1.0e-3, &b->ftel0, &b->ftel1, &b->ftel2 );
 get_ushort( x, 6834 + i*6, 1.0e-3, &b->fext0, &b->fext1, &b->fext2 );
 get_ushort( x, 7326 + i*6, 1.0e-3, &b->fland0, &b->fland1, &b->fland2 );
}

void ascat_file::get_node( int i, int swath, ascat_node *b )
{
 switch( nn )
  {
   case NODES_SZO :
    get_szo_node(mdr,i,swath,b);
    break;
   case NODES_SZR :
    get_szr_node(mdr,i,swath,b);
    break;
   default :
    THROW;
  }
 b->sat = sat;
 b->fmt = fmt;
 b->index = i;
 b->swath = swath;
 b->asc = calc_asc( b->track );
}
