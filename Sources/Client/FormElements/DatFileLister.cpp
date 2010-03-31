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
/*-----------------------------------------------
   $Log: /ILWIS 3.0/FormElements/NonIlwisObjectLister.cpp $
 * 
 * 2     15/01/01 14:26 Willem
 * - Files without extension are now also accepted as valid
 * - Ilwis data files without special characters are now also rejected
 * (.isf, .fnd)
 * 
 * 1     20/11/00 12:46 Willem
 * File lister for non-Ilwis objects and non-Ilwis data files. Excluded
 * files:
 * - All ODF's
 * - All files with extensions ending in '#','!','@','$','^','%'

   NonIlwisObjectLister.cpp
   by Willem Nieuwenhuis, 11/20/00
   ILWIS Department ITC
  -----------------------------------------------*/

#include "Client\Headers\formelementspch.h"
#include "Client\FormElements\DatFileLister.h"
//#include "Tools\Filename.h"
//#include "Tools\strng.h"

bool DatFileLister::fOK(const FileName& fnFile, const String&)
{
	FileName fn(fnFile);
	String sExt = fn.sExt.toLower();
	// Only accept extensions in sFileExt()

	size_t pos = sFileExt().toLower().iPos(sExt);

	return pos != shUNDEF;
}

