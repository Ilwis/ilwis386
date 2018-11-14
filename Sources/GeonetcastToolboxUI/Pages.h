#pragma once

class PageType1 : public DataPage {
public:
	PageType1(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
private:
	int queryServer(Event *);
};

class PageType2 : public DataPage {
public:
	PageType2(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
private:
	int days;
};

class PageType3 : public DataPage {
public:
	PageType3(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
};

class PageType4 : public DataPage {
public:
	PageType4(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
};

class PageType5 : public DataPage {
public:
	PageType5(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
	bool directAction() { return false; }
};

class PicturePage : public GNCPage {
public:
	PicturePage(GeonetCastFrm *f, FormEntry *);
	void setPicture(FileName fn, int w, int h, const bool resampleBicubic);
    bool directAction() { return false; }

};

class DirectoryPage : public GNCPage {
public:
	DirectoryPage(GeonetCastFrm *f, FormEntry *);
	void setInput(String dir);
	void setOutput(String dir);
	String makePath(String p);
	void set();
	void setPages(vector<GeonetCastFrm::Page>* p);
	bool directAction() { return false; }
private:
	int save(Event *);
	String inputDir, outputDir;
	String path;
	vector<GeonetCastFrm::Page>* pages;
	FieldBrowseDir *fb1;
	FieldBrowseDir *fb2;
	bool hasInput;
	bool hasOutput;
	String folderId;
};

class AnglePage : public DataPage {
public:
	AnglePage(GeonetCastFrm *f, FormEntry *);
	void setOutput(String dir);
	void set();
    void setYearDayMonthHour(int yr, int mth, int dy,double hr);
private:
	int make(Event *ev);

	int year,day,month;
	double hour;
};

class StopAnimationPage : public DataPage {
public:
	StopAnimationPage(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int Stop(Event *);

};

class MultiLayerHDF4 : public DataPage {
public:
	MultiLayerHDF4(GeonetCastFrm *f, FormEntry *);
	void set();
private:
	int Export(Event *ev);
	int BrowseClick(Event*);
	  FieldDataTypeCreate *fdt;

	  String curDir;
	  String m_date;
	  String sInput;
	  int type;
	  String bands;
	  String sOutput;
};

class LLCornerPage : public DataPage {
public:
	LLCornerPage(GeonetCastFrm *f, FormEntry *);
	virtual void set();
	int import(Event *ev);
private:
	String setString(const String& f1);
	String createString(const String& part, const String& value);
	String createCommand(const String& ilwDir, const String& pluginDir, const String& gdalDir, const String& utilDir,const String& inputDrive, const String& inputPath, const String& outputDrive, const String& outputPath);
	Coord crd;
	String first, second;
	String s1,s2;
};






