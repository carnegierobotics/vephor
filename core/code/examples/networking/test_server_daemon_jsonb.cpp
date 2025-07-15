/**
 * Copyright 2025
 * Carnegie Robotics, LLC
 * 4501 Hatfield Street, Pittsburgh, PA 15201
 * https://www.carnegierobotics.com
 *
 * This source code is licensed under the Apache License, Version 2.0
 * found in the LICENSE file in the root directory of this source tree.
**/

#include "vephor.h"

using namespace vephor;

int main()
{
    NetworkManager net;
	net.connectServer(false, 2001);

    v4print "Connected.";

	while (true)
	{
		auto msgs = net.getIncomingJSONBMessagesFromAll();
		for (const auto& msg : msgs)
		{
			v4print msg.header;
		}
		
		net.sendJSONBMessageToAll({{"from","server"}}, {});
		
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

    return 0;
}