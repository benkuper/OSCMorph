/*
  ==============================================================================

    OSCOutputManagerUI.h
    Created: 11 Dec 2017 5:01:52pm
    Author:  Ben

  ==============================================================================
*/

#pragma once
#include "OSCOutputManager.h"

class OSCOutputManagerUI :
	public BaseManagerShapeShifterUI<OSCOutputManager, OSCOutput, BaseItemUI<OSCOutput>>
{
public:
	OSCOutputManagerUI(const String &contentName, OSCOutputManager *_manager);
	~OSCOutputManagerUI();


	static OSCOutputManagerUI * create(const String &contentName) { return new OSCOutputManagerUI(contentName, OSCOutputManager::getInstance()); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OSCOutputManagerUI)
};

