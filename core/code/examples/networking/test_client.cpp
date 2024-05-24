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
    TCPSocket sock;
    sock.connect("localhost", 2001);

    v4print "Connected.";

    sock.send("To you!");

    auto buf = sock.receive();
    v4print buf_to_string(buf);

    return 0;
}