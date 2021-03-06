//
// Created by hlhd on 2021/4/26.
//

#ifndef MODERNCPP_TIMER_H
#define MODERNCPP_TIMER_H

#include "../all.h"

namespace Linkaging
{
using TimeoutCallbak = std::function<void()>;
using Clock = std::chrono::high_resolution_clock;
using MS = std::chrono::milliseconds;
using TimeStamp = Clock::time_point;

class HttpRequest;

class Timer
{
private:
    TimeStamp m_expiredTime;
    TimeoutCallbak c_callBack;
    bool m_delete;

public:
    /* like muduo's runAt() */
    Timer(const TimeStamp &when, const TimeoutCallbak &cb)
            : m_expiredTime(when), c_callBack(cb), m_delete(false) {}

    ~Timer() {}

    void del()
    {
        m_delete = true;
    }

    bool isDeleted()
    {
        return m_delete;
    }

    void runCallBack()
    {
        c_callBack();
    }

    TimeStamp getExpiredTime() const
    {
        return m_expiredTime;
    }
};

struct cmp
{
    bool operator()(Timer *lhs, Timer *rhs)
    {
        assert(lhs != nullptr && rhs != nullptr);
        return (lhs->getExpiredTime()) > (rhs->getExpiredTime());
    }
};

class TimerManager
{
private:
    using TimerQueue = std::priority_queue<Timer *, std::vector<Timer *>, cmp>;
    TimerQueue m_timerQueue;
    TimeStamp m_nowTime;
    std::mutex m_mtx;

public:
    TimerManager()
            : m_nowTime(Clock::now()) {}

    ~TimerManager() {}

    void updateTime()
    {
        m_nowTime = Clock::now();
    }

    void addTimer(HttpRequest *req, const int &time_out, const TimeoutCallbak &cb);

    void delTimer(HttpRequest *req);

    void handleExpiredTimers();

    int getNearestExpiredTimer();
};
}
#endif //MODERNCPP_TIMER_H
