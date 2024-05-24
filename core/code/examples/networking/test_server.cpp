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
    TCPSocket listen_sock;
    auto sock = listen_sock.accept(2001);

    v4print "Connected.";

    auto buf = sock->receive();
    v4print buf_to_string(buf);

    sock->send("Back at ya");

    return 0;
}