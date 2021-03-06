/*
  ==============================================================================

    MorphTarget.cpp
    Created: 11 Dec 2017 5:00:09pm
    Author:  Ben

  ==============================================================================
*/

#include "MorphTarget.h"
#include "Morpher.h"

MorphTarget::MorphTarget(const String &name, bool isMain) :
	BaseItem(name),
	isMain(isMain),
	values("Values")
{
	Random rnd;
	viewUISize->setPoint(40, 40);
	color = new ColorParameter("Color", "color", Colour::fromHSV(rnd.nextFloat(), 1, 1, 1));
	addParameter(color);

	position = addPoint2DParameter("Position", "Relative Position");
	position->setBounds(-100, -100, 100, 100);
	//position->hideInEditor = true;

	if (isMain)
	{
		values.hideInEditor = true; 
	}else
	{
		Morpher::getInstance()->values->addBaseManagerListener(this);
		for (GenericControllableItem * i : Morpher::getInstance()->values->items) addValueFromItem(i);
		
		position->setControllableFeedbackOnly(true);

		weight = addFloatParameter("Weight", "Current weight of this target", 0, 0, 1);
		addChildControllableContainer(&values);

		attraction = addFloatParameter("Attraction", "Attraction of this target", 0, 0, 1);
		attractionDecay = addFloatParameter("Decay Factor", "Decay factor for attraction, multiplied with Morpher global decay factor", 1, 0, 1);
	}

	values.editorIsCollapsed = false;

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

void MorphTarget::syncValuesWithModel(bool syncValues)
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
		if (syncValues) vList[index]->setValue(p->value);
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
