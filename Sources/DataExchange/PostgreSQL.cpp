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
#include "Headers\toolspch.h"
#include "postgresql.h"
#include "Headers\Hs\IMPEXP.hs"

PostGreSQL::PostGreSQL(const char *conninfo, bool fThrowException) : res(NULL), conn(NULL) {
     conn = PQconnectdb(conninfo);

    /* Check to see that the backend connection was successfully made */
    if (PQstatus(conn) != CONNECTION_OK)
    {
		char *s = PQerrorMessage(conn);
		String error(String(TR("Connection to database failed: %s:").c_str(), s));

        finish();
		
		if (fThrowException)
			throw ErrorObject(error);
    }
	res = NULL;
}

PostGreSQL::PostGreSQL(PGconn *_conn) {
	res = NULL;
    conn = _conn;
}

bool PostGreSQL::fValid() {
	return conn != NULL;
}

PGconn *PostGreSQL::getConnection() {
	return conn;
}

void PostGreSQL::finish()	{
	clear();
	res = NULL;

	if ( conn != NULL)
		PQfinish(conn);
	conn = NULL;
}

PostGreSQL::~PostGreSQL() {
	finish();
}

void PostGreSQL::getNTResult(const char *query) {
	clear();

	res = PQexec(conn, query);

	ExecStatusType status = PQresultStatus(res);
	if ( !(status == PGRES_TUPLES_OK || status == PGRES_COMMAND_OK) )
    {
		const char *s = PQerrorMessage(conn);
		String error(String(TR("Query failed: %s").c_str(), PQerrorMessage(conn)));
        clear();
		throw ErrorObject(error);
    }
}

int PostGreSQL::getNumberOf(PostGreSQL::Dim d) {
	if ( d == COLUMN)
		return PQnfields(res);
	else
		return	PQntuples(res);
}

char * PostGreSQL::getValue(int row, const char* column) {
	int columnIndex = PQfnumber(res, column);
	if ( res == NULL) {
		String error(TR("No query yet executed"));
		throw ErrorObject(error);
	}
	if (columnIndex == -1) {
		String error(String(TR("No column found: %s").c_str(), column));
		throw ErrorObject(error);
	}
	if ( row < 0)
		return NULL;
	return PQgetvalue(res, row, columnIndex);
}

char * PostGreSQL::getValue(int row, int column) {
	if ( res == NULL) {
		String error(TR("No query yet executed"));
		throw ErrorObject(error);
	}
	if ( row < 0 || column < 0 )
		return NULL;

	return PQgetvalue(res, row, column);
}

bool PostGreSQL::isNull(int row, int column) {
	if ( res == NULL) {
		String error(TR("No query yet executed"));
		throw ErrorObject(error);
	}
	if ( row < 0 || column < 0 )
		return NULL;

	return 1 == PQgetisnull(res, row, column);
}

void PostGreSQL::putStringValue(char *column, const String& sV) {

}

void PostGreSQL::clear() {
	if ( res != NULL)
		PQclear(res);
	res = NULL;
}

String PostGreSQL::getFieldName(int index) {
	if ( res != NULL)
		return String(PQfname(res, index));
	return "";
}