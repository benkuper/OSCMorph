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
	animateItemOnAdd = false;


	//canNavigate = false;
	setupBGImage();
	manager->addAsyncContainerListener(this);
	manager->addMorpherListener(this);

	addExistingItems();

	mainTargetUI = new MorphTargetViewUI(manager->mainTarget);
	mainTargetUI->setHandleColor(Colours::transparentBlack);
	mainTargetUI->addItemUIListener(this);
	mainTargetUI->removeBT->setVisible(false);
	mainTargetUI->enabledBT->setVisible(false);
	addAndMakeVisible(mainTargetUI);
	mainTargetUI->setAlwaysOnTop(true);

	mainTargetUI->setViewZoom(viewZoom);
	
	setInterceptsMouseClicks(true, true);
	setRepaintsOnMouseActivity(true);
}

MorpherViewUI::~MorpherViewUI()
{
	manager->removeMorpherListener(this);
	manager->removeAsyncContainerListener(this);
}

void MorpherViewUI::paintBackground(Graphics & g)
{
	//not paint checkerboard
	BaseManagerShapeShifterViewUI::paintBackground(g);
	
	Morpher::BlendMode bm = manager->blendMode->getValueDataAsEnum<Morpher::BlendMode>();
	//Voronoi
	switch(bm)
	{
		
	case Morpher::Voronoi:
        {
		if (manager->diagram == nullptr || manager->items.size() == 0) break;

		const jcv_site * sites = jcv_diagram_get_sites(manager->diagram);
		for (int i = 0; i < manager->diagram->numsites; i++)
		{
			jcv_site s = sites[i];
			jcv_graphedge * e = s.edges;
			MorphTarget * target = manager->getEnabledTargetAtIndex(s.index);
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


	//BG Image
	g.setColour(Colours::white.withAlpha(manager->bgOpacity->floatValue()));
	float tw = manager->bgScale->floatValue();
	float th = bgImage.getHeight()*tw / bgImage.getWidth();
	Rectangle<float> r = getBoundsInView(Rectangle<float>(-tw / 2, -th / 2, tw, th));
	g.drawImage(bgImage, r);


	if(bm == Morpher::Voronoi)
	{
		if (Morpher::getInstance()->showDebug->boolValue())
		{
			Point<float> mp = manager->mainTarget->position->getPoint();

			int index = Morpher::getInstance()->getSiteIndexForPoint(mp);

			if (index >= 0)
			{
				const jcv_site * sites = jcv_diagram_get_sites(manager->diagram);
				jcv_site s = sites[index];

				//curZoneIndex->setValue(getSiteIndexForPoint(mp));

				//Compute direct site
				//MorphTarget * mt = manager->getEnabledTargetAtIndex(s.index);
				//float d = mp.getDistanceFrom(Point<float>(s.p.x, s.p.y));

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
				g.drawLine(mpVPos.x, mpVPos.y, sVPos.x, sVPos.y);

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


				for (int i = 0; i < edges.size(); i++)
				{
					//float edgeDist = edgeDists[i];
					//float edgeNDist = edgeNeighbourDists[i];
					//float totalDist = edgeDist + edgeNDist;


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
						//float ratio = 1 - (edgeDist / (edgeDist + minOtherEdgeDist));
						g.setColour(Colours::orange);
						g.drawLine(mpVPos.x, mpVPos.y, edgeViewPoints[minEdgeIndex].x, edgeViewPoints[minEdgeIndex].y);
						g.setColour(Colours::yellow);
						g.drawLine(mpVPos.x, mpVPos.y, edgeViewPoints[i].x, edgeViewPoints[i].y);
						g.setColour(Colours::red);
						g.drawLine(edgeViewPoints[i].x, edgeViewPoints[i].y, neighboursViewPoints[i].x, neighboursViewPoints[i].y);
					} else
					{
						//float directDist = mp.getDistanceFrom(Point<float>(neighbourSites[i]->p.x, neighbourSites[i]->p.y)); //if we want to check direct distance instead of path to point
																															 //w = 1.0f / directDist;
						g.setColour(Colours::purple);
						g.drawLine(mpVPos.x, mpVPos.y, neighboursViewPoints[i].x, neighboursViewPoints[i].y);
					}
				}
			}
		}

		if (Morpher::getInstance()->useAttraction->boolValue())
		{
			Point<float> mtp = Morpher::getInstance()->mainTarget->position->getPoint();
			Point<float> mp = getPosInView(mtp);
			Point<float> tp = getPosInView(mtp + Morpher::getInstance()->attractionDir);
			g.setColour(Colours::yellow);
			g.drawLine(mp.x, mp.y, tp.x, tp.y);
			g.fillEllipse(tp.x-5, tp.y-5, 10, 10);
		}
	}
		
	
	
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

void MorpherViewUI::setViewZoom(float value)
{
	BaseManagerViewUI::setViewZoom(value);
	mainTargetUI->setViewZoom(value);
	updateViewUIPosition(mainTargetUI);
}

void MorpherViewUI::weightsUpdated()
{
	MessageManagerLock mmLock;
	if (mmLock.lockWasGained())
	{
		for (auto &mui : itemsUI) mui->repaint();
	}
	repaint();
}

void MorpherViewUI::mouseDown(const MouseEvent & e)
{
	BaseManagerShapeShifterViewUI::mouseDown(e);
	if (e.mods.isLeftButtonDown() && e.mods.isCommandDown())
	{
		manager->mainTarget->viewUIPosition->setPoint(getViewMousePosition().toFloat()/viewZoom);
	}
}

void MorpherViewUI::mouseDrag(const MouseEvent & e)
{
	BaseManagerShapeShifterViewUI::mouseDrag(e);

	if (e.mods.isLeftButtonDown() && e.mods.isCommandDown())
	{
		manager->mainTarget->viewUIPosition->setPoint(getViewMousePosition().toFloat()/viewZoom);
	}
}

void MorpherViewUI::mouseDoubleClick(const MouseEvent & e)
{
	BaseManagerShapeShifterViewUI::mouseDoubleClick(e);

	manager->addItem(getViewMousePosition().toFloat());
}

bool MorpherViewUI::keyPressed(const KeyPress & e)
{
	if (e.getModifiers().isCommandDown() && e.getKeyCode() == KeyPress::createFromDescription("i").getKeyCode())
	{
		manager->addTargetAtCurrentPosition->trigger();
		return true;
	}

	return BaseManagerShapeShifterViewUI::keyPressed(e);
}

void MorpherViewUI::itemUIGrabbed(BaseItemUI<MorphTarget>* se)
{
	BaseManagerShapeShifterViewUI::itemUIGrabbed(se);
	se->item->position->setPoint((se->item->viewUIPosition->getPoint()) / defaultCheckerSize);
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
		if (!mainTargetUI->isMouseOverOrDragging(true))
		{

			manager->mainTarget->viewUIPosition->setPoint(manager->mainTarget->position->getPoint() * defaultCheckerSize);
			updateViewUIPosition(mainTargetUI);
		}

		//DBG("Target pos : " << manager->mainTarget->viewUIPosition->getPoint().toString());
		//BaseManagerShapeShifterViewUI::updateViewUIPosition(mainTargetUI); //only update ui bounds, not reaffecting target's position parameter
		//repaint();
	}
	else if (dynamic_cast<MorphTarget *>(c->parentContainer) != nullptr)
	{
		MorphTarget * t = dynamic_cast<MorphTarget *>(c->parentContainer);
		if (c == t->position || c == t->color || c == t->enabled) repaint();
		//
	}
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
	} else if (c == manager->targetSize)
	{
		float s = Morpher::getInstance()->targetSize->floatValue();
		for (auto &mui : itemsUI)
		{
			mui->margin = 0;
			mui->headerHeight = 15;
			mui->grabberHeight = 0;
			mui->resizerHeight = 0;
			mui->resizerWidth = 0;
			mui->headerGap = 0;
			mui->setContentSize(s, s-mui->headerHeight);
			mui->item->viewUIPosition->setPoint(mui->item->position->getPoint() * defaultCheckerSize);
			updateViewUIPosition(mui);
		}
		
	}
}


