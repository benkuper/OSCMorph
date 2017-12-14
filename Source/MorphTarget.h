/*
  ==============================================================================

    MorphTarget.h
    Created: 11 Dec 2017 5:00:09pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class MorphTarget :
	public BaseItem,
	BaseManager<GenericControllableItem>::Listener
{
public:
	MorphTarget(const String &name = "MorphTarget");
	~MorphTarget();

	ColorParameter * color;
	Point2DParameter * position;

	FloatParameter * weight;

	ControllableContainer values;

	void addValueFromItem(GenericControllableItem * item);

	void syncValuesWithModel(bool syncValues = false);

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;

	void itemAdded(GenericControllableItem * item) override;
	void itemRemoved(GenericControllableItem * item) override;


};