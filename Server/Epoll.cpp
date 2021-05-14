//
// Created by hlhd on 2021/5/13.
//

#include "Epoll.h"
#include "HttpRequest.h"
#include "ThreadPool.h"

namespace Linkaging
{

Epoll::Epoll() : m_epollFd(::epoll_create(EPOLL_CLOEXEC)), m_events(MAXEVENTS)
{
    assert(m_epollFd >= 0);
}

Epoll::~Epoll()
{
    ::close(m_epollFd);
}

int Epoll::add(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev);
    return ret;
}

int Epoll::mod(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev);
    return ret;
}

int Epoll::del(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev);
    return ret;
}

int Epoll::wait(int timeoutMs)
{
    int eventsNum = ::epoll_wait(m_epollFd, &*m_events.begin(),
                                 static_cast<int>(m_events.size()), timeoutMs);
    if(eventsNum == 0)
    {
        // printf("[Epoll::wait] nothing happen, epoll timeout\n");
    }
    else if(eventsNum < 0)
    {
        printf("[Epoll::wait] epoll : %s\n", strerror(errno));
    }

    return eventsNum;
}

void Epoll::handleEvent(int lsnFd, std::shared_ptr<ThreadPool> &threadPool, int eventsNum)
{
    assert(eventsNum > 0);
    for (int i = 0; i < eventsNum; i ++ )
    {
        HttpRequest* req = (HttpRequest*)(m_events[i].data.ptr);
        int fd = req->getFd();
        if (fd == lsnFd)
        {
            c_buildConnection;
        }
        else
        {
            if ((m_events[i].events & EPOLLERR) ||
                (m_events[i].events & EPOLLHUP) ||
                (m_events[i].events & EPOLLIN))
            {
                req->setNoWorking();
                c_closeConnection(req);
            }
            else if (m_events[i].events & EPOLLIN)
            {
                req->setWorking();
                threadPool->pushJob(std::bind(c_request, req));
            }
            else if (m_events[i].events & EPOLLOUT)
            {
                req->setWorking();
                threadPool->pushJob(std::bind(c_response, req));
            }
            else
            {
                printf("[Epoll::handleEvent()] unexpected event\n");
            }
        }
    }

    return;
}

}