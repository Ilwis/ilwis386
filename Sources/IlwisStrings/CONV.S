// Tranquilizer messages; string parts for tranquilizer messages
SCVTextConverting       "Converting..."
SCVTextGifSkipExt       "Skipping the extension block"
SCVTextGifGetGlobalLUT  "Retrieving global color table"
SCVTextGifGetLocalLUT   "Retrieving local color table"
SCVTextConvertWorking   "Conversion in progress"
SCVTextPolygons         "Polygons"
SCVTextSegments         "Segments"
SCVTextInLine_l         ", in line %ld"
SCVTextPolygonMap_S     "Polygon map %S"
SCVTextSegmentMap_S     "Segment map %S"
SCVTextPointMap_S       "Point Map %S"
SCVTextSegmentMap       "Segment Map"
SCVTextPolygonMap       "Polygon Map"
SCVTextPointMap         "Point Map"
SCVTextProcessing       "Processing..."
SCVTextProcSegment_     "Importing Segment "
SCVTextScanSegment_     "Scanning ... Segment "
SCVTextImportPoint_     "Importing Point "
SCVTextScanPoint_       "Scanning ... Point "
SCVTextArcInfoLinScan   "Scanning Arc/Info Generate"
SCVTextDXFImportScan    "Scanning DXF file ..."
SCVTextAtlasBnaScan     "Scanning BNA file for boundary"
SCVTextTableName_       "Table "
SCVTextReadRecord_      "Reading Record "
SCVTextCreateCols       "Creating new columns"
SCVTextWriteTopologies  "Writing segments"
SCVTextWritePolInfo     "Writing topology information"
SCVTextErdasTrailer     "Writing trailer file"
SCVTextNewDomain        "Create new domain"
SCVText_ImportedFrom_   " imported from "
SCVTextScanningFile     "Scanning File"
SCVTextScanning         "Scanning..."
SCVTextImportingSEQMap  "Imported SEQ map : "
SCVTextCreateMaplist    "Creating maplist"
SCVTextCodingPolygons   "Coding Polygons"
SCVTextInitializing     "Initializing maps"
SCVTextReadDBFRecords   "Reading records"
SCVTextReadDBFFile      "Reading File"
SCVTextWriteAttributes  "Writing attributes"
SCVTextWritePolLabels   "Writing polygon label points"
SCVTextWriteLabels      "Writing points"
SCVTextWriteSegments    "Writing segments"
SCVTextInLine_SI        "%S, in line %ld"
SCVTextReadSegAttrib    "Segment attributes"
SCVTextReadPPPAttrib_S  "%S attributes"
SCVTextBuildValueLayer  "Building layer table for values"
SCVTextWriteLayerTable  "Writing layer table"
SCVTextWritePolygons    "Writing polygons"
SCVTextRasterMap        "Raster Map"
SCVTextCopy             "Copy"
SCVTextConvert          "Convert"
SCVTextTiffHeader       "Writing TIFF file header"
SCVTextTiffIFD          "Writing TIFF Key directory"
SCVTextTiffGeoKeys      "Writing GeoTIFF information"
SCVTextReadTable        "Plain table"
SCVTextWritingToDisk    "Storing imported data..."
SCVTextOpeningTable     "Loading Table. Please wait..."
SCVTextWriteColumnInfo  "Write Column Info"
SCVTextOpeningAttributes "Loading attributes. Please wait..."
SCVTextImportRoutes     "Importing Routes"
SCVTextImportGartrip    "Import GARtrip text file"
SCVTextImportWaypoints  "Importing Waypoints"
SCVTextImportTracks     "Importing Tracks"

// Messages
SCVMsgSEQPointMap       "Imported SEQ Pointmap : "
SCVMsgSEQSegmentMap     "Imported SEQ Segmentmap : "
SCVMsgSEQPolygonMap     "Imported SEQ Polygonmap : "

// General convert error messages
SCVErrReading           "File Reading Error"
SCVErrHeadSizeMismatch  "Solving File size mismatch in header."
SCVErrLIPBandInput      "Input file or band number error"
SCVErrGifCode           "Error code in the file"
SCVErrGifNoImage        "No image in the file"
SCVErrWrongLineCol      "Incorrect number of lines or columns"
SCVErrGRDWrongCoords    "Wrong coordinates in GRD file"
SCVErrFileNotFound      "File not found"
SCVErrWrongBoundary     "Error in boundary coordinates"
SCVErrCreateVector      "Can not create vector map"
SCVErrOutOfRange        "Range too large for Erdas format"
SCVErrUnexpectedEOF     "Unexpected end of file"
SCVErrPolygonizeFailed  "Polygonization failed"
SCVErrCoordExpected     "Coordinate pair expected"
SCVErrDomain            "Domain error"
SCVErrTooMuchPoints     "Ilwis polygon contains too many points to export"
SCVErrIncorrectDomain   "Only domain Class or ID allowed"
SCVErrImpTableError     "dBase table could not be imported"
SCVErrAbortImport       "Error during converting, aborting conversion."
SCVErrRecordOutOfRange  "Record number out of Range."
SCVErrErrorTitle        "Conversion Error"
SCVErrNoPol             "Could not create Polygon"
SCVErrNoNumeric         "Numeric values expected"
SCVErrImportError       "Import Error"
SCVErrExportError       "Export Error"
SCVErrDuplicateKeyValues "Key column is not allowed to have duplicate values"
SCVErrNotTextFile_S       "%S is not a text file"
SCVErrInvalidFileName     "Invalid file name"
SCVErrCoordinateSys     "Error in coordinate system"

// Format error messages: unsupported features
SCVErrNotSupported      "File format not supported"
SCVErrNoCompresAllow    "Compressed Bitmap is not supported"
SCVErrTooManyPlanes     "Only 1, 4, 8, 16 or 24 bit format are supported"
SCVErrTooOldVersion     "Bitmap versions 1.x and 2.x are not supported"
SCVErrCannotDo24bit     "Cannot convert 24 bit color format"
SCVErrUnsupCompres      "Compression type not supported"
SCVErrOnlySize12        "Only grid data element size 1 and 2 are supported"
SCVErrNoPlanarTIF       "Planar format is not supported"
SCVErrNoBinaryDXFAllow  "Binary DXF is not supported"
SCVErrNoHorDiffLZW      "LZW Compression with horizontal differencing is not supported"
SCVErrE00Compressed     "Compressed E00 format not supported"
SCVErrUnsuppShapeType_I "Unsupported Shape Type: %d"
SCVErrUnsuppTIFLZW      "LZW compression for 1 and 4 bit not supported"
SCVErrIDAExpNoColor     "Color and Picture maps cannot be exported to IDA format"
SCVErrTooManyValues255  "Values are not in 0 to 255 range"
SCVErrTooManyValues_S   "Values will not fit in %S file"
SCVErrTooManyColumnIlwis14_SI "ILWIS 1.4 can only have less than 100 columns.\nTable %S contains %i columns."
SCVErrTooManyRecords2900_SI   "ILWIS 1.4 only allows 2900 records for string columns.\nTable %S contains %i records."
SCVErrTooManyRecords16000_SI  "ILWIS 1.4 only allows 16000 records for number columns.\nTable %S contains %i records."
SCVErrInvalidMapsize    "Invalid map size found"
SCVErrInvalidImageType  "Invalid image type found"

//Format error messages: wrong format; illegal formats
SCVErrNoBitmap          "Not a Microsoft Windows Bitmap"
SCVErrNoPCX             "Not a PCX file"
SCVErrNoErdasGL         "Not an Erdas LAN or GIS File"
SCVErrNoGIF             "not a GIF file"
SCVErrNoTIFF            "not a TIFF file"
SCVErrSMTFormat         "Error in SMT format"
SCVErrIllegalSMT        "Illegal SMT format"
SCVErrIllegalLIN        "Illegal Arc/Info format"
SCVErrIllegalBNA        "Illegal Atlas BNA format"
SCVErrIllegalDXF        "Illegal DXF format"
SCVErrFormatViolation   "Error in format"
SCVErrInternalFailure   "Internal conversion error"
SCVErrEmptyMaplist      "Error Empty map list"
SCVErrDomainNotEqual    "Domain of maps must be equal"
SCVErrCreateColFailed   "Create column failed"
SCVErrUnknownConversion "Unknown conversion format"
SCVErrTooManyClasses    "Export cannot handle more than 255 classes"
SCVErrUnrecognizedTag_S "Unrecognized E00 tag: %S"
SCVErrMapTooLittleParms_i "Too little parameters, at least %i needed"
SCVErrMapParamNr_i      "Incorrect parameter %i"
SCVErrMapFormat_S       "Incorrect raster import format: %S"
SCVErrMapSwap_S         "Incorrect 'swap bytes' parameter: %S"
SCVErrMapBytes_S        "Incorrect 'nr of bytes' parameter: %S"
SCVErrMapPixStruct_S    "Incorrect 'pixel structure' parameter: %S"
SCVErrMapHeaderSize_S   "Incorrect 'header size' parameter: %S"
SCVErrMapColumns_S      "Incorrect 'nr. of columns' parameter: %S"
SCVErrMapConvType_S     "Incorrect 'conversion type' parameter: %S"
SCVErrMapListFileStruct_S   "Incorrect 'file structure' parameter: %S"
SCVErrMapListCreateMPR_S "Incorrect 'create MPR' parameter: %S"
SCVErrPolExportUnsupported "Polygon export to 1.4 is not supported anymore"
SCVErrIncorrectParam_S   "Incorrect parameter format for Import (parameter was '%S')"
SCVErrCompressionUnknown_SI "Unrecognized compression token in %S, in line %li"
SCVErrCoordIndexOutOfBounds "CoordIndex out of bounds"
SCVErrCoordinateExpected    "Coordinate pair expected"
SCVErrClosingCoordMissing   "Closing coordinate of outer polygon missing"
SCVErrCircleRadiusExpected  "Circle radius expected"
SCVErrNumberExpected        "Number expected"
SCVErrWrongFileSize_S       "Size of %S does not match number of lines and columns"
SCVErrNoGartrip         "Not a valid GARtrip file"

// Warnings
SCVWarnNotNorthOriented "Map not North oriented or pixels not evenly spaced. Continue?"
SCVWarnBadTopology      "Topology information incomplete; will convert the segments only"
SCVWarnAbortedByUser    "Aborted by user"
SCVWarnElementExist     "Domain element exists already. Continue ?"
SCVWarnIdrDoc           "At line %li in %S file \n found %S\n expected %S. Continue?"
SCVWarnEmptyMap_S       "%S Map is empty, nothing has been exported"
SCVWarnIdExportBMP      "Maps with Id Domain cannot be exported to Windows BMP"
SCVWarnTooManySegments_I  "Too many segments in input, only first %ld will be converted.\nContinue ?"
SCVWarnNoElements       "No elements recognized, no ILWIS maps will be created"
SCVWarnNoProjectionOrLL		"Coordinate system is not a Projection or LatLon system"
SCVWarnUnknownProjection  "Coordinate system has an Unknown Projection"
SCVWarnNoDatumUseWGS84	"Projection is on WGS 84 spheroid but has no Datum. You want to use WGS84 ?"
SCVWarnNoSpheroidWithoutDatum "Esri doesn't support Spheroids without Datum"

// Titles raster
SCVTitleImportIDA       "Importing from IDA"
SCVTitleImportBMP       "Importing from Windows Bitmap"
SCVTitleImportGeoGrid   "Importing from Geosoft grid"
SCVTitleImportPCX       "Importing from Painbrush-PCX"
SCVTitleImportIlwisASC  "Importing from ILWIS ASCII"
SCVTitleImportLIP       "Importing from Lip-File"
SCVTitleImportErdasLG   "Importing from Erdas"
SCVTitleImportGIF       "Importing from GIF image"
SCVTitleImportTIFF      "Importing from TIFF image"
SCVTitleImportOther     "Importing General raster"
SCVTitleImportNAS       "Importing from Arc/Info uncompressed ASCII"
SCVTitleImportAtlasIMG  "Importing from Atlas Image"
SCVTitleImportIDRISI    "Importing from Idrisi DOC"
SCVTitleExportIdrisi    "Export to Idrisi IMG"
SCVTitleExportASCII     "Export to Ilwis ASCII format"
SCVTitleExportLAN       "Export to LAN format"
SCVTitleImportRaster    "Import Raster"
SCVTitleExportTiff      "Export to TIFF format"
SCVTitleExportIDA       "Export to IDA format"
SCVTitleExportARCASC    "Exporting to Arc/Info ASCII"
SCVTitleExportArcGisPRJ	"Exporting to ArcGis PRJ-file"

// Titles vectors
SCVTitleImportSmtList   "Importing from SMT list"
SCVTitleImportFromDXF   "Importing from AutoCAD DXF"
SCVTitleImportBNA       "Importing from Atlas BNA"
SCVTitleImportLin       "Importing from Arc/Info Generate"
SCVTitleImpPtsArcInfo   "Importing from Arc/Info Generate"
SCVTitleExportSmtList   "Exporting to SMT list"
SCVTitleExportToDXF     "Exporting to AutoCAD DXF"
SCVTitleExportBNA       "Exporting to Atlas BNA"
SCVTitleExportE00       "Exporting to Arc/Info E00"
SCVTitleExportToLin     "Exporting to Arc/Info Generate"
SCVTitleExpPntArcInfo   "Exporting to Arc/Info Generate"
SCVTitleExpPntShape     "Exporting Points to Arcview shapefile"
SCVTitleExpPolShape     "Exporting Polygons to Arcview shapefile"
SCVTitleExpSegShape     "Exporting Segments to Arcview shapefile"
SCVTitleExportGIS       "Exporting to Erdas GIS"
SCVTitleExportBMP       "Exporting to Windows BMP"
SCVTitleExpSegInfocam   "Exporting Segments to Infocam SEQ"
SCVTitleExpPntInfocam   "Exporting Points to Infocam SEQ"
SCVTitleExpPolygonInfocam "Exporting Polygons to Infocam SEQ"
SCVTitleImportInfocam   "Importing from SEQ file"
SCVTitleImportE00       "Importing from Arc/Info E00"
SCVTitleImportShape     "Importing from Arcview Shapefile"

//Titles Tables
SCVTitleExportDBF       "Export table to dBase DBF"
SCVTitleExportSDF       "Export table to dBase SDF"
SCVTitleExportDelim     "Export table to delimited text"
SCVTitleImportDBF       "Importing dBase III/IV table"
SCVTitleImportASCII     "Import ASCII Table"

// Interaction Form messages
SCVItemColumn           "Column"
SCVItemClass            "Class"
SCVItemID               "ID"
SCVItemString           "String"
SCVItemTableDomain      "Table Domain"
SCVItemValue            "Value"
SCVItemBool             "Bool"

// Interaction Form remarks
SCVRemSpecifyDomains    "Specify the domain types of all columns"

// Interaction Form messages
SCVMsgAskPolygonMap     "Polygons detected, create Polygon Map?"
SCVMsgImportWarning     "Import warning"
SCVMsgExportWarning     "Export warning"

// Remaining garbage (still in use)
SDRCVNOT                "Not yet implemented"

// Errors Import Table
SCVImpAsErrNotEnougPar       "Too few parameters"
SCVImpAsErrUnknownInpType    "Unknown input type"
SCVImpAsErrNoWidth           "Fixed Format: no or invalid column width found"
SCVImpAsErrDomainDef         "Expect Domain definition or Error in Domain"
SCVImpAsErrNoUseAs           "UseAs not available for this type of table"
SCVImpAsErrOneKeyOnly        "Only one key column per table may be used"
SCVImpAsErrUnexpectedTokem_S "Unexpected word ' %S 'found"
SCVImpAsErrEmptyDomain_i     "Empty domain specified for column %d. "
SCVImpAsErrEmptyColumnName_i "Empty column name specified for column %d. "
SCVImpAsErrIncorrectUse      "Incorrect use of syntax.\n All Columns must be specified or replacements."
SCVImpNoUseAsOrConvertFound  "UseAs or Convert parameter expected"
SCVImpImportTableNotFound_S  "Import table '%S' not found"
SCVImpSkipMustBePositive     "Skip lines must be a positive number"
SCVImpInvalidValueRange_S    "Invalid domain or value range specified: '%S'"
SCVImpDBFheaderincorrect     "DBF file header information is incorrect"
SCVImpDBFFileTooSmall        "DBF file is smaller than indicated by the number of records"
SCVImpRecordsTooLarge        "Record size is larger than allowed"
SCVImpFieldCountIncorrect    "The number of fields must be larger than 0"
SCVImpRecordCountIncorrect   "The number of records must be larger than 0"