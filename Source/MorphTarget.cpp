/*
  ==============================================================================

    MorphTarget.cpp
    Created: 11 Dec 2017 5:00:09pm
    Author:  Ben

  ==============================================================================
*/

#include "MorphTarget.h"
#include "Morpher.h"

MorphTarget::MorphTarget(const String &name) :
	BaseItem(name),
	values("Values")
{
	Random rnd;
	viewUISize->setPoint(40, 40);
	color = new ColorParameter("Color", "color", Colour::fromHSV(rnd.nextFloat(), 1, 1, 1));
	addParameter(color);

	position = addPoint2DParameter("Position", "Relative Position");
	position->setBounds(-100, -100, 100, 100);
	position->isEditable = false;

	weight = addFloatParameter("Weight", "Current weight of this target", 0, 0, 1);
	addChildControllableContainer(&values);

	if (Morpher::getInstanceWithoutCreating() != nullptr)
	{
		DBG("Add listener to morpher values");
		Morpher::getInstance()->values->addBaseManagerListener(this);
		for (GenericControllableItem * i : Morpher::getInstance()->values->items) addValueFromItem(i);
	}
	
}

MorphTarget::~MorphTarget()
{
	if(Morpher::getInstanceWithoutCreating() != nullptr) Morpher::getInstance()->values->removeBaseManagerListener(this);
}

void MorphTarget::addValueFromItem(GenericControllableItem * item)
{
	if (item->controllable->type == Controllable::TRIGGER) return;
	Controllable * c = ControllableFactory::createControllable(item->controllable->getTypeString());
	c->setNiceName(item->controllable->niceName);
	values.addControllable(c);
}

var MorphTarget::getJSONData()
{
	var data = BaseItem::getJSONData();
	data.getDynamicObject()->setProperty("values", values.getJSONData());
	return data;
}

void MorphTarget::loadJSONDataInternal(var data)
{
	BaseItem::loadJSONDataInternal(data);
	values.loadJSONData(data, true);
}

void MorphTarget::itemAdded(GenericControllableItem * item)
{
	DBG("Item added !");
	addValueFromItem(item);
}

void MorphTarget::itemRemoved(GenericControllableItem * item)
{
	if (item->controllable->type == Controllable::TRIGGER) return;
	Controllable * c = values.getControllableByName(item->shortName);
	if (c != nullptr)
	{
		values.removeControllable(c);
	}
}
