class PostGresOptions : public FormExtraImportOptions {
public:
	PostGresOptions(CWnd *parent);
	String sGetExtraOptionsPart(const String& currentExp);
	FormEntry *CheckData();
	virtual bool fValidData();
private:
	FieldString *fsUserName;
	FieldString *fsPassword;
	String userName;
	String passWord;
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