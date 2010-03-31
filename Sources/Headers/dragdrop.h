#ifndef UNDOCWIN_DRAGROP_H
#define UNDOCWIN_DRAGROP_H

/*
DRAGDROP.H -- USER-related Drag and Drop protocol

from "Undocumented Windows" by Schulman et al. (Addison-Wesley, 1992)
Chapter 6: USER

Copyright (c) Dave Maxey 1992
*/

#ifdef __cplusplus
extern "C" {
#endif


/* === UNDOCUMENTED MESSAGES === */

#define WM_DROPOBJECT			0x022A
#define WM_QUERYDROPOBJECT		0x022B

#define WM_BEGINDRAG			0x022C
#define WM_DRAGLOOP				0x022D

#define WM_DRAGSELECT			0x022E
#define WM_DRAGMOVE				0x022F

#ifndef WM_DROPFILES
#define WM_DROPFILES			0x0233
#endif


/* === UNDOCUMENTED CONSTANTS === */

// Used in the wFlags field of the DRAGINFO sructure
#define DRAGOBJ_PROGRAM			0x0001	// A single executable
#define DRAGOBJ_DATA			0x0002	// A single 'other' file
#define DRAGOBJ_DIRECTORY		0x0003	// A single pathed directory
#define DRAGOBJ_MULTIPLE		0x0004	// Any combination 2 or more
#define DRAGOBJ_EXTERNAL		0x8000	// ORed with one of the above

// Used as the return to the source (caller of DragObject)
#define DRAG_PRNT 0x544e5250
#define DRAG_FILE 0x454c4946

/* === UNDOCUMENTED STRUCTURE === */

// Structure pointed at by lParam of WM_DRAGDROP
typedef struct {
	HWND hwndSource;	// File Manager window that owns source listbox
	HANDLE h1;			// Handle of drag icon ???
	WORD wFlags;		// One of the DRAGOBJ_ constants below. They
						// include a top bit set, indicating drag
						// is allowed outside source app (FileMan).
	near char *szList;	// near pointer to list of files.
	HANDLE hOfstruct;	// handle to a global OFSTRUCT. If a list, or
						// file has no association, FileMan sets
						// this to NULL.
	int	x, y;			// Position of cursor at drop
	long lUnknown;		// ????
	} UDDRAGINFO, FAR * LPUDDRAGINFO;


// Structure in global memory - handle in wParam of WM_DROPFILES in 3.1
typedef struct {
	WORD wOfsFirst;		// Offset of the first filename in the block
	WORD xDrop;			// X coordinate of the drop point
	WORD yDrop;			// Y coordinate of the drop point
	BOOL bClient;		// 1 if Dropped on client area, 0 if NonClient
	char chBuffer[1];	// Buffer with null-terminated list
						//  of null-terminated strings
	} DROPINFO, FAR * LPDROPINFO;


#ifdef __cplusplus
}
#endif

#endif // UNDOCWIN_DRAGROP_H




