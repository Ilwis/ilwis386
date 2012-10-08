#include "Client\Headers\AppFormsPCH.h"
#include "FormNetworkDistance.h"
#include "Engine\Table\tbl2dim.h"
#include "Client\ilwis.h"

LRESULT CmdNetworkDistance(CWnd *wnd, const String& s)
{
	new FormNetworkDistance(wnd, s.c_str());
	return -1;
}

FormNetworkDistance::FormNetworkDistance(CWnd* mw, const char* sPar)
: FormTableCreate(mw, TR("Network Distance of two Point Maps"))
{
	fProject = true;
	new FieldPointMap(root, TR("&Origins Point Map"), &sPointMapOrigins, new MapListerDomainType(".mpp", 0, true));
	new FieldPointMap(root, TR("&Destinations Point Map"), &sPointMapDestinations, new MapListerDomainType(".mpp", 0, true));
	new FieldPointMap(root, TR("&Network Segment Map"), &sSegmentMapNetwork, new MapListerDomainType(".mps", 0, true));
	new CheckBox(root, TR("&Project points onto segments"), &fProject);

	initTableOut(false);
	SetHelpItem("ilwisapp\\mask_points_dialog_box.htm");
	create();
}

FormNetworkDistance::~FormNetworkDistance()
{
}

int FormNetworkDistance::exec() 
{
	FormTableCreate::exec();
	FileName fn(sOutTable);
	if (fn.sExt == "")
		fn.sExt = ".ta2";
	FileName fnPointMapOrigins(sPointMapOrigins);
	sPointMapOrigins = fnPointMapOrigins.sRelativeQuoted(false,fn.sPath());
	FileName fnPointMapDestinations(sPointMapDestinations);
	sPointMapDestinations = fnPointMapDestinations.sRelativeQuoted(false,fn.sPath());
	FileName fnSegmentMapNetwork(sSegmentMapNetwork);
	sSegmentMapNetwork = fnSegmentMapNetwork.sRelativeQuoted(false,fn.sPath());
	String sExpr = String("Table2DimNetworkDistance(%S,%S,%S,%s)", sPointMapOrigins, sPointMapDestinations, sSegmentMapNetwork, fProject?"project":"noproject");

	if (fn.fExist()) {
		String sErr(TR("File %S already exists.\nOverwrite?").c_str(), fn.sFullPath(true));
		int iRet= mw->MessageBox(sErr.c_str(), TR("File already exists").c_str(), MB_YESNO|MB_ICONEXCLAMATION);
		if (iRet != IDYES)
			return 0;
	}

	// execTableOut(sExpr);  // execTableOut was not made for .ta2

	String sCmd = fn.sShortNameQuoted();
	sCmd &= " = ";
	sCmd &= sExpr;
	SetCommand(sCmd);
	try {
		Table2Dim tbOut(fn, sExpr);
		if ( tbOut.fValid() )
		{
			if (sDescr[0])
			tbOut->sDescription = sDescr;
			tbOut->Store();
			if (fShow) 
			{
				String s = "show ";
				s &= tbOut->sNameQuoted(true);
				ILWISAPP->Execute(s);
			}
		}
	}  
	catch (ErrorObject& err) {
		err.Show();
	}
	return 0;
}
