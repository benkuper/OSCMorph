/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"
#include "MorphEngine.h"
#include "Morpher.h"

//==============================================================================
class OSCMorphApplication : public OrganicApplication
{
public:
	//==============================================================================
	OSCMorphApplication() : 
		OrganicApplication("OSCMorph", true)
	{
	}
	 
	void initialiseInternal(const String& commandLine) override
	{
		engine = new MorphEngine();
		mainComponent = new MainContentComponent();

		ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_morphlayout);
		ShapeShifterManager::getInstance()->setLayoutInformations("morphlayout", "OSCMorph/layouts");
	}

};

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (OSCMorphApplication)
