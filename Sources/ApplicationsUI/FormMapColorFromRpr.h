#pragma once

LRESULT Cmdmapcolorfromrpr(CWnd *wnd, const String& s);

class FormMapColorFromRpr : public FormMapCreate  
{
public:
  _export FormMapColorFromRpr(CWnd* mw, const char* sPar);
private:  
  int exec();
  String mpName;
  String rprName;
};