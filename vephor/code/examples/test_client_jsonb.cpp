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