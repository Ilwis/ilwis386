

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Mar 02 13:20:56 2010
 */
/* Compiler settings for ..\..\Sources\Engine\COMInterface\ICommandLine.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __ICommandLine_h_h__
#define __ICommandLine_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IILWIS_CommandLine_FWD_DEFINED__
#define __IILWIS_CommandLine_FWD_DEFINED__
typedef interface IILWIS_CommandLine IILWIS_CommandLine;
#endif 	/* __IILWIS_CommandLine_FWD_DEFINED__ */


#ifndef __ILWIS_CommandLine_FWD_DEFINED__
#define __ILWIS_CommandLine_FWD_DEFINED__

#ifdef __cplusplus
typedef class ILWIS_CommandLine ILWIS_CommandLine;
#else
typedef struct ILWIS_CommandLine ILWIS_CommandLine;
#endif /* __cplusplus */

#endif 	/* __ILWIS_CommandLine_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IILWIS_CommandLine_INTERFACE_DEFINED__
#define __IILWIS_CommandLine_INTERFACE_DEFINED__

/* interface IILWIS_CommandLine */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IILWIS_CommandLine;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0895F1BD-DF1C-411a-A352-3575319B592D")
    IILWIS_CommandLine : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ BSTR sCmd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IILWIS_CommandLineVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IILWIS_CommandLine * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IILWIS_CommandLine * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IILWIS_CommandLine * This);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            IILWIS_CommandLine * This,
            /* [in] */ BSTR sCmd);
        
        END_INTERFACE
    } IILWIS_CommandLineVtbl;

    interface IILWIS_CommandLine
    {
        CONST_VTBL struct IILWIS_CommandLineVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IILWIS_CommandLine_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IILWIS_CommandLine_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IILWIS_CommandLine_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IILWIS_CommandLine_Execute(This,sCmd)	\
    ( (This)->lpVtbl -> Execute(This,sCmd) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IILWIS_CommandLine_INTERFACE_DEFINED__ */



#ifndef __CommandLineLib_LIBRARY_DEFINED__
#define __CommandLineLib_LIBRARY_DEFINED__

/* library CommandLineLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_CommandLineLib;

EXTERN_C const CLSID CLSID_ILWIS_CommandLine;

#ifdef __cplusplus

class DECLSPEC_UUID("524BCEF4-4370-4021-92D3-CEC162577C08")
ILWIS_CommandLine;
#endif
#endif /* __CommandLineLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


