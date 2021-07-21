## 介绍
#### Linkaging是一个基于Reactor模式的网络服务器，由C++11编写

## 技术要点
- 核心是事件循环来响应计时器和IO事件，事件回调接口以`std::function` + `std::bind`来表达。
- Linkaging中的Buffer的实现借鉴了muduo和libevent中的bufferevent。数据的读写是通过buffer来进行的，buffer中封装了read()和write()接口。
- 事件循环处于LkgServer中的run()，作为反应器reactor，run()跑在主线程上，事件循环负责IO事件和定时器事件的分发。
- Epoll类并不简单的是对epoll的封装，属于muduo中Channel和Poller的组合体，分别负责注册与响应IO事件和epoll_wait()
- Linkaging的线程模型符合`one loop per thread` + `thread pool`模型。主线程来负责事件循环   

## 详细文档
| Part I | Part II | Part III 
| :-: | :-: | :-: |
|[服务器模型](./Docs/ServerModel.md)|[服务器架构](./Docs/ServerArch.md)|[Cpp11](./Docs/CPP11.md)
