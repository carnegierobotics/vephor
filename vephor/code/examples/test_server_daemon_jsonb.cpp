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