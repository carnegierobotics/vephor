#include "vephor.h"

using namespace vephor;

int main()
{
    TCPSocket listen_sock;
    auto sock = listen_sock.listen(2001);

    v4print "Connected.";

    auto buf = sock->receive();
    v4print buf_to_string(buf);

    sock->send("Back at ya");

    return 0;
}