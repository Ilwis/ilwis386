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
   $Log: /ILWIS 3.0/FormElements/FilenameLister.h $
 * 
 * 3     8/03/01 16:42 Willem
 * - Added function to retrieve all selected items
 * - Added style parameter to constructor
 * 
 * 2     27/02/01 14:34 Willem
 * Added functionality to be able to also handle internal bands in
 * maplists properly
 * 
 * 1     11/01/01 17:05 Willem
 * Added new class FilenameLister derived from selector to display
 * filenames with icons

   FilenameLister.h
   by Willem Nieuwenhuis, 1/11/01
   ILWIS Department ITC
  -----------------------------------------------*/

#ifndef ILW_FILENAMELISTER_H
#define ILW_FILENAMELISTER_H

class FilenameLister : public BaseSelector
{
public:
	_export FilenameLister(FormEntry* fe, Array<String>& as, DWORD styles = LBS_MULTIPLESEL);

protected:
	void StoreData();

	void create();

	String sName(int id);
	String sGetText();

	void GetSelectedNames(Array<String>& asSelected);

private:
	Array<String>& m_as;
};


#endif
