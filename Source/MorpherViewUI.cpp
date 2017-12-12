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

	updateViewUIPosition(mainTargetUI);
	
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
	if (manager->blendMode->getValueDataAsEnum<Morpher::BlendMode>() == Morpher::Voronoi && manager->diagram != nullptr && manager->items.size() > 0)
	{
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

			Point<float> np = getPosInView(Point<float>(s.p.x, s.p.y));
			Rectangle<float> wr(np.x-30,np.y-30, 60, 60);
			float tSize = wr.getWidth() * target->weight->floatValue();
			g.setColour(c.brighter().withAlpha(.6f));
			g.fillEllipse(wr.withSizeKeepingCentre(tSize,tSize));
			g.setColour(c.brighter());
			g.drawEllipse(wr, 2);
		}
	}


	//BG Image
	g.setColour(Colours::white.withAlpha(manager->bgOpacity->floatValue()));
	float tw = manager->bgScale->floatValue();
	float th = bgImage.getHeight()*tw / bgImage.getWidth();
	Rectangle<float> r = getBoundsInView(Rectangle<float>(-tw / 2, -th / 2, tw, th));
	g.drawImage(bgImage, r);
}

void MorpherViewUI::setupBGImage()
{
	bgImage.clear(bgImage.getBounds(), Colours::transparentWhite);
	File f = File(manager->bgImagePath->stringValue());
	if (f.existsAsFile()) bgImage = ImageFileFormat::loadFrom(f);
	repaint();
}

void MorpherViewUI::updateViewUIPosition(MorphTargetViewUI * mtvui)
{
	BaseManagerShapeShifterViewUI::updateViewUIPosition(mtvui);
	mtvui->item->position->setPoint((mtvui->item->viewUIPosition->getPoint()+mtvui->getLocalBounds().getCentre().toFloat()) / getUnitSize());
}

void MorpherViewUI::newMessage(const ContainerAsyncEvent & e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:
		controllableFeedbackUpdateAsync(e.targetContainer, e.targetControllable);
		break;
	}
}

void MorpherViewUI::controllableFeedbackUpdateAsync(ControllableContainer * cc, Controllable * c)
{
	if (c == manager->bgImagePath)		 setupBGImage();
	else if (c == manager->bgOpacity)	 repaint();
	else if (c == manager->bgScale)		 repaint();
	else if (c == manager->diagramOpacity)		 repaint();
	else if (cc == manager->mainTarget.get())
	{
		updateViewUIPosition(mainTargetUI);
		repaint();
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
