/* HoorGlassCursor
   by Wim Koolhoven
   (c) Ilwis System Development ITC
	Last change:  WK   17 Nov 95   12:09 pm
*/

class HourglassCursor
{
public:
  HourglassCursor(CWnd* w)
  : win(w)
  { 
    curSave = ::GetCursor();
    ::SetCursor(::LoadCursor(IlwWinApp()->m_hInstance, MAKEINTRESOURCE(Hourglass)));
  }
  ~HourglassCursor()
  {
    ::SetCursor(curSave);
  }
private:
  CWnd* win;  
  HCURSOR curSave;
};

 

