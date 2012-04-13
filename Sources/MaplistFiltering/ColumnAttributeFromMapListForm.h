#pragma once

LRESULT createColumnAttributeFromMapList(CWnd *wnd, const String& s);

class FormColumnFromMapList : public FormTableCreate  
{
public:
  _export FormColumnFromMapList(CWnd* mw, const char* sPar);
private:  
  int exec();
  MapList mpl;
  String mplName;
  vector<String> methods;
  long methodIndex;
  String sCol;

};