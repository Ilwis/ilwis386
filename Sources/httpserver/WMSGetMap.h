#pragma once

namespace ILWIS {
	class RootDrawer;

	class WMSGetMap : public RequestHandler{
public:
	WMSGetMap(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps);
	void writeResponse(IlwisServer*server=0) const;
	bool needsResponse() const;
	static long getNewId();
private:
	void init();
	void createBaseMapDrawer(RootDrawer *drw, const BaseMap& bmp, const String& type, const String& subtype) const;
	void setConfig(map<String, String>* _config);

	static long idCount;
	vector<FileName> layers;
	int width, height;
	CoordBounds cb;
};

}