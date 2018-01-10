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

	isSelectable = true; 
	
	Morpher::getInstance()->addMorpherListener(this);

	autoSlipComplexParams = addBoolParameter("Auto Split Complex", "Auto Split complex params such as point 2d/3d and colors", false);
	sendColorNativeOSC = addBoolParameter("Send Color Native", "Send Color values as native 'r' data types in osc message", false);
}

OSCOutputManager::~OSCOutputManager()
{
	if (Morpher::getInstanceWithoutCreating() != nullptr) Morpher::getInstance()->removeMorpherListener(this);
}

void OSCOutputManager::sendWeightMessage(const OSCMessage & m)
{
	for (auto &o : items) if(o->sendWeights->boolValue()) o->sendOSC(m);
}

void OSCOutputManager::sendValueMessage(const OSCMessage & m)
{
	for (auto &o : items) if(o->sendValues->boolValue()) o->sendOSC(m);
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
	for (auto &i : Morpher::getInstance()->items)
	{
		OSCMessage m("/weight/" + i->shortName);
		m.addFloat32(i->weight->floatValue());
		sendWeightMessage(m);
	}

	for (auto &i : Morpher::getInstance()->values->items)
	{
		if (i->controllable->type == Controllable::TRIGGER) continue;
		if (!i->enabled->boolValue()) continue;

		Parameter * p = (Parameter *)(i->controllable);
		String targetAddress = i->niceName;
		if (!targetAddress.startsWithChar('/')) targetAddress = "/" + targetAddress;
		targetAddress = targetAddress.replaceCharacter(' ', '_');

		if (p->type == Parameter::COLOR && sendColorNativeOSC->boolValue())
		{
			OSCMessage m(targetAddress);
			Colour c = ((ColorParameter *)p)->getColor();
			uint32 uc = c.getRed() << 24 | c.getGreen() << 16 | c.getBlue() << 8 | c.getAlpha();
			m.addArgument(OSCArgument(uc));
			sendValueMessage(m);

		}else if (p->isComplex())
		{
			
			if (!autoSlipComplexParams->boolValue())
			{
				OSCMessage m(targetAddress);
				for (int v = 0; v < p->value.size(); v++) m.addArgument(varToArgument(p->value[v]));
				sendValueMessage(m);
			} else
			{
				StringArray addSplit;
				addSplit.addTokens(targetAddress, "/", "\"");
				String parms = addSplit[addSplit.size() - 1];
				String prefix = addSplit.joinIntoString("/",0,addSplit.size()-1);

				//DBG("Prefix : " << prefix << ", parms : " << parms << ", parms length : " << parms.length() << ", v size : " << p->value.size());
				for (int v = 0; v < p->value.size(); v++)
				{
					String splitAddress = prefix + "/" + (v >= parms.length() ? "-" : String::charToString(parms[v]));
					//DBG("Split address (" << v << ") " << splitAddress);
					OSCMessage m(splitAddress);
					m.addArgument(varToArgument(p->value[v]));
					sendValueMessage(m);
				}
			}
		}
		else
		{
			OSCMessage m(targetAddress);
			m.addArgument(varToArgument(p->value));
			sendValueMessage(m);
		}
	}
}
