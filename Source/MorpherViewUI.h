/*
  ==============================================================================

    MorpherManagerUI.h
    Created: 11 Dec 2017 5:01:24pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "Morpher.h"
#include "MorphTargetViewUI.h"

class MorpherViewUI :
	public BaseManagerShapeShifterViewUI<Morpher, MorphTarget, MorphTargetViewUI>,
	public ControllableContainer::ContainerAsyncListener
{
public:
	MorpherViewUI(const String &contentName, Morpher * _manager);
	~MorpherViewUI();

	Image bgImage;

	ScopedPointer<MorphTargetViewUI> mainTargetUI;

	void paintBackground(Graphics &g) override;

	void setupBGImage();

	void updateViewUIPosition(MorphTargetViewUI * mtvui) override;

	void newMessage(const ContainerAsyncEvent &e) override;
	void controllableFeedbackUpdateAsync(ControllableContainer * cc, Controllable * c);
	
	static MorpherViewUI * create(const String &contentName) { return new MorpherViewUI(contentName, Morpher::getInstance()); }
};