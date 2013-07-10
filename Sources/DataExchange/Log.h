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
//  @ File Name : Log.hpp
//  @ Date : 30/10/2012
//  @ Author : Francisco Ruiz-Lopez
//  @ Version : 1.1
//
//  ---------------------------------------------------------------------------------------


#if !defined(_LOG_H)
#define _LOG_H


#include <fstream>
#include <vector>


//Implementing Singleton patron.
class Log {
	public:
		static Log* Instance();
		~Log();
		void printToLog(const String& logline);
		void printToLog(const String& vectorName, const std::vector<String> v);
	protected:
		Log(); //Constructor
		//Log(const Log &);
		//Log &operator= (const Log &);
	private:
		std::ofstream m_stream;
		static Log* pInstance;
};

#endif  //_LOG_H
