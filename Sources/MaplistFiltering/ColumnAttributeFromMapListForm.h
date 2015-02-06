#pragma once

LRESULT createColumnAttributeFromMapList(CWnd *wnd, const String& s);
class FieldMapList;

class FormColumnFromMapList : public FormTableCreate  
{
public:
  _export FormColumnFromMapList(CWnd* mw, const char* sPar);
private:  
  int exec();
  int setDefaultTable(Event *ev);
  MapList mpl;
  String mplName;
  vector<String> methods;
  long methodIndex;
  String sCol;

  FieldMapList *fldmpl;
  FieldString *fldTab;

};