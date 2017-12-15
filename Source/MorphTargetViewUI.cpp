/*
  ==============================================================================

    MorphTargetViewUI.cpp
    Created: 11 Dec 2017 5:44:50pm
    Author:  Ben

  ==============================================================================
*/

#include "MorphTargetViewUI.h"
#include "Morpher.h"

MorphTargetViewUI::MorphTargetViewUI(MorphTarget * mt) :
	BaseItemUI<MorphTarget>(mt, ResizeMode::ALL, true)
{
	autoDrawHighlightWhenSelected = false;
	
	margin = 0;
	headerHeight = 15;
	
	grabberHeight = 0;
	resizerHeight = 0;
	resizerWidth = 0;
	headerGap = 0;

	float s = Morpher::getInstance()->targetSize->floatValue();
	setContentSize(s, s-headerHeight);

	setGrabber(new MGrabber());
	grabber->toBack();
	
	removeChildComponent(resizer);
	//removeChildComponent(enabledBT);
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

	g.setColour(item->color->getColor().brighter().withAlpha(.6f));
	g.fillEllipse(getLocalBounds().withSizeKeepingCentre(getWidth()*item->weight->floatValue(), getHeight()*item->weight->floatValue()).toFloat());
	g.setColour(item->color->getColor());
	g.drawEllipse(getLocalBounds().reduced(1).toFloat(), 2);

	g.setColour(item->color->getColor().brighter());
	if(!item->isMain) g.drawText(item->niceName, getLocalBounds().reduced(2).toFloat(), Justification::centred);
}

void MorphTargetViewUI::resized()
{
	BaseItemUI::resized();
	grabber->setBounds(getLocalBounds());
	grabber->toBack();
}
 
void MorphTargetViewUI::controllableFeedbackUpdateInternal(Controllable * c)
{
	BaseItemUI::controllableFeedbackUpdateInternal(c);
	//if(c == item->weight) repaint();
}


MorphTargetViewUI::MGrabber::MGrabber() :
	handleColor(Colours::transparentBlack)
{
}

void MorphTargetViewUI::MGrabber::paint(Graphics & g)
{
	g.setColour(handleColor);
	g.drawLine(getWidth()/4, getHeight()/2, getWidth()*3/4, getHeight() / 2, 2);
	g.drawLine(getWidth()/2, getHeight()/4, getWidth()/ 2, getHeight()*3/4, 2);
}
