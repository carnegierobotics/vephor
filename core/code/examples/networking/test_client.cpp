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
    TCPSocket sock;
    sock.connect("localhost", 2001);

    v4print "Connected.";

    sock.send("To you!");

    auto buf = sock.receive();
    v4print buf_to_string(buf);

    return 0;
}