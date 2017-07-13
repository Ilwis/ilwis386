#pragma once

LRESULT Cmdcorrectparallax(CWnd *wnd, const String& s);

class FormCorrectParallaxMap: public FormMapCreate
{
public:
  _export FormCorrectParallaxMap(CWnd* mw, const char* sPar);
private:  
  int exec();
  int MapCallBack(Event*);
  int DemCallBack(Event*);
  int MethodCallBack(Event*);
  FieldDataType* fldMap;
  FieldDataType* fldDem;
  StaticText* stMapRemark;
  StaticText* stDemRemark;
  String sMap;
  String sDem;
  int iMeth;
  double rLatSat;
  double rLonSat;
  bool fFill;
  RadioGroup* rg;
  RadioButton *rbNearest, *rbBiLin, *rbBiCub;
  FieldReal *frLonSat;
  CheckBox* cbFill;
  Map mp;
  Map dem;
};
