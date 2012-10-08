#ifndef FORMNETWORKDISTANCE_H
#define FORMNETWORKDISTANCE_H
#include "Client\Forms\frmtblcr.h"

LRESULT CmdNetworkDistance(CWnd *wnd, const String& s);

class _export FormNetworkDistance: public FormTableCreate
{
public:
  FormNetworkDistance(CWnd* mw, const char* sPar);
  virtual ~FormNetworkDistance();
private:
  int exec();
  String sPointMapOrigins;
  String sPointMapDestinations;
  String sSegmentMapNetwork;
  bool fProject;
};

#endif // FORMNETWORKDISTANCE_H