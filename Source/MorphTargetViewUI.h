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

	ScopedPointer<BoolToggleUI> activeUI;
	
	void setHandleColor(Colour c);
	
	void paint(Graphics &g) override;
	void resized() override;

	class MGrabber : public BaseItemUI::Grabber
	{
	public:
		MGrabber();
		Colour handleColor;
		void paint(Graphics &g) override;
	};
};