//
// Created by hlhd on 2021/4/29.
//

#ifndef MODERNCPP_UTILS_H
#define MODERNCPP_UTILS_H

#define LSTNQUE 1024 // 监听队列长度,操作系统默认值为SOMAXCONN

#include "../all.h"

namespace Linkaging
{

namespace Utils
{
    int createListenFd(int port);   // 创建监听描述符
    int setNonBlocking(int fd);     // 设置非阻塞模式
};

}


#endif //MODERNCPP_UTILS_H
