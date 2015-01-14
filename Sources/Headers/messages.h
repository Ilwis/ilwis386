#ifndef MESSAGES_H
#define MESSAGES_H

#define ID_ILWISSTART  (WM_USER + 200)
#define ID_ILWISREP    (WM_USER + 201)
#define ID_TOROOT      (WM_USER + 202)
#define ID_ILWISCMDLINE (WM_USER + 203)

// ID_ILWISREP codes:
#define ILWREP_START  1
#define ILWREP_STOP   2
#define ILWREP_TITLE  3
#define ILWREP_UPDATE 4
#define ILWREP_TEXT   5
#define ILWREP_UPDATETEXT   6

// View Settings parameters
#define LOADSETTINGS   1
#define SAVESETTINGS   2

#define ILWM_DESCRIPTIONTEXT  (WM_APP + 1)   //send the ilwobj description text to view
#define ILWM_SETCOLOR         (WM_APP + 2)   // sends the color framewindow
#define ILWM_SETCOMMANDLINE   (WM_APP + 3)   //sends string with the text to any one who uses it 
#define ILWM_RECALCULATEFORM  (WM_APP + 4)   // reinitializes the form
#define ILWM_EXECUTE          (WM_APP + 5)   // execute string in lParam
#define ILWM_VIEWSETTINGS     (WM_APP + 6)   // message to notify views to save their settings
#define ILWM_SENDDIGSTRING    (WM_APP + 7)   // send digitizer string to be interpreted
#define ILWM_CLOSECOLLECTION  (WM_APP + 8)   // send a close message from a different thread to close a foreigncollection
#define ILWM_OPENCATALOG      (WM_APP + 9)   // send a message to the mainapp to open a catalog from a different thread

// message for Commandline from COM client
#define ILWM_COMCMDLN_EXECUTE (WM_APP + 10)  // Send COM command line to a CWinThread server thread
#define ILWM_CMDHANDLERUI     (WM_APP + 11)  // sends commands to the commandhandlerui object

#define ILWM_CALLBACK		  (WM_APP + 12)	
#define ILWM_OPENMAP		  (WM_APP + 13) // sends a message to the mappane view to open a certain map; the WPARAM is a pointer to the filename(string)
#define ILWM_UPDATE_ANIM		(WM_APP+232)

#endif // MESSAGES_H




