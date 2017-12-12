/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic startup code for a Juce application.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

#include "MorphEngine.h"


String getAppVersion();

//==============================================================================
class OSCMorphApplication  : public JUCEApplication
{
public:
    //==============================================================================
    OSCMorphApplication() {}

	ScopedPointer<ApplicationProperties> appProperties;
	ApplicationCommandManager commandManager;
	ScopedPointer<MorphEngine> engine;

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {

		PropertiesFile::Options options;
		options.applicationName = "OSCMorph";
		options.filenameSuffix = "settings";
		options.osxLibrarySubFolder = "Preferences";

		appProperties = new ApplicationProperties();
		appProperties->setStorageParameters(options);


		engine = new MorphEngine(appProperties, getAppVersion());

		mainWindow = new MainWindow(getApplicationName());

		engine->parseCommandline(commandLine);
		if (!engine->getFile().existsAsFile()) {
			engine->createNewGraph();
			engine->setChangedFlag(false);
		}

		mainWindow->setName(getApplicationName() + " " + getApplicationVersion());

    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        mainWindow = nullptr; // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainContentComponent class.
    */
    class MainWindow    : public DocumentWindow
    {
    public:
		MainContentComponent * mainComponent;
		
#if JUCE_OPENGL
		OpenGLContext openGLContext;
#endif
		MainWindow (String name)  : DocumentWindow (name,
                                                    Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                    DocumentWindow::allButtons)
        {
			setResizable(true, true);
			setUsingNativeTitleBar(true);
			mainComponent = new MainContentComponent();
			setContentOwned(mainComponent, true);
			setOpaque(true);


			int tx = getAppProperties().getCommonSettings(true)->getIntValue("windowX");
			int ty = getAppProperties().getCommonSettings(true)->getIntValue("windowY");
			int tw = getAppProperties().getCommonSettings(true)->getIntValue("windowWidth");
			int th = getAppProperties().getCommonSettings(true)->getIntValue("windowHeight");
			bool fs = getAppProperties().getCommonSettings(true)->getBoolValue("fullscreen", true);

			setBounds(jmax<int>(tx, 20), jmax<int>(ty, 20), jmax<int>(tw, 100), jmax<int>(th, 100));
			setFullScreen(fs);

#if ! JUCE_MAC
			setMenuBar(mainComponent);
#endif

			setVisible(true);


#if JUCE_OPENGL && JUCE_WINDOWS
			openGLContext.setComponentPaintingEnabled(true);
			openGLContext.attachTo(*this);
#endif
			mainComponent->init();

        }

        void closeButtonPressed() override
        {
			var boundsVar = var(new DynamicObject());
			juce::Rectangle<int> r = getScreenBounds();

			getAppProperties().getCommonSettings(true)->setValue("windowX", r.getPosition().x);
			getAppProperties().getCommonSettings(true)->setValue("windowY", r.getPosition().y);
			getAppProperties().getCommonSettings(true)->setValue("windowWidth", r.getWidth());
			getAppProperties().getCommonSettings(true)->setValue("windowHeight", r.getHeight());
			getAppProperties().getCommonSettings(true)->setValue("fullscreen", isFullScreen());
			getAppProperties().getCommonSettings(true)->saveIfNeeded();

#if JUCE_OPENGL && JUCE_WINDOWS
			openGLContext.detach();
#endif

			JUCEApplication::getInstance()->systemRequestedQuit();
        }


    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };


    ScopedPointer<MainWindow> mainWindow;
};


static OSCMorphApplication& getApp() { return *dynamic_cast<OSCMorphApplication*>(JUCEApplication::getInstance()); }
String getAppVersion() { return getApp().getApplicationVersion(); }
ApplicationProperties& getAppProperties() { return *getApp().appProperties; }

OpenGLContext * getOpenGLContext() { return &getApp().mainWindow->openGLContext; }

ApplicationCommandManager& getCommandManager() { return getApp().commandManager; }
OSCMorphApplication::MainWindow * getMainWindow() { return getApp().mainWindow; }


//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (OSCMorphApplication)
