
#include "MainComponent.h"
#include "MorpherViewUI.h"
#include "OSCOutputManagerUI.h"

String getAppVersion();

//==============================================================================
MainContentComponent::MainContentComponent()
{
	getCommandManager().registerAllCommandsForTarget(this);
}


MainContentComponent::~MainContentComponent()
{

}

void MainContentComponent::init()
{
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Morpher", &MorpherViewUI::create));
	ShapeShifterFactory::getInstance()->defs.add(new ShapeShifterDefinition("Outputs", &OSCOutputManagerUI::create));

	OrganicMainContentComponent::init();
}
