DBColumnSelector::DBColumnSelector(FormEntry* par, DBInfo *inf, DBChoiceForm *frm)
  : BaseSelector(par),
    dbf(frm),
    info(inf)
{
  style = WS_VSCROLL;
  psn->iMinWidth *= 1.5;
}

DBColumnSelector::~DBColumnSelector()
{
}

void DBColumnSelector::create()
{
  BaseSelector::create();
  lb->setNotify(dbf, (NotifyProc)&DBChoiceForm::Select, Notify(CBN_SELCHANGE));
  lb->setNotify(this, (NotifyProc)&DBColumnSelector::DrawItem, WM_DRAWITEM);
  Fill();
  SendMessage(*lb,LB_SETTOPINDEX,0,0);
}

int DBColumnSelector::idSelect()
{
  return lb->GetCurSel();
}

void DBColumnSelector::DrawItem(Event *ev)
{
  DrawItemEvent *dev=dynamic_cast<DrawItemEvent *>(ev);
  ISTRUE(fINotEqual, dev, (DrawItemEvent *)NULL);

  DRAWITEMSTRUCT *dis=reinterpret_cast<DRAWITEMSTRUCT *> (dev->lParm);

  int iV = dis->rcItem.top / 2 + dis->rcItem.bottom / 2 + 5;
  int iLeft = dis->rcItem.left + 20;
  int id = dis->itemID;
  if (id < 0)
    return ;
  BaseSelector::DrawItem(ev);

	CDC dc;
	dc.Attach(dis->hDC);
  iLeft = iLeft + dc.GetTextExtent("ABCDEFGHIJKLMNOP", 16).cx;
  String sRem;
  switch (info->dtType(id)) {
    case dtTableClass:
      sRem = String("* %S", SCVItemClass);
      break;
    case dtClass:
      sRem = SCVItemClass;
      break;
    case dtId:
      sRem = SCVItemID;
      break;
    case dtTableId:
      sRem = String("* %S", SCVItemID);
      break;
    case dtString:
      sRem = SCVItemString;
      break;
    case dtBool:
      sRem = SCVItemBool;
      break;
    case dtValue:
      sRem = SCVItemValue;
      break;
    case dtDate:
      sRem = SCVItemValue;
      break;
    default:
      sRem = "??";
      break;
  }
  dc.TextOut( iLeft, iV, sRem.scVal(), sRem.length());
	dc.Detach();
}

void DBColumnSelector::Fill()
{
  lb->ResetContent();
	char sField[12];
  for (short iFld = 0; iFld < info->iNrFields; iFld++)
	{
		memcpy(sField, info->sName(iFld).scVal(), 12);
    lb->AddString(sField);
	}
}

String DBColumnSelector::sName(int id)
{
	String s = info->sName(id);
	s &=  ".clm";
	return s;
}

void DBColumnSelector::StoreData()
{

}

//----------------------------------------------------------------------------------------
DBChoiceForm::DBChoiceForm(CWnd *w, DBInfo* inf) : FormWithDest(w, SCVTitleImportDBF),
    info(inf),
    choice(ccString),
    fTableDom(false),
    ids(-1),
    fInSelect(false)
{
    if ( info->dtType(0) == dtTableClass || info->dtType(0) == dtTableId ) fTableDom=true;
    StaticText* st = new StaticText(root, SCVRemSpecifyDomains);
    st->SetIndependentPos();
    cs = new DBColumnSelector(root, info, this);
    cs->SetWidth(120);
    cs->SetIndependentPos();
    gr = new RadioGroup(root, SCVItemColumn, &choice, true);
    gr->SetIndependentPos();
    new RadioButton(gr, SCVItemClass);
    new RadioButton(gr, SCVItemID);
    new RadioButton(gr, SCVItemValue);
    new RadioButton(gr, SCVItemString);

    cb = new CheckBox(root, SCVItemTableDomain, &fTableDom);
    cb->Align(gr, AL_UNDER);
    cb->SetCallBack((NotifyProc)&DBChoiceForm::SetDomain);
    gr->SetCallBack((NotifyProc)&DBChoiceForm::SetDomain);
    //SetMenHelpTopic(htpImportDbase);
    create();

}

int DBChoiceForm::SetDomain(Event *)
{
    if ( ids < 0 || fInSelect) return 1;

    dtDomainType newclass = dtString;
    ccColumnClass cl = (ccColumnClass)gr->iVal();
    bool fTableDom = cb->fVal();
    switch ( cl )
    {
        case ccClass:
            newclass = fTableDom ? dtTableClass : dtClass;
            break;
        case ccId :
            newclass = fTableDom ? dtTableId : dtId;
            break;
        case ccValue :
            newclass = dtValue;
            break;
        case ccString :
            newclass = dtString;
            break;
    }
    info->SetAltDomain(ids, newclass);
    cs->setDirty();
    Select(0); // update !
    return 1;
}

int DBChoiceForm::Select(Event* Evt)
{
  fInSelect = true;
  ids = cs->idSelect();
  if (ids < 0) {
    gr->Hide();
    cb->Hide();
    return 1;
  }
  dtDomainType type = info->dtType(ids);

  switch (type) {
    case dtClass:
    case dtTableClass:
      gr->SetVal(ccClass);
      gr->Show();
      cb->SetVal(type == dtTableClass);
      cb->Show();
      break;
    case dtId:
    case dtTableId:
      gr->SetVal(ccId);
      gr->Show();
      cb->SetVal(type == dtTableId);
      cb->Show();
      break;
    case dtValue:
    case dtDate:
      gr->SetVal(ccValue);
      gr->Show();
      cb->SetVal(false);
      cb->Hide();
      break;
    case dtString:
    gr->SetVal(ccString);
      gr->Show();
      cb->SetVal(false);
    cb->Hide();
      break;
    default:
      gr->Hide();
      cb->SetVal(false);
      cb->Hide();
  }
  fInSelect = false;

//cs->Fill();  22/7/97 Wim: disrupts all selections everything, why should it be needed anyway?
  return 1;
}
