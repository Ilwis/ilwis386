#ifdef HAVE_PABLO 
#include "ProcIDs.h"
#else
#define HDF_TRACE_ON( eventID )
#define HDF_TRACE_OFF( eventID, p1, p2, p3 )
#define HDF_MASK_TRACE_ON( mask, eventID )
#define HDF_MASK_TRACE_OFF( mask, eventID, p1, p2, p3 )
#define TRACE_ON(mask, eventID)
#define TRACE_OFF(mask, eventID )
#endif /* HAVE_PABLO */
