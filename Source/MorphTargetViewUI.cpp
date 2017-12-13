/*
  ==============================================================================

    MorphTargetViewUI.cpp
    Created: 11 Dec 2017 5:44:50pm
    Author:  Ben

  ==============================================================================
*/

#include "MorphTargetViewUI.h"


MorphTargetViewUI::MorphTargetViewUI(MorphTarget * mt) :
	BaseItemUI<MorphTarget>(mt, ResizeMode::ALL, true)
{
	autoDrawHighlightWhenSelected = false;

	headerHeight = 0;
	grabberHeight = 0;
	resizerHeight = 0;
	resizerWidth = 0;
	headerGap = 0;
	setContentSize(40, 40);

	setGrabber(new MGrabber());
	
	removeChildComponent(resizer);
	removeChildComponent(enabledBT);
	removeChildComponent(nameUI);

	resized();
}

MorphTargetViewUI::~MorphTargetViewUI()
{
}

void MorphTargetViewUI::setHandleColor(Colour c)
{
	((MGrabber *)grabber.get())->handleColor = c;
}

void MorphTargetViewUI::paint(Graphics & g)
{
	g.setColour(item->color->getColor().withAlpha(.1f));
	g.fillEllipse(getLocalBounds().reduced(2).toFloat());
	g.setColour(item->color->getColor().withAlpha(.6f));
	g.drawEllipse(getLocalBounds().reduced(2).toFloat(), 2);
}

void MorphTargetViewUI::resized()
{
	removeBT->setBounds(Rectangle<int>(getWidth() - 10, 10, 10, 10));
	grabber->setBounds(getLocalBounds().withSizeKeepingCentre(15,15));
	
}
 
void MorphTargetViewUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	BaseItemUI::controllableFeedbackUpdateInternal(c);
	//if(c == item->weight) repaint();
}


MorphTargetViewUI::MGrabber::MGrabber() :
	handleColor(Colours::yellow)
{
}

void MorphTargetViewUI::MGrabber::paint(Graphics & g)
{
	g.setColour(handleColor);
	g.drawLine(0, getHeight()/2, getWidth(), getHeight() / 2, 2);
	g.drawLine(getWidth()/2, 0, getWidth() / 2, getHeight(), 2);

}
