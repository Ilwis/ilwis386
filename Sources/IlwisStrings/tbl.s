// Tranquilizer messages (Text)
STBLTextGetNeigbours        "Determining neighbours"
STBLTextGoSort              "Prepare for sorting"
STBLTextSortValues          "Sort values"
STBLFillTable               "Fill table"
STBLTextFillOtherColumns    "Fill other columns"
STBLCalculate               "Calculate"
STBLTextCopyingData         "Copying data"
STBLTextFillHashTable       "Fill hash table"
STBLTextFillCrossTable      "Fill cross table"
STBLTextSorting             "Sorting"
STBLTextWritingCrossMap     "Writing cross map"
STBLTextAddingRecords       "Adding records"
STBLTextMakingRecodeTable   "Making recoding table"
STBLTestMergingTableCol_S   "Merging table %S : Col %S"
STBLTextCalcHistogram       "Calculating histogram"
STBLTextCalcMeanStdDev      "Calculate mean and standard deviation"
STBLTextDetermineFreq       "Determine frequencies"
STBLTextCreateColumns       "Create columns"
STBLTextFillColumns         "Fill columns"
STBLTextAddElements         "Adding elements"
STBLTextStoreTable          "Store Table"

// General Messages (Msg)
STBLMsgPointDistance        "Point distance"
STBLMsgNumberPointPairs     "Number of point pairs"
STBLMsgCorrPointPairs       "Moran's I, a standardized spatial autocorrelation"
STBLMsgNormVarPointPairs    "Geary's c, a statistic for spatial autovariance"
STBLMsgOmniDirSemiVariogram "OmniDirectional SemiVariogram"
STBLMsgNumberPointPairs_S   "Number of point pairs %S"
STBLMsgSemiVariogram_S      "SemiVariogram %S"
STBLMsgSemiVariance_S       "SemiVariance %S"
STBLMsgNumberAggrRecords    "Number of aggregated records"
STBLMsgPixelValue           "Pixel value"
STBLMsgNumberPixels         "Number of pixels"
STBLMsgPercPixels           "Percentage of pixels"
STBLMsgNotUndef             "not undef"
STBLMsgNotZero              "not zero"
STBLMsgPercNrPixels_S       "Percentage of %S pixels"
STBLMsgCumulativePixels     "Cumulative number of pixels"
STBLMsgCumulativePercPixels "Cumulative percentage of pixels"
STBLMsgAreaPixelValue       "Area for pixel value"
STBLMsgUndefPixels_i        "Number of Undef pixels = %li (100%%)"
STBLMsgUndefPixels_if       "Number of Undef pixels = %li (%.2f%%)"
STBLMsgPolygonValue         "Polygon value"
STBLMsgNumberPolygons       "Number of polygons"
STBLMsgCumulativePolygons   "Cumulative number of polygons"
STBLMsgPerimeterPolygon     "Perimeter of polygons"
STBLMsgCumPolPerimeter      "Cumulative perimeter of polygons"
STBLMsgPolygonArea          "Area of polygons"
STBLMsgCumPolygonArea       "Cumulative area of polygons"
STBLMsgPointValue           "Point value"
STBLMsgPointCount           "Number of points"
STBLMsgCumPointCount        "Cumulative number of points"
STBLMsgSegmentValue         "Segment value"
STBLMsgSegmentCount         "Number of segments"
STBLMsgCumSegmentCount      "Cumulative number of segments"
STBLMsgSegmentLengths       "Length of segments"
STBLMsgCumSegmentLengths    "Cumulative length of segments"
STBLMsgHorCorrelation       "Horizontal correlation"
STBLMsgVertCorrelation      "Vertical correlation"
STBLMsgHorVariance          "Horizontal variance"
STBLMsgVertVariance         "Vertical variance"
STBLMsgHorSemVariance       "Horizontal semivariance"
STBLMsgVertSemVariance      "Vertical semivariance"
STBLMsgAvgPointDistanceAll  "Average Lag length in all directions"
STBLMsgCrossVariogram_S     "CrossVariogram %S"
STBLMsgAvgPointDistance1	"Average Lag length in direction 1: "
STBLMsgAvgPointDistance2	"Average Lag length in direction 2: "

// Title messages (Title)
STBLTitleSpatialCorrelation "Spatial Correlation"
STBLTitleTableGlue          "TableGlue"
STBLTitleTableHistogram     "TableHistogram"
STBLTitleCreateDomain       "Creating domain"
STBLTitleCrossVariogram     "CrossVariogram"

// Warnings (Warn)

// Error messages (Err)
STBLErrTooFewValidPoints        "More than two valid points needed"
STBLErrTooFewPoints             "More than two points needed"
STBLErrDomainSortOrNoneRequired "Domain must be Class/Id or None"
STBLErrDomSortOrNoneRequired_S  "Only domain class/ID/none allowed: '%S'"
STBLErrDomainSortRequired       "Column Domain must be Class or ID"
STBLErrColumnHasDuplicates      "Column contains duplicates"
STBLErrCreateTable_S            "Could not create table %S"
STBLErrIncompatibleDomains      "Tables to be merged do not have \nthe correct domain combinations"
STBLErrColorDomainNotAllowed_S  "No color domain allowed %S"
STBLErrTooLargeDomain           "Too large domain"
STBLErrMax1000Records_S         "No more than 1000 records allowed: '%S'"
STBLErrInvalidCharinCol_c       "Invalid character '%c' in column"
STBLErrWrongFirstColChar        "First character of column must be A..Z"
STBLErrNoBinaryValues           "Binary value asked for non-binary Column"
STBLErrEmptyExpression          "Empty Expression"
STBLErrCyclicDefinition         "Cyclic definition"
STBLErrNoWeightAllowed_S        "No weight allowed for '%S'"
STBLErrIncompTblDomains_SS      "Incompatible domain of tables '%S' and '%S'"
STBLErrIncompDomains_SS         "Incompatible domains '%S' and '%S'"
STBLErrDomStringNotAllowed_S    "No string domain allowed for predominant function for column '%S'"
STBLInvalidFitType              "Invalid fittype:"
STBLErrCurlyExpected            "\'}\' expected"
STBLErrCoordColUnsupported      "Coordinate Columns are not yet supported"
STBLErrMaxPixShiftNotNegative   "Maximum pixel shift must be a positive integer number"
STBLErrAttrTableNotFound        "Attribute table not found "
STBLErrAttrColNotFound          "Attribute column not found in table"
STBLErrMaxPixShiftTooLarge      "Maximum pixel shift too large, (more than 8000)"
STBLErrMaxPixShiftLargerThanMap "Maximum pixel shift larger than map size"
STBLErrLagSpacingNotPos         "Lag spacing must be positive"
STBLErrDirecFrom_360To360       "Direction must be between -360 and 360"
STBLErrTolerFrom0To45           "Tolerance must be between 0 and 45"
STBLErrBandWidthPositive        "Band width must be positive"
STBLErrInvalidParameter         "Invalid parameter"
STBLErrInvalidColumn_S          "Invalid column %S"
STBLErrNonTopoMap  "Polygon Map does not have topology"
STBLErrIndependentColumnsDomainsDoNotMatch "column domain must match original"
STBLErrInvalidProbabColumn_S	"Invalid Probabilty Column %S"

// Other strings
STBLOthBandWidth            "BandWidth"
STBLOthAngle                "Angle"
STBLOthTolerance            "Tolerance"
STBLOthMean                 "Mean"
STBLOthMedian               "Median"
STBLOthStdDev               "Std.Dev"
STBLOthPred                 "Pred"
STBLOthUndefCount           "Undef count"
STBLOthUnexpected           "Unexpected"
STBLOthInvalidExpr          "Invalid calc expression:"

