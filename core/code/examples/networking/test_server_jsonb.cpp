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