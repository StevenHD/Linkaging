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
        m_connection = cb;
    }

    void setCloseConnectionCallbak(const CloseConnectionCallbak& cb)
    {
        m_closeConnection = cb;
    }

    void setRequest(const HandleRequestCallbak& cb)
    {
        m_request = cb;
    }

    void setResponse(const HandleResponseCallbak& cb)
    {
        m_response = cb;
    }

public:
    int wait(int timeoutMs);
    void handleEvent(int lsnFd, std::shared_ptr<ThreadPool>& threadPool, int eventsNum);

private:
    using m_eventList = std::vector<struct epoll_event>;
    int   m_epollFd;
    m_eventList m_events;

    NewConnectionCallbak m_connection;
    CloseConnectionCallbak m_closeConnection;
    HandleRequestCallbak m_request;
    HandleResponseCallbak m_response;
};

}   // namespace Linkaging
#endif //LINKAGING_EPOLL_H
