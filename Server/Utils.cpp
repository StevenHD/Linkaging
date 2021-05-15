//
// Created by hlhd on 2021/4/29.
//

#include "Utils.h"

namespace Linkaging
{

int Utils::createListenFd(int port)
{
    // 处理非法端口
    port = ((port <= 1024) || (port >= 65535)) ? 9999 : port;

    // 创建套接字（IPv4，TCP，非阻塞）
    int lsnFd = 0;
    if ((lsnFd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) == -1) // O_NONBLOCK??
    {
        printf("[Utils::createListenFd] lsnFd = %d socket: %s\n", lsnFd, strerror(errno));
        return -1;
    }

    // 避免"Address already in use"
    int optVal = 1;
    if (::setsockopt(lsnFd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optVal, sizeof(int)) == -1)
    {
        printf("[Utils::createListenFd] lsnFd = %d setsockopt: %s\n", lsnFd, strerror(errno));
        return -1;
    }

    // 绑定IP和端口
    struct sockaddr_in serverAddr;
    ::bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    serverAddr.sin_port = ::htons((unsigned short)port);

    if (::bind(lsnFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        printf("[Utils::createListenFd] lsnFd = %d bind: %s\n", lsnFd, strerror(errno));
        return -1;
    }

    // 开始监听，监听队列最大为LSTNQUE
    if (::listen(lsnFd, LSTNQUE) == -1)
    {
        printf("[Utils::createListenFd] lsnFd = %d listen: %s\n", lsnFd, strerror(errno));
        return -1;
    }

    // 关闭无效监听描述符
    if (lsnFd == -1)
    {
        ::close(lsnFd);
        return -1;
    }

    return lsnFd;
}

int Utils::setNonBlocking(int fd)
{
    // 获取套接字选项
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag == -1)
    {
        printf("[Utils::setNonBlocking] lsnFd = %d fcntl: %s\n", fd, strerror(errno));
        return -1;
    }

    // 设置非阻塞
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) == -1)
    {
        printf("[Utils::setNonBlocking] lsnFd = %d fcntl: %s\n", fd, strerror(errno));
        return -1;
    }

    return 0;
}

}