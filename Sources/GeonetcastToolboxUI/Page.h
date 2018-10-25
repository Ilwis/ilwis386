#pragma once

#define EDIT_FIELD_SIZE 150

class GeonetCastFrm;

class GNCPage : public FieldGroup
{
	public:
		GNCPage(GeonetCastFrm *frm, FormEntry *entry);
		void create();
		virtual void Fill(){};
		virtual void set() {};
		virtual bool directAction() { return false;}
		void setRootPath(const String& path, const String& file);
		bool isHidden() const;
		void setHidden(bool yesno);
protected:
	String rootPath;
	String iniFile;
	GeonetCastFrm *mainFrm;
	bool hidden;

};

class DataPage : public GNCPage {
public:
	DataPage(GeonetCastFrm *frm, FormEntry *parent);
	String getName();
	void setName(const String& b);
	String getScript();
	void setScript(const String& s);
	String getFormat();
	void setFormat(const String& f);
	void setFolders(const String& type);
	virtual void set() {}
	void setFolderId(const String& s) ;
	void setComment(const String& comment);
	void setRegionMap(bool * _useRegion, String * _regionMap);
	String getComment();
	void InitImport(String& ilwDir, String& pluginDir, String& gdalDir, String& utilDir,String& inputDrive, String& inputPath, String& outputDrive, String& outPutPath);
	virtual String createCommand(const String& ilwDir, const String& pluginDir, const String& gdalDir, const String& utilDir,const String& inputDrive, const String& inputPath, const String& outputDrive, const String& outPutPath);
	virtual void updateFolders(const String& in, const String& out);
	String getFolderId() const;
	void setId(const String& iid);
	String getId() const;
	String getBranch() const;
	void setBranch(const String& b);
	int RegionChanged(Event* ev);
	virtual void show(int sw);
protected:
	String name, format,script, branch;
	String time;
	String dirIn;
	String dirOut;
	String folderId;
	String comment;
	String id;
	bool * useRegion;
	String * regionMap;
	long choice;
	vector<String> timeValues;
	FieldBrowseDir *fbIn;
	FieldBrowseDir *fbOut;
	FieldString *fsIn;
	FieldOneSelectString * foString;
	CheckBox * cbRegion;
	FieldPolygonMap * fmRegion;
	bool nooutput;
	bool noinput;
};