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

	nameParam->hideInEditor = false;

	position = addPoint2DParameter("Position", "Relative Position");
	position->setBounds(-100, -100, 100, 100);
	//position->hideInEditor = true;

	weight = addFloatParameter("Weight", "Current weight of this target", 0, 0, 1);
	addChildControllableContainer(&values);

	if (Morpher::getInstanceWithoutCreating() != nullptr)
	{
		DBG("Add listener to morpher values");
		Morpher::getInstance()->values->addBaseManagerListener(this);
		for (GenericControllableItem * i : Morpher::getInstance()->values->items) addValueFromItem(i);
		
		position->isEditable = false;
	} else
	{
		values.hideInEditor = true;
	}

	syncValuesWithModel();
	
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

void MorphTarget::syncValuesWithModel()
{
	if (Morpher::getInstanceWithoutCreating() == nullptr) return;
	Array<WeakReference<Parameter>> vList = values.getAllParameters();
	if (vList.size() != Morpher::getInstance()->values->items.size()) return;

	int index = 0;

	for (auto &gci : Morpher::getInstance()->values->items)
	{
		vList[index]->setNiceName(gci->niceName);
		Parameter * p = dynamic_cast<Parameter *>(gci->controllable);
		vList[index]->setRange(p->minimumValue, p->maximumValue);
		index++;
	}
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
	values.loadJSONData(data.getProperty("values",var()), true);
}

void MorphTarget::controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	if (cc == &values)
	{
		Morpher::getInstance()->computeWeights();
	}
}

void MorphTarget::itemAdded(GenericControllableItem * item)
{
	addValueFromItem(item);
	syncValuesWithModel();
}

void MorphTarget::itemRemoved(GenericControllableItem * item)
{
	if (item->controllable->type == Controllable::TRIGGER) return;
	Controllable * c = values.getControllableByName(item->niceName,true);
	if (c != nullptr)
	{
		values.removeControllable(c);
	}
	syncValuesWithModel();
}
