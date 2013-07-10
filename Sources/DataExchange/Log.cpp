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
//  @ File Name : Log.cpp
//  @ Date : 30/10/2012
//  @ Author : Francisco Ruiz-Lopez
//  @ Version : 1.1
//
//  ---------------------------------------------------------------------------------------
#include "Headers\toolspch.h"
#include <iostream>
#include <ctime>
#include "Log.h"
//#include <string>

//Implementing Singleton patron.
Log* Log::pInstance = NULL; //Initialize pointer

Log* Log::Instance() {
	if (pInstance == NULL){ //Is it the first call?
		pInstance = new Log; //Create the instance
	}
	return pInstance; //Return the address of the instance
}

//Constructor
Log::Log() {
	std::cout << "Creating log object 'LogWFS.txt'" << std::endl << std::endl;
}

Log::~Log() {
	std::cout << std::endl << "Destroying log object 'LogWFS.txt'" << std::endl << std::endl;
}

void Log::printToLog(const String& logline) {
	m_stream.open("LogWFS.txt",std::ios_base::app); //Open file in append mode.
	time_t now = time(NULL);
	struct tm* tmPtr;
	char tmp[40];

	tmPtr = localtime(&now);
	strftime( tmp, 40, "%Y/%m/%d %H:%M:%S", tmPtr );
	//printf( "La hora y fecha locales son: %s\n", tmp );

	//Redirect also to the console, just for development. It could easily changed.
	m_stream << tmp << " - " << logline.c_str() << std::endl;
	m_stream.flush();
	m_stream.close();
	std::cout << "[LOG] " << tmp << " - " << logline.c_str() << std::endl;
}


void Log::printToLog(const String& vectorName, const std::vector<String> v) {
	m_stream.open("LogWFS.txt",std::ios_base::app); //Open file in append mode.
	time_t now = time(NULL);
	struct tm* tmPtr;
	char tmp[40];

	tmPtr = localtime(&now);
	strftime( tmp, 40, "%Y/%m/%d %H:%M:%S", tmPtr );
	//printf( "La hora y fecha locales son: %s\n", tmp );

	//Redirect also to the console, just for development. It could easily changed.
	m_stream << tmp << " - " << "Vector " << vectorName.c_str() << " [size: " << v.size() << "]" << std::endl;			//File
	std::cout << "[LOG] " << tmp << " - Vector " << vectorName.c_str() << " [size: " << v.size() << "]" << std::endl;	//Console
	for (int i=0; i< v.size(); i++)
	{
		
		m_stream << tmp << "        + " << v[i].c_str() << std::endl;
		m_stream.flush();
		std::cout << "[LOG] " << tmp << "        + " << v[i].c_str() << std::endl;
	}
	m_stream.close();
	
}

