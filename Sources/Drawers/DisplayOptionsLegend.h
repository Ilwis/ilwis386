namespace ILWIS {
	class AnimationDrawer;
}

class DisplayOptionsLegend {
public:
	DisplayOptionsLegend(LayerTreeView  *tv, HTREEITEM displayOptionsLastItem);
	void createForAnimation(ILWIS::AnimationDrawer *animdr);
	void createForSet(SetDrawer *setdr);
	void displayOptionSubRpr(CWnd *parent);
	void setcheckRpr(void *value, LayerTreeView *tree);
	void updateLegendItem();
private:
	void insertLegendItems(const DomainValueRangeStruct& dvs);
	void insertLegendItemsValue(const DomainValueRangeStruct& dvs);
	void insertLegendItemsClass();
	LayerTreeView *ltv;
	HTREEITEM parent;
	HTREEITEM rprItem;
	HTREEITEM htiLeg;
	Representation rpr;
	SetChecks *colorCheck;
	SetDrawer *setdr;
	AnimationDrawer *animDrw;


};

class RepresentationFormL : public DisplayOptionsForm {
public:
	RepresentationFormL(CWnd *wPar, ILWIS::SetDrawer *dr, AnimationDrawer *adr);
	void apply(); 
private:
	String rpr;
	FieldRepresentation *fldRpr;
	SetDrawer *setDrawer;
	AnimationDrawer *animDrw;

};
