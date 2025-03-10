#define ILW_ADDDATAWINDOW     WM_USER +   1
#define ILW_REMOVEDATAWINDOW  WM_USER +   2
#define ILW_CONFIGURE	      WM_USER +   3
#define ILW_UPDATE	      WM_USER +   4
#define ILW_GOTOFIELD	      WM_USER +   5
#define ILW_ISDIGACTIVE       WM_USER +   6
#define ILW_GETFILENAME       WM_USER +   7
#define ILW_QUIT	      WM_USER +   8
#define ILW_EXEC	      WM_USER +   9
#define ILW_READCATALOG       WM_USER + 100
#define ILW_ENTER	      WM_USER + 101

// WParams/LParms of messages
//ILW_READCATALOG
#define WP_DONTCARE      0
#define WP_STOPREADING   1
#define WP_RESUMEREADING 2


#define ID_CUT	       ID_EDIT_CUT
#define ID_COPY        ID_EDIT_COPY
#define ID_PASTE       ID_EDIT_PASTE
#define ID_CLEAR       104

#define  ID_MEN_DISPLAY   200
#define  ID_MEN_EDIT	  201
#define  ID_MEN_FILE	  202
#define  ID_MEN_HELP	     203
#define  ID_MEN_OPERATIONS   204
#define  ID_MEN_WINDOWS      205
#define  ID_MEN_WINDOW	     206
#define  ID_MEN_OPTIONS   207
#define  ID_MEN_LAYERS	  208
#define  ID_MEN_COLUMNS   209
#define  ID_MEN_RECORDS   210
#define  ID_EDITOR	  211
#define  ID_MEN_VIEW  212
#define  ID_MEN_GRAPH 213
#define  ID_MEN_INSERT 214

// #define ID_FILE_OPEN   301  - already defined in MFC
#define ID_FILE_CREATE 302
#define ID_FILE_DEF    303
#define ID_FILE_PROP   304
#define ID_FILE_COPY   305
#define ID_FILE_DEL    306
#define ID_FILE_EXPORT14 307
#define ID_FILE_IMPORT14 308
#define ID_FILE_OPENOBJ  309
#define ID_FILE_OPENASTBL 310
#define ID_FILE_OPENMPL_COLORCOMP 311
#define ID_FILE_OPENMPL_FILM 312
#define ID_FILE_PREFERENCES 313

#define ID_EDIT_DESC   401
#define ID_EDIT_OBJ    402
#define ID_EDIT_DELETE 403

#define ID_HELP_RELATED_TOPICS 500
#define ID_HELP_ILWIS_OBJECTS 501

// Toolbars
#define ID_TOOLBARS 600
#define ID_TOOLBAR_STANDARD 601
#define ID_TOOLBAR_LAYOUT 602

// Catalogue, context sensitive menu
#define ID_CAT_START   900
#define ID_CAT_SHOW    900
#define ID_CAT_PROP    901
#define ID_CAT_DEL     902
#define ID_CAT_COPY    903
#define ID_CAT_REN     904
#define ID_CAT_DESC    905
#define ID_CAT_DEF     906
#define ID_CAT_DEP     907
#define ID_CAT_EDIT    908
#define ID_CAT_HELP    909
#define ID_CAT_SHOWASDOM 910
#define ID_CAT_SHOWASRPR 911
#define ID_CAT_SHOWASTBL 912
#define ID_CAT_OPENHERE 913
#define ID_CAT_LAST    913
#define ID_CAT_NEW     914
#define ID_CAT_REDRAW  915
#define ID_CAT_REMOVE  916
#define ID_CAT_DEL2    917
#define ID_DIRUP       918
#define ID_CAT_PYRAMIDS 919
#define ID_CAT_TABLEVIEW 920
#define ID_CAT_ASLAYER 921
#define ID_CAT_ASANIMATION 922
#define ID_CAT_ACOLORCOMPOSITE 923

#define ID_ACT_START   950
#define ID_ACT_RUN     950
#define ID_ACT_HELP    951
#define ID_ACT_LAST    951

// Main Window
//#define ID_TOROOT      1000 - defined in messages.h
#define ID_EXIT        1001
#define ID_HIDE        1002
#define ID_TILE        1003
#define ID_CASCADE     1004
#define ID_CATVIEW_OPTIONS 1005
#define ID_CATALOG_QUERIES 1006
#define ID_START_CATALOGQUERIES 1007
#define ID_END_CATALOGQUERIES 1030
#define ID_VIEW 1033
#define ID_WINDOW_DATA 1034
#define ID_CAT_SORTCOL_START 1035
#define ID_CAT_SORTCOL_END   1060
#define ID_CAT_SORT 1061
#define ID_CLOSE_ALL 1062
#define ID_ARRANGE_CAT 1063
#define ID_CAT_SHOWMENU 1064
#define ID_CAT_SHOW_MAPTBL 1065
#define ID_CAT_SHOW_DATA 1066
#define ID_CAT_SHOW_ALL 1067
#define ID_CAT_SHOW_BASEMAPS 1085

// MainWindow import
#define ID_FILE_IMPORT_MAP        1068
#define ID_FILE_IMPORT_TABLE      1069
#define ID_FILE_IMPORT_GEOGATEWAY 1070
#define ID_FILE_IMPORT_GENRASTER  1071
#define ID_CAT_NEW2     1072
#define ID_NAV_CREATEDIR 1073
#define ID_FILE_IMPORT_CONNECT_DATABASE 1074
#define ID_FILE_IMPORT_GDAL 1075
#define ID_NEW_WMS 1076
#define ID_REMOVE_WMS 1077
#define ID_OPEN_WMS 1078
#define ID_EDIT_WMS 1079



// open
// create
// import
// digitizer
#define ID_DIG	       1080
#define ID_DIGACTIVE   1081
#define ID_DIGRESET    1083
#define ID_FILE_DIGSETUP 1084
#define ID_FILE_DIGREF 1082

// Map Window Options
#define ID_NORMAL      1100
#define ID_ENTIREMAP   1101
#define ID_ZOOMIN      1102
#define ID_ZOOMOUT     1103
#define ID_SELECTAREA  1104
#define ID_EXTPERC     1105
#define ID_LARGER      1106
#define ID_SMALLER     1107
#define ID_REDRAW      1108
#define ID_EXTCOORD    1109
#define ID_ADJUSTSIZE  1110
#define ID_EXTEND      1111
#define ID_BGCOLOR     1112
#define ID_MAPDBLCLK   1113
#define ID_SCALE1      1114
#define ID_MEASUREDIST 1115
#define ID_MAPDBLCLKRECORD 1116
#define ID_MAPDBLCLKRPR    1117
#define ID_MAPDBLCLKACTION 1118
#define ID_DEFAULTSCALE   1119
#define ID_PANAREA  1120
#define ID_FULLSCREEN  1121
#define ID_SELECTFEATURES  1122
#define ID_SELECTFEATURES_BYCOLUMN 1123
#define ID_SAVE_SELECTION 1124
#define ID_VIEW_SELECTED_ONLY 1125
#define ID_VIEW_MAP_RECORDS_ONLY 1126
#define ID_VIEW_ALL_RECORDS 1127
#define ID_COPY_COORD 1128


// TableWindow
#define ID_SEMIVARIOGRAM 1190
#define ID_SHOWRECORDVIEW 1191
#define ID_TIMECOLUMN 1192

// Too much, need to be split up!
#define ID_OPENMAP     1200
#define ID_ABOUT       1201
#define ID_CLOSEALL    1202
#define ID_IMPORT      1203
#define ID_EXPORT      1204
#define ID_PRINT       1205
#define ID_PLOT        1206
#define ID_SETUP       1207
#define ID_HLPCONTENTS 1208
#define ID_HLPSEARCH   1209
#define ID_HLPHOW      1210
#define ID_HLPKEY      1211
// #define ID_HELP        1212  - already defined in MFC
#define ID_OPENSEGMAP  1213
#define ID_OPENPOLMAP  1214
#define ID_OPENTABLE   1215
#define ID_PRINTSETUP  1216
#define ID_OPENPIXELINFO 1217
#define ID_EDITRECORD  1218
#define ID_EDITTABLE   1219
#define ID_DISPLAYRECORD  1220
#define ID_DISPLAYTABLE   1221
#define ID_GOTORECORD	  1222
#define ID_GOTORECORD0	 1223
#define ID_GOTORECORD1	 1224
#define ID_GOTORECORD2	 1225
#define ID_GOTORECORD3	 1226
#define ID_SELECT	 1227
#define ID_CONTUPDATE	 1228
#define ID_ADDMAPS	 1229
#define ID_PIXCONFIGURE  1230
#define ID_GOTOCOLUMN	 1231
#define ID_COLUMNS	 1232
#define ID_GRAPH	 1233
#define ID_ADDCOLUMN	 1234
#define ID_ADDGRAPH	 1235
#define ID_ADDGRF		 1236
#define ID_SAVEVIEW	 1237
#define ID_SAVEVIEWAS	 1238
#define ID_OPENVIEW	 1239
#define ID_CLONEVIEW	 1240
#define ID_PIXELEDIT	 1241
#define ID_POINTEDIT	 1244
#define ID_SEGEDIT	 1250
#define ID_OPENGENMAP	 1255
#define ID_SORT 	 1256
#define ID_POLEDIT	 1257
#define ID_ADDRECORD	 1258
#define ID_OPENPNTMAP  1259
#define ID_DSPOPTIONS	1261
#define ID_HLPREGISTER	1263
#define ID_HLPINDEX	 1264
#define ID_HLPGLOSSARY	 1265
#define ID_HLPHOWTO	 1266
#define ID_ALWAYSONTOP	 1267
#define ID_SAVE 	 1268
#define ID_SAVEAS	 1269
#define ID_MINIMIZEALL	 1270
#define ID_RESTOREALL	 1271
#define ID_COORDSYSTEM	 1274
#define ID_EDITFIELD	 1275
#define ID_PROP 	 1276
#define ID_UNDOALL	 1277
#define ID_ANNADDLAY	 1278
#define ID_AGGREGATE	 1279
#define ID_JOIN 	 1280
#define ID_SAVERASMAPAS  1283
#define ID_CONFMATRIX	 1284
#define ID_SELALL	 1285
#define ID_ADDCSYS	 1286
#define ID_CUMULATIVE	 1287
#define ID_LEASTSQUARES	 1290
#define ID_DISPLAY2DIMTBL 1291
#define ID_HLPOPERATIONS 1292
#define ID_HLPMENUCMDS	 1293
#define ID_PIXPRINT	 1296
#define ID_UPDATEALLCOLS 1297
#define ID_TBLSTATS	 1298
#define ID_COPYALL       1299

// BaseMapWindow File Create
#define ID_CREATEMAP	 1300
#define ID_CREATERASMAP  1301
#define ID_CREATESEGMAP  1302
#define ID_CREATEPOLMAP  1303
#define ID_CREATEPNTMAP  1304
#define ID_CREATETBL	 1310
#define ID_CREATE2DIMTBL 1311
#define ID_CREATEDOM	 1320
#define ID_CREATERPR	 1321
#define ID_CREATECSY	 1322
#define ID_CREATEGRF	 1323
#define ID_CREATEMPL	 1324
#define ID_CREATEFUN	 1325
#define ID_CREATEISL	 1326
#define ID_CREATEFIL	 1327
#define ID_CREATESMS	 1328
#define ID_CREATEANNTEXT 1329
#define ID_CREATEIOC 1330
#define ID_CREATEILO 1331
#define ID_CREATESUBMAP 1332
#define ID_HLPMAPTABCALC 1350
#define ID_HLPILWEXPRES	 1351
#define ID_HLPSCRIPTSYNTAX 1352
#define ID_HLPSEBS 1353

#define ID_OPERATION0  5000
#define ID_OPERATION1  5500
#define ID_FONT        1401
#define ID_TEST        1402
#define ID_CATFILTER   1403
#define ID_APNFILTER   1404
#define ID_SHOWADDINFO 1405
#define ID_EXEC        2000
#define MESSAGE_SELECT_ROW 1921

#define ID_STATUSLINE  1501
#define ID_COMMANDLINE 1502
#define ID_PIXINFO     1503
#define ID_BUTTONBAR   1504
#define ID_OPERLIST    1505
#define ID_NAVIGATOR   1506
#define ID_SELBAR			 1507
#define ID_MAINWINDBAR 1508
#define ID_SCALECONTROL 1509

// Representation and Legend
#define ID_RPRRAS      1701
#define ID_RPRSEG      1702
#define ID_RPRPOL      1703
#define ID_RPRPNT      1704
#define ID_RPRWIDTH    1710
#define ID_RPRPROP     1720
#define ID_RPRINSERT   1721
#define ID_RPRDELETE   1722
#define ID_RPRSTEPS    1723
#define ID_RPREDITLIMIT 1724
#define ID_RPREDITITEM 1725
#define ID_RPRDOM      1726
#define ID_RPREDITITEMS 1727
#define ID_RPRLEGEND   1730
#define ID_LEGCOLS     1731
#define ID_CLOSELEGEND 1732
#define ID_OPENCLOSELEGEND  1733

// Domains
#define ID_DOMADD      1801
#define ID_DOMEDIT     1802
#define ID_DOMDEL      1803
#define ID_DOMMERGE    1804
#define ID_DOMSRTMAN   1811
#define ID_DOMSRTALPH  1812
#define ID_DOMSRTALPHNUM	1813
#define ID_DOMSRTCODEALPH	1814
#define ID_DOMSRTCODEALNUM	1815
#define ID_DOMPREFIX   1816
#define ID_DOMPROP     1820
#define ID_DOMRPR      1821
#define ID_DOMSORTBUTTONBAR     1822
#define ID_DOMSORTESCAPE	1823

// MapView (in place activation, annotation, printing)
#define ID_COPYSCALEBARLINK 1900


#define ID_LAYFIRST    4000
#define ID_ADDDATALAYER 4910
#define ID_ADDSEGMAP   4900
#define ID_ADDPOLMAP   4901
#define ID_ADDANNOT    4902
#define ID_ADDPOINTS   4903
#define ID_ADDRASMAP   4904
#define ID_LAYERMANAGE 4911
#define ID_SELDISPLAY  4912
#define ID_LAYDELETE   4913
#define ID_OVERVIEW    4914
#define ID_ADDGRID     4921
#define ID_ADDMETAFILE 4922
#define ID_ADDBITMAP   4923
#define ID_ADDGRATICULE 4924
#define ID_ADDSCALEBAR 4925
#define ID_ADDNORTH_ARROW 4926
#define ID_ADDTITLE    4927
#define ID_ADDLEGEND   4928
#define ID_ADDBOX      4929
#define ID_ADDANNTEXT  4930
#define ID_ADDLAYER    4940
#define ID_ADD_GRID    4941
#define ID_ADD_GRATICULE 4942
#define ID_REMOVELAYER   4943
#define ID_ZOOM_TO_LAYER   4945
#define ID_METRIC_COORD 4944
#define ID_ZOOM_TO_SELECTED 4946
#define ID_LAYLAST     4999
#define ID_EDITLAYER   12000
#define ID_PROPLAYER   13000
#define ID_LAYEROPTIONS 14000
#define ID_RPRLAYER    15000
#define ID_ATTLAYER    16000
#define ID_DOMLAYER    17000
#define ID_HISTOLAYER 18000
#define ID_EDITGRF     1600
#define ID_EDITANN     1601
#define ID_EDITCSY     1602

#define ID_DATAWIND   10000
#define ID_GRAPHITEM  11000

#define ID_PROPEDITDOM	 3001
#define ID_PROPEDITGRF	 3002

// GeoReference
#define ID_EDITGRFTRANSF  3101
#define ID_EDITGRFADDPNT  3102
#define ID_EDITGRFFIDMARKS 3103
#define ID_EDITGRFSTOPFIDMARKS 3104
#define ID_EDITGRFDELPNT  3105
#define ID_TIEPOINTTABLE  3106
#define ID_SETFROMVIEWPOINT   3107
#define ID_SETTOVIEWPOINT    3108
#define ID_EXTENDBOUNDARIES 3109
#define ID_GRF3DBAR 3110
#define ID_QUALITYINFOBAR 3111

// CoordSystem
#define ID_EDITCSYBOUNDS 3201

// Graphs
/*#define ID_GRAPH_3D   2110
#define ID_GRAPH_POINT  2111
#define ID_GRAPH_LINE   2112
#define ID_GRAPH_AREA   2113
#define ID_GRAPH_RIBBON 2114
#define ID_GRAPH_BAR    2115
#define ID_GRAPH_COLUMN 2116
#define ID_GRAPH_STACK  2117
#define ID_GRAPH_PIE    2118
#define ID_GRAPH_EXPPIE 2119
*/

#define ID_GRPH_NEW         2100
#define ID_GRPH_PRINT   2101
#define ID_GRPH_HELP	  2102
#define ID_GRPH_EXIT	  2103
#define ID_GRPH_OPTIONS 2104
#define ID_GRPH_ADD_COLUMN 2105
#define ID_GRPH_REMOVE_AXIS 2106
#define ID_GRPH_GRID 2107
#define ID_GRPH_ADD_FORMULA 2108
#define ID_GRPH_ADD_LSF  2109
#define ID_GRPH_ADD_SVM  2110
#define ID_GRPH_ZOOM_IN 2111
#define ID_GRPH_ZOOM_OUT 2112
#define ID_GRPH_FITINWINDOW 2113
#define ID_MEN_COPY_CLIPBOARD 2115
#define ID_GRPH_COPY 2116
#define ID_GRPH_COPY_METAFILE 2117
#define ID_GRPH_REDRAW 2118
#define ID_ROSE  2119
#define ID_GRPHMAN	2120
#define ID_GRPH_LEGEND 2121
#define ID_PIECHART 2122
#define ID_GRPH_ADD 2123
#define ID_GRPH_OPENTABLE 2124

#define ID_PRINT_TABLE   2200

// Editors:
#define ID_NONEEDIT	 2300
#define ID_QUITEDITOR	 2301
#define ID_EXITEDITOR	 2302
#define ID_CONFIGURE	 2303
#define ID_EDIT 	 2304
#define ID_SETVAL	 2305
// Modes
#define ID_SELECTMODE	 2320
#define ID_INSERTMODE	 2321
#define ID_MOVEMODE	 2322
#define ID_MERGEMODE	 2323
#define ID_SPLITMODE	 2324
#define ID_TOPMODE	 2325
#define ID_UNDELSEG	 2326
#define ID_UNDELBND	 2327
#define ID_FINDUNDEFS 2328
#define ID_UNDO 2329
#define ID_REDO 2330
// Points
#define ID_ADDPOINT	 2340
// Segments
#define ID_SEGPACK	 2350
#define ID_SEGPOLYGONIZE 2351
#define ID_SEGCHECK	 2352
#define ID_SEGCHECKSELF  2353
#define ID_SEGCHECKINTERSECT 2354
#define ID_SEGCHECKDEADENDS  2355
#define ID_SEGCHECKCODECONS  2356
#define ID_SEGREMOVEREDUNDANT 2357
#define ID_SEGSETBOUNDS 2358
#define ID_SEGCHECKCLOSEDSEGMENTS 2359
// Polygons
#define ID_POLCREATELBL  2370
#define ID_POLAPPLYLBL   2371
#define ID_POLPOLYGONIZE 2372
#define ID_POLBOUNDARIES 2373
// Sample Set
#define ID_FEATURESPACE  2390
#define ID_MERGECLASS	 2391
#define ID_DELCLASS	 2392
#define ID_SMPLSTATPANE 2393
// Annotation Text
#define ID_ADDTEXT	 2400
#define ID_ANNTXTOPTIONS 2401
#define ID_ANNTXTCHANGE  2410
#define ID_ANNTXTFONT	 2411
#define ID_ANNTXTSIZE	 2412
#define ID_ANNTXTBOLD	 2413
#define ID_ANNTXTITALIC  2414
#define ID_ANNTXTUNDERLINE 2415
#define ID_ANNTXTCOLOR	 2416
#define ID_ANNTXTJUST	 2417
#define ID_ANNTXTTRANSPARENT   2418
#define ID_ANNTXTROTATION 2419

// Representation
#define ID_DESCRIPTIONBAR 2501
#define ID_REPRESENTATIONBAR 2502
#define ID_COLORGRIDBAR 2503
#define ID_COLORINTENSITYBAR 2504
#define ID_RPRCLASSBUTTONBAR 2505
#define ID_NO_OF_CELLS 2506
#define ID_EDIT_UNDO_COLOR 2507
#define ID_SCRIPTBUTTONBAR 2508
#define ID_RPRMULTSTEPS 2509
#define ID_RPRLOWER 2510
#define ID_RPRUPPER 2511
#define ID_RPRSTRETCH 2512
#define ID_FUNCBUTTONBAR 2513
#define ID_RPR_REFRESH 2514
#define ID_RPRVALUEBUTTONBAR 2515

//Script
#define ID_FILE_RUN_SCRIPT 2601

#define ID_SHOWHISTOGRAM 2700
#define ID_HISTOGRAM 2701

//Object Collection
#define ID_ADD_OBJECT 2801
#define ID_REMOVE_OBJECT 2802
#define ID_CREATE_OBJECTCOLLECTION 2803

// Layout
#define ID_ENTIRE_PAGE 2901
#define ID_ADD_MAPVIEW 2902
#define ID_ITEM_FONT   2903
#define ID_ITEM_EDIT   2904
#define ID_ADD_TEXT      2905
#define ID_ADD_SCALETEXT 2906
#define ID_ADD_SCALEBAR  2907
#define ID_ADD_NORTHARROW 2908
#define ID_ADD_LEGEND     2909
#define ID_ADD_PICTURE    2910
#define ID_PAGEBORDER     2911 
#define ID_ADD_MAPBORDER  2912
#define ID_ADD_BOX       2913
#define ID_KEEPSCALE     2914
#define ID_LAYOUT_MANAGEMENT 2915
#define ID_ITEM_REPLACEMAPVIEW 2916
#define ID_CREATE_LAYOUT 2917
#define ID_ITEM_REDRAW  2918
#define ID_SET_SCALE    2919
#define ID_MAKE_BITMAP 2920

// TableWindow
#define ID_SORT_ASCENDING  3301
#define ID_SORT_DESCENDING 3302
#define ID_STATPANE 3303
#define ID_COLSLICING 3304
#define ID_TBLBUTTONBAR 3305
#define ID_TBL2BUTTONBAR 3306

// FilterEditor
#define ID_GAINCONTROLBAR 3401

// Stereoscopy
#define ID_NEWSTEREOPAIR 3501
#define ID_OPENSTEREOPAIR 3502
#define ID_EXITSTEREOPAIR 3503
#define ID_EXITSHOWSTEREOPAIR 3504
#define ID_QUITSTEREOPAIR 3505
#define ID_SELECTFIDUCIALS 3506
#define ID_SELECTPPOINT 3507
#define ID_SELECTTPPOINT 3508
#define ID_SELECTSCALINGPTS 3509
#define ID_STEREO_CUSTOMIZE 3510
#define ID_TOOLBAR_STEREOSCOPY 3511
#define ID_ACTIVATE_LEFT 3512
#define ID_ACTIVATE_RIGHT 3513
#define ID_TOOLBAR_INFOWINDOW 3514
#define ID_UNLOCK_HSCROLL 3515
#define ID_TOOLBAR_STEREOSCOPE 3516
#define ID_LEFTRIGHT_STEREOSCOPY 3517
#define ID_EXITSTEREOSCOPE 3518

#define ID_MWD_STARTSELBUT 7001
#define ID_MWD_ENDSELBUT 7050
#define ID_MWD_SELECTALL 7049

// Spatial Multi Criteria Evaluation
#define ID_CT_ANALYSIS 3601
#define ID_CT_GENERATE 3602
#define ID_CT_SELECTEDMAP 3603
#define ID_CT_ALLMAPS 3604
#define ID_CT_GENSELECTEDMAP 3605
#define ID_CT_GENALLMAPS 3606
#define ID_CT_INSERT 3607
#define ID_CT_DELETE 3608
#define ID_CT_EDIT 3609
#define ID_CT_WEIGH 3610
#define ID_CT_SETALTERNATIVES 3611
#define ID_CT_DESCRIPTION 3612
#define ID_CT_TOOLBAR 3613
#define ID_CT_FILE_NEW 3614
#define ID_CT_INSERT_SUB 3615
#define ID_CT_GROUP 3616
#define ID_CT_FACTOR 3617
#define ID_CT_CONSTRAINT 3618
#define ID_CT_INSERTGROUP 3619
#define ID_CT_INSERTFACTOR 3620
#define ID_CT_INSERTCONSTRAINT 3621
#define ID_CT_STANDARDIZE 3622
#define ID_CT_EDITTREE 3623
#define ID_CT_STDWEIGH 3624
#define ID_CT_MODE 3625
#define ID_CT_SHOW 3626
#define ID_CT_PROPERTIES 3627
#define ID_CT_FILE_OPEN 3628
#define ID_CT_FILE_SAVE 3629
#define ID_CT_FILE_SAVE_AS 3630
#define ID_CT_FILE_PRINT 3631
#define ID_CT_FILE_PRINT_PREVIEW 3632
#define ID_CT_FILE_PRINT_SETUP 3633
#define ID_CT_EXIT 3634
#define ID_CT_HISTOGRAM 3635
#define ID_CT_AGGREGATE_VALUES 3636
#define ID_CT_SLICE 3637
#define ID_CT_SHOW_SLICED 3638
#define ID_CT_HISTOGRAM_SLICED 3639
#define ID_CT_AGGREGATE_VALUES_SLICED 3640
#define ID_CT_OVERLAYMAPS 3641
#define ID_CT_SCOREFACTOR 3642
#define ID_CT_SCORECONSTRAINT 3643
#define ID_CT_INSERTSCOREFACTOR 3644
#define ID_CT_INSERTSCORECONSTRAINT 3645
#define ID_CT_MAPAGGREGATION 3646
#define ID_CT_NOMAPAGGREGATION 3647
#define ID_CT_GENERATE_CONTOUR_MAPS 3648
#define ID_CT_SHOW_CONTOUR_MAPS 3649
#define ID_CT_SEGMENT_TO_POLYGON 3650
#define ID_CT_POLYGON_TO_RASTER 3651
#define ID_CT_SHOW_STANDARDIZED 3652

// GeneralBar ID's 
#define ID_RECORDBAR     4000
#define ID_ADDINFOBAR    4001
#define ID_ADDHISTBAR    4002
#define ID_ADDHISTRGBBAR 4003

// DataBaseCatalog ID's
#define ID_OPEN_WITH_QUERY 4200

#define ID_NEWBARS      10000

// General Bar commands
#define ID_ALLOWDOCKING 4100
