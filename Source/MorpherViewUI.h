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
	public ContainerAsyncListener,
	public Morpher::MorpherListener
{
public:
	MorpherViewUI(const String &contentName, Morpher * _manager);
	~MorpherViewUI();

	Image bgImage;

	ScopedPointer<MorphTargetViewUI> mainTargetUI;

	void paintBackground(Graphics &g) override;

	void setupBGImage();

	void resized() override;

	void setViewZoom(float value) override;
	
	void weightsUpdated() override;

	void mouseDown(const MouseEvent &e) override;
	void mouseDrag(const MouseEvent &e) override;
	void mouseDoubleClick(const MouseEvent &e) override;

	bool keyPressed(const KeyPress &e) override;

	virtual void itemUIGrabbed(BaseItemUI<MorphTarget> * se) override;

	void newMessage(const ContainerAsyncEvent &e) override;
	void controllableFeedbackUpdateAsync(ControllableContainer * cc, Controllable * c);
	
	static MorpherViewUI * create(const String &contentName) { return new MorpherViewUI(contentName, Morpher::getInstance()); }
};