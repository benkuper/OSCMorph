/*
  ==============================================================================

    OSCOutput.cpp
    Created: 11 Dec 2017 5:00:21pm
    Author:  Ben

  ==============================================================================
*/

#include "OSCOutput.h"

OSCOutput::OSCOutput(const String & name, int defaultRemotePort) :
	BaseItem(name)
{
	//Send
	remoteHost = addStringParameter("Remote Host", "Remote Host to send to.", "127.0.0.1");
	remotePort = addIntParameter("Remote port", "Port on which the remote host is listening to", defaultRemotePort, 1024, 65535);

	setupSender();
	localPort = addIntParameter("Local Port", "Local Port to bind to receive OSC Messages", 11450, 1024, 65535);
	localPort->hideInOutliner = true;
	localPort->isTargettable = false;

	setupSender();
	//setupReceiver();
}

void OSCOutput::setupSender()
{
	sender.disconnect();
	sender.connect(remoteHost->stringValue(), remotePort->intValue());
	NLOG(niceName, "Now sending to " << remoteHost->stringValue() << ":" << remotePort->intValue());
}

void OSCOutput::sendOSC(const OSCMessage & msg)
{
	/*
	if (logOutgoingData->boolValue())
	{
		NLOG(niceName, "Send OSC : " << msg.getAddressPattern().toString());
	}
	*/
	sender.sendToIPAddress(remoteHost->stringValue(), remotePort->intValue(), msg);
}




/*
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
	String address = msg.getAddressPattern().toString();
	if (address == "/vibrate")
	{
		if (msg.size() >= 3)
		{
			int controllerId = msg[0].getInt32();
			float strength = msg[1].getFloat32();
			float duration = msg[2].getFloat32();
			SourcesManager::getInstance()->vibrateController(controllerId, strength, duration);
		}
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
*/