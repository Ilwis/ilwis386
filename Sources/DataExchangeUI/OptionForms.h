
class ConnectorWFS ;

class PostGresOptions : public FormExtraImportOptions {
public:
	PostGresOptions(CWnd *parent);
	String sGetExtraOptionsPart(const String& currentExp);
	FormEntry *CheckData();
	virtual bool fValidData();
private:
	FieldString *fsUserName;
	FieldString *fsPassword;
	FieldString *fsSchema;
	String userName;
	String passWord;
	String schema;
};

class AsterOptions : public FormExtraImportOptions {

public:
	AsterOptions(CWnd *p);
	String sGetExtraOptionsPart(const String& currentExp);
	bool fValidData();
private:
	CheckBox *cb;
	bool fRadiances;

};

class WFSOptions : public FormExtraImportOptions {
public:
	WFSOptions(CWnd *p);
	~WFSOptions();
	String sGetExtraOptionsPart(const String& currentExp);
	void setInput(const String& inp);

private:
	int doGetCapabilities(Event *ev);
	int doSetAbstract(Event *ev);
	FieldString *gcap;
	String gcapurl;
	FieldOneSelectString *fldLayers;
	FieldStringMulti *fldAbstract;
	FieldString *fldGmlType;
	vector<String> layernames;
	long layer;
	String layerAbstract;
	String featureType;
	ConnectorWFS *conn;

};