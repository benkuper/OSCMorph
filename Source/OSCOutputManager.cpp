/*
  ==============================================================================

    OSCOutputManager.cpp
    Created: 11 Dec 2017 5:00:27pm
    Author:  Ben

  ==============================================================================
*/

#include "OSCOutputManager.h"

juce_ImplementSingleton(OSCOutputManager)

OSCOutputManager::OSCOutputManager() :
	BaseManager("Outputs")
{
	Morpher::getInstance()->addMorpherListener(this);
}

OSCOutputManager::~OSCOutputManager()
{
	if (Morpher::getInstanceWithoutCreating() != nullptr) Morpher::getInstance()->removeMorpherListener(this);
}

void OSCOutputManager::sendMessage(const OSCMessage & m)
{
	for (auto &o : items) o->sendOSC(m);
}


OSCArgument OSCOutputManager::varToArgument(const var & v)
{
	if (v.isBool()) return OSCArgument(((bool)v) ? 1 : 0);
	else if (v.isInt()) return OSCArgument((int)v);
	else if (v.isInt64()) return OSCArgument((int)v);
	else if (v.isDouble()) return OSCArgument((float)v);
	else if (v.isString()) return OSCArgument(v.toString());

	jassert(false);
	return OSCArgument("error");
}

void OSCOutputManager::weightsUpdated()
{
	for (auto &i : Morpher::getInstance()->values->items)
	{
		if (i->controllable->type == Controllable::TRIGGER) continue;
		Parameter * p = (Parameter *)(i->controllable);
		String targetAddress = i->niceName;
		if (!targetAddress.startsWithChar('/')) targetAddress = "/" + targetAddress;
		targetAddress = targetAddress.replaceCharacter(' ', '_');
		OSCMessage m(targetAddress);
		m.addArgument(varToArgument(p->value));
		sendMessage(m);
	}
}
