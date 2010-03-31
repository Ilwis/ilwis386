/***************************************************************
 ILWIS integrates image, vector and thematic data in one unique 
 and powerful package on the desktop. ILWIS delivers a wide 
 range of feautures including import/export, digitizing, editing, 
 analysis and display of data as well as production of 
 quality mapsinformation about the sensor mounting platform
 
 Exclusive rights of use by 52°North Initiative for Geospatial 
 Open Source Software GmbH 2007, Germany

 Copyright (C) 2007 by 52°North Initiative for Geospatial
 Open Source Software GmbH

 Author: Jan Hendrikse, Willem Nieuwenhuis,Wim Koolhoven 
 Bas Restsios, Martin Schouwenburg, Lichun Wang, Jelle Wind 

 Contact: Martin Schouwenburg; schouwenburg@itc.nl; 
 tel +31-534874371

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt); if not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the web page of the Free
 Software Foundation, http://www.fsf.org.

 Created on: 2007-02-8
 ***************************************************************/
#ifndef EVENTS_H
#define EVENTS_H

#include "Engine\Base\DataObjects\strng.h"
#include "Client\Base\ZappTools.h"
#include "Headers\dragdrop.h"


// Messages are needed to identify message uniquely as e.g command messages
// may have a code (notification) with it that make the pair command + notification unique
class Message
{
  public:
     Message(unsigned int iMes=-1, unsigned int extra=-1) : // -1 == 0xffffff
      first(iMes),
      second(extra)
    {}

    const bool operator==(const Message& mp) const
      { return mp.first == first && mp.second==second; }

  unsigned int first, second;
};

class Notify : public Message
{
  public:
    Notify(unsigned int iNot) :
      Message(WM_COMMAND, iNot)
    {}
};


class Event
{
  public:
    Event(unsigned int iM=-1, WPARAM wp=0, LPARAM lp=0) : wParm(wp), lParm(lp), iMessage(iM) {}
    WPARAM          wParm;
    LPARAM          lParm;
    unsigned int    iMessage;

    unsigned short iLowW() { return LOWORD(wParm); }
    unsigned short iHighW() { return HIWORD(wParm); }

    virtual const Message message() const { return Message(iMessage, -1); }
};

class NotificationEvent : public Event
{
  public:
    NotificationEvent(unsigned short iLW, unsigned short iNotification, LPARAM h=0) :
      Event(WM_COMMAND, MAKELONG(iLW, iNotification), h) {}

    virtual const Message message() const { return Message(iMessage, HIWORD(wParm)); }
};
       
class ButtonClickEvent : public NotificationEvent
{
  public:
    ButtonClickEvent(unsigned short id, HANDLE winHandle) :
      NotificationEvent(id, BN_CLICKED, (LPARAM) winHandle) {}
};

class DrawItemEvent : public Event
{
  public:
    DrawItemEvent(UINT iCtlId, DRAWITEMSTRUCT* dis) :
      Event(WM_DRAWITEM, iCtlId, (WPARAM) dis) {}
};

class MouseMoveEvent : public Event
{
public:
  MouseMoveEvent(UINT nFlags, CPoint point) :
    Event(WM_MOUSEMOVE,nFlags, MAKELONG(point.x, point.y)) {}
};

class MouseClickEvent : public Event
{
  public:
    MouseClickEvent(unsigned int iMes, WPARAM nFlags, CPoint point) :
      Event(iMes, nFlags, MAKELONG(point.x, point.y)) {}

  bool fCtrlPressed()  { return wParm && MK_CONTROL; }
  bool fShiftPressed() { return wParm && MK_SHIFT; }
  CPoint pos() { return CPoint((signed short)LOWORD(lParm), (signed short)HIWORD(lParm)); }
};
       
class MouseLBDownEvent : public MouseClickEvent
{
public:
  MouseLBDownEvent(UINT nFlags, CPoint point) :
    MouseClickEvent(WM_LBUTTONDOWN,nFlags, point) {}
};

class MouseRBDownEvent : public MouseClickEvent
{
public:
  MouseRBDownEvent(UINT nFlags, CPoint point) :
    MouseClickEvent(WM_RBUTTONDOWN,nFlags, point) {}
};

class MouseLBUpEvent : public MouseClickEvent
{

public:
  MouseLBUpEvent(UINT nFlags, CPoint point) :
    MouseClickEvent(WM_LBUTTONUP,nFlags, point) {}
};

class MouseRBUpEvent : public MouseClickEvent
{

public:
  MouseRBUpEvent(UINT nFlags, CPoint point) :
    MouseClickEvent(WM_RBUTTONUP,nFlags, point) {}
};


class CBDoubleClickEvent : public NotificationEvent
{
  public:
    CBDoubleClickEvent(int idCB, HWND handleCB  ) :
      NotificationEvent(idCB, CBN_DBLCLK, (LPARAM) handleCB) {}

};

class LBDoubleClickEvent : public NotificationEvent
{
  public:
    LBDoubleClickEvent(int idCB, HWND handleCB  ) :
      NotificationEvent(idCB, LBN_DBLCLK, (LPARAM)handleCB) {}

};


class EditChangeEvent : public NotificationEvent
{
  public:
    EditChangeEvent(unsigned short id, HANDLE winHandle) :
      NotificationEvent(id , EN_CHANGE, (LPARAM)winHandle)  {}

};

class DragEvt: public Event
{
public:
  DragEvt(unsigned int iM=-1, WPARAM wp=0, LPARAM lp=0) : Event(iM, wp, lp) {}
  bool fValid() const;
  zPoint pos() const;
  String _export sFile() const;
  HWND hWinSource() const;
};
  
#endif



