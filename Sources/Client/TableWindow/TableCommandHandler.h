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
// TableCommandHandler.h: interface for the TableCommandHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABLECOMMANDHANDLER_H__D2A788F4_2E39_11D3_B78B_00A0C9D5342F__INCLUDED_)
#define AFX_TABLECOMMANDHANDLER_H__D2A788F4_2E39_11D3_B78B_00A0C9D5342F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class TableDoc;

class TableCommandHandler : public BaseCommandHandlerUI  
{
public:
	TableCommandHandler();
	virtual ~TableCommandHandler();
	virtual LRESULT Execute(const String& sCmd, CWnd *parent = NULL);	 
private:
	bool fCmdSimpleCalc(const String& sCmd);
 // void CmdTblStats(const String& s);
 // void CmdAggregate(const String& s);
 // void CmdJoin(const String& s);
 // void CmdLeastSquares(const String& s);
 // void CmdCumulative(const String& s);
 // void CmdSemivariogram(const String& s);
	//void CmdGraph(const String& s);
	//void CmdRoseDiagram(const String& s);
 // void CmdUpdateAllColumns(const String& s);
 // void CmdSortOn(const String& s);
 // void CmdSlicing(const String& s);
 // void CmdProp(const String& s);
 // void CmdAdditionalInfo(const String& s);
 // void CmdAddRecord(const String& s);
 // void CmdColMan(const String& s);
 // void CmdConfMat(const String& s);
	//void CmdInvMol(const String& s);
	//void CmdFindDatumShifts(const String& s);
	//void CmdFind7Parameters(const String& s);
	//void CmdFindAzimuthAndDistance(const String& s);
	TableDoc* tbldoc;
};

#endif // !defined(AFX_TABLECOMMANDHANDLER_H__D2A788F4_2E39_11D3_B78B_00A0C9D5342F__INCLUDED_)
