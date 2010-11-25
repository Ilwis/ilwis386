#pragma once

LRESULT Cmdaggregatemaplist(CWnd *wnd, const String& s);

class FormMapAggregateMapList : public FormMapCreate  
{
public:
  _export FormMapAggregateMapList(CWnd* mw, const char* sPar);
private:  
  int exec();
  MapList mpl;
  String mplName;
  vector<string> methods;
  long methodIndex;
};