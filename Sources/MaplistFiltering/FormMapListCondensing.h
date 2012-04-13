#pragma once

LRESULT CmdMapListCondensing(CWnd *wnd, const String& s);

class FormMapListCondensing : public FormGeneralApplication  
{
public:
  _export FormMapListCondensing(CWnd* mw, const char* sPar);
private:  
  int exec();
  int startIndex;
  String mplOut;
  String mplName;
  vector<String> methods;
  long methodIndex;
  int step;
};