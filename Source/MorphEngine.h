/*
  ==============================================================================

    MorphEngine.h
    Created: 11 Dec 2017 5:03:11pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class MorphEngine :
	public Engine
{
public:
	MorphEngine();
	~MorphEngine();

	void clearInternal() override;

	var getJSONData() override;
	void loadJSONDataInternalEngine(var data, ProgressTask * loadingTask) override;


	String getMinimumRequiredFileVersion() override;
};
