/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
/* infocam.h
   Import/Export InfoCam
   by Martin Schouwenburg, february, 1997
   (c) ILWIS Department ITC
   Last change:  MS   24 Nov 97    3:33 pm
*/

#ifndef INFOCAM_H
#define INFOCAM_H

#include "Engine\Base\DataObjects\Dat2.h"
#include "Engine\Domain\dmsort.h"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Segment\SEGSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Table\Rec.h"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Map\Segment\Seg.h"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Table\tbl.h"
#include "Engine\Domain\Dmvalue.h"

#define ST_DATA             4
#define POINT_ID_HEADER     "C"
#define ST_PNT_DATA         4
#define EN_PNT_DATA         5
#define ST_PNT_NAME         6
#define EN_PNT_NAME         17
#define ST_PNT_EAST         18
#define EN_PNT_EAST         32
#define ST_PNT_NORTH        34
#define EN_PNT_NORTH        48
#define END_POINT_REC       71

#define SYMBOL_ID_HEADER    "SYMBOL"
#define POINT_CODE          "ILWPNT"
#define ST_SYMBOL           1
#define EN_SYMBOL           6
#define ST_SYMBOL_CODE      8
#define EN_SYMBOL_CODE      13
#define ST_SYMBOL_ROT       14
#define EN_SYMBOL_ROT       23
#define ST_SYMBOL_VAL       24
#define EN_SYMBOL_VAL       25
#define END_SYMBOL_REC      27
#define SYMBOL_ID_LEN       (EN_SYMBOL-ST_SYMBOL+1)

#define LINE_ID_HEADER      "LINE"
#define SEG_CODE            "ILWSEG"
#define ST_LINE             1
#define EN_LINE             4
#define ST_LINE_CODE        6
#define EN_LINE_CODE        11
#define ST_LINE_VAL         12
#define EDGE_ID_HEADER      "EDGE"
#define EN_LINE_VAL         13
#define ST_EDGE_TYPE        7
#define EN_EDGE_TYPE        12
#define ST_EDGE_VAL         13
#define EN_EDGE_VAL         14
#define LINE_ID_LEN         (EN_LINE-ST_LINE+1)
#define EDGE_ID_LEN         4
#define END_EDGE_REC        16
#define END_LINE_REC        15

#define REGION_ID_HEADER    "REGION"
#define POL_CODE            "ILWPOL"
#define ST_REGION           1
#define EN_REGION           6
#define ST_REGION_CODE      8
#define EN_REGION_CODE      13
#define ST_REGION_VAL       14
#define EN_REGION_VAL       15
#define END_REGION_RECORD   16
#define ISLAND_ID_HEADER    "ISLAND"
#define ST_ISLAND           4
#define EN_ISLAND           9
#define END_ISLAND_RECORD   12
#define REGION_ID_LEN       (EN_REGION-ST_REGION+1)
#define ISLAND_ID_LEN       (EN_ISLAND- ST_ISLAND+1)

#define ATTRIB_ID_HEADER    "ATTRIBUTE"
#define ST_ATTRIB           4
#define EN_ATTRIB           12
#define ST_ATTRIB_CODE      16
#define EN_ATTRIB_CODE      21
#define ST_ATTRIB_LENGTH    22
#define EN_ATTRIB_LENGTH    24
#define ST_ATTRIB_VAL       26
#define EN_ATTRIB_VAL       75
#define END_ATTRIB_RECORD    76
#define ATTRIB_LEN         (EN_ATTRIB_VAL - ST_ATTRIB_VAL)
#define DEF_ATTRIB          "ILWVAL"

#define HEADER_LENGTH       6
#define CODE_LENGTH         6
#define EXT_INFOCAM_CODE    "ICCODE"

#define CENTER_OF_GRAVITY  "ELE_CG"
#define COG_LENGTH          6

#define MAX_RECORD_SIZE     200


#endif


