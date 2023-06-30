#include "vephor.h"

using namespace vephor;

int main()
{
    TCPSocket sock;
    sock.connect("localhost", 2001);

    v4print "Connected.";

    sock.send("To you!");

    auto buf = sock.receive();
    v4print buf_to_string(buf);

    return 0;
}