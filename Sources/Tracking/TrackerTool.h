#pragma once

#define TRACKER_TOOL_ID 30419
#define TRACK_DRAWER_ID 326

ILWIS::DrawerTool *createTrackerTool(ZoomableView* zv, LayerTreeView *view, ILWIS::NewDrawer *drw);

class TrackerTool : public DrawerTool
{
public:
	TrackerTool(ZoomableView* zv, LayerTreeView *view, NewDrawer *drw);         

	virtual void OnLButtonUp(UINT nFlags, CPoint point);
	void Stop();
	HTREEITEM configure( HTREEITEM parentItem);
	virtual ~TrackerTool();
	bool isToolUseableFor(ILWIS::DrawerTool *tool);
	String cursorName() const;
	void getMaxLengthTrack(vector<Coord>& crds) const;
protected:
	void reset();
	String getMenuString() const;
	void toggleTracking(void *value, HTREEITEM);
	void options();
	//SpatialDataDrawer *adr;
	bool tracking;
};

class TrackingForm : public DisplayOptionsForm2 {
public:
	TrackingForm(CWnd *par, TrackerTool *dr) ;
	void apply();
	int openAsTable(Event *);
private:
	TrackerTool *tool;

};