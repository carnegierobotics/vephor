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
	net.connectClient(true);

    v4print "Connected.";

    while (true)
	{
		net.sendJSONBMessageToAll({{"from","client"}}, {});
		
		auto msgs = net.getIncomingJSONBMessagesFromAll();
		for (const auto& msg : msgs)
		{
			v4print msg.header;
		}
		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    return 0;
}