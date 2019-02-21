/*
  ==============================================================================

    OSCOutput.cpp
    Created: 11 Dec 2017 5:00:21pm
    Author:  Ben

  ==============================================================================
*/

#include "OSCOutput.h"
#include "Morpher.h"

OSCOutput::OSCOutput(const String & name, int defaultRemotePort) :
	BaseItem(name)
{
	logIncoming = addBoolParameter("Log Incoming", "Log incoming messages", false);
	logOutgoing = addBoolParameter("Log Outgoing", "Log outgoing messages", false);

	//Send
	localPort = addIntParameter("Local Port", "Local Port to bind to receive OSC Messages", 11450, 1024, 65535);
	remoteHost = addStringParameter("Remote Host", "Remote Host to send to.", "127.0.0.1");
	remotePort = addIntParameter("Remote port", "Port on which the remote host is listening to", defaultRemotePort, 1024, 65535);

	sendValues = addBoolParameter("Send Values", "Send computed values of the Morpher", true);
	sendWeights = addBoolParameter("Send Weights", "Send weights of each MorphTarget", false);
	sendTarget = addBoolParameter("Send Target Position", "Send target position", false);

	receiver.addListener(this);
	setupSender();
	setupReceiver();
}

void OSCOutput::setupSender()
{
	sender.disconnect();
	sender.connect(remoteHost->stringValue(), remotePort->intValue());
	NLOG(niceName, "Now sending to " << remoteHost->stringValue() << ":" << remotePort->intValue());
}

void OSCOutput::sendOSC(const OSCMessage & msg)
{
	if (!enabled->boolValue()) return;
	
	if (logOutgoing->boolValue())
	{
		NLOG(niceName, "Send OSC : " << msg.getAddressPattern().toString());
	}
	
	sender.sendToIPAddress(remoteHost->stringValue(), remotePort->intValue(), msg);
}


void OSCOutput::onContainerParameterChangedInternal(Parameter * p)
{
	if (p == localPort) setupReceiver();
	else if (p == remoteHost || p == remotePort) setupSender();
}

void OSCOutput::setupReceiver()
{
bool result = receiver.connect(localPort->intValue());

if (result)
{
NLOG(niceName, "Now receiving on port : " + localPort->stringValue());
} else
{
NLOG(niceName, "Error binding port " + localPort->stringValue());
}

Array<IPAddress> ad;
IPAddress::findAllAddresses(ad);

String s = "Local IPs:";
for (auto &a : ad) s += String("\n > ") + a.toString();
NLOG(niceName, s);
}


void OSCOutput::processMessage(const OSCMessage & msg)
{
	if (!enabled->boolValue()) return;

	String address = msg.getAddressPattern().toString();
	//DBG("Receive : " << address << " with " << msg.size() << " args");
	
	StringArray addSplit;
	addSplit.addTokens(address, "/", "\"");

	if (logIncoming->boolValue())
	{
		NLOG(niceName, "Receive OSC : " << msg.getAddressPattern().toString());
	}

	if (address == "/target/position" || address == "/3/xy") //default simple layout for touchosc
	{
		if (msg.size() >= 2)
		{
			Morpher::getInstance()->setTargetPosition(getFloatArg(msg[0]) - .5f, getFloatArg(msg[1]) - .5f);
		}
	}else if(addSplit[1] == "attraction")
	{
		String id = addSplit[2];
		MorphTarget * mt = Morpher::getInstance()->getItemWithName(id, true);
		if (mt != nullptr && msg.size() >= 1 && Morpher::getInstance()->blendMode->getValueDataAsEnum<Morpher::BlendMode>() == Morpher::Voronoi) mt->attraction->setValue(getFloatArg(msg[0]));
	} else
	{
		String id = addSplit[1];
		
		MorphTarget * mt = Morpher::getInstance()->getItemWithName(id, true);

		if (mt != nullptr && msg.size() >= 1 && Morpher::getInstance()->blendMode->getValueDataAsEnum<Morpher::BlendMode>() == Morpher::Weights) mt->weight->setValue(getFloatArg(msg[0]));

		Morpher::getInstance()->computeWeights();
	}
}

void OSCOutput::oscMessageReceived(const OSCMessage & message)
{
	if (!enabled->boolValue()) return;
	processMessage(message);
}

void OSCOutput::oscBundleReceived(const OSCBundle & bundle)
{
	if (!enabled->boolValue()) return;
	for (auto &m : bundle)
	{
		processMessage(m.getMessage());
	}
}


float OSCOutput::getFloatArg(OSCArgument a)
{
	if (a.isFloat32()) return a.getFloat32();
	if (a.isInt32()) return (float)a.getInt32();
	if (a.isString()) return a.getString().getFloatValue();
	return 0;
}

int OSCOutput::getIntArg(OSCArgument a)
{
	if (a.isInt32()) return a.getInt32();
	if (a.isFloat32()) return roundToInt(a.getFloat32());
	if (a.isString()) return a.getString().getIntValue();
	return 0;
}

String OSCOutput::getStringArg(OSCArgument a)
{
	if (a.isString()) return a.getString();
	if (a.isInt32()) return String(a.getInt32());
	if (a.isFloat32()) return String(a.getFloat32());
	return String();
}

var OSCOutput::argumentToVar(const OSCArgument & a)
{
	if (a.isFloat32()) return var(a.getFloat32());
	else if (a.isInt32()) return var(a.getInt32());
	else if (a.isString()) return var(a.getString());
	return var("error");
}