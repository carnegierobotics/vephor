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