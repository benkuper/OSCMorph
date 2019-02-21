
#include "JuceHeader.h"

ApplicationProperties& getAppProperties();
ApplicationCommandManager& getCommandManager();

class MainContentComponent : public OrganicMainContentComponent
{
public:
	//==============================================================================
	MainContentComponent();
	~MainContentComponent();

	void init() override;

	/*
	void getAllCommands(Array<CommandID>& commands) override;
	virtual void getCommandInfo(CommandID commandID, ApplicationCommandInfo& result) override;
	virtual bool perform(const InvocationInfo& info) override;
	StringArray getMenuBarNames() override;
	virtual PopupMenu getMenuForIndex(int topLevelMenuIndex, const String& menuName) override;
	void fillFileMenuInternal(PopupMenu &menu) override;
	*/
};