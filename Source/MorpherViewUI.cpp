/*
  ==============================================================================

    MorpherManagerUI.cpp
    Created: 11 Dec 2017 5:01:24pm
    Author:  Ben

  ==============================================================================
*/

#include "MorpherViewUI.h"

MorpherViewUI::MorpherViewUI(const String & contentName, Morpher * _manager) :
	BaseManagerShapeShifterViewUI(contentName,_manager)
{
	//canNavigate = false;
	setupBGImage();
	manager->addAsyncContainerListener(this);


	mainTargetUI = new MorphTargetViewUI(manager->mainTarget);
	mainTargetUI->setHandleColor(Colours::red);
	mainTargetUI->addItemUIListener(this);
	addAndMakeVisible(mainTargetUI);

	
	addExistingItems();
}

MorpherViewUI::~MorpherViewUI()
{
	manager->removeAsyncContainerListener(this);
}

void MorpherViewUI::paintBackground(Graphics & g)
{
	//not paint checkerboard
	BaseManagerShapeShifterViewUI::paintBackground(g);
	
	//Voronoi
	switch(manager->blendMode->getValueDataAsEnum<Morpher::BlendMode>())
	{
		
	case Morpher::Voronoi:
        {
		if (manager->diagram == nullptr || manager->items.size() == 0) break;

		const jcv_site * sites = jcv_diagram_get_sites(manager->diagram);
		for (int i = 0; i < manager->items.size(); i++)
		{
			jcv_site s = sites[i];
			jcv_graphedge * e = s.edges;
			MorphTarget * target = manager->items[s.index];
			Colour c = target->color->getColor();
			float alpha = manager->diagramOpacity->floatValue();
			//if (i == manager->curZoneIndex->intValue()) alpha = jmin<float>(alpha*2,1);
			g.setColour(c.withAlpha(alpha));

			Path p;
			while (e != nullptr)
			{
				Point<float> p1 = getPosInView(Point<float>(e->pos[0].x, e->pos[0].y));
				Point<float> p2 = getPosInView(Point<float>(e->pos[1].x, e->pos[1].y));
				

				if (p.getLength() == 0)
				{
					p.startNewSubPath(p1);
				}
				p.lineTo(p2);

				e = e->next;
			}
			p.closeSubPath();
			g.fillPath(p);
		}
        }
        default:
            break;
	}

	for (MorphTarget * target : manager->items)
	{
		Point<float> np = getPosInView(target->position->getPoint());
		Rectangle<float> wr(np.x - 30, np.y - 30, 60, 60);
		float tSize = wr.getWidth() * target->weight->floatValue();
		Colour c = target->color->getColor();
		g.setColour(c.brighter().withAlpha(.6f));
		g.fillEllipse(wr.withSizeKeepingCentre(tSize, tSize));
		g.setColour(c.brighter());
		g.drawEllipse(wr, 2);
	}
	


	//BG Image
	g.setColour(Colours::white.withAlpha(manager->bgOpacity->floatValue()));
	float tw = manager->bgScale->floatValue();
	float th = bgImage.getHeight()*tw / bgImage.getWidth();
	Rectangle<float> r = getBoundsInView(Rectangle<float>(-tw / 2, -th / 2, tw, th));
	g.drawImage(bgImage, r);





	// TEEEEEEEEEEEEEEEEEEEEEEEMP
	/*	
	Point<float> mp = manager->mainTarget->position->getPoint();

	int index = Morpher::getInstance()->getSiteIndexForPoint(mp);

	if (index >= 0)
	{
		const jcv_site * sites = jcv_diagram_get_sites(manager->diagram);
		jcv_site s = sites[index];

		//curZoneIndex->setValue(getSiteIndexForPoint(mp));

		//Compute direct site
		MorphTarget * mt = manager->items[s.index];
		float d = mp.getDistanceFrom(Point<float>(s.p.x, s.p.y));

		Point<float> mpVPos = getPosInView(mp);
		

		jcv_graphedge * edge = s.edges;
		Array<jcv_graphedge *> edges;

		Array<float> edgeDists;
		Array<float> edgeNeighbourDists;
		Array<jcv_site *> neighbourSites;
		Array<Line<float>> edgeLines;
		Array<Point<float>> neighboursViewPoints;
		Array<Point<float>> edgeViewPoints;

		g.setColour(Colours::white.withAlpha(.1f));

		Point<float> sVPos = getPosInView(Point<float>(s.p.x, s.p.y));
		g.drawLine(mpVPos.x,mpVPos.y,sVPos.x,sVPos.y);

		g.setColour(Colours::white.withAlpha(.6f));

		while (edge != nullptr)
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
			float distNeighbourToEdgePoint = np.getDistanceFrom(Point<float>(ns->p.x, ns->p.y));

			edges.add(edge);

			edgeDists.add(distToEdge);
			edgeNeighbourDists.add(distNeighbourToEdgePoint);
			neighbourSites.add(ns);
			edgeLines.add(line);

			edge = edge->next;

			Point<float> nsVPos = getPosInView(Point<float>(ns->p.x, ns->p.y));
			neighboursViewPoints.add(nsVPos);
			Point<float> npVPos = getPosInView(np);
			edgeViewPoints.add(npVPos);

			g.drawLine(mpVPos.x, mpVPos.y, nsVPos.x, nsVPos.y);
		}

		//DBG("Num edges to test " << edges.size());

		//Compute weight for each neighbour

		g.setColour(Colours::white.withAlpha(.6f));


		for (int i = 0; i<edges.size(); i++)
		{
			float edgeDist = edgeDists[i];
			float edgeNDist = edgeNeighbourDists[i];
			float totalDist = edgeDist + edgeNDist;


			float minOtherEdgeDist = (float)INT_MAX;
			jcv_graphedge * minEdge = nullptr;
			int minEdgeIndex = -1;

			for (int j = 0; j < edges.size(); j++)
			{
				if (i == j) continue;

				if (Morpher::getInstance()->checkSitesAreNeighbours(edges[i]->neighbor, edges[j]->neighbor)) continue;

				float ed = edgeDists[j];
				if (ed < minOtherEdgeDist)
				{
					minOtherEdgeDist = ed;
					minEdge = edges[j];
					minEdgeIndex = j;
				}
			}

			if (minEdge != nullptr)
			{
				float ratio = 1 - (edgeDist / (edgeDist + minOtherEdgeDist));
				g.setColour(Colours::orange);
				g.drawLine(mpVPos.x, mpVPos.y, edgeViewPoints[minEdgeIndex].x, edgeViewPoints[minEdgeIndex].y);
				g.setColour(Colours::yellow);
				g.drawLine(mpVPos.x, mpVPos.y, edgeViewPoints[i].x, edgeViewPoints[i].y);
				g.setColour(Colours::red);
				g.drawLine(edgeViewPoints[i].x, edgeViewPoints[i].y, neighboursViewPoints[i].x, neighboursViewPoints[i].y);
			} else
			{
				float directDist = mp.getDistanceFrom(Point<float>(neighbourSites[i]->p.x, neighbourSites[i]->p.y)); //if we want to check direct distance instead of path to point
				//w = 1.0f / directDist;
				g.setColour(Colours::purple);
				g.drawLine(mpVPos.x,mpVPos.y, neighboursViewPoints[i].x, neighboursViewPoints[i].y);
			}
		}
		
	}
	*/
}

void MorpherViewUI::setupBGImage()
{
	bgImage.clear(bgImage.getBounds(), Colours::transparentWhite);
	File f = File(manager->bgImagePath->stringValue());
	if (f.existsAsFile()) bgImage = ImageFileFormat::loadFrom(f);
	repaint();
}

void MorpherViewUI::resized()
{
	BaseManagerShapeShifterViewUI::resized();
	updateViewUIPosition(mainTargetUI);
}

void MorpherViewUI::itemUIGrabbed(BaseItemUI<MorphTarget>* se)
{
	BaseManagerShapeShifterViewUI::itemUIGrabbed(se);
	se->item->position->setPoint((se->item->viewUIPosition->getPoint() + se->getLocalBounds().getCentre().toFloat()) / getUnitSize());
}


void MorpherViewUI::newMessage(const ContainerAsyncEvent & e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:
		controllableFeedbackUpdateAsync(e.targetContainer, e.targetControllable);
		break;
            
        default:
            break;
	}
}

void MorpherViewUI::controllableFeedbackUpdateAsync(ControllableContainer * cc, Controllable * c)
{
	if (c == manager->bgImagePath)		 setupBGImage();
	else if (c == manager->bgOpacity)	 repaint();
	else if (c == manager->bgScale)		 repaint();
	else if (c == manager->diagramOpacity)		 repaint();
	else if (c == manager->mainTarget->position)
	{
		//DBG("position changed : " << manager->mainTarget->position->getPoint().toString());
		if (!mainTargetUI->isMouseOverOrDragging(true))
		{

			manager->mainTarget->viewUIPosition->setPoint((manager->mainTarget->position->getPoint() * getUnitSize()) - mainTargetUI->getLocalBounds().getCentre().toFloat());
			updateViewUIPosition(mainTargetUI);
		}
		
		//DBG("Target pos : " << manager->mainTarget->viewUIPosition->getPoint().toString());
		//BaseManagerShapeShifterViewUI::updateViewUIPosition(mainTargetUI); //only update ui bounds, not reaffecting target's position parameter
		//repaint();
	}
	else if (dynamic_cast<MorphTarget *>(c->parentContainer) != nullptr) repaint();
	else if (c == manager->blendMode)
	{
		Morpher::BlendMode bm = manager->blendMode->getValueDataAsEnum<Morpher::BlendMode>();
		switch (bm)
		{
		case Morpher::Voronoi:
			mainTargetUI->setVisible(true);
			break;

		case Morpher::Weights:
			mainTargetUI->setVisible(false);
			break;

		case Morpher::GradientBand:
			mainTargetUI->setVisible(true);
			break;
		}

		repaint();
	}
}


