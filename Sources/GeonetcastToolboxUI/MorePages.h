class ExportGeoTiff : public DataPage {
public:
	ExportGeoTiff(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int BrowseClick(Event*)  ;
	FieldDataTypeCreate *fdt;
	String sInput;
	String sOutput;
	int Export(Event *ev);
};

class ExportR : public DataPage {
public:
	ExportR(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int BrowseClick(Event*)  ;
	FieldDataTypeCreate *fdt;
	String sInput;
	String sOutput;
	int Export(Event *ev);
};

class ProductNavigator : public DataPage {
public:
	ProductNavigator(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int start(Event *ev);
};

class DataManagerPage : public DataPage {
public:
	DataManagerPage(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int start(Event *ev);
};

class MSGDataRetrieverPage : public DataPage {
public:
	MSGDataRetrieverPage(GeonetCastFrm *f, FormEntry *);
	void set();
	virtual void updateFolders(const String& in, const String& out);
	void saveDSFile(CString sLocation, CString sSatellite, CTime tFirstDay, CTime tLastDay, bool fInSameFolder, bool fDailyIncrement, bool fTilToday);
private:
	int start(Event *ev);
};

class Jason2Page : public DataPage {
public:
	Jason2Page(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int BrowseClickIn(Event*)  ;
	int BrowseClickOut(Event*)  ;
	FileName GetPath(FieldDataTypeCreate *fdt);
	FieldDataTypeCreate *fdtIn, *fdtOut;
	int import(Event *ev);
	String sInput;
	String sOutput;
	int resolution;
};

class AVHRRPage : public DataPage {
public:
	AVHRRPage(GeonetCastFrm *f, FormEntry *);
	void set();
private:
  int BrowseClickIn(Event*)  ;
  int BrowseClickOut(Event*)  ;
  FileName GetPath(FieldDataTypeCreate *fdt);
  FieldDataTypeCreate *fdtIn, *fdtOut;
  int import(Event *);
  String sInput;
  String sOutput;
};

class TimePlusOrbit : public DataPage {
public:
	TimePlusOrbit(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
private:
	int orbit;
};

class ImportMomentNoInput : public DataPage {
public:
	ImportMomentNoInput(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
};

class SpecialLocationsPage : public GNCPage {
	public:
	SpecialLocationsPage(GeonetCastFrm *f, FormEntry *entry, map<String, String>& _specialLocations);
	void set();
	bool directAction() { return false; }
private:
	int save(Event *ev);
	int select(Event *);

	FieldString *fs;
	map<String, String>& specialLocations;
	String locationIrfanview;
	String locationFire;
};

class URLPage : public DataPage {
public:
	URLPage(GeonetCastFrm *f, FormEntry *entry);
	void set();
	bool directAction() { return false; }
private:
	int save(Event *ev);
};

class StationOutputPage : public DataPage {
public:
	StationOutputPage(GeonetCastFrm *f, FormEntry *e);
	void set();
	bool directAction() { return false; }
	void setSource(const String& s);
private:
	int import(Event *ev);
	int metadata(Event *ev);
	int year;
	int station;
	String source;

};

class StringArrayLister;

struct SearchInfo {
	int keyColumn;
	int dataColumn;
	String tag;
};


class StationSearchPage : public DataPage  {
public:
	StationSearchPage(GeonetCastFrm *f, FormEntry *e);
	void set();
	void addSearchInfo(SearchInfo& inf);
	void setSource(const String&);
protected:
	int load(Event *ev);
	static UINT loadSource(void *p) ;
	void loadSource();
	int setTerm(Event *ev);
	int setFilter(Event *ev) ;
	String dataSource;
	StringArrayLister *lister;
	vector<SearchInfo> infos;
	vector<map<String,String>> searchMaps;
	vector<String> values;
	Array<String> allcodes;
	Array<String> codes;
	String term;
	String key, oldKey;
	long index;
	FieldOneSelectString *fsTerm;
	FieldString *fsKey;
};

class FinderPage: public GNCPage {
public:
	FinderPage(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	void setKeys(const vector<string>& keys);
private:
	int addAnd(Event *ev);
	int addOr(Event *ev);
	int find(Event *ev);
	int clear(Event *ev);
	int handleKeySelection(Event *ev);
	int handleSelection(Event *ev);
	void handleAnd(vector<DataPage *>& tempPages, const String& key);
	void handleOr(vector<DataPage *>& tempPages, const String& key);
	long choice;
	String query;
	vector<String> keywords;
	vector<String> words;
	Array<String> resultString;
	vector<DataPage *> pages;
	FieldOneSelectString *fos;
	FieldString *fsm;
	StringArrayLister *lst;
};


