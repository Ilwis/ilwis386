#pragma once

LRESULT createTableFromMapList(CWnd *wnd, const String& s);

class FormTableFromMapList : public FormTableCreate  
{
public:
  _export FormTableFromMapList(CWnd* mw, const char* sPar);
private:  
  int exec();
  MapList mpl;
  String mplName;
  vector<String> methods;
  long methodIndex;

};