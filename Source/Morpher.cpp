/*
  ==============================================================================

    Morpher.cpp
    Created: 11 Dec 2017 5:00:03pm
    Author:  Ben

  ==============================================================================
*/
#define JC_VORONOI_IMPLEMENTATION
#include "Morpher.h"

juce_ImplementSingleton(Morpher)

Morpher::Morpher() :
	BaseManager("Morpher")
{
	isSelectable = true;

	blendMode = addEnumParameter("BlendMode", "Mode for value blending");
	blendMode->addOption("Voronoi", Voronoi)->addOption("Weights", Weights)->addOption("Gradient Band", GradientBand);

	bgImagePath = addStringParameter("Background Path", "", "");
	bgScale = addFloatParameter("Background Scale", "", 10,1,100);
	bgOpacity = addFloatParameter("Background Opacity", "", 1);
	diagramOpacity = addFloatParameter("Diagram Opacity", "Opacity of the Voronoi Diagram", .2f, 0, 1);

	//curZoneIndex = addIntParameter("Current Site", "",-1,-1,20);

	mainTarget = new MorphTarget();
	mainTarget->color->setColor(Colours::white);
	addChildControllableContainer(mainTarget);
	mainTarget->showInspectorOnSelect = false;

	values = new GenericControllableManager("Values");
	values->setCustomShortName("values");
	addChildControllableContainer(values);
}

Morpher::~Morpher()
{
}

Array<Point<float>> Morpher::getNormalizedTargetPoints()
{
	Array<Point<float>> result;
	for (MorphTarget * mt : items) result.add(mt->position->getPoint());
	return result;
}



void Morpher::computeZones()
{

	diagram = new jcv_diagram();
	Array<Point<float>> points = getNormalizedTargetPoints();

	if (points.size() == 0) return;

	Array<jcv_point> jPoints;

	for (Point<float> p : points)
	{
		jcv_point jp;
		jp.x = p.x;
		jp.y = p.y;
		jPoints.add(jp);
	}

	jcv_diagram_generate(points.size(), jPoints.getRawDataPointer(), nullptr, diagram);

	computeWeights();
}

int Morpher::getSiteIndexForPoint(Point<float> p)
{
	if (diagram == nullptr) return -1;

	if (diagram->numsites == 0) return -1;

	const jcv_site * sites = jcv_diagram_get_sites(diagram);

	float minDist = p.getDistanceFrom(Point<float>(sites[0].p.x,sites[0].p.y));
	float index = 0;
	for (int i = 1; i < diagram->numsites; i++)
	{
		float dist = p.getDistanceFrom(Point<float>(sites[i].p.x, sites[i].p.y));

		if (dist < minDist)
		{
			minDist = dist;
			index = i;
		}
	}

	return index;
}


void Morpher::computeWeights()
{
	BlendMode bm = blendMode->getValueDataAsEnum<BlendMode>();

	switch (bm)
	{
	case Voronoi:
		for (MorphTarget * mt : items) mt->weight->setValue(0);

		HashMap<MorphTarget *, float> rawWeights;
		float totalRawWeight = 0;

		Point<float> mp = mainTarget->position->getPoint();

		int index = getSiteIndexForPoint(mp);
		if (index == -1) break;


				
		const jcv_site * sites = jcv_diagram_get_sites(diagram);
		jcv_site s = sites[index];

		//curZoneIndex->setValue(getSiteIndexForPoint(mp));

		//Compute direct site
		MorphTarget * mt = items[s.index];
		float d = mp.getDistanceFrom(Point<float>(s.p.x, s.p.y));

		float mw = (float)INT_MAX;
		if (d != 0) mw = 1.0f / d;
		rawWeights.set(mt, mw);
		totalRawWeight += mw;

		
		
		//Fill edge distances

		jcv_graphedge * edge = s.edges;
		Array<jcv_graphedge *> edges;

		Array<float> edgeDists;
		Array<float> edgeNeighbourDists;
		Array<jcv_site *> neighbourSites;
		Array<Line<float>> edgeLines;

		while(edge != nullptr)
		{
			jcv_site * ns = edge->neighbor;// e->sites[0]->index == s.index ? e->sites[1] : e->sites[0];

			if (ns == nullptr)
			{
				edge = edge->next;
				continue;
			}

			//DBG("Site " << s.index << " : Edge has sites " << e->sites[0]->index << "and " << e->sites[1]->index << ", chosen " << ns->index);
			//DBG("\t\t" << edge->pos[0].x << " / " << edge->pos[0].y << " / " << edge->pos[1].x << " / " << edge->pos[1].y);

			Line<float> line(Point<float>(edge->pos[0].x, edge->pos[0].y), Point<float>(edge->pos[1].x, edge->pos[1].y));
			Point<float> np;
			float distToEdge = line.getDistanceFromPoint(mp, np);
			Point<float> sp;
			float distNeighbourToEdge = line.getDistanceFromPoint(Point<float>(ns->p.x, ns->p.y), sp);

			edges.add(edge);

			edgeDists.add(distToEdge);
			edgeNeighbourDists.add(distNeighbourToEdge);
			neighbourSites.add(ns);
			edgeLines.add(line);

			edge = edge->next;
		}

		//DBG("Num edges to test " << edges.size());
		
		//Compute weight for each neighbour
		for(int i=0;i<edges.size();i++)
		{
			float edgeDist = edgeDists[i];
			float edgeNDist = edgeNeighbourDists[i];
			float totalDist = edgeDist + edgeNDist; //mp.getDistanceFrom(neighbourPos); //if we want to check direct distance instead of path to point

			//Get min distance to non common-site edge
			float minOtherEdgeDist = (float)INT_MAX;
			jcv_graphedge * minEdge = nullptr;
			for (int j = 0; j < edges.size(); j++)
			{
				if (i == j) continue;
				
				if (checkSitesAreNeighbours(edges[i]->neighbor, edges[j]->neighbor)) continue;

				float ed = edgeDists[j];
				if (ed < minOtherEdgeDist)
				{
					minOtherEdgeDist = ed;
					minEdge = edges[j];
				}
			}


			float ratio = 1;
			if (minEdge != nullptr) ratio = 1 - (edgeDist / (edgeDist + minOtherEdgeDist));
			float w = ratio / totalDist;

			MorphTarget * nmt = items[neighbourSites[i]->index]; 
			rawWeights.set(nmt, w);
			totalRawWeight += w;
		}

		//DBG("Cur site " << s.index << " : " << mt->niceName << ": " << mw << ", total =  " << totalRawWeight << ", numWeighted neighbours " << rawWeights.size());


		//Normalize weights
		for (HashMap<MorphTarget *, float>::Iterator i(rawWeights); i.next();)
		{
			i.getKey()->weight->setValue(i.getValue() / totalRawWeight);
		}

		break;
	}


	//Assign values from weights
	for (GenericControllableItem * i : values->items)
	{
		switch (i->controllable->type)
		{
		case Controllable::FLOAT:
			{
			
			float val = 0;
			for (MorphTarget * imt : items)
			{
				FloatParameter * tC = (FloatParameter *)imt->values.getControllableByName(i->controllable->shortName);
				if (tC != nullptr) val += tC->floatValue() * imt->weight->floatValue();
				else DBG("Controllable null with name : " << tC->shortName);
			}
			DBG("Assign final value : " << i->controllable->shortName << " / " << val);
			((FloatParameter *)i->controllable)->setValue(val);
			}
			break;

			/*
		case Controllable::INT:
		{
			float val = 0;
			for (MorphTarget * imt : items) val += ((IntParameter *)imt->values.getControllableByName(i->shortName))->floatValue() * imt->weight->floatValue();
			((IntParameter *)i->controllable)->setValue((int)val);
		}
		break;
		*/
		}
	}

	morpherListeners.call(&MorpherListener::weightsUpdated);
}

bool Morpher::checkSitesAreNeighbours(jcv_site * s1, jcv_site * s2)
{
	jcv_graphedge * e = s1->edges;
	while (e != nullptr)
	{
		if (e->neighbor != nullptr && e->neighbor->index == s2->index) return true;
		e = e->next;
	}
	return false;
}


void Morpher::addItemInternal(MorphTarget * mt, var data)
{
	computeZones();
}

void Morpher::removeItemInternal(MorphTarget * mt)
{
	computeZones();
}

void Morpher::onContainerParameterChanged(Parameter * p)
{
	if (p == blendMode)
	{
		BlendMode bm = blendMode->getValueDataAsEnum<BlendMode>();
		diagramOpacity->hideInEditor = bm != Voronoi;
		
		switch (bm)
		{
		case Voronoi:

			break;

		case GradientBand:
		case Weights:

			break;
		}
	}
}

void Morpher::controllableFeedbackUpdate(ControllableContainer * cc, Controllable * c)
{
	if (cc == mainTarget)
	{
		computeWeights();
	} else
	{
		MorphTarget * mt = dynamic_cast<MorphTarget *>(cc);
		if (mt != nullptr && mt != mainTarget)
		{
			if(c == mt->position) computeZones();
		}
	}
	
}

