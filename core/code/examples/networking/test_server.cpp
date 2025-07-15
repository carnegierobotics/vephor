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
    TCPSocket listen_sock;
    auto sock = listen_sock.accept(2001);

    v4print "Connected.";

    auto buf = sock->receive();
    v4print buf_to_string(buf);

    sock->send("Back at ya");

    return 0;
}