#pragma once

#include "Engine\Base\DataObjects\URL.h"

LRESULT Cmdgeonetcastmanager(CWnd *wnd, const String& s);
class TreeSelector;
class GNCPage;
class URL;
class StationSearchPage;
class StationOutputPage;
class FinderPage;
class DataPage;

class _export GeonetCastFrm : public FormWithDest
{
  public:
	  enum PageTypes{ptImportMoment, ptPicture,ptDirectory, ptImportMomentJulian, ptLast24Hrs, ptInOut,ptDec, ptAngle, ptStop, 
		  ptMLHDF4, ptExpGTiff, ptProdNav, ptDataManager, ptMSGDataRetriever, ptJason2, ptAVHRR, ptImportMomentOrbit, ptExpR, ptImportMomentNoInput, 
		  ptUrlPage,ptStationPage,ptStationSearchPage,ptLLCorner,
		  ptUnknown};

	struct Page
	{
		Page(const String& sN, GNCPage *p) : sName(sN), page(p) {}

		GNCPage *page;
		String    sName;
	};

	GeonetCastFrm(const String& title, const String& type);
	~GeonetCastFrm() {};
	GNCPage *GetPage(const String& sVal);
	String getSpecialLocation(const String& s) const;
	URL getServer() const;
	bool fRemote() const;
	const vector<DataPage *> getKeyPages(const String& key) const;
	void SetPage(const String& sVal, bool expand = false) ;

protected:
	TreeSelector			 *tree;
	vector<Page>             pages;
	set<String>			    folders;
	map<String, String>     specialLocations;
	bool	  		         fInitial;
	GNCPage					 *curPage;
	Directory				 workingDir;
	String					xmlVersion;
	String rootpath, iniFile;

	int Fill(Event*);
	int dummy(Event *ev);
	GNCPage * pageFactory(PageTypes type);
	void StartPage();
	PageTypes getType(const String& type);
	map<String, vector<DataPage *> > pageKeys;

	void build(pugi::xml_node node, String current, String idPath, const String& keyU);
	void parseSearchNode(pugi::xml_node node,StationSearchPage* spage);
	void parseStationNode(pugi::xml_node node,StationOutputPage* spage) ;
	void ReadConfigFile(FileName fn);
	String removeWhiteSpace(const String& value);
	String parseKeywordNode(pugi::xml_node node, const String& keysUpp);
	String	formType;
	bool useRemote;
	URL remoteServer;
	FinderPage *fp;
};

typedef vector<GeonetCastFrm::Page>::iterator PageIter;