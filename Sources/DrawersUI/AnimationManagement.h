#pragma once

#define ILWM_UPDATE_ANIM (WM_APP+232)

class MapCompositionDoc;

namespace ILWIS {

	struct AnimationProperties {
		AnimationDrawer *drawer;
		MapCompositionDoc *mdoc;
	};

	class AnimationPropertySheet : public CPropertySheet
	{
	public:
		enum Pages{pRun=1, pSynchornization=2, pAttributes=4, pAll=32767};
		AnimationPropertySheet();

		BOOL OnInitDialog();
		void addAnimation(const AnimationProperties& props);
		void setActiveAnimation(AnimationDrawer * drw);
		void removeAnimation(AnimationDrawer * drw);
		AnimationProperties* getActiveAnimation();
		AnimationProperties* getAnimation(int i);
		int getAnimationCount() const;
		int getActiveIndex() const;
		void OnSysCommand(UINT nID, LPARAM p);
		void OnClose();
		LRESULT command(WPARAM, LPARAM);
	
	private:
		vector<AnimationProperties> animations;
		int activeIndex;
		CCriticalSection cs;

		DECLARE_MESSAGE_MAP( );
	};

	class AnimationRun : public FormBasePropertyPage {
	public:
		AnimationRun(AnimationPropertySheet& sheet);
		int changeActive(Event *ev);
	private:
		FieldOneSelect *foAnimations;
		FieldRealSliderEx *sliderFps;
		long animIndex;
		double fps;

		AnimationPropertySheet	&propsheet;
		virtual int DataChanged(Event*);
		int speed(Event *ev);
		int begin(Event  *ev);
		int end(Event  *ev);
		int pause(Event  *ev);
		int run(Event  *ev);
		int stop(Event  *ev);
	};

	class AnimationSynchronization : public FormBasePropertyPage {
	public:
		AnimationSynchronization(AnimationPropertySheet& sheet);
	private:
		FieldOneSelect *foMaster, *foSlave1;
		FieldGroup *fgMaster;
		int offset1;
		long choiceMaster, choiceSlave1;
		double step1;
		AnimationPropertySheet	&propsheet;
		bool initial;

		int DataChanged(Event*);
		int synchronize(Event*);
	};

	class AnimationProgress : public FormBasePropertyPage {
		public:
			AnimationProgress(AnimationPropertySheet& sheet);
		private:
			int DataChanged(Event*ev);
			AnimationPropertySheet	&propsheet;
			TimeGraphSlider *graphSlider;
			FieldGroup *fgMaster;
	};
}

