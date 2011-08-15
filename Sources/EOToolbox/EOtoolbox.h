#pragma once

struct FormatInfo {
	FileName fnInput;
	FileName fnOutput;
	String command;
	String folderId;
	String type;
	String format;
};

class EOToolbox {
public:
	EOToolbox();
	void ReadConfigFile(FileName fnConfig);
	String gdalLocation() const;
	String utilLocation() const;
	String pluginDir() const;
	FormatInfo get(const String& id) ;
private:
	String makeId(const String& path) ;
	void build(pugi::xml_node node,  String current, int& count);
	map<String, FormatInfo> formats;
};

