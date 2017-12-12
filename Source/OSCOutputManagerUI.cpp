/*
  ==============================================================================

    OSCOutputManagerUI.cpp
    Created: 11 Dec 2017 5:01:52pm
    Author:  Ben

  ==============================================================================
*/

#include "OSCOutputManagerUI.h"

OSCOutputManagerUI::OSCOutputManagerUI(const String &contentName, OSCOutputManager * _manager) :
	BaseManagerShapeShifterUI(contentName, _manager)
{
	addItemText = "Add Output";
	addExistingItems();
}

OSCOutputManagerUI::~OSCOutputManagerUI()
{
}
