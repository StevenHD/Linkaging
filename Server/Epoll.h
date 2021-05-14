//
// Created by hlhd on 2021/5/13.
//

#ifndef LINKAGING_EPOLL_H
#define LINKAGING_EPOLL_H

#include "../all.h"

#define MAXEVENTS 1024

namespace Linkaging
{

class HttpRequest;
class ThreadPool;

class Epoll
{
public:
    using NewConnectionCallbak      = std::function<void()>;
    using CloseConnectionCallbak    = std::function<void(HttpRequest*)>;
    using HandleRequestCallbak      = std::function<void(HttpRequest*)>;
    using HandleResponseCallbak     = std::function<void(HttpRequest*)>;

public:
    Epoll();
    ~Epoll();

public:
    int add(int fd, HttpRequest* req, int evnts);
    int mod(int fd, HttpRequest* req, int evnts);
    int del(int fd, HttpRequest* req, int evnts);

public:
    void setConnectionCallbak(const NewConnectionCallbak& cb)
    {
        c_buildConnection = cb;
    }

    void setCloseConnectionCallbak(const CloseConnectionCallbak& cb)
    {
        c_closeConnection = cb;
    }

    void setRequest(const HandleRequestCallbak& cb)
    {
        c_request = cb;
    }

    void setResponse(const HandleResponseCallbak& cb)
    {
        c_response = cb;
    }

public:
    int wait(int timeoutMs);
    void handleEvent(int lsnFd, std::shared_ptr<ThreadPool>& threadPool, int eventsNum);

private:
    using m_eventList = std::vector<struct epoll_event>;
    int   m_epollFd;
    m_eventList m_events;

    NewConnectionCallbak c_buildConnection;
    CloseConnectionCallbak c_closeConnection;
    HandleRequestCallbak c_request;
    HandleResponseCallbak c_response;
};

}   // namespace Linkaging
#endif //LINKAGING_EPOLL_H
