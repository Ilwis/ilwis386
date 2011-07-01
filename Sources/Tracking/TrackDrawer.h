#pragma once

DrawerInfoVector *createDrawer();
ILWIS::NewDrawer *createTrackSetDrawer(ILWIS::DrawerParameters *parms);

class FieldColor;
class FieldLineType;
enum LineDspType;

namespace ILWIS{

struct TrackerDrawProperties : public GeneralDrawerProperties {
	LineProperties lproperties;
	PointProperties pproperties;
};


class TrackSetDrawer : public ComplexDrawer {

	public:
		ILWIS::NewDrawer *createTrackSetDrawer(DrawerParameters *parms);

		TrackSetDrawer(DrawerParameters *parms);
		virtual ~TrackSetDrawer();
		GeneralDrawerProperties *getProperties();
		void prepare(PreparationParameters *parm);
		bool isToolUseableFor(NewDrawer *drw);
		virtual void addDataSource(void *,int options=0);
		bool draw(const CoordBounds& cb=CoordBounds()) const;
		void getTrack(vector<Coord>& crds) const;
		long getTrackLength() const;

	protected:
		virtual NewDrawer *createElementDrawer(PreparationParameters *pp,ILWIS::DrawerParameters* parms) const;
		void setDrawMethod(DrawMethod method=drmINIT);
		String store(const FileName& fnView, const String& parenSection) const;
		void load(const FileName& fnView, const String& parenSection);

		TrackerDrawProperties trackerDrawProperties; 

		vector<Coord> coords;
		NewDrawer *linedrawer;
		NewDrawer *pointdrawer;
	};


}