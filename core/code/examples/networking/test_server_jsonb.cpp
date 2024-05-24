/**
 * Copyright 2023
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This code is provided under the terms of the Master Services Agreement (the Agreement).
 * This code constitutes CRL Background Intellectual Property, as defined in the Agreement.
**/

#include "vephor.h"

using namespace vephor;

int main()
{
    NetworkManager net;
	net.connectServer(true, 2001);

    v4print "Connected.";

	while (true)
	{
		auto msgs = net.getIncomingJSONBMessagesFromAll();
		if (!msgs.empty())
		{
			v4print msgs.begin()->header;
			break;
		}
	}

	net.sendJSONBMessageToAll({"from","server"}, {});

    return 0;
}