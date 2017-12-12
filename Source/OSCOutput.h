/*
  ==============================================================================

    OSCOutput.h
    Created: 11 Dec 2017 5:00:21pm
    Author:  Ben

  ==============================================================================
*/

#pragma once

#include "JuceHeader.h"

class OSCOutput :
	public BaseItem
	//public OSCReceiver::Listener<OSCReceiver::RealtimeCallback>
{
public:
	OSCOutput(const String &name = "OSC", int defaultRemotePort = 9000);
	~OSCOutput() {}

	//SEND
	StringParameter * remoteHost;
	IntParameter * remotePort;

	IntParameter * localPort;

	OSCSender sender;
	void setupSender();
	void sendOSC(const OSCMessage &msg);

	static OSCOutput * create() { return new OSCOutput(); }

	/*
	OSCReceiver receiver;
	void setupReceiver();
	void processMessage(const OSCMessage &msg);

	virtual void oscMessageReceived(const OSCMessage & message) override;
	virtual void oscBundleReceived(const OSCBundle & bundle) override;
	*/
};

