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
	public BaseItem,
	public OSCReceiver::Listener<OSCReceiver::MessageLoopCallback>
{
public:
	OSCOutput(const String &name = "OSC", int defaultRemotePort = 9000);
	~OSCOutput() {}

	//SEND
	BoolParameter * logIncoming;
	BoolParameter * logOutgoing;

	StringParameter * remoteHost;
	IntParameter * remotePort;

	IntParameter * localPort;

	BoolParameter * sendWeights;
	BoolParameter * sendValues;
	BoolParameter * sendTarget;

	OSCSender sender;
	void setupSender();

	void sendOSC(const OSCMessage &msg);

	static OSCOutput * create() { return new OSCOutput(); }

	void onContainerParameterChangedInternal(Parameter *) override;
	
	OSCReceiver receiver;
	void setupReceiver();
	void processMessage(const OSCMessage &msg);

	float getFloatArg(OSCArgument a);
	int getIntArg(OSCArgument a);
	String getStringArg(OSCArgument a);
	static var argumentToVar(const OSCArgument &a);


	virtual void oscMessageReceived(const OSCMessage & message) override;
	virtual void oscBundleReceived(const OSCBundle & bundle) override;
	
};

