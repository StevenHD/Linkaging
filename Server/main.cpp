//
// Created by hlhd on 2021/5/15.
//

#include "LkgServer.h"

int main(int argc, char** argv)
{
    int port = 9998;
    if(argc >= 2)
    {
        port = atoi(argv[1]);
    }

    int numThread = 4;
    if(argc >= 3)
    {
        numThread = atoi(argv[2]);
    }

    Linkaging::LkgServer server(port, numThread);
    server.run();

    return 0;
}
