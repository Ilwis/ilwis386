#pragma once

LRESULT Cmdchangedetection(CWnd *wnd, const String& s);

class FormMapListChangeDetection : public FormGeneralApplication  
{
public:
  _export FormMapListChangeDetection(CWnd* mw, const char* sPar);
private:  
  int exec();
  int choice;
  String name;
  String mplName;
  String mplOut;
};