/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"
ApplicationProperties& getAppProperties();
ApplicationCommandManager& getCommandManager();

class MainContentComponent   : public Component,
	public ApplicationCommandTarget, public MenuBarModel, public EngineListener
{
public:
	MainContentComponent();
	~MainContentComponent();
	
	void init();

	TooltipWindow tooltipWindow; // to add tooltips to an application, you
								 // just need to create one of these and leave it
								 // there to do its work..

	ScopedPointer<LookAndFeelOO> lookAndFeelOO;
	ScopedPointer<ProgressWindow> fileProgressWindow;


	void paint(Graphics&) override;
	void resized() override;


	//engine
	void startLoadFile() override;
	void fileProgress(float percent, int state) override;
	void endLoadFile() override;

	// inherited from MenuBarModel , ApplicationCommandTarget
	ApplicationCommandTarget* getNextCommandTarget() override { return findFirstTargetParentComponent(); }
	void getAllCommands(Array<CommandID>& commands) override;
	virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
	virtual bool perform(const InvocationInfo& info) override;
	StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	void menuItemSelected(int /*menuItemID*/, int /*topLevelMenuIndex*/) override;


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainContentComponent)
};
