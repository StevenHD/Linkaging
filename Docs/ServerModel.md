## 一、并发模型
#### 采用了事件驱动模型，基于Epoll实现，通过事件回调的方式实现业务逻辑，并结合了线程池，避免了线程频繁创建和销毁。

## 二、Epoll
```C++
int Epoll::add(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_ADD, fd, &ev);
    return ret;
}
```

```C++
int Epoll::mod(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_MOD, fd, &ev);
    return ret;
}
```

```C++
int Epoll::del(int fd, HttpRequest *req, int evnts)
{
    struct epoll_event ev;
    ev.data.ptr = (void*)req;
    ev.events = evnts;

    int ret = ::epoll_ctl(m_epollFd, EPOLL_CTL_DEL, fd, &ev);
    return ret;
}
```

## 三、Reactor模型
#### Linkaging采用了同步非阻塞I/O模型Reactor
- 应用程序注册读就绪事件和相关联的事件处理器
- 事件分离器等待事件的发生
- 当发生读就绪事件的时候，事件分离器调用第一步注册的事件处理器
- 事件处理器首先执行实际的读取操作，然后根据读取到的内容进行进一步的处理
