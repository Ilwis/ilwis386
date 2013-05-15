//
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
//  @ File Name : ConnectorWFS.cpp
//  @ Date : 30/10/2012
//  @ Author : Francisco Ruiz-Lopez
//
//  ---------------------------------------------------------------------------------------
#include "Headers\messages.h"
#include "Headers\toolspch.h"
#include "Engine\SpatialReference\Coordsys.h"
#include "Engine\Table\Col.h"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\DataExchange\ForeignFormat.h"
#include "Engine\Map\Point\ilwPoint.h"
#include "Engine\Map\Point\PNT.H"
#include "Engine\Map\Polygon\POL.H"
#include "Engine\Map\Point\PNTSTORE.H"
#include "Engine\Map\Polygon\POLSTORE.H"
#include "Engine\Map\Polygon\POL14.H"
#include "Engine\Base\DataObjects\valrange.h"
#include "Engine\Domain\Dmvalue.h"
#include "Engine\Base\System\Engine.h"
#include "Engine\DataExchange\gdalproxy.h"
#include "engine\domain\dmsort.h"
#include "engine\domain\dmvalue.h"
#include <iostream>
#include <sstream> 
#include <string.h>
#include <algorithm> //Used for transform to upper/lowercase
#include "Engine\Base\DataObjects\URL.h"
//#include "Versions.hpp"

//These includes only work if I use the library by myself, ILWIS compiles also those libraries so if I use them there will be a name conflict.
//#include <pugixml/pugixml.hpp>	//pugi version = 1.2
//#include <curl/curl.h>	
#include "Engine/Base/XML/pugixml.hpp"
#include "Engine/DataExchange/curlIncludes/curl.h"
#include "ConnectorWFS.h"

using namespace std;



//Try to create this on the heap instead of the stack (Is it actually created on the stack?)
//struct MemoryStruct 
//{
//	char *memory;
//	size_t size;
//};
//Try to create this on the heap instead of the stack (Is it actually created on the stack?)
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) 
{
	size_t realsize = size *nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (unsigned char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) 
	{
		// out of memory!
		printf("not enough memory (realloc returned NULL)\n");
		return -1;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	//std::cout << "Calling writeMemoryCallback" << std::endl;
	return realsize;
}



// Class Variables:[defined in HEADER FILE!]
 //ATTRIBUTE[ NAME, TYPE, NULLABLE*, MINOCC*, MAXOCC*] *Optionals



//Define the log as singleton element for all the class.


ConnectorWFS::ConnectorWFS(const String& ip, const String& port, const String& servicePath) {
	string aux = "http://";
	aux.append(ip).append(":").append(port).append(servicePath);
	ConnectorWFS *c_01 = new ConnectorWFS(aux); //Use the next constructor
}

ConnectorWFS::ConnectorWFS(const String& cmd) {
	//The address MUST end with "?"
	try{
	ParmList parms(cmd);
	String temp = parms.sGet(0);
	if ( parms.iFixed() >= 3) {
		v_currentLayer = parms.sGet(1).sUnQuote();
		v_currentFeatureType = parms.sGet(2).sUnQuote();
		if( parms.iFixed() == 4) {
			v_outputName = parms.sGet(3).sUnQuote();
		}
	}
	if ( temp.toLower().iPos(String("service=WFS")) != shUNDEF) {
		URL url(temp);
		serverURL = url.getPath() + "?";
	} else {
		if(temp[temp.size()-1]!='?')
		{
			temp.append("?");
		}
		serverURL = temp;
	}
	trq.SetTitle(TR("Importing through WFS"));
	// Initialize the variables for further processes:
	if (initializeConnector()==-1){
	}
	if ( v_currentFeatureType == "gml:point" || v_currentFeatureType == "gml:multipoint") {
		generatePointMap();
	} else if (v_currentFeatureType == "gml:linestring" || v_currentFeatureType == "gml:multilinestring") {
		generateLineMap();
	} else if (v_currentFeatureType == "gml:polygon" || v_currentFeatureType == "gml:multisurface") {
		generatePolygonMap();
	}
	trq.Stop();
	}
	catch(ErrorObject& err){
		err.Show();
	}
}

ConnectorWFS::~ConnectorWFS() {
}

int ConnectorWFS::getCapabilities() {
	//Retrieve the relevant details about the Web Service to manage further operations.
	//DETAILS
	int status = -1;

	//BUILD THE URL GET  "http://localhost:8080/geoserver/ows?service=wfs&version=1.1.0&request=GetCapabilities"
	String url_GetCapabilities = serverURL;
	url_GetCapabilities.append("SERVICE=WFS&VERSION=1.1.0");			// Service + Version
	url_GetCapabilities.append("&REQUEST=GETCAPABILITIES");				// Request

	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);	// will be grown as needed by the realloc above
	chunk.size = 0;						// no data at this point 

	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	//init the curl session
	curl_handle = curl_easy_init();
	//specify URL to get
	curl_easy_setopt(curl_handle, CURLOPT_URL, url_GetCapabilities.c_str()); 
	//send all data to this function 
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	//we pass our 'chunk' struct to the callback function
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	//some servers don't like requests that are made without a user-agent field, so we provide one
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	//get it!
	CURLcode err =curl_easy_perform(curl_handle);
	//cleanup curl stuff
	curl_easy_cleanup(curl_handle);
	//we're done with libcurl, so clean it up
	curl_global_cleanup();
	
	
	//logWFS->printToLog(chunk.memory);
	if ((long)chunk.size == 0)
	{
		throw ErrorObject("No data retrieved! Check the service address");
	}


	//*************************************************************************************************
	// Before using the Xpath Search functions we should homogeneize the tags from the namespaces!
	// By reading the document and adding the required ones if this exists:
	// All tags that use as default namespace WFS namespace will be added as tags form that namespace. Add also the namespaces avoiding dupplicates.

	String cap_doc = "";
	cap_doc = (char *)chunk.memory;
	free(chunk.memory); //We are not gonna use the original chunk data anymore.
	int index = -2;
	String aux_nameSpaceWfs = "xmlns=\"http://www.opengis.net/wfs\"";
	index = cap_doc.find(aux_nameSpaceWfs,0); //index = -1 if there's no coincidence.

	//ATTENTION!: Besides updating the tags we have to add the namespace definition, otherwise we get an error!
	//Avoiding duplicate WFS namespace!! add it only if it does not exsist!!!
	int aux_indexNS = -1;
	aux_indexNS = cap_doc.find("xmlns:wfs=\"http://www.opengis.net/wfs\"",0);
	if (aux_indexNS == -1 && index != -1)
	{
		cap_doc.replace(index, aux_nameSpaceWfs.size(), aux_nameSpaceWfs+" xmlns:wfs=\"http://www.opengis.net/wfs\"");
	}
	//std::cout << "Indice: " << index << std::endl;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load(cap_doc.c_str());

	if (index !=-1){ //If the default namespace is wfs then add to all tags without ns "wfs:"
		for ( int i = 0 ; i<tagsWFS110.size(); i++ ){ //Loop to explore all the elements in WFS Schema.
			pugi::xpath_node_set ns;
			String expression = String("//").append(tagsWFS110[i]);
			ns = doc.select_nodes(expression.c_str()); //We select all the nodes with that exactly name and replace them for the wfs:nodename element.
			for(pugi::xpath_node_set::const_iterator it = ns.begin(); it!=ns.end(); ++it){
				pugi::xpath_node n = *it;
				String newTag = string("wfs:").append(tagsWFS110[i]);
				n.node().set_name(newTag.c_str());
			}
		}
	}



	// Service Details
	// ******************************************************************************************************************
	pugi::xpath_node capab_node;
	pugi::xpath_node_set capab_node_set;
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:Title");
	serviceTitle=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:Abstract");
	serviceAbstract=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:ServiceType");
	serviceType=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:ServiceTypeVersion");
	serviceTypeVersion=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:Fees");
	serviceFees=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceIdentification/ows:AccessConstraints");
	serviceAccessConstraints=capab_node.node().child_value();
	capab_node =  doc.select_single_node("/wfs:WFS_Capabilities");
	for (pugi::xml_attribute_iterator att_it = capab_node.node().attributes_begin(); att_it!=capab_node.node().attributes_end(); ++att_it)
	{
		pugi::xml_attribute at = *att_it;
		std::vector<String> v_aux;
		String att_name = at.name();
		String att_val = at.value();
		size_t found = String::npos;
		found=att_name.find("xmlns:");
		if (found!=String::npos){
			v_aux.push_back(att_name);
			v_aux.push_back(att_val);
			v_serviceNSurls.push_back(v_aux);
		}
	}
	

	// Service Provider
	// ******************************************************************************************************************
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceProvider/ows:ServiceContact/ows:IndividualName");
	serviceProviderName=capab_node.node().child_value();
	capab_node = doc.select_single_node("/wfs:WFS_Capabilities/ows:ServiceProvider/ows:ServiceContact/ows:PositionName");
	serviceProviderContactIndividualName=capab_node.node().child_value();


	// Operations Metadata {Supported operations} 
	// ******************************************************************************************************************
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ows:OperationsMetadata/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceOperationsMetadataOperations.push_back(node.node().attribute("name").value());
	}

	// Feature Type List  {List of available layers}
	// ******************************************************************************************************************
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:Operations/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFeatureTypeListOperations.push_back(node.node().child_value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFeatureTypeListFeatureTypes.push_back(node.node().child("wfs:Name").child_value());
	}


	// WFS Connector attributes [Filter_Capabilities]
	// ******************************************************************************************************************	
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Spatial_Capabilities/ogc:GeometryOperands/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterSpatialGeometryOperands.push_back(node.node().child_value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Spatial_Capabilities/ogc:SpatialOperators/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterSpatialSpatialOperators.push_back(node.node().attribute("name").value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Scalar_Capabilities/ogc:LogicalOperators/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterScalarLogicalOperators.push_back(node.node().child_value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Scalar_Capabilities/ogc:ComparisonOperators/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterScalarComparisonOperators.push_back(node.node().child_value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Scalar_Capabilities/ogc:ArithmeticOperators/ogc:SimpleArithmetic/*");
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterScalarArithmeticOperatorsSimpleAritmetic.push_back(node.node().child_value());
	}
	capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/ogc:Filter_Capabilities/ogc:Scalar_Capabilities/ogc:ArithmeticOperators/ogc:Functions/ogc:FunctionNames/*"); //Here we retrieve ONLY the names, but we could also retrieve (from the attributes of the node) the number of parameters required.
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		v_serviceFilterScalarArithmeticOperatorsFunctions.push_back(node.node().child_value());
	}

	// Prepare the layer's list.  [NAME (id), TITLE (Descriptive title), ABSTRACT, DEFAULT_SRS, BBox{LOWERcornerLON, LOWERcornerLAT, UPPERcornerLON, UPPERcornerLAT}]
	vLayers.clear();
	//capab_node_set = doc.select_nodes("/wfs:WFS_Capabilities/wfs:FeatureTypeList/wfs:FeatureType");
	capab_node_set = doc.select_nodes("//wfs:FeatureType");
	String aux_lowerCorner = "";
	String aux_upperCorner = "";
	size_t pos_lower=0;
	size_t pos_upper=0;
	for (pugi::xpath_node_set::const_iterator it = capab_node_set.begin(); it != capab_node_set.end(); ++it)
	{
		pugi::xpath_node node = *it;
		std::vector<String> aux_vect;
		aux_vect.push_back(node.node().child("wfs:Name").child_value());
		aux_vect.push_back(node.node().child("wfs:Title").child_value());
		aux_vect.push_back(node.node().child("wfs:Abstract").child_value());
		aux_vect.push_back(node.node().child("wfs:DefaultSRS").child_value());
		pugi::xpath_node n1 = node.node().select_single_node("ows:WGS84BoundingBox/ows:LowerCorner | WGS84BoundingBox/LowerCorner");
		pugi::xpath_node n2 = node.node().select_single_node("ows:WGS84BoundingBox/ows:UpperCorner | WGS84BoundingBox/UpperCorner");
		aux_lowerCorner = n1.node().child_value();
		aux_upperCorner = n2.node().child_value();
		pos_lower = aux_lowerCorner.find(" "); //In the list the coordinates are split by " ".
		pos_upper = aux_upperCorner.find(" "); //In the list the coordinates are split by " ".
		aux_vect.push_back(aux_lowerCorner.substr(0,pos_lower)); //LOWER CORNER LONGITUDE
		aux_vect.push_back(aux_lowerCorner.substr(pos_lower));   //LOWER CORNER LATITUDE
		aux_vect.push_back(aux_upperCorner.substr(0,pos_upper)); //UPPER CORNER LONGITUDE
		aux_vect.push_back(aux_upperCorner.substr(pos_upper));   //UPPER CORNER LATITUDE
		vLayers.push_back(aux_vect);
	}
	// SUMMARIZE THE LAYER LIST
	//for (int i=0; i< vLayers.size(); i++) { logWFS->printToLog("Layer",vLayers[i]);	}

	// Initial value for status = -1, If there's no error it's changed and returned as 0.
	status = 0; // Status = 0 means OK, Status = -1 Means Error
	return status;
}


// ConnectorWFS::describeFeatureType DEPRECATED.
void ConnectorWFS::describeFeatureType(const String& featureTypeName, const String& service, const String& version, const String& handle) {
	// BUILD THE URL GET 
	String url_DescribeFeatureType = serverURL;
	url_DescribeFeatureType.append("SERVICE=").append(service);
	url_DescribeFeatureType.append("&VERSION=").append(version);
	url_DescribeFeatureType.append("&REQUEST=DESCRIBEFEATURETYPE");			// Request
	url_DescribeFeatureType.append("&TYPENAME=");							// Optional parameter, where we define the name of the schema.
	url_DescribeFeatureType.append(featureTypeName);						// LAYER TO BE DESCRIBED!
	//url_DescribeFeatureType="http://localhost:8080/geoserver/wfs?request=DescribeFeatureType&version=1.1.0&service=WFS&typeName=topp:states";
	

	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url_DescribeFeatureType.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_perform(curl_handle); //This is the function that makes the process slower.
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();


	// RETRIEVE THE SCHEMA OF THE LAYER
	String layer_schema_doc = "";
	layer_schema_doc = (char *)chunk.memory;
	
	//Let's see if the tag exists:
	int index_1 = layer_schema_doc.find("xsd:complexType name=",0); //index = -1 if there's no coincidence.
	//Control errors (The service might use namespace xsd (xmlns:xsd="http://www.w3.org/2001/XMLSchema"), in this case we should find the tag (complexType name=)
	//If we have to look for the tag withouth xsd namespace we will put it in a variable for preventing error tag not found on next stage.
	int aux_useNS = -1;
	if (index_1==-1 && layer_schema_doc.find("xmlns:xsd=\"http://www.w3.org/2001/XMLSchema")!=-1){
		index_1 = layer_schema_doc.find("complexType name=");
		aux_useNS = 0;
	}
	if (index_1 == -1)
	{
		//If the server does not work well it might be interesting. But if the service is OGC compilant there's no need to do it.
		
	}else
	{
		pugi::xml_document doc_dft;	
		pugi::xml_parse_result result = doc_dft.load_buffer(chunk.memory, chunk.size); //We can access directly to memory. No reparsing needed.
		free(chunk.memory);

		pugi::xpath_node_set dft_node_set;
		if (aux_useNS==-1){
			dft_node_set = doc_dft.select_nodes("//xsd:sequence/*");
		}else{
			dft_node_set = doc_dft.select_nodes("//sequence/*");
		}
		//std::cout << "ATTRIBUTE NUMBER OF COLUMNS: " << dft_node_set.size() << std::endl;

		//Reset the vector:
		v_AttributesLayer.clear();
		for (pugi::xpath_node_set::const_iterator it = dft_node_set.begin(); it != dft_node_set.end(); ++it)
		{
			pugi::xpath_node dft_node = *it;
			std::vector<String> aux_vectAtt;
			//Add attributes to the auxiliar vector.
			String aux_attName = "";
			String aux_atttype = "";
			String aux_attNull = "";
			String aux_attMaxO = "";
			String aux_attMinO = "";
			aux_attName = dft_node.node().attribute("name").value();
			aux_atttype = dft_node.node().attribute("type").value();
			aux_attNull = dft_node.node().attribute("nillable").value();
			aux_attMaxO = dft_node.node().attribute("maxOccurs").value();
			aux_attMinO = dft_node.node().attribute("minOccurs").value();
			//std::cout << "" << aux_attName << "   \t | TYPE: \t" << aux_atttype << " | NULL   " << aux_attNull << " | MAX   " << aux_attMaxO << " | MIN   " << aux_attMinO << std::endl;

			aux_vectAtt.push_back(aux_attName);
			aux_vectAtt.push_back(aux_atttype);
			aux_vectAtt.push_back(aux_attNull);
			aux_vectAtt.push_back(aux_attMaxO);
			aux_vectAtt.push_back(aux_attMinO);

			//Add the auxuliar vector to the main vector of Attributes.
			v_AttributesLayer.push_back(aux_vectAtt);
		}
	}
}


//Retrieve features from a layer.
String ConnectorWFS::getFeature(const String& featureTypeName, const String& version, const String& queryFilter) {
	String aux_response = "";
	// BUILD THE URL GET 
	String url_GetFeature = serverURL;
	url_GetFeature.append("SERVICE=WFS");
	url_GetFeature.append("&VERSION=").append(version);
	url_GetFeature.append("&REQUEST=GETFEATURE");							// Request
	url_GetFeature.append("&TYPENAME=");									// Optional parameter, where we define the name of the schema.
	url_GetFeature.append(featureTypeName);									// LAYER TO BE DESCRIBED!
	if (queryFilter!=""){url_GetFeature.append("&").append(queryFilter);}	//In case there is filter we add it.

	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL, url_GetFeature.c_str());
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_perform(curl_handle);
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();


	// RETRIEVE THE SELECTED FEATURES
	String features_doc = "";
	features_doc = (char *)chunk.memory;

	//TODO: We could implement filter in future versions to extract a part of the full layer.
	
	aux_response = features_doc;

	if(aux_response==""){
		throw ErrorObject("The request didn't produce results");
	}
	return aux_response;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implemented for make easier the management of the intermediate steps of the processes.
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Retrieving layer's list:
std::vector<String> ConnectorWFS::getListLayerNames(){
	std::vector<String> aux_answer;
	//getCapabilities(); //To have up-to-date data. It makes the process much slower, for now we will use getCapabilities just creating the object.
	
	for (unsigned int i=0; i<vLayers.size(); i++){
		aux_answer.push_back(vLayers[i][0]); //Retrieve the name.
	}
	return aux_answer;
}
//Retrieving attributes list
std::vector<String> ConnectorWFS::getAttributeNamesList(const String& layerName, bool includeGeometry){
	std::vector<String> aux_answer;
	//getCapabilities(); //To have up-to-date data. It makes the process much slower, for now we will use getCapabilities just creating the object.
	describeFeatureType(layerName);
	
	for (unsigned int i=0; i<v_AttributesLayer.size(); i++){
		if ( !includeGeometry) {
			if ( v_AttributesLayer[i][1] == "gml:GeometryPropertyType")
				continue;
		}
		aux_answer.push_back(v_AttributesLayer[i][0]);
	}
	//logWFS->printToLog("ATTRIBUTE NAMES: ", aux_answer);
	return aux_answer;
}
//Retrieving attribute type
String ConnectorWFS::getAttributeType(const String& attributeName){
	String aux_answer = "";

	for (unsigned int i=0; i<v_AttributesLayer.size(); i++){
		if(v_AttributesLayer[i][0]==attributeName){
			aux_answer = v_AttributesLayer[i][1];
		}
	}

	//Remove the last part (PropertyType) of the FeatureType to make it more understandable.
	String key ("PropertyType");
	size_t found;
	found=aux_answer.rfind(key);
	if (found!=string::npos){
		aux_answer.replace (found,key.length(),"");
	}
	if(aux_answer==""){
		throw ErrorObject("Attribute name not found. Check the name and try again");
	}
	return aux_answer;
}





// Auxiliar functions ***********************************************************************************************************
	// Provides the LAYER TITLE
String ConnectorWFS::getLayerTitle(const String& layerName){
	String var_aux = "";
	for (unsigned int i=0; i< vLayers.size(); i++){
		if(vLayers[i][0]==layerName){
			var_aux = vLayers[i][1]; //If found then update the value.
		}
	}
	if (var_aux=="")
		throw ErrorObject("Layer name not found on layer's list. Try refreshing the service.");
	return var_aux;
}
	// Provides the LAYER ABSTRACT
String ConnectorWFS::getLayerAbstract(const String& layerName){
	String var_aux = "";
	for (unsigned int i=0; i< vLayers.size(); i++){
		if(vLayers[i][0]==layerName){
			var_aux = vLayers[i][2]; //If found then update the value.
		}
	}
	if (var_aux=="")
		throw ErrorObject("Layer name not found on layer's list. Try refreshing the service.");
	return var_aux;
}

String ConnectorWFS::getGeometryColumn(const String& layerName){
	getAttributeNamesList(layerName);
	String var_aux = "";
	for (unsigned int i=0; i< v_AttributesLayer.size(); i++){
		if(v_AttributesLayer[i][1].rfind("gml:")==0){ //starts with gml:
			var_aux = v_AttributesLayer[i][0]; //If found then update the value.
		}
	}
	if (var_aux=="")
		throw ErrorObject("Geometry column found.");

	return var_aux;
}

String ConnectorWFS::getGeometryType(const String& layerName){
	String str_gc = getGeometryColumn(layerName);
	String var_aux = "";
	var_aux = getAttributeType(str_gc);
	//If the geometry type retrieved is "Geometry" we have to perform a getFeature request to know what kind of element it will have.
	//It is the only way because that means the geometry type is not in the schema definition. [Deegree servers!]
	//The geometry type will be considered for all the layer the first basic type that will be found (gml:Point, gml:LineString, gml:Polygon).
	//var_aux should be translated to all caps or lowercase to check perfect match and not omit any result.
	std::transform(var_aux.begin(),var_aux.end(),var_aux.begin(), ::tolower); // Function that transforms a string to lowercase.
	if (var_aux == "gml:geometry" || var_aux == "geometry"){
		String aux_gf = getFeature(layerName);
		size_t found=0;
		String key;
		key = ("gml:Point");
		found=aux_gf.find(key);
		if(found != -1){
			var_aux = key;
		}else{
			key = ("gml:LineString");
			found=aux_gf.find(key);
			if(found != -1){
				var_aux = key;
			}else{
				key = ("gml:Polygon");
				found=aux_gf.find(key);
				if(found != -1){
					var_aux = key;
				}else{
					var_aux = "UNKNOWN TYPE";
				}
			}
		}
	}
	if (var_aux==""){
		throw ErrorObject("Layer name not found on layer's list. Try refreshing the service.");
	}
	return var_aux.toLower();
}

std::vector<double> ConnectorWFS::getLayerBoundingBox(const String& layerName){
	std::vector<double> aux_result;	
	for (unsigned int i=0; i< vLayers.size(); i++){
		if(vLayers[i][0]==layerName){
			aux_result.push_back(atof(vLayers[i][4].c_str())); 
			aux_result.push_back(atof(vLayers[i][5].c_str())); 
			aux_result.push_back(atof(vLayers[i][6].c_str())); 
			aux_result.push_back(atof(vLayers[i][7].c_str())); 
		}
	}
	if (aux_result.size() == 0){
		throw ErrorObject("Layer name not found on layer's list. Try refreshing the service.");
	}
	return aux_result; //LOWERcornerLON, LOWERcornerLAT, UPPERcornerLON, UPPERcornerLAT
}

String ConnectorWFS::getLayerNameSpaceURL(const String& layerName){
	//I don't check if the layer exists or not, only the reference to the namespace.
	String aux_result = "no-url-found"; //Just in case the URL attribute is not found.
	//Extract the namespace from the layername ---> layerName = namespace:layer.
	size_t found = String::npos;
	found = layerName.find(":");
	String ns_aux = String("xmlns:").append(layerName.substr(0,found));
	if (found==String::npos) return aux_result; //If the layername doesn't contain namespace we retunt "no-url-found", in further versions we might return the default ns.
	for (unsigned int i=0; i< v_serviceNSurls.size(); i++){
		if(v_serviceNSurls[i][0]==ns_aux){
			aux_result = v_serviceNSurls[i][1].c_str(); 
		}
	}
	return aux_result;
}

void ConnectorWFS::fillAttributes(const String& geom_column_name, pugi::xpath_node_set::const_iterator& att_it,
								  std::vector<String>& aux_Attributes, map<String, ColumnInfoWFS>& columns, 
								  bool& firstRecord, int& columnCount) 
{
	pugi::xpath_node feature_att_node = *att_it;
	String cname = feature_att_node.node().name();
	cname = cname.sTail(":");
	if (cname != geom_column_name){
		ColumnInfoWFS &ci = columns[cname];
		ci.colName = cname;
		if ( firstRecord) {
			ci.index = columnCount++;
		}
		String str_field_value = feature_att_node.node().child_value();
		double val = str_field_value.rVal();
		if ( val == rUNDEF)
			ci.isNumeric = false;
		else {
			ci.isInteger = ci.isInteger && (val - (int)val) == 0;
			ci.range+= val;
		}
		if ( !ci.isNumeric)
			ci.elementCount[str_field_value]++;
		aux_Attributes.push_back(str_field_value);//Add to the vector the values for each column except the geometrycolumn.
	}
}

//Returns a matrix for being able to store MULTIELEMENTS, ALTOUGH IN THIS VERSION WON'T BE IMPLEMENTED FULLY FUNCTIONAL. For knowing if it's a multi element check the same id for more than one row in the vector.
//IMPORTANT NOTE: The connector doesn't work wit multi-elements, multi-elements are splitted to get simple ones.
std::vector<std::vector<String> > ConnectorWFS::getFeaturesFromLayer(const String& layerName, map<String, ColumnInfoWFS>& columns, const String& version, const String& filter){
	std::vector<std::vector<String> > aux_fullFeaturesList;
	std::vector<String> aux_feature;
	String aux_gf = ConnectorWFS::getFeature(layerName, version, filter);
	String aux_str = "";
	String geom_column_name = "";
	geom_column_name = getGeometryColumn(layerName);
	//We have to check if the feature type contains the namespace (element separated by :) In that case add it to geometry columns:
	String ns = "";
	size_t pos_dots = layerName.find(":");
	if(pos_dots!=String::npos){
		//This means the layer name contains namespace.
		ns = layerName.substr(0,pos_dots);
		geom_column_name = ns.append(":").append(geom_column_name);
	}	

	// Version 1.0.0 ***********************************************************************************************************
	if (version == "1.0.0"){
		throw ErrorObject("Version (1.0.0) not supported yet!");
	}
	// Version 1.1.0 ***********************************************************************************************************
	if (version == "1.1.0"){
		pugi::xml_document doc_gf;
		pugi::xml_parse_result result = doc_gf.load(aux_gf.c_str());
		//doc_gf.print(std::cout);
		pugi::xpath_node_set featureMember_node_set;
		//NOTE: Some servers have implemented this tag no standard mode, so we will search for "gml:featureMembers" too.
		featureMember_node_set = doc_gf.select_nodes("//gml:featureMember");
		if(featureMember_node_set.size()==0){
			featureMember_node_set = doc_gf.select_nodes("//gml:featureMembers");
			if(featureMember_node_set.size()==0){
				throw ErrorObject("WARNING: No <gml:featureMember> or <gml:featureMembers> tags found. Check the query.");
			}
		}

		//Determine if the geometry is composed (MULTIelement contains several elementMEMBER {"1"MultiSurface ->"n"surfaceMember})
		if (v_currentFeatureType=="gml:multisurface" || v_currentFeatureType=="gml:Polygon"){		// MultiSurface could contain n polygons, each polygon could contain island polygons.
			bool firstRecord = true;
			int columnCount=1;
			for (pugi::xpath_node_set::const_iterator it = featureMember_node_set.begin(); it != featureMember_node_set.end(); ++it){
				pugi::xpath_node featureMember_node = *it;
				pugi::xpath_node_set feature_nodes; //In case there are more than one feature per entity.
				feature_nodes = featureMember_node.node().select_nodes("*"); //Collection of features, that can be also made of multypolygons
				//from those nodes hang the different fields, so we can retrieve from here.

				
				for(pugi::xpath_node_set::const_iterator it2 = feature_nodes.begin(); it2 != feature_nodes.end(); ++it2){
					pugi::xpath_node feature_node = *it2;
					//Now we're in each one of the features, it's here where we can look for multipart or singlepart and extract it.
					//Each feature can contain several surfaceMember tags, but each surfaceMember can contain only ONE polygon,
					//So we can extract polygons directly from features as node_set.
					pugi::xpath_node_set polygon_node_set;
					polygon_node_set = feature_node.node().select_nodes(".//gml:Polygon");
					//---------------------------------------------------------------
					//Store the feature id in the gml file for further operations.
					//String fid = "";
					//fid = feature_node.node().attribute("gml:id").value();
					//std::vector<String> aux_Attributes; //Vector that contains all the attributes (except the geom) in string mode
					//aux_Attributes.clear();
					////THE FIRST ELEMENT IN THIS ATT. VECTOR WILL BE THE gml:id ATTRIBUTE, BECAUSE FOR TRANSACTIONS WILL BE USEFUL.
					//aux_Attributes.push_back(fid);
					////We use the attribute in order, that simplifies the process
					//pugi::xpath_node_set poly_attributes_set;
					//poly_attributes_set = feature_node.node().select_nodes("./*");
					//for(pugi::xpath_node_set::const_iterator att_it = poly_attributes_set.begin(); att_it!= poly_attributes_set.end(); ++att_it){
					//	pugi::xpath_node feature_att_node = *att_it;
					//	if (feature_att_node.node().name()!=geom_column_name){
					//		String str_field_value = feature_att_node.node().child_value();
					//		aux_Attributes.push_back(str_field_value);//Add to the vector the values for each column except the geometrycolumn.
					//	}
					//}
					String fid = "";
					fid = feature_node.node().attribute("gml:id").value();
					std::vector<String> aux_Attributes; //Vector that contains all the attributes (except the geom) in string mode
					aux_Attributes.clear();
					//THE FIRST ELEMENT IN THIS ATT. VECTOR WILL BE THE gml:id ATTRIBUTE, BECAUSE FOR TRANSACTIONS WILL BE USEFUL.
					aux_Attributes.push_back(fid);
					if ( firstRecord){
						columns["gmlid"].index = 1;
						columns["gmlid"].colName = "gmlid";
						columns["gmlid"].isNumeric = false;
						++columnCount;
					}
					columns["gmlid"].elementCount[fid]++;
					//We use the attribute in order, that simplifies the process
					pugi::xpath_node_set line_attributes_set;
					line_attributes_set = feature_node.node().select_nodes("./*");
					for(pugi::xpath_node_set::const_iterator att_it = line_attributes_set.begin(); att_it!= line_attributes_set.end(); ++att_it){
						fillAttributes(geom_column_name,att_it,aux_Attributes,columns, firstRecord, columnCount);
					}
					firstRecord = false;
					//---------------------------------------------------------------


					//Now we have to check if the polygon contains "holes".
					//The structure is 1 tag gml:exterior 0..n tags gml:interior (the holes)
					for(pugi::xpath_node_set::const_iterator it3 = polygon_node_set.begin(); it3 != polygon_node_set.end(); ++it3){
						pugi::xpath_node polygon_node = *it3;
						pugi::xpath_node_set interior_polygon_nodes;
						interior_polygon_nodes = polygon_node.node().select_nodes(".//gml:interior");

						String str_ext = "";
						String str_CompletePosList = "";
						str_ext = polygon_node.node().select_single_node(".//gml:posList").node().child_value();
						//std::cout << "LISTA: " << str_ext << std::endl;
						str_CompletePosList.append(str_ext);

						std::vector<String> singleFeatureParsed; //Contains in STRING mode the information relative to each single feature part.
						//Structure [GEOMLIST{EXTERIOR(,interior1, ..., interiorn)}, ATT1, ATT2.....]
						if(interior_polygon_nodes.size()>0){
							String str_int = "";
							for (pugi::xpath_node_set::const_iterator it4=interior_polygon_nodes.begin();it4!=interior_polygon_nodes.end();++it4){
								pugi::xpath_node interior_node = *it4;
								str_int = interior_node.node().select_single_node(".//gml:posList").node().child_value();
								str_CompletePosList.append(",").append(str_int);
							}
						}
						singleFeatureParsed.push_back(str_CompletePosList); //FIRST MEMBER OF THE VECTOR. [GEOMETRY LIST] [EXT,int,int...] 
						//singleFeatureParsed.push_back()//Attributes
						for (unsigned int i=0; i< aux_Attributes.size(); i++){
							singleFeatureParsed.push_back(aux_Attributes[i]);
						}
						aux_fullFeaturesList.push_back(singleFeatureParsed);
					}
				}
			}
		}
		if (v_currentFeatureType=="gml:multilinestring" || v_currentFeatureType=="gml:linestring"){		// MultiLineString could contain n linestrings.
			bool firstRecord = true;
			int columnCount=1;
			int rec=0;
			for (pugi::xpath_node_set::const_iterator it = featureMember_node_set.begin(); it != featureMember_node_set.end(); ++it){
				pugi::xpath_node featureMember_node = *it;
				pugi::xpath_node_set feature_nodes; //In case there are more than one feature per entity.
				feature_nodes = featureMember_node.node().select_nodes("*"); 
				//from those nodes hang the different fields, so we can retrieve from here.

				for(pugi::xpath_node_set::const_iterator it2 = feature_nodes.begin(); it2 != feature_nodes.end(); ++it2){
					pugi::xpath_node feature_node = *it2;
					trq.fUpdate(rec++);
					//Now we're in each one of the features, it's here where we can look for multipart or singlepart and extract it.
					//Each feature can contain several gml:lineStringMember tags, but each lineStringMember can contain only ONE line,
					//So we can extract linestrings directly from features as node_set.
					//IF lineString_node_set.size()>1 then it's a multiline element.
					pugi::xpath_node_set lineString_node_set;
					lineString_node_set = feature_node.node().select_nodes(".//gml:LineString");
					//---------------------------------------------------------------
					//Store the feature id in the gml file for further operations.
					String fid = "";
					fid = feature_node.node().attribute("gml:id").value();
					std::vector<String> aux_Attributes; //Vector that contains all the attributes (except the geom) in string mode
					aux_Attributes.clear();
					//THE FIRST ELEMENT IN THIS ATT. VECTOR WILL BE THE gml:id ATTRIBUTE, BECAUSE FOR TRANSACTIONS WILL BE USEFUL.
					aux_Attributes.push_back(fid);
					if ( firstRecord){
						columns["gmlid"].index = 1;
						columns["gmlid"].colName = "gmlid";
						columns["gmlid"].isNumeric = false;
						++columnCount;
					}
					columns["gmlid"].elementCount[fid]++;
					//We use the attribute in order, that simplifies the process
					pugi::xpath_node_set line_attributes_set;
					line_attributes_set = feature_node.node().select_nodes("./*");
					for(pugi::xpath_node_set::const_iterator att_it = line_attributes_set.begin(); att_it!= line_attributes_set.end(); ++att_it){
						fillAttributes(geom_column_name,att_it,aux_Attributes,columns, firstRecord, columnCount);
					}
					firstRecord = false;
					//---------------------------------------------------------------

					for(pugi::xpath_node_set::const_iterator it3=lineString_node_set.begin();it3!=lineString_node_set.end();++it3){
						pugi::xpath_node single_line_node = *it3;

						std::vector<String> singleFeatureParsed; //Contains in STRING mode the information relative to each single feature part.
						//Structure [GEOMLIST, ATT1, ATT2.....]
						String str_CompletePosList = single_line_node.node().select_single_node("./gml:posList").node().child_value();
						singleFeatureParsed.push_back(str_CompletePosList);
						//Add the other 'regular' attribute values to this list.
						for (unsigned int i=0; i< aux_Attributes.size(); i++){
							singleFeatureParsed.push_back(aux_Attributes[i]);
						}
						aux_fullFeaturesList.push_back(singleFeatureParsed);
					}
				}
			}
		}
		if (v_currentFeatureType=="gml:multipoint" || v_currentFeatureType=="gml:point"){		// MultiPoint could contain n points.
			bool firstRecord = true;
			int columnCount=0;
			int rec= 0;
			for (pugi::xpath_node_set::const_iterator it = featureMember_node_set.begin(); it != featureMember_node_set.end(); ++it){
				pugi::xpath_node featureMember_node = *it;
				pugi::xpath_node_set feature_nodes; //In case there are more than one feature per entity.
				feature_nodes = featureMember_node.node().select_nodes("*"); 
				//from those nodes hang the different fields, so we can retrieve from here.

				for(pugi::xpath_node_set::const_iterator it2 = feature_nodes.begin(); it2 != feature_nodes.end(); ++it2){
					trq.fUpdate(rec++);
					pugi::xpath_node feature_node = *it2;
					//Now we're in each one of the features, it's here where we can look for multipart or singlepart and extract it.
					//Each feature can contain several gml:pointMember tags, but each pointMember can contain only ONE point,
					//So we can extract points directly from features as node_set.
					//IF point_node_set.size()>1 then it's a multipoint element.
					pugi::xpath_node_set point_node_set;
					point_node_set = feature_node.node().select_nodes(".//gml:Point");
					//---------------------------------------------------------------
					//Store the feature id in the gml file for further operations.
					String fid = "";
					fid = feature_node.node().attribute("gml:id").value();
					std::vector<String> aux_Attributes; //Vector that contains all the attributes (except the geom) in string mode
					aux_Attributes.clear();
					//THE FIRST ELEMENT IN THIS ATT. VECTOR WILL BE THE gml:id ATTRIBUTE, BECAUSE FOR TRANSACTIONS WILL BE USEFUL.
					aux_Attributes.push_back(fid);
					if ( firstRecord){
						columns["gmlid"].index = 1;
						columns["gmlid"].colName = "gmlid";
						columns["gmlid"].isNumeric = false;
						++columnCount;
					}
					columns["gmlid"].elementCount[fid]++;
					//We use the attribute in order, that simplifies the process
					pugi::xpath_node_set point_attributes_set;
					point_attributes_set = feature_node.node().select_nodes("./*");
					for(pugi::xpath_node_set::const_iterator att_it = point_attributes_set.begin(); att_it!= point_attributes_set.end(); ++att_it){
						fillAttributes(geom_column_name,att_it,aux_Attributes,columns, firstRecord, columnCount);
					}
					firstRecord = false;
					//---------------------------------------------------------------

					for(pugi::xpath_node_set::const_iterator it3=point_node_set.begin();it3!=point_node_set.end();++it3){
						pugi::xpath_node single_point_node = *it3;

						std::vector<String> singleFeatureParsed; //Contains in STRING mode the information relative to each single feature part.
						//Structure [GEOMLIST, ATT1, ATT2.....]
						String str_CompletePosList = single_point_node.node().select_single_node("./gml:pos").node().child_value();
						singleFeatureParsed.push_back(str_CompletePosList);
						//Add the other 'regular' attribute values to this list.//We suppose the first column is the geometry column always, the second the gml:id
						for (unsigned int i=0; i< aux_Attributes.size(); i++){
							singleFeatureParsed.push_back(aux_Attributes[i]);
						}
						aux_fullFeaturesList.push_back(singleFeatureParsed);
					}
				}
			}
		}
		//
		//
	}
	// Version 2.1.0 ***********************************************************************************************************
	if (version == "2.0.0"){
		throw ErrorObject("Version (2.0.0) not supported yet!");
	}

	return aux_fullFeaturesList;
}

// Get the EPSG Code for one layer (The default EPSG Code obtained from capabilities document) THE SAME THAN THE SERVER PRODUCES
String ConnectorWFS::getLayerSRS_FullEPSG(const String& layerName){
	String result = "";
	for(unsigned int i=0; i<vLayers.size(); i++){
		if(vLayers[i][0]==layerName){
			result = vLayers[i][3];
		}
	}
	if(result==""){
		throw ErrorObject("CoordinateSystem NOT FOUND.");
	}
	return result;
}

// Get the EPSG Code for one layer (The default EPSG Code obtained from capabilities document) ONLY THE NUMERIC CODE!
String ConnectorWFS::getLayerSRS_OnlyEPSGCode(const String& layerName){
	String result = "";
	String aux = getLayerSRS_FullEPSG(layerName);
	if (aux==""){
		throw ErrorObject("ERROR: IMPOSSIBLE EXTRACT EPSG CODE, SRS Not retrieved successfully.");
	}else{
		size_t found=0;
		found = aux.rfind("EPSG:");
		if (found!=string::npos)
			result = aux.substr(found+5,aux.length()-found);//Number 5 is the lenght of "EPSG:" element.
		//logWFS->printToLog("SRS EPSG code retrieved");

	}
	return result;
}

//Initialize, We retrieve the basic information to ensure good performance later.
//In this connector we expect the service capabilities WON'T change while is being used.
int ConnectorWFS::initializeConnector(){
	int init_status = -1; //-1:Errors initializing, 0:No errors

	//Call the functions to fill the main vectors
	if (initializeFieldsCapabilities()==-1){ 
		throw ErrorObject("Initializing basic variables process failed.");
		return init_status;
	}
	if (getCapabilities()==-1){ 
		throw ErrorObject("Getcapabilities process failed.");
		return init_status;
	}
	init_status = 0;
	return init_status;
}

//Execute a transaction against a service. [Transactions use mandatory HTTP-POST.]
int ConnectorWFS::executeTransactionDelete(const String& layerName, std::vector<String>& vector_ids,const String& serv, const String& vers){
	int result = -1;
	const String str_nswfs = "wfs:";


	// BUILD THE URL POST 
	String url_executeTransaction = serverURL;

	// PREPARE THE XML DOCUMENT FOR SENDING
	pugi::xml_document doc;
	//pugi::xml_node node = doc.append_child("GetCapabilities"); // Create the main node
	pugi::xml_node node = doc.append_child("wfs:Transaction"); // Create the main node
	// Add the attributes
	node.append_attribute("service")   = serv.c_str();
	node.append_attribute("version")   = vers.c_str(); //GET CAPABILITIES DO NOT USE VERSION PARAMETER.
	node.append_attribute("xmlns")     = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	node.append_attribute("xmlns:wfs") = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:gml") = "http://www.opengis.net/gml";
	node.append_attribute("xmlns:ogc") = "http://www.opengis.net/ogc";
	//node.append_attribute("xsi:schemaLocation") = "http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.1.0/wfs.xsd";


	//We create the OPERATION node for the transaction, in this case DELETE NODE.
	String str_operation = "Delete";
	str_operation.insert(0, str_nswfs);
	//node.append_child("asdasd");
	//The layer is defined as attribute in the "Operation" node.
	pugi::xml_node op_node = node.append_child(str_operation.c_str());
	op_node.append_attribute("typeName") = layerName.c_str();
	pugi::xml_node filter_node = op_node.append_child("ogc:Filter");
	//Collection of elemenents to be deleted
	//**************************************************************************************** USING THE gml:id
	for (unsigned int i=0; i<vector_ids.size(); i++){
		pugi::xml_node node_id = filter_node.append_child("ogc:GmlObjectId");
		node_id.append_attribute("gml:id") = vector_ids[i].c_str();
	}
	
	//Transform the xml_document in a string that could be managed by curl
	stringstream xml_document("");
	doc.save(xml_document);
	xml_document.flush(); //Synchronizing the buffer
	String str_xml = xml_document.str();
	//str_xml = "<?xml version=\"1.0\"?><GetCapabilities service=\"WFS\" xmlns=\"http://www.opengis.net/wfs\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.1.0/wfs.xsd\"/>";
	
	doc.save_file("request.xml");
	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);// In windows, this will init the winsock stuff
	curl_handle = curl_easy_init();//get a curl handle
	if(curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url_executeTransaction.c_str());	// First set the URL that is about to receive our POST.
		//curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "service=wfs&request=getCapabilities"); //This only works with the standard curl header 
		struct curl_slist *slist = curl_slist_append(NULL, "Content-Type: text/xml; charset=utf-8");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, str_xml.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, str_xml.length());
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
	    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl_handle);// Perform the request, res will get the return code
		// Check for errors
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl_handle);// always cleanup 
	}
	curl_global_cleanup();

	// RETRIEVE THE ANSWER!
	pugi::xml_document answer_trans;
	answer_trans.load(String(chunk.memory).c_str());
	answer_trans.save_file("trans_answer.xml");
	//If the answer document contains tags "ows:Exception" then something went wrong.
	String aux_check(chunk.memory);
	size_t found = 0;
	found = aux_check.find("ows:Exception", 0);
	if (found==String::npos){
		found = aux_check.find("<Exception>", 0);
	}
	if (found!=String::npos){
		throw ErrorObject("ERROR. TRANSACTION NOT EXECUTED");
		return -1;
	}else{
		pugi::xpath_node node_summary = answer_trans.select_single_node("//wfs:totalDeleted");
	}
	free(chunk.memory);
	result = 0; //If reaches this point without crash the operation was successful.
	return result;
}

int ConnectorWFS::executeTransactionDelete(const String& layerName, const String& str_field, const String& str_value, const String& serv, const String& vers){
	int result = -1;
	const String str_nswfs = "wfs:";


	// BUILD THE URL POST 
	String url_executeTransaction = serverURL;

	// PREPARE THE XML DOCUMENT FOR SENDING
	pugi::xml_document doc;
	//pugi::xml_node node = doc.append_child("GetCapabilities"); // Create the main node
	pugi::xml_node node = doc.append_child("wfs:Transaction"); // Create the main node
	// Add the attributes
	node.append_attribute("service")   = serv.c_str();
	node.append_attribute("version")   = vers.c_str(); //GET CAPABILITIES DO NOT USE VERSION PARAMETER.
	node.append_attribute("xmlns")     = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	node.append_attribute("xmlns:wfs") = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:gml") = "http://www.opengis.net/gml";
	node.append_attribute("xmlns:ogc") = "http://www.opengis.net/ogc";
	//node.append_attribute("xsi:schemaLocation") = "http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.1.0/wfs.xsd";


	//We create the OPERATION node for the transaction, in this case DELETE NODE.
	String str_operation = "Delete";
	str_operation.insert(0, str_nswfs);
	//node.append_child("asdasd");
	//The layer is defined as attribute in the "Operation" node.
	pugi::xml_node op_node = node.append_child(str_operation.c_str());
	op_node.append_attribute("typeName") = layerName.c_str();
	pugi::xml_node filter_node = op_node.append_child("ogc:Filter");
	//**************************************************************************************** SELECTING BY ONE ATTRIBUTE VALUE
	pugi::xml_node node_property = filter_node.append_child("ogc:PropertyIsEqualTo");
	pugi::xml_node node_field = node_property.append_child("ogc:PropertyName");
	pugi::xml_node node_literal = node_property.append_child("ogc:Literal");
	node_field.append_child(pugi::node_pcdata).set_value(str_field.c_str());
	node_literal.append_child(pugi::node_pcdata).set_value(str_value.c_str());
	
	//Transform the xml_document in a string that could be managed by curl
	stringstream xml_document("");
	doc.save(xml_document);
	xml_document.flush(); //Synchronizing the buffer
	String str_xml = xml_document.str();
	//doc.save_file("request.xml");
	
	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);// In windows, this will init the winsock stuff
	curl_handle = curl_easy_init();//get a curl handle
	if(curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url_executeTransaction.c_str());	// First set the URL that is about to receive our POST.
		//curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "service=wfs&request=getCapabilities"); //This only works with the standard curl header 
		struct curl_slist *slist = curl_slist_append(NULL, "Content-Type: text/xml; charset=utf-8");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, str_xml.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, str_xml.length());
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
	    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl_handle);// Perform the request, res will get the return code
		// Check for errors
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl_handle);// always cleanup 
	}
	curl_global_cleanup();

	// RETRIEVE THE ANSWER!
	pugi::xml_document answer_trans;
	answer_trans.load(String(chunk.memory).c_str());
	answer_trans.save_file("trans_answer.xml");
	//If the answer document contains tags "ows:Exception" then something went wrong.
	String aux_check(chunk.memory);
	size_t found = 0;
	found = aux_check.find("ows:Exception", 0);
	if (found==String::npos){
		found = aux_check.find("<Exception>", 0);
	}
	if (found!=String::npos){
		throw ErrorObject("ERROR. TRANSACTION NOT EXECUTED");
	}else{
		pugi::xpath_node node_summary = answer_trans.select_single_node("//wfs:totalDeleted");
	}
	free(chunk.memory);
	result = 0; //If reaches this point without crash the operation was successful.
	return result;
}

int ConnectorWFS::executeTransactionInsert(const String& layerName, std::vector<std::vector<String>>& v_attributes, const String& serv, const String& vers){
	if (vers != "1.1.0"){
		throw ErrorObject("Version not supported yet.");
		return -1;
	}
	const String str_nswfs = "wfs:";

	// BUILD THE URL POST 
	String url_executeTransaction = serverURL;

	// PREPARE THE XML DOCUMENT FOR SENDING
	pugi::xml_document doc;
	pugi::xml_node node = doc.append_child("wfs:Transaction"); // Create the main node
	// Add the attributes
	node.append_attribute("service")   = serv.c_str();
	node.append_attribute("version")   = vers.c_str();
	node.append_attribute("xmlns")     = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	node.append_attribute("xmlns:wfs") = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:gml") = "http://www.opengis.net/gml";
	node.append_attribute("xmlns:ogc") = "http://www.opengis.net/ogc";
	node.append_attribute("xsi:schemaLocation") = "xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\">";


	//NAMESPACE OF THE LAYER MUST EXIST!
	String str_nsLayer = "";
	size_t foundns = 0;
	foundns = layerName.find(":",0);
	if (foundns != String::npos){
		str_nsLayer = layerName.substr(0,foundns);
		String aux = "xmlns:";
		aux.append(str_nsLayer);
		node.append_attribute(aux.c_str()) = getLayerNameSpaceURL(layerName).c_str();
	}

	//We create the OPERATION node for the transaction, in this case INSERT NODE.
	String str_operation = "Insert";
	str_operation.insert(0, str_nswfs);
	pugi::xml_node op_node = node.append_child(str_operation.c_str());
	pugi::xml_node layer_node = op_node.append_child(layerName.c_str());
	//layer_node.append_attribute("xmlns:fran") = "http://localhost:8080/geoserver/fran/wfs?service=WFS&version=1.1.0&request=DescribeFeatureType&typeName=fran:testpoints1";
	
	//Which kind of geometry does the layer have? (depending on the geometry we will define different node structure)
	String lyr_geomType = getGeometryType(layerName); //pasar a minusculas.
	std::transform(lyr_geomType.begin(),lyr_geomType.end(),lyr_geomType.begin(), ::tolower); // Function that transforms a string to lowercase.
	if (lyr_geomType == "gml:point" || lyr_geomType == "point" || lyr_geomType == "gml:multilinestring" || lyr_geomType == "multilinestring" || lyr_geomType == "gml:linestring" || lyr_geomType == "linestring" || lyr_geomType == "gml:polygon" || lyr_geomType == "polygon" || lyr_geomType == "gml:multipolygon" || lyr_geomType == "multipolygon" || lyr_geomType == "gml:multisurface" || lyr_geomType == "multisurface"){
		//Geometry detected, building customized transaction xml file
	}else{
		throw ErrorObject("Geometry type undefined, impossible build transaction. execute transaction canceled.");
	}
	//Add the attribute nodes. The vector of information to be added always contains the first column as geometry and the second as gml:id!
	String column_geom = getGeometryColumn(layerName);
	column_geom.insert(0,":").insert(0,str_nsLayer.c_str());
	pugi::xml_node geom_node = layer_node.append_child(column_geom.c_str());
	//First the "normal" attributes and then the geometry column.
	for (unsigned int i=2; i<v_attributes.size(); i++){
		pugi::xml_node att_node = layer_node.append_child(v_attributes[i][0].c_str());
		att_node.append_child(pugi::node_pcdata).set_value(v_attributes[i][1].c_str());
	}
	if (lyr_geomType == "gml:point" || lyr_geomType == "point"){
		pugi::xml_node point_node = geom_node.append_child("gml:Point");
		pugi::xml_node pos_node = point_node.append_child("gml:pos"); //FOR WFS 1.0.0 pugi::xml_node pos_node = point_node.append_child("gml:coordinates");
		pos_node.append_child(pugi::node_pcdata).set_value(v_attributes[0][1].c_str());//Geometry coordinates list 
	}
	//There's no support for "simple" linestring elements yet. only multiLine structures.
	if (lyr_geomType == "gml:linestring" || lyr_geomType == "linestring" || lyr_geomType == "gml:multilinestring" || lyr_geomType == "multilinestring"){
		pugi::xml_node multiElement_node = geom_node.append_child("gml:MultiLineString");
		pugi::xml_node member_node = multiElement_node.append_child("gml:lineStringMember");
		pugi::xml_node ls_node = member_node.append_child("gml:LineString");
		pugi::xml_node posl_node = ls_node.append_child("gml:posList");
		posl_node.append_child(pugi::node_pcdata).set_value(v_attributes[0][1].c_str());//Geometry coordinates list
	}
	//There's no support for "simple" linestring elements yet. only multiLine structures. Polygons with holes are supported.
	if (lyr_geomType == "gml:polygon" || lyr_geomType == "polygon" || lyr_geomType == "gml:multipolygon" || lyr_geomType == "multipolygon" || lyr_geomType == "gml:multisurface" || lyr_geomType == "multisurface"){
		//Obtain number of interior polygons (Holes), such elements are separated by ",". The first element is the exterior one.
		size_t found = 0;
		size_t lastfound = 0;
		std::vector<int> v; //Vector withe the main splitting points, includes head and tail of the sequence.
		v.push_back(0);
		while (found!=String::npos){
			found = v_attributes[0][1].find(",",lastfound);
			if (found!=String::npos){
				v.push_back(found);
				lastfound=found+1;
			}
		}
		v.push_back(v_attributes[0][1].size());
		std::vector<String> v_boundaries;
		for (unsigned int i=0; i<v.size()-1; i++){
			if (i==0){ //External boundary
				v_boundaries.push_back(v_attributes[0][1].substr(v[0],v[1]));
			}else{
				int l0 = v[i]+1;
				int l1 = v[i+1];
				v_boundaries.push_back(v_attributes[0][1].substr(l0,l1-l0));
			}
		}
		pugi::xml_node multiElement_node = geom_node.append_child("gml:MultiSurface");
		pugi::xml_node member_node = multiElement_node.append_child("gml:surfaceMember");
		pugi::xml_node polygon_node = member_node.append_child("gml:Polygon");
		pugi::xml_node exterior_node = polygon_node.append_child("gml:exterior"); //-------------------- Exterior boundary
		pugi::xml_node linear_ring_node = exterior_node.append_child("gml:LinearRing");
		pugi::xml_node posl_node = linear_ring_node.append_child("gml:posList");
		posl_node.append_child(pugi::node_pcdata).set_value(v_boundaries[0].c_str());//Geometry coordinates list
		for (unsigned int i=1; i<v_boundaries.size(); i++){
			pugi::xml_node interior_node = polygon_node.append_child("gml:interior"); //-------------------- Interior boundaries
			pugi::xml_node linear_ring_node = interior_node.append_child("gml:LinearRing");
			pugi::xml_node posl_node = linear_ring_node.append_child("gml:posList");
			posl_node.append_child(pugi::node_pcdata).set_value(v_boundaries[i].c_str());//Geometry coordinates list
		}
	}	
	
	
	//now we have to create the children for the geomertry column and for the standard attributes.
	

	/*
	layer_node.append_attribute("typeName") = layerName.c_str();
	pugi::xml_node filter_node = layer_node.append_child("ogc:Filter");
	//**************************************************************************************** SELECTING BY ONE ATTRIBUTE VALUE
	pugi::xml_node node_property = filter_node.append_child("ogc:PropertyIsEqualTo");
	pugi::xml_node node_field = node_property.append_child("ogc:PropertyName");
	pugi::xml_node node_literal = node_property.append_child("ogc:Literal");
	node_field.append_child(pugi::node_pcdata).set_value(str_field.c_str());
	node_literal.append_child(pugi::node_pcdata).set_value(str_value.c_str());
	*/

	//Transform the xml_document in a string that could be managed by curl
	stringstream xml_document("");
	doc.save(xml_document);
	xml_document.flush(); //Synchronizing the buffer
	String str_xml = xml_document.str();
	doc.save_file("request.xml");
	//str_xml = "<wfs:Transaction service=\"WFS\" version=\"1.0.0\" xmlns:wfs=\"http://www.opengis.net/wfs\" xmlns:topp=\"http://www.openplans.org/topp\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd http://www.openplans.org/topp http://localhost:8080/geoserver/wfs/DescribeFeatureType?typename=topp:tasmania_roads\"><wfs:Insert><topp:tasmania_roads><topp:the_geom><gml:MultiLineString srsName=\"http://www.opengis.net/gml/srs/epsg.xml#4326\"><gml:lineStringMember><gml:LineString><gml:coordinates decimal=\".\" cs=\",\" ts=\" \">494475.71056415,5433016.8189323 494982.70115662,5435041.95096618</gml:coordinates></gml:LineString></gml:lineStringMember></gml:MultiLineString></topp:the_geom><topp:TYPE>xxx</topp:TYPE></topp:tasmania_roads></wfs:Insert></wfs:Transaction>";


	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);// In windows, this will init the winsock stuff
	curl_handle = curl_easy_init();//get a curl handle
	if(curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url_executeTransaction.c_str());	// First set the URL that is about to receive our POST.
		//curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "service=wfs&request=getCapabilities"); //This only works with the standard curl header 
		struct curl_slist *slist = curl_slist_append(NULL, "Content-Type: text/xml; charset=utf-8");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, str_xml.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, str_xml.length());
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
	    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl_handle);// Perform the request, res will get the return code
		// Check for errors
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl_handle);// always cleanup 
	}
	curl_global_cleanup();

	// RETRIEVE THE ANSWER!
	pugi::xml_document answer_trans;
	answer_trans.load(String(chunk.memory).c_str());
	answer_trans.save_file("trans_answer.xml");
	//If the answer document contains tags "ows:Exception" then something went wrong.
	String aux_check(chunk.memory);
	size_t found = 0;
	found = aux_check.find("ows:Exception", 0);
	if (found==String::npos){
		found = aux_check.find("<Exception>", 0);
	}
	if (found!=String::npos){
		throw ErrorObject("Error. transaction not executed");
		return -1;
	}else{
		pugi::xpath_node node_summary = answer_trans.select_single_node("//wfs:totalInserted");
	}
	free(chunk.memory);
	return 0;//If reaches this point without crash the operation was successful.
}

int ConnectorWFS::executeTransactionUpdate(const String& layerName, const String& field, const String& value, std::vector<String>& vector_ids, const String& serv, const String& vers){
	if (vers != "1.1.0"){
		throw ErrorObject("Error. version not supported yet.");
		return -1;
	}
	const String str_nswfs = "wfs:";

	// BUILD THE URL POST 
	String url_executeTransaction = serverURL;

	// PREPARE THE XML DOCUMENT FOR SENDING
	pugi::xml_document doc;
	pugi::xml_node node = doc.append_child("wfs:Transaction"); // Create the main node
	// Add the attributes
	node.append_attribute("service")   = serv.c_str();
	node.append_attribute("version")   = vers.c_str();
	node.append_attribute("xmlns")     = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:xsi") = "http://www.w3.org/2001/XMLSchema-instance";
	node.append_attribute("xmlns:wfs") = "http://www.opengis.net/wfs";
	node.append_attribute("xmlns:gml") = "http://www.opengis.net/gml";
	node.append_attribute("xmlns:ogc") = "http://www.opengis.net/ogc";
	node.append_attribute("xsi:schemaLocation") = "xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\">";

	//NAMESPACE OF THE LAYER MUST EXIST!
	String str_nsLayer = "";
	size_t foundns = 0;
	foundns = layerName.find(":",0);
	if (foundns != String::npos){
		str_nsLayer = layerName.substr(0,foundns);
		String aux = "xmlns:";
		aux.append(str_nsLayer);
		node.append_attribute(aux.c_str()) = getLayerNameSpaceURL(layerName).c_str();
	}

	//We create the OPERATION node for the transaction, in this case INSERT NODE.
	String str_operation = "Update";
	str_operation.insert(0, str_nswfs);
	pugi::xml_node op_node = node.append_child(str_operation.c_str());
	pugi::xml_attribute op_att_node = op_node.append_attribute("typeName") = layerName.c_str();
	//layer_node.append_attribute("xmlns:fran") = "http://localhost:8080/geoserver/fran/wfs?service=WFS&version=1.1.0&request=DescribeFeatureType&typeName=fran:testpoints1";
	
	//We create the Property tags that contain the modifications. [On further versions we can create this as vector, so we can modify more than one field at once]
	pugi::xml_node prop_node = op_node.append_child("wfs:Property");
	pugi::xml_node prop_name_node = prop_node.append_child("wfs:Name");
	prop_name_node.append_child(pugi::node_pcdata).set_value(field.c_str());
	pugi::xml_node prop_val_node = prop_node.append_child("wfs:Value");
	prop_val_node.append_child(pugi::node_pcdata).set_value(value.c_str());
	
	//In case the ids Vector is empty there's no filtering and apply to all layer
	if(vector_ids.size()>0){
		pugi::xml_node filter_node = op_node.append_child("wfs:Filter");
		for (unsigned int i=0; i<vector_ids.size(); i++){
			pugi::xml_node n = filter_node.append_child("ogc:GmlObjectId");
			pugi::xml_attribute a = n.append_attribute("gml:id") = vector_ids[i].c_str();
		}
	}


	//Transform the xml_document in a string that could be managed by curl
	stringstream xml_document("");
	doc.save(xml_document);
	xml_document.flush(); //Synchronizing the buffer
	String str_xml = xml_document.str();
	doc.save_file("request.xml");


	// RETRIEVE THE INFORMATION 
	MemoryStruct chunk;
	chunk.memory = (unsigned char*)malloc(1);  // will be grown as needed by the realloc above
	chunk.size = 0;    // no data at this point

	CURL *curl_handle;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);// In windows, this will init the winsock stuff
	curl_handle = curl_easy_init();//get a curl handle
	if(curl_handle) {
		curl_easy_setopt(curl_handle, CURLOPT_URL, url_executeTransaction.c_str());	// First set the URL that is about to receive our POST.
		//curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, "service=wfs&request=getCapabilities"); //This only works with the standard curl header 
		struct curl_slist *slist = curl_slist_append(NULL, "Content-Type: text/xml; charset=utf-8");
		curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, slist);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, str_xml.c_str());
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, str_xml.length());
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
	    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);
		res = curl_easy_perform(curl_handle);// Perform the request, res will get the return code
		// Check for errors
		if(res != CURLE_OK){
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl_handle);// always cleanup 
	}
	curl_global_cleanup();

	// RETRIEVE THE ANSWER!
	pugi::xml_document answer_trans;
	answer_trans.load(String(chunk.memory).c_str());
	answer_trans.save_file("trans_answer.xml");
	//If the answer document contains tags "ows:Exception" then something went wrong.
	String aux_check(chunk.memory);
	size_t found = 0;
	found = aux_check.find("ows:Exception", 0);
	if (found==String::npos){
		found = aux_check.find("<Exception>", 0);
	}
	if (found!=String::npos){
		throw ErrorObject("Error. transaction not executed");
	}else{
		pugi::xpath_node node_summary = answer_trans.select_single_node("//wfs:totalUpdated");
	}
	free(chunk.memory);
	return 0;//If reaches this point without crash the operation was successful.
}


//Tags and variables.
int ConnectorWFS::initializeFieldsCapabilities(){
	int init_status = -1;
	tagsWFS110.clear();
	tagsWFS110.push_back("Abstract");
	tagsWFS110.push_back("Action");
	tagsWFS110.push_back("DefaultSRS");
	tagsWFS110.push_back("Delete");
	tagsWFS110.push_back("DescribeFeatureType");
	tagsWFS110.push_back("Feature");
	tagsWFS110.push_back("FeatureCollection");
	tagsWFS110.push_back("FeatureType");
	tagsWFS110.push_back("FeatureTypeList");
	tagsWFS110.push_back("FeaturesLocked");
	tagsWFS110.push_back("FeaturesNotLocked");
	tagsWFS110.push_back("Format");
	tagsWFS110.push_back("GMLObjectType");
	tagsWFS110.push_back("GetCapabilities");
	tagsWFS110.push_back("GetFeature");
	tagsWFS110.push_back("GetFeatureWithLock");
	tagsWFS110.push_back("GetGmlObject");
	tagsWFS110.push_back("Insert");
	tagsWFS110.push_back("InsertResults");
	tagsWFS110.push_back("Lock");
	tagsWFS110.push_back("LockFeature");
	tagsWFS110.push_back("LockFeatureResponse");
	tagsWFS110.push_back("LockId");
	tagsWFS110.push_back("Message");
	tagsWFS110.push_back("MetadataURL");
	tagsWFS110.push_back("Name");
	tagsWFS110.push_back("Native");
	tagsWFS110.push_back("NoSRS");
	tagsWFS110.push_back("Operation");
	tagsWFS110.push_back("Operations");
	tagsWFS110.push_back("OtherSRS");
	tagsWFS110.push_back("OutputFormats");
	tagsWFS110.push_back("Property");
	tagsWFS110.push_back("PropertyName");
	tagsWFS110.push_back("Query");
	tagsWFS110.push_back("ServesGMLObjectTypeList");
	tagsWFS110.push_back("SupportsGMLObjectTypeList");
	tagsWFS110.push_back("Title");
	tagsWFS110.push_back("Transaction");
	tagsWFS110.push_back("TransactionResponse");
	tagsWFS110.push_back("TransactionResults");
	tagsWFS110.push_back("TransactionSummary");
	tagsWFS110.push_back("TypeName");
	tagsWFS110.push_back("Update");
	tagsWFS110.push_back("Value");
	tagsWFS110.push_back("WFS_Capabilities");
	tagsWFS110.push_back("XlinkPropertyName");
	tagsWFS110.push_back("totalDeleted");
	tagsWFS110.push_back("totalInserted");
	tagsWFS110.push_back("totalUpdated");
	tagsWFS110.push_back("ActionType");
	tagsWFS110.push_back("BaseRequestType");
	tagsWFS110.push_back("DeleteElementType");
	tagsWFS110.push_back("DescribeFeatureTypeType");
	tagsWFS110.push_back("FeatureCollectionType");
	tagsWFS110.push_back("FeatureTypeListType");
	tagsWFS110.push_back("FeatureTypeType");
	tagsWFS110.push_back("FeaturesLockedType");
	tagsWFS110.push_back("FeaturesNotLockedType");
	tagsWFS110.push_back("GMLObjectTypeListType");
	tagsWFS110.push_back("GMLObjectTypeType");
	tagsWFS110.push_back("GetCapabilitiesType");
	tagsWFS110.push_back("GetFeatureType");
	tagsWFS110.push_back("GetFeatureWithLockType");
	tagsWFS110.push_back("GetGmlObjectType");
	tagsWFS110.push_back("InsertElementType");
	tagsWFS110.push_back("InsertResultType");
	tagsWFS110.push_back("InsertedFeatureType");
	tagsWFS110.push_back("LockFeatureResponseType");
	tagsWFS110.push_back("LockFeatureType");
	tagsWFS110.push_back("LockType");
	tagsWFS110.push_back("MetadataURLType");
	tagsWFS110.push_back("NativeType");
	tagsWFS110.push_back("OperationsType");
	tagsWFS110.push_back("OutputFormatListType");
	tagsWFS110.push_back("PropertyType");
	tagsWFS110.push_back("QueryType");
	tagsWFS110.push_back("TransactionResponseType");
	tagsWFS110.push_back("TransactionResultsType");
	tagsWFS110.push_back("TransactionSummaryType");
	tagsWFS110.push_back("TransactionType");
	tagsWFS110.push_back("UpdateElementType");
	tagsWFS110.push_back("WFS_CapabilitiesType");
	tagsWFS110.push_back("AllSomeType");
	tagsWFS110.push_back("Base_TypeNameListType");
	tagsWFS110.push_back("IdentifierGenerationOptionType");
	tagsWFS110.push_back("OperationType");
	tagsWFS110.push_back("ResultTypeType");
	tagsWFS110.push_back("TypeNameListType");

	init_status = 0;
	return init_status;
}


void ConnectorWFS::generatePointMap(){
	try{
	map<String,ColumnInfoWFS> columns;
	std::vector<vector<String>> geometries;
		trq.SetText("retrieving point data");
	Domain dom = generateBaseData(columns, "point",geometries);
	FileName fnMap(coordSys->fnObj,".mpp");

	PointMap pm(fnMap, coordSys, coordSys->cb, dom); //dm is the linking domain between features and tables.
	

	//Create the features and its geometries.
	CoordBounds bbox2;
	std::vector<double> v_coord;
	trq.SetText(TR("Filling point map"));
	long raw_index = 1; //All features in Ilwis have a “raw” value which is basically a long ( starting at index 1, 0 is reserved for the undefined value).
	for (int i=0; i<geometries.size(); i++){
		trq.fUpdate(i, geometries.size());

		raw_index++;
		v_coord.clear();
		v_coord = parseCoordinatesPoint(geometries[i][0]);
		Coord crd (v_coord[0],v_coord[1],0);
		ILWIS::Point *feature = CPOINT(pm->newFeature());
		feature->setCoord(crd);
		feature->PutVal(raw_index);
		//We increase the bbox size depending on the coordinates of the new points. //Alternative way to stablish the bbox.
		bbox2 += crd;
	}
	bbox2 *= 1.05;
	pm->SetAttributeTable(attable);
	pm->SetCoordBounds(bbox2);
	coordSys->cb = bbox2;
	trq.Stop();

	} catch(const ErrorObject& err) {
		err.Show();
	}
}

String ConnectorWFS::getServerUrl() const {
	return serverURL;
}

std::vector<double> ConnectorWFS::parseCoordinatesPoint(const std::string coordList) {
	std::vector<double> v_res;
	std::string x = "";
	std::string y = "";
	size_t pos = std::string::npos;
	pos = coordList.find(" ",0);
	x = coordList.substr(0,pos);
	y = coordList.substr(pos);
	v_res.push_back(atof(x.c_str()));
	v_res.push_back(atof(y.c_str()));
	return v_res;
}

Domain ConnectorWFS::createClassDomain(const ColumnInfoWFS& ci) {
	Domain dm(FileName::fnUnique(FileName(ci.colName,".dom")),ci.elementCount.size());
	long raw = 1;
	for(map<String, int>::const_iterator cur = ci.elementCount.begin(); cur !=  ci.elementCount.end(); ++cur){
		dm->pdsrt()->pdsrt()->SetVal(raw, (*cur).first);
		++raw;
	}
	return dm;

}

Domain ConnectorWFS::createIdDomain(const ColumnInfoWFS& ci) {
	Domain dm(FileName::fnUnique(FileName(ci.colName,".dom")),ci.elementCount.size(),dmtID);
	long raw = 1;
	for(map<String, int>::const_iterator cur = ci.elementCount.begin(); cur !=  ci.elementCount.end(); ++cur){
		dm->pdsrt()->pdsrt()->SetVal(raw, (*cur).first);
		++raw;
	}
	return dm;
}

Domain ConnectorWFS::generateBaseData(map<String,ColumnInfoWFS>& columns, const String& label,std::vector<vector<String>>& geometries) {
	String transformedName;
	for(int i=0; i< v_currentLayer.size(); ++i) {
		char lastChar = v_currentLayer[i];
		if ( lastChar == '.' || lastChar==':' || lastChar == '/' || lastChar == '\\' || lastChar ==' ' || lastChar=='-')
			transformedName += '_';
		else
			transformedName += lastChar;
	}
	FileName fnTemp(v_outputName != "" ? v_outputName : transformedName);
	FileName fnMap (FileName::fnUnique(fnTemp));
	//FileName fnMap ("PointMap2.mpp");
	FileName fnDom(fnMap, ".dom");
	FileName fnAttr(fnMap, ".tbt");
	FileName fnBBCsys(fnMap, ".csy");

	geometries = getFeaturesFromLayer(v_currentLayer, columns);
	if ( geometries.size() == 0)
		throw ErrorObject(String("No geometries reported in layer %S",v_currentLayer));
	Domain dom(fnDom, geometries.size(), dmtUNIQUEID, label);
	String srs = getLayerSRS_OnlyEPSGCode(v_currentLayer);
	coordSys = getEngine()->gdal->getCoordSystem(fnMap,atoi(srs.c_str())); //Filename, EPSG Code in INTEGER!.
	
	CoordSystem csyWGS84 ("LatlonWGS84.csy");
	
	//This way to establish the boundaries DOES NOT work. It will be recalculated adding the geometries.
	std::vector<double> bbox = getLayerBoundingBox(v_currentLayer);
	CoordBounds bb = CoordBounds(bbox[0],bbox[1],bbox[2],bbox[3]); //LOWER CORNER LON, LAT, UPPER CORNER LON, LAT.
	coordSys->cb = bb;
	CoordBounds BBox = csyWGS84->cbConv(csyWGS84, bb);
	attable = Table(fnAttr, dom);
	trq.SetText(TR("Creating domains and attribute table"));

	//Add the gml:id column.
	//Column c = attable->colNew("gmlid",Domain("string"));
	//Add the remaining tables. for now all will be strings.
	vector<String> columnNames = getAttributeNamesList(v_currentLayer, false);
	columnNames.push_back("gmlid");
	for (int i=0; i<columnNames.size(); i++){
		const ColumnInfoWFS& ci = columns[columnNames[i]];
		DomainValueRangeStruct dvs(Domain("string"));
		if ( ci.isNumeric) {
			if ( ci.isInteger) {
				dvs = DomainValueRangeStruct(Domain("value"),RangeInt((long)ci.range.rLo(), (long)ci.range.rHi()));
			}else {
				dvs = DomainValueRangeStruct("value",RangeReal(ci.range.rLo(), ci.range.rHi(),0));
			}
		}
		else if ( ci.elementCount.size() > 0 && ci.elementCount.size() < 100) {
			dvs = createClassDomain(ci);
		} else if ( ci.elementCount.size() >= 100)
			dvs = createIdDomain(ci);
		Column c = attable->colNew(columnNames[i], dvs);
		c->SetOwnedByTable(true);
	}	

	//Filling the attribute table. Attention: We consider too the gml:id property as a column in the att table!!.
	for (int i=0; i<columnNames.size(); ++i){
		//Select the column we're gonna fill
		int index = columns[columnNames[i]].index;
		Column c = attable->col(columnNames[i]);
		for (int j=0; j<geometries.size(); j++){
			c->PutVal(j+1, geometries[j][index]); 
		}
	}

	return dom;


}
void ConnectorWFS::generateLineMap(){
	map<String,ColumnInfoWFS> columns;
	std::vector<vector<String>> geometries;
	trq.SetText("retrieving segment data");
	Domain dom = generateBaseData(columns, "segment",geometries);
	FileName fnMap(coordSys->fnObj,".mps");
	
	SegmentMap sm(fnMap, coordSys, coordSys->cb, dom); //dm is the linking domain between features and tables.
	
	//Create the features and its geometries.
	CoordBounds bbox;
	//std::vector<double> v_coord;
	long raw_index = 1; //All features in Ilwis have a “raw” value which is basically a long ( starting at index 1, 0 is reserved for the undefined value).
	std::vector<Coordinate> v_coord;

	trq.SetText(TR("Filling segment map"));
	for (int count=0; count<geometries.size(); count++){
		ILWIS::Segment *feature = CSEGMENT(sm->newFeature());
		v_coord.clear();
		parseCoordinatesPolyline(geometries[count][0], v_coord);
		for (int i=0; i<v_coord.size(); i++){
			bbox += v_coord[i];
		}
		geos::geom::CoordinateSequence *seq = feature->getCoordinates(); //Initialize the sequence.
		seq->setPoints(v_coord);
		feature->PutCoords(seq);
		feature->PutVal(raw_index);
		raw_index++;

	}
	bbox *= 1.05;
	//Establish the real value for the bounding box
	sm->SetCoordBounds(bbox);
	coordSys->cb = bbox;
	coordSys->Store();
	sm->SetAttributeTable(attable);

}

//Reads the coordinate list for points and return a vector with two coords X1,Y1,X2,Y2...
void ConnectorWFS::parseCoordinatesPolyline(const String& coordList,std::vector<Coordinate>& result) {
	Array<String> parts;
	Split(coordList,parts);
	for(int i=0; i < parts.size(); i+=2) {
		result.push_back(Coordinate(parts[i].rVal(), parts[i+1].rVal()));
	}
}

void ConnectorWFS::generatePolygonMap(){
	map<String,ColumnInfoWFS> columns;
	std::vector<vector<String>> geometries;
	trq.SetText("retrieving polygon data");
	Domain dom = generateBaseData(columns, "polygon",geometries);
	FileName fnMap(coordSys->fnObj,".mpa");
	
	//This way to establish the boundaries DOES NOT work. It will be recalculated adding the geometries.
	PolygonMap pm(fnMap, coordSys, coordSys->cb, dom); //dm is the linking domain between features and tables.
	

	std::vector<geos::geom::Coordinate> coord_seq;
	double xmin=0;
	double xmax=0;
	double ymin=0;
	double ymax=0;
	long raw_index = 1;
	CoordBounds bbox;

	for (int i=0; i<geometries.size();i++){		//Iterate for all polygons in the layer.
		std::vector<std::vector<Coordinate>> boundaries;
		parseCoordinatesPolygon(geometries[i][0],boundaries);

		ILWIS::Polygon *feature = CPOLYGON(pm->newFeature());
			
		for (int j=0; j<boundaries.size(); j++){	//Iterate for all rings in the polygon.
			std::vector<Coordinate> &b = boundaries[j];
			geos::geom::CoordinateSequence *seq = new CoordinateArraySequence (); //empty_feature->getCoordinates(); //Initialize the sequence, since there's no previous data this initialization implies an empty set.
			std::vector<geos::geom::Coordinate> coord_seq;
			
			for (int k=0; k<b.size(); k++){		//Iterate for all vertex nodes that define the boundary.
				Coordinate &crd = b[k];
				coord_seq.push_back(crd);
				seq->setPoints(coord_seq);

				//Bounding box checking -----------------------
				bbox += crd;

			}
			LinearRing *ring = new LinearRing(seq, new geos::geom::GeometryFactory());
			if (j==0){
				//External polygon boundary
				feature->addBoundary(ring);
			}else{
				//Internal polygon boundaries
				feature->addHole(ring);
			}
			feature->PutVal(raw_index);
			raw_index++;
		}

	}



	bbox *= 1.05;
	//Establish the real value for the bounding box
	pm->SetCoordBounds(bbox);
	coordSys->cb = bbox;
	coordSys->Store();
	pm->SetAttributeTable(attable);

}

void ConnectorWFS::parseCoordinatesPolygon(const String& coordList,std::vector<std::vector<Coordinate>>& result){
	Array<String> boundaryParts;
	Split(coordList,boundaryParts,",");
	result.resize(boundaryParts.size());
	for(int i=0; i < boundaryParts.size(); ++i) {
		Array<String> coords;
		Split(boundaryParts[i],coords," ");
		for(int j=0; j < coords.size(); j+=2) {
			Coordinate crd(coords[j].rVal(), coords[j+1].rVal());
			result[i].push_back(crd);
		}

	}
}