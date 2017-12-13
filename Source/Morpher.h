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
	public BaseManager<MorphTarget>
{
public:
	juce_DeclareSingleton(Morpher,true);

	Morpher();
	~Morpher();

	StringParameter * bgImagePath;
	FloatParameter * bgOpacity;
	FloatParameter * bgScale;
	FloatParameter * diagramOpacity;

	ScopedPointer<MorphTarget> mainTarget;


	enum BlendMode {Voronoi, Weights, GradientBand};
	EnumParameter * blendMode;

	Array<Point<float>> getNormalizedTargetPoints();
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

	void controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c) override;
	void childStructureChanged(ControllableContainer * cc) override;

	var getJSONData() override;
	void loadJSONDataInternal(var data) override;

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

	
};

#endif