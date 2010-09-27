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
void isundef_so(StackObject* soRes, const StackObject* so);

void isundefraw_so(StackObject* soRes, const StackObject* so);

void iff_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);
void ifundef2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);
void ifnotundef2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);
void ifundef3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);
void ifnotundef3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);

void time_so(StackObject* soRes, const StackObject* so);

void length_so(StackObject* soRes, const StackObject* so);

void substr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);

void leftstr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void rightstr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void instr_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void strpos_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void inrange_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);

void min2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void max2_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void min3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);

void max3_so(StackObject* soRes, const StackObject* so0, const StackObject* so1, const StackObject* so2);

void avgcol_so(StackObject* soRes, const StackObject* so);
void sumcol_so(StackObject* soRes, const StackObject* so);
void cntcol_so(StackObject* soRes, const StackObject* so);

void stdevcol_so(StackObject* soRes, const StackObject* so);
void varcol_so(StackObject* soRes, const StackObject* so);

void ttestcol_so(StackObject* soRes, const StackObject* so1, const StackObject* so2);
void stderrcol_so(StackObject* soRes, const StackObject* so);
void chisquarecol_so(StackObject* soRes, const StackObject* so1, const StackObject* so2);

void corrcol_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);
void covarcol_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void mincol_so(StackObject* soRes, const StackObject* so);

void maxcol_so(StackObject* soRes, const StackObject* so);

void ndvi_so(StackObject* soRes, const StackObject* so0, const StackObject* so1);

void string_so(StackObject* soRes, const StackObject* so);

void value_so(StackObject* soRes, const StackObject* so);

void dist_so(StackObject* soRes, const StackObject* so1, const StackObject* so2);

void dist2_so(StackObject* soRes, const StackObject* so1, const StackObject* so2);

void crdx_so(StackObject* soRes, const StackObject* so1);
void crdy_so(StackObject* soRes, const StackObject* so1);

void clrred_so(StackObject* soRes, const StackObject* so);
void clrgreen_so(StackObject* soRes, const StackObject* so);
void clrblue_so(StackObject* soRes, const StackObject* so);
void clryellow_so(StackObject* soRes, const StackObject* so);
void clrmagenta_so(StackObject* soRes, const StackObject* so);
void clrcyan_so(StackObject* soRes, const StackObject* so);
void clrgrey_so(StackObject* soRes, const StackObject* so);
void clrhue_so(StackObject* soRes, const StackObject* so);
void clrsat_so(StackObject* soRes, const StackObject* so);
void clrintens_so(StackObject* soRes, const StackObject* so);

void code_so(StackObject* soRes, const StackObject* so);
void name_so(StackObject* soRes, const StackObject* so);
void dms_so(StackObject* soRes, const StackObject* so);
void lat_so(StackObject* soRes, const StackObject* so);
void lon_so(StackObject* soRes, const StackObject* so);
void degree_so(StackObject* soRes, const StackObject* so);
void sec_so(StackObject* soRes, const StackObject* so);





