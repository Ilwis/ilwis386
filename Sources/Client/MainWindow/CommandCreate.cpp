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
#include "Client\Headers\AppFormsPCH.h"
#include "Client\FormElements\FunctionCreateForm.h"
#include "Client\ilwis.h"
#include "Engine\Domain\dm.h"
#include "Engine\Base\System\commandhandler.h"
#include "Client\FormElements\fldmap.h"
#include "Client\FormElements\fldtbl.h"
#include "Client\FormElements\fld2dtbl.h"
#include "Client\FormElements\fldfil.h"
#include "Client\FormElements\flddom.h"
#include "Client\FormElements\fldrpr.h"
#include "Client\FormElements\fldmsmpl.h"
#include "Client\FormElements\fldlist.h"
#include "Client\FormElements\fldgrf.h"
#include "Client\FormElements\fldcs.h"
#include "Engine\SpatialReference\Gr.h"
#include "Headers\Hs\DAT.hs"

// Create stuff

void EditObject(const FileName& fn)
{
	IlwWinApp()->OpenDocumentFile(fn.sFullNameQuoted().scVal(), IlwisDocument::otEDIT);
}

LRESULT Cmdcreatemap(CWnd *parent, const String& parm )
{
	String sDom = parm.sHead(",");
	String sGrf = parm.sTail(",");
	String sMap;
	bool fOk;
	{
		FormCreateMap frm(parent, &sMap, sGrf, sDom);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{
		FileName fn(sMap);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreateseg(CWnd *parent, const String& parm)
{
	String sDomain = parm.sHead(",");
	String sCsys = parm.sTail(",");
	String sMap;
	bool fOk;
	{
		FormCreateSeg frm(parent, &sMap, sCsys, sDomain);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{
		FileName fn(sMap);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreatepol(CWnd *parent, const String& parm)
{
	String sDomain = parm.sHead(",");
	String sCsys = parm.sTail(",");
	String sMap;
	bool fOk;
	{
		FormCreatePol frm(parent, &sMap, sCsys, sDomain);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{
		FileName fn(sMap);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreatepnt(CWnd *parent, const String& parm)
{
	String sDomain = parm.sHead(",");
	String sCsys = parm.sTail(",");

	String sMap;
	bool fOk;
	{
		FormCreatePnt frm(parent, &sMap, sCsys, sDomain);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{
		FileName fn(sMap);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreatetbl(CWnd *parent, const String& sDomain)
{
	bool fOk;
	String sTbl;
	String sDom = sDomain;
	if ("" == sDom)
		sDom = "none.dom";
	{
		FormCreateTable frm(parent, &sTbl, sDom);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sTbl, ".tbt");
		EditObject(fn);
	} 
	return -1;
}

LRESULT Cmdcreatetb2(CWnd* parent, const String& sDomain)
{
	bool fOk;
	String sTbl;
	String sDom = sDomain;
	{
		FormCreateTable2Dim frm(parent, &sTbl, sDom);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{
		FileName fn(sTbl, ".ta2");
		EditObject(fn);
	} 
	return -1;
}

LRESULT Cmdcreatedom(CWnd* parent, const String& dummy)
{
	String sDom;
	bool fOk;
	{
		FormCreateDomain frm(parent, &sDom);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sDom);
		if (Domain::dmt(fn) != dmtVALUE)
			EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreaterpr(CWnd* parent, const String& sDomain)
{
	String sRpr;
	String sDom = sDomain;
	if ("" == sDom)
		sDom = "value.dom";
	bool fOk;
	{
		FormCreateRepresentation frm(parent, &sRpr, Domain(sDom));
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sRpr);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreatecsy(CWnd* parent, const String& dummy)
{
	String sCsy;
	bool fOk;
	{
		FormCreateCoordSystem frm(parent, &sCsy);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sCsy, ".csy");
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreatefil(CWnd* parent, const String& sDomain)
{
	String sFilter;
	bool fOk;
	{
		FormCreateFilter frm(parent, &sFilter);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sFilter);
		EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreateisl(CWnd* parent, const String& dummy)
{
	IlwWinApp()->OpenDocumentScript("");

	return -1;
}

LRESULT Cmdcreatefun(CWnd* parent, const String& dummy)
{
  String sFunction;
  bool fOk;
  {
    FormCreateFunction frm(parent, &sFunction);
    fOk = frm.fOkClicked();
  }
  if (fOk) {  
    FileName fn(sFunction);
    EditObject(fn);
  }
	return -1;
}

LRESULT Cmdcreatempl(CWnd* parent, const String& dummy)
{
	String sMpl;
	bool fOk;
	{
		FormCreateMapList frm(parent, &sMpl);
		fOk = frm.fOkClicked();
	}
	return -1;
}

LRESULT Cmdcreatesms(CWnd* parent, const String& parm)
{
	String sMpr = parm.sHead(",");
	String sMpl = parm.sTail(",");
	String sSms;
	bool fOk;
	{
		FormCreateSampleSet frm(parent, &sSms, sMpr, sMpl);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sSms, ".sms");
		if (fn.fExist())
			EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreategrf(CWnd* parent, const String& sCsy)
{
	String sGrf;
	bool fOk;
	{
		CoordSystem cs;
		try 
		{
			if ("" != sCsy)
				cs = CoordSystem(sCsy);
		}
		catch (...)
		{
		}

		FormCreateGeoRef frm(parent, &sGrf, cs, CoordBounds(), false);
		fOk = frm.fOkClicked();
	}
	if (fOk) 
	{  
		FileName fn(sGrf, ".grf");
		GeoRef gr(fn);
		if (gr->pgc() == 0)  // GeoRefCorners editor is included in create form, so skip edit
			EditObject(fn);
	}
	return -1;
}

LRESULT Cmdcreateioc(CWnd* parent, const String& sN)
{
	String sTail = sN.sTail(" ");
	String sHead = sN.sHead(" ");
	String sName = sHead;
	String sDescription;
	if (sName == "ioc")
	{
		class NameForm : public FormWithDest
		{
		public:
			NameForm(CWnd *wnd, String& sVal) 
				: FormWithDest(wnd, SDATTitleCreateObjectCollection)
			{
				SetMenHelpTopic("ilwismen\\create_an_object_collection.htm");
				iImg = IlwWinApp()->iImage(".ioc");
			
				new FieldDataTypeCreate(root, SDATTitleObjectCollection, &sVal, ".ioc", false);
				StaticText *st = new StaticText(root, SDATMsgDescription);
				st->psn->SetBound(0,0,0,0);
				FieldString *fsDesc = new FieldString(root, "", &m_sDesc);

				fsDesc->SetWidth(200);
				fsDesc->SetIndependentPos();
				create();
			}
			String sDescription()
			{
				return m_sDesc;
			}
		private:
			String m_sDesc;
		};
		sName = "";
		NameForm frm(parent, sName);
		if (!frm.fOkClicked())
			return -1;

		sDescription = frm.sDescription();
	}
	FileName fn = FileName(sName);
	fn.sExt = ".ioc";
	ObjectCollection oc(fn, "ObjectCollection", ParmList());
	oc->SetDescription(sDescription);
	if ( sTail == "") {
		oc->Store();
		EditObject(fn);
	} else {
		Array<String> files;
		Split(sTail, files,",");
		for(int i = 0; i< files.size(); ++i) {
			String f = files[i].sTrimSpaces();
			oc->Add(FileName(f));
		}
		oc->Store();
	}

	return -1;
}
