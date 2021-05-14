//
// Created by hlhd on 2021/5/14.
//

#ifndef LINKAGING_LKGSERVER_H
#define LINKAGING_LKGSERVER_H

#include "../all.h"

#define TIMEOUTMS -1
#define CONNECT_TIMEOUT 500
#define NUM_WORKERS 4

namespace Linkaging
{

class HttpRequest;
class Epoll;
class ThreadPool;
class TimerManager;

class LkgServer
{
public:
    LkgServer(int port, int numThread);
    ~LkgServer();
    void run();

private:
    void acceptConnection();
    void closeConnection(HttpRequest* req);
    void doRequest(HttpRequest* req);
    void doResponse(HttpRequest* req);

private:
    using RequestPtr = std::unique_ptr<HttpRequest>;
    using EpollPtr = std::unique_ptr<Epoll>;
    using ThreadPoolPtr = std::shared_ptr<ThreadPool>;
    using TimerManagerPtr = std::unique_ptr<TimerManager>;

    int m_port;
    int m_lsnFd;

    RequestPtr m_lsnReq;
    EpollPtr m_epoll;
    ThreadPoolPtr m_threadPool;
    TimerManagerPtr m_timerManager;
};

}

#endif //LINKAGING_LKGSERVER_H
