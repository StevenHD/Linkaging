//
// Created by hlhd on 2021/5/14.
//

#include "LkgServer.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "Utils.h"
#include "Epoll.h"
#include "ThreadPool.h"
#include "Timer.h"

namespace Linkaging
{

LkgServer::LkgServer(int port, int numThread)
    : m_port(port), m_lsnFd(Utils::createListenFd(m_port)),
      m_lsnReq(new HttpRequest(m_lsnFd)), m_epoll(new Epoll()),
      m_threadPool(new ThreadPool(numThread)), m_timerManager(new TimerManager())
{
    assert(m_lsnFd >= 0);
}

LkgServer::~LkgServer()
{

}

void LkgServer::run()
{
    m_epoll->add(m_lsnFd, m_lsnReq.get(), (EPOLLIN | EPOLLET));
    m_epoll->setConnectionCallbak(std::bind(&LkgServer::acceptConnection, this));
    m_epoll->setCloseConnectionCallbak(std::bind(&LkgServer::closeConnection, this, std::placeholders::_1));
    m_epoll->setRequest(std::bind(&LkgServer::doRequest, this, std::placeholders::_1));
    m_epoll->setResponse(std::bind(&LkgServer::doResponse, this, std::placeholders::_1));

    while (1)
    {
        int waitTime = m_timerManager->getNearestExpiredTimer();
        int eventsNum = m_epoll->wait(waitTime);
        if (eventsNum > 0)
        {
            m_epoll->handleEvent(m_lsnFd, m_threadPool, eventsNum);
        }
        m_timerManager->handleExpiredTimers();
    }
}

void LkgServer::acceptConnection()
{
    while (1)
    {
        int acceptFd = ::accept4(m_lsnFd, nullptr, nullptr, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (acceptFd == -1)
        {
            if (errno == EAGAIN) break;
            printf("[LkgServer::acceptConnection] accept: %s\n", strerror(errno));
            break;
        }

        HttpRequest* req = new HttpRequest(acceptFd);
        m_timerManager->addTimer(req, CONNECT_TIMEOUT, std::bind(&LkgServer::closeConnection, this, req));
        m_epoll->add(acceptFd, req, (EPOLLIN | EPOLLONESHOT));
    }
}

void LkgServer::closeConnection(HttpRequest *req)
{
    int fd = req->getFd();
    if (req->isWorking()) return;

    m_timerManager->delTimer(req);
    m_epoll->del(fd, req, 0);
    delete req;
    req = nullptr;
}

void LkgServer::doRequest(HttpRequest *req) {
    m_timerManager->delTimer(req);
    assert(req != nullptr);
    int fd = req->getFd();

    int rdErrno;
    int retRead = req->readData(&rdErrno);

    if (retRead == 0) {
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    if (retRead < 0 && (rdErrno != EAGAIN)) {
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    if (retRead < 0 && rdErrno == EAGAIN)
    {
        m_epoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT));
        req->setNoWorking();
        m_timerManager->addTimer(req, CONNECT_TIMEOUT, std::bind(&LkgServer::closeConnection, this, req));
        return;
    }

    if (!req->parseRequest())
    {
        HttpResponse response(400, "", false);
        req->appendOutBuffer(response.makeResponse());
        int wrErrno;
        req->writeData(&wrErrno);
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    if (req->parseFinish())
    {
        HttpResponse response(200, req->getPath(), req->keepAlive());
        req->appendOutBuffer(response.makeResponse());
        m_epoll->mod(fd, req, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
    }
}

void LkgServer::doResponse(HttpRequest *req)
{
    m_timerManager->delTimer(req);
    assert(req != nullptr);
    int fd = req->getFd();
    int bytesToWrite = req->writableBytes();
    if (bytesToWrite == 0)
    {
        m_epoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT));
        req->setNoWorking();
        m_timerManager->addTimer(req, CONNECT_TIMEOUT, std::bind(&LkgServer::closeConnection, this, req));
        return;
    }

    int wrErrno;
    int ret = req->writeData(&wrErrno);

    if (ret < 0 && wrErrno == EAGAIN)
    {
        m_epoll->mod(fd, req, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
        return;
    }

    if (ret < 0 && (wrErrno != EAGAIN))
    {
        req->setNoWorking();
        closeConnection(req);
        return;
    }

    if (ret == bytesToWrite)
    {
        if (req->keepAlive())
        {
            req->resetParse();
            m_epoll->mod(fd, req, (EPOLLIN | EPOLLONESHOT));
            req->setNoWorking();
            m_timerManager->addTimer(req, CONNECT_TIMEOUT, std::bind(&LkgServer::closeConnection, this, req));
        }
        else
        {
            req->setNoWorking();
            closeConnection(req);
        }

        return;
    }

    m_epoll->mod(fd, req, (EPOLLIN | EPOLLOUT | EPOLLONESHOT));
    req->setNoWorking();
    m_timerManager->addTimer(req, CONNECT_TIMEOUT, std::bind(&LkgServer::closeConnection, this, req));
    return;
}

}
