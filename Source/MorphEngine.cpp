/*
==============================================================================

Engine.cpp
Created: 2 Apr 2016 11:03:21am
Author:  Martin Hermant

==============================================================================
*/

#include "MorphEngine.h"
#include "Morpher.h"
#include "OSCOutputManager.h"

MorphEngine::MorphEngine() :
	Engine("Morph", ".morph")
{
	//init here
	Engine::mainEngine = this;

	addChildControllableContainer(Morpher::getInstance());
	addChildControllableContainer(OSCOutputManager::getInstance());
}

MorphEngine::~MorphEngine()
{
	//Application-end cleanup, nothing should be recreated after this

	//delete singletons here
	OSCOutputManager::deleteInstance();
	Morpher::deleteInstance();

}

void MorphEngine::clearInternal()
{
	isClearing = true;

	//clear
	Morpher::getInstance()->clear();
	OSCOutputManager::getInstance()->clear();

}

var MorphEngine::getJSONData()
{
	var data = Engine::getJSONData();

	//save here
	data.getDynamicObject()->setProperty("morpher", Morpher::getInstance()->getJSONData());
	data.getDynamicObject()->setProperty("outputs", OSCOutputManager::getInstance()->getJSONData());

	return data;
}

void MorphEngine::loadJSONDataInternalEngine(var data, ProgressTask * loadingTask)
{
	ProgressTask * t = loadingTask->addTask("all");


	//load here
	t->start();
	Morpher::getInstance()->loadJSONData(data.getProperty("morpher", var()));
	OSCOutputManager::getInstance()->loadJSONData(data.getProperty("outputs", var()));
	t->setProgress(1);
	t->end();


}

String MorphEngine::getMinimumRequiredFileVersion()
{
	return "1.0.0";
}
