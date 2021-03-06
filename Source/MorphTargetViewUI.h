/*
  ==============================================================================

    MorphTargetViewUI.h
    Created: 11 Dec 2017 5:44:50pm
    Author:  Ben

  ==============================================================================
*/

#pragma once


#include "MorphTarget.h"

class MorphTargetViewUI :
	public BaseItemUI<MorphTarget>
{
public:
	MorphTargetViewUI(MorphTarget * mt);
	~MorphTargetViewUI();

	Image targetImage;
	bool isMain;

	ScopedPointer<BoolToggleUI> activeUI;
	
	void setHandleColor(Colour c);

	void paint(Graphics &g) override;
	void resized() override;

	void updateMiniModeUI() override {} //do nothing, no minimode

	void controllableFeedbackUpdateInternal(Controllable * c) override;

	class MGrabber : public Grabber
	{
	public:
		MGrabber();
		Colour handleColor;
		void paint(Graphics &g) override;
	};
};