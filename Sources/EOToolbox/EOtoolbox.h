#pragma once

struct FormatInfo {
	FileName fnInput;
	FileName fnOutput;
	String command;
	String folderId;
	String type;
	String format;
	String id;
	String filePattern;
};

void executeGNCCommand(const String& cmd);

class _export EOToolbox {
public:
	EOToolbox();
	void ReadConfigFile(FileName fnConfig);
	String gdalLocation() const;
	String utilLocation() const;
	String pluginDir() const;
	FormatInfo get(const String& id) ;
private:
	String makeLocator(const String& path) ;
	void build(pugi::xml_node node, int& count, const String& idPath);
	map<String, FormatInfo> formats;
};

