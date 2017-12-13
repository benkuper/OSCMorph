/*
  ==============================================================================

    OSCOutputManager.h
    Created: 11 Dec 2017 5:00:27pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "OSCOutput.h"
#include "Morpher.h"

class OSCOutputManager :
	public BaseManager<OSCOutput>,
	public Morpher::MorpherListener
{
public:
	juce_DeclareSingleton(OSCOutputManager, true)

	OSCOutputManager();
	~OSCOutputManager();

	BoolParameter * autoSlipComplexParams;
	BoolParameter * sendColorNativeOSC;

	void sendMessage(const OSCMessage &m);
	OSCArgument varToArgument(const var & v);

	void weightsUpdated() override;

};