#pragma once

namespace ILWIS {
	class RootDrawer;

	class WMSGetMap : public RequestHandler{
public:
	static RequestHandler *createHandler(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);

	WMSGetMap(struct mg_connection *c, const struct mg_request_info *request_info, const map<String, String>& kvps, IlwisServer *serv);
	void writeResponse() const;
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