
//
//  MSc. Thesis: Optimizing VGI production using status maps requested through web services
//
//  Official Master's Degree in Software Engineering, Formal Methods and Information Systems
//  Polytechnic University of Valencia, Spain (UPV)
//  Faculty of Geo-Information Science and Earth Observation, The Netherlands (ITC)
//
//  Supervisor UPV: Vicente Pelechano
//  Supervisor ITC: Rob Lemmens
//
//  ---------------------------------------------------------------------------------------
//
//  @ Project : WFS Module for ILWIS
//  @ File Name : ConnectorWFS.hpp
//  @ Date : 30/10/2012
//  @ Author : Francisco Ruiz-Lopez
//
//  ---------------------------------------------------------------------------------------


#if !defined(_CONNECTORWFS_H)
#define _CONNECTORWFS_H

#include <iostream>
#include <string.h>
#include <vector>
//#include "Versions.hpp"
#include "Log.h"


struct ColumnInfoWFS {
	ColumnInfoWFS() : index(1), isNumeric(true),isInteger(true) {}
	int index;
	bool isNumeric;
	bool isInteger;
	map<String, int> elementCount;
	String colName;
	RangeReal range;
};

class _export ConnectorWFS {
public:
	ConnectorWFS(const String& ip, const String& port, const String& servicePath);
	ConnectorWFS(const String& fullAddress);
	~ConnectorWFS();


	//
	//
	//// The GetPropertyValue operation allows the value of a feature property or part ofthe value of a complex feature property to be retrieved from the data store for a set of features identified using a query expression
	//void getPropertyValue(const String& query, const String& valueReference);
	//void lockFeature(LockAction lockAction = all, int expiry = 300, const String& query="", const String& lockId = "");
	//// The GetFeatureWithLock operation is functionally similar to the GetFeature operation except 
	//// that in response to a GetFeatureWithLock operation, a WFS shall not only generate a response document 
	//// similar to that of the GetFeature operation but shall also lock the featuresin the result set; presumably to 
	//// update the features in a subsequent Transaction operation.
	//void getFeatureWithLock(const String& query, int expiry = 300, LockAction lockAction = all);
	// The client fetches a feature instance. The feature is then modified on the client side, and submitted back to the WFS via a Transaction request for update. Serializability is lost since there is nothing to guarantee that while the feature was being modified on the client side, another client did not come along and update that same feature in the database. 
	// One way to ensure serializability is to require that access to data be done in a mutually exclusive manner; that is, while one transaction accesses a data item, no other transaction may modify the same data item. This may be accomplished by using locks thatcontrol access to the data. 
	// The purpose of the LockFeature operation is to expose a long-term feature lockingmechanism to ensure consistency. The lock is considered long term because network latency would make feature locks last relatively longer than native commercial database locks. 
	// If a server implements the LockFeature operation, this fact shall be advertised in the server's capabilities Document.
	//void executeTransaction(TransactionAction actions, const String& lockId, LockAction releaseAction = all, const String& srsName = "-1");







	// This function calls getCapabilities (To get an updated answer) and answers a list with the NAMES (not the titles).
	std::vector<String> ConnectorWFS::getListLayerNames();
	// Retrieve the attribute names of a layer (using the NAME of the layer).
	std::vector<String> ConnectorWFS::getAttributeNamesList(const String& layerName, bool includeGeometry=true);
	// Retrieve the data type of an attribute
	String ConnectorWFS::getAttributeType(const String& attributeName);
	// Provides a vector of vectors with the geometries of the layer and its attributes
	std::vector<std::vector<String>> getFeaturesFromLayer(const String& layerName, map<String, ColumnInfoWFS>& columns, const String& version = "1.1.0", const String& filter = "");

	// Provides the LAYER TITLE
	String ConnectorWFS::getLayerTitle(const String& layerName);
	// Provides the LAYER ABSTRACT (If exists, otherwise returns "").
	String ConnectorWFS::getLayerAbstract(const String& layerName);
	// Searches in the attribute table for the column that contains the geometry
	String ConnectorWFS::getGeometryColumn(const String& layerName);
	// Search for the primitive geometry of the layer
	String ConnectorWFS::getGeometryType(const String& layerName);
	// Get the EPSG Code for one layer (The default EPSG Code obtained from capabilities document) THE SAME THAN THE SERVER PRODUCES
	String ConnectorWFS::getLayerSRS_FullEPSG(const String& layerName);
	// Get the EPSG Code for one layer (The default EPSG Code obtained from capabilities document) ONLY THE NUMERIC CODE!
	String ConnectorWFS::getLayerSRS_OnlyEPSGCode(const String& layerName);
	// Get the BoundingBox for one layer (As defined in the Capabilities document)
	std::vector<double> ConnectorWFS::getLayerBoundingBox(const String& layerName);

	String getServerUrl() const;

	int ConnectorWFS::executeTransactionDelete(const String& layerName, std::vector<String>& vector_gmlIds, const String& service = "WFS", const String& version = "1.1.0"); //Return 0 = OK; Return -1 = ERROR;
	int ConnectorWFS::executeTransactionDelete(const String& layerName, const String& str_field, const String& str_value, const String& service = "WFS", const String& version = "1.1.0"); //Return 0 = OK; Return -1 = ERROR;
	int ConnectorWFS::executeTransactionInsert(const String& layerName, std::vector<std::vector<String>>& v_attributes, const String& service = "WFS", const String& version = "1.1.0"); //Return 0 = OK; Return -1 = ERROR;
	// Update transaction only updates FIELDS, not geometries (yet).
	int ConnectorWFS::executeTransactionUpdate(const String& layerName, const String& field, const String& value, std::vector<String>& vector_gmlIds = std::vector<String>(), const String& service = "WFS", const String& version = "1.1.0"); //Return 0 = OK; Return -1 = ERROR; //If there's no gmlId values the changes will be applied to all layer.

	void generatePointMap();
	void generateLineMap();
	void generatePolygonMap();
private:
	Domain createClassDomain(const ColumnInfoWFS& ci);
	Domain createIdDomain(const ColumnInfoWFS& ci);
	Domain generateBaseData(std::map<String,ColumnInfoWFS>& columns, const String& ext,std::vector<vector<String>>& geometries);
	void fillAttributes(const String& geom_column_name, pugi::xpath_node_set::const_iterator& att_it,
								  std::vector<String>& aux_Attributes, map<String, ColumnInfoWFS>& columns, 
								  bool& firstRecord, int& columnCount) ;
	// I think there's no need to specify global variables here. const String& serverURL;
	// In case of non existence creates a new text file in project's root folder to store the configuration.
	// It includes:
	// List of WFS Services used recently.
	//void createHistoryFile();
	//// Reads the preferences file and updates the variables  of the class. In case preferences file is non existent it should create an empty one.
	//void readHistoryFile();
	//// Somehow we're going to keep this preferences file up-to-date using this method.
	//void addServiceToHistory(const String& address);
	//void removeServiceFromHistory(const String& address);




	int ConnectorWFS::getCapabilities(); // Return 0 = OK; Return -1 = ERROR;
	// The DescribeFeatureType operation returns a schema description of feature types offered by a WFS instance. 
	// The schema descriptions define how a WFS expects feature instances to be encoded on input (via Insert, Update and Replace actions) and how feature instances shall be encoded on output (in response to a  GetPropertyValue, GetFeature or GetFeatureWithLock operation).
	void ConnectorWFS::describeFeatureType(const String& featureTypeName = "NAMESPACE:FEATURETYPENAME", const String& service = "WFS", const String& version = "1.1.0", const String& handle = "");
	// The GetFeature operation returns a selection of features from a data store. A WFS processes a GetFeature request and returns a response document to the client that contains zero or more feature instances that satisfy the query expressions specified in the request.
	String ConnectorWFS::getFeature(const String& featureTypeName, const String& version = "1.1.0", const String& queryFilter = "");
	// Get the URL of the namespace for the layer returns "no-url-found" value if not found, IT IS MANDATORY FOR INSERT/UPDATE TRANSACTIONS, BUT IF NO WFS TRANSACTIONAL there's no need to control errors.
	String ConnectorWFS::getLayerNameSpaceURL(const String& layerName);


	// Initialize variables and environment for the basic capabilities operations and retrieving features.
	int ConnectorWFS::initializeConnector(); //Return 0 = OK; Return -1 = ERROR;

	// Tags and variables for WFS 1.1.0
	int ConnectorWFS::initializeFieldsCapabilities(); //Tags and variables. //Return 0 = OK; Return -1 = ERROR
	std::vector<double> parseCoordinatesPoint(const std::string coordList);
	void parseCoordinatesPolyline(const String& coordList,std::vector<Coordinate>& result);
	void parseCoordinatesPolygon(const String& coordList,std::vector<std::vector<Coordinate>>& result);

	String serverURL;				// Variable with the main service URL
	std::vector<String> tagsWFS110;	// Vector with all the tags from WFS 1.1.0
	//************************************* 
	// Variables for GetCapabilities Operation
	//**************************************
	//************************************** WFS Connector attributes [Service Details]
	String serviceTitle            ;
	String serviceAbstract         ;
	String serviceType             ;
	String serviceTypeVersion      ;
	String serviceFees             ;
	String serviceAccessConstraints;
	std::vector<std::vector<String>> v_serviceNSurls; //Vector to store all the URL addresses of the namespaces in Geosever.
	//************************************** WFS Connector attributes [Service Provider]
	String serviceProviderName                  ;
	String serviceProviderContactIndividualName ;
	String serviceProviderContactPositionName   ;
	//************************************** WFS Connector attributes [Operations Metadata] {Supported operations}
	std::vector<String> v_serviceOperationsMetadataOperations;
	//************************************** WFS Connector attributes [Feature Type List]   {List of available layers}
	std::vector<String> v_serviceFeatureTypeListOperations;	//Not mandatory, could be size 0
	std::vector<String> v_serviceFeatureTypeListFeatureTypes;	//Here we point to the NAME child node of each feature!
	//************************************** WFS Connector attributes [Filter_Capabilities]
	std::vector<String> v_serviceFilterSpatialGeometryOperands;	//Mandatory
	std::vector<String> v_serviceFilterSpatialSpatialOperators;	//Mandatory
	std::vector<String> v_serviceFilterScalarLogicalOperators;
	std::vector<String> v_serviceFilterScalarComparisonOperators;
	std::vector<String> v_serviceFilterScalarArithmeticOperatorsSimpleAritmetic;
	std::vector<String> v_serviceFilterScalarArithmeticOperatorsFunctions;
	//std::vector<String> v_serviceFilterScalarId;
	//************************************* Layer's list
	std::vector<std::vector<String>> vLayers;		// [NAME (id), TITLE (Descriptive title), ABSTRACT, DEFAULT_SRS, BBox{LOWERcornerLON, LOWERcornerLAT, UPPERcornerLON, UPPERcornerLAT}] 
	//************************************* 
	// Variables for DescribeFeatureType Operation
	//**************************************
	std::vector< std::vector<String> > v_AttributesLayer;
	String v_wfsurl;
	String v_currentLayer;
	String v_currentFeatureType;
	String v_outputName;
	CoordSystem coordSys;
	Table attable;
	Tranquilizer trq;

};


#endif  //_CONNECTORWFS_H
