// Join Wizard Strings
// Titles
SJWTitleInputCol     "Join Wizard - Select input table and column"
SJWTitleOutputCol    "Join Wizard - Select output column"
SJWTitleJoinMethod   "Join Wizard - Join method"
SJWTitleLink         "Join Wizard - Link between two tables"
SJWTitleAggr         "Join Wizard - Aggregate data values"
SJWTitleAggrMethod   "Join Wizard - Aggregation method"

// Intros
SJWIntroInputCol "Select a table and a column to be joined into the current table:"
SJWIntroOutputCol    "Specify a new output column in the current table:"
SJWIntroJoinMethod1        "To be able to join data, a link has to be established between the two tables."
SJWIntroJoinMethod2        "You need to select and confirm the Join method, i.e. the link between these tables:"
SJWIntroAggr         "Join Wizard - Aggregate data values"
SJWIntroLink1A "The domain of the selected table will be used for key 2."
SJWIntroLink1B "Select a column from the current table for key 1."
SJWIntroLink2A "The domain of the current table will be used for key 1."
SJWIntroLink2B "Select a column from the selected table for key 2."
SJWIntroLink3A "Select a column from the current table for key 1"
SJWIntroLink3B "and select a column from the selected table for key 2."

// Remarks
SJWRemNone1 "There is no common Class or ID domain in the two tables through which a link could be established."
SJWRemNone2 "You can not join any column from the selected table into the current table."
SJWRemAggr1 "The classes/ID's of the selected column (key 2) are not unique."
SJWRemAggr2_SS "You need to aggregate the data values in column '%S' by groups of key 2 (column '%S')"
SJWRemAggr3_S "Data in column '%S' are automatically aggregated using the predominant value per group."
SJWRemUseWeight  "A column can be selected to be used as weight factor for the aggregation."
SJWRemColExists "Column name already exists in current table."

// other
SJWJoinType1 "Use domain of current table (key1) and use Domain of selected table (key2)"
SJWJoinType2 "Choose key Column from current table (key1) and use Domain of selected table (key2)"
SJWJoinType3 "Use Domain of current table (key1) and choose key Column from selected table (key2)"
SJWJoinType4 "Choose key Column from current table (key1) and choose key Column from selected table (key2)"

SJWCurrentTable "Current table:"
SJWSelectedTable "Selected table:"
SJWSelectedJoinCol "Selected column to be joined:"
SJWKey1  "Key 1:"
SJWKey2  "Key 2:"
SJWTableDomain_S "Table domain '%S'"
SJWToBeSelected "To be selected..."