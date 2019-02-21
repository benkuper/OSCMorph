/*
  ==============================================================================

    Morpher.h
    Created: 11 Dec 2017 5:00:03pm
    Author:  Ben

  ==============================================================================
*/

#ifndef MORPHER_H_INCLUDED
#define MORPHER_H_INCLUDED

#pragma once

#include "MorphTarget.h"
#include "jc_voronoi.h"

class Morpher :
	public BaseManager<MorphTarget>,
	public Timer
{
public:
	juce_DeclareSingleton(Morpher,true);

	Morpher();
	virtual ~Morpher();

	//ui
	StringParameter * bgImagePath;
	FloatParameter * bgOpacity;
	FloatParameter * bgScale;
	FloatParameter * diagramOpacity;
	FloatParameter * targetSize;
	BoolParameter * showDebug;
	BoolParameter * useAttraction;
	FloatParameter * attractionSpeed;
	FloatParameter * attractionDecay;

	Point<float> attractionDir;

	enum AttractionMode { SIMPLE, PHYSICS };
	EnumParameter * attractionMode;

	ScopedPointer<MorphTarget> mainTarget;

	Trigger * addTargetAtCurrentPosition;

	enum BlendMode {Voronoi, Weights, GradientBand};
	EnumParameter * blendMode;

	Array<Point<float>> getNormalizedTargetPoints();
	MorphTarget * getEnabledTargetAtIndex(int index);

	ScopedPointer<jcv_diagram> diagram;

	ScopedPointer<GenericControllableManager> values;
	
	//Voronoi
	void computeZones();
	int getSiteIndexForPoint(Point<float> p);

	void computeWeights();

	bool checkSitesAreNeighbours(jcv_site * s1, jcv_site * s2);

	void setTargetPosition(float x, float y);


	//debug
	//IntParameter * curZoneIndex;

	void addItemInternal(MorphTarget * mt, var data) override;
	void removeItemInternal(MorphTarget * mt) override;

	void onContainerParameterChanged(Parameter * p) override;
	void onContainerTriggerTriggered(Trigger * t) override;

	void controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
	void childStructureChanged(ControllableContainer * cc) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

	void clear() override;

	class MorpherListener
	{
	public:
		/** Destructor. */
		virtual ~MorpherListener() {}
		virtual void weightsUpdated() {}
	};

	ListenerList<MorpherListener> morpherListeners;
	void addMorpherListener(MorpherListener* newListener) { morpherListeners.add(newListener); }
	void removeMorpherListener(MorpherListener* listener) { morpherListeners.remove(listener); }

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Morpher)	

		// Inherited via Timer
		virtual void timerCallback() override;
};

#endif