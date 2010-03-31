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
/* $Log: /ILWIS 3.0/Calculator/Funcsdm.h $
 * 
 * 6     2-06-05 18:29 Retsios
 * [bug=6538] Added a custom function to set the domain of the output map
 * of the 2-parameter ifnotundef (previously the same function was used as
 * with ifundef)
 * 
 * 5     11/07/01 2:05p Martin
 * function cnt also works on domain sort now
 * 
 * 4     11/05/01 11:32a Martin
 * added function for checking "value domains" this will also accept color
 * domains
 * 
 * 3     26-02-01 10:56 Hendrikse
 * corrected file header (comments)
 * 
 * 2     22-02-01 3:48p Martin
 * ifnotundef used the same function to check if valid params were given
 * as ifundef. The functions are not symetric if they have two params so
 * the function of ifundef was appropriate. ifnotundef has now its own
 * function
// Revision 1.5  1998/10/08 15:29:40  Wim
// Added fSortDomains()
//
// Revision 1.4  1998-09-16 18:30:36+01  Wim
// 22beta2
//
// Revision 1.3  1997/08/26 14:43:27  Wim
// Added MinHalfPiToPiDomain, MinPiToPiDomain, ZeroToPiDomain
//
// Revision 1.2  1997-08-06 10:27:12+02  Wim
// Replaced LongDomain by LongPosDomain()
//
/* funcsdm.h
	Last change:  WK    8 Oct 98    4:27 pm
*/
extern bool fDontCare(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fValueDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fValueCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fSortDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fStringCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fStringCompatibleDomainsNoID(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fCompatibleDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fCoordDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fColorDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fIffCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fIfUndefCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fIfNotUndefCheckDomains(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern bool fString1Values(const Array<CalcVariable>& acv, int iStartIndex, int& iWrongParm);
extern void SameDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv);
extern void AddDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv);
extern void SubtractDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                            FuncMath, const Array<CalcVariable>& acv);
extern void TimesDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>& acv);
extern void DivideDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv);
extern void DivDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv);
extern void ModDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv);
extern void MinDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv);
extern void MaxDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                          FuncMath, const Array<CalcVariable>& acv);
extern void Min1To1Domain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv);
extern void Min1To1IntDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv);
extern void MinHalfPiToHalfPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv);
extern void MinPiToPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv);
extern void ZeroToPiDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>& acv);
extern void RealOrLongDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);
extern void RealDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);
extern void LongPosDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);
extern void IntDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);
extern void ImageDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);
extern void BoolDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv);
extern void BoolDomainFromRaws(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv);
extern void BoolDomainFromStrings(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath, const Array<CalcVariable>& acv);
extern void BitDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                      FuncMath, const Array<CalcVariable>& acv);
extern void FuncDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv);
extern void ExpDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv);
extern void FuncPosDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv);
extern void FuncNonNegDomains(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv);
extern void HypoDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                       FuncMath fm, const Array<CalcVariable>& acv);
extern void FuncDomainsInt(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath fm, const Array<CalcVariable>& acv);
extern void StringDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>&);
extern void IffDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>&);
extern void IfUndefDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>&);
extern void IfNotUndefDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                         FuncMath, const Array<CalcVariable>&);
extern void DegDomain(DomainValueRangeStruct& dvs, StackObjectType& sot,
                        FuncMath, const Array<CalcVariable>&);




