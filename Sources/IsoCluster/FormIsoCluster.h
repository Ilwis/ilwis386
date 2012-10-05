#pragma once

LRESULT Cmdisocluster(CWnd *wnd, const String& s);

class FieldMapList;

class FormIsoCluster : public FormMapCreate  
{
public:
  _export FormIsoCluster(CWnd* mw, const char* sPar);
private:  
  int exec();
  int changeInput(Event *ev);
  FormEntry* CheckData() ;
  String mplName;
  byte numclasses;
  long minsamplesize;
  long iter;
  double stdev;
  double mindist;
  long maxlump;
  FieldMapList *fmpl;
  FieldInt *fsamplesize;
  FieldInt *fclass;
  FieldInt *fiter;
  FieldReal *fstdev;
  FieldReal *fmindist;
  FieldInt *fmaxlump;
};