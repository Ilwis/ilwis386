class _export DBColumnSelector : public BaseSelector
{
	friend DBChoiceForm;

public:
	DBColumnSelector(FormEntry* r, DBInfo* in, DBChoiceForm *frm);

protected:
	~DBColumnSelector();
	void create();
	int idSelect();
	void DrawItem(Event* dis);
	void Fill();
	String sName(int id);
	void StoreData();

	DBChoiceForm *dbf;
	DBInfo *info;
};

class _export DBChoiceForm : public FormWithDest
{
public:
	DBChoiceForm(CWnd *fr, DBInfo*);
	int Select(Event* Evt);
	int SetDomain(Event *);

protected:
	DBColumnSelector *cs;
	RadioGroup *gr;
	CheckBox *cb;
	DBInfo *info;
	int choice;
	bool fTableDom;
	int ids;
	bool fInSelect;

};
