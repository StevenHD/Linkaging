//
// Created by hlhd on 2021/4/26.
//

#ifndef MODERNCPP_TIMER_H
#define MODERNCPP_TIMER_H

#include "../all.h"
#include "HttpRequest.h"

namespace Linkaging
{
    using TimeoutCallback = std::function<void(void)>;
    using Clock = std::chrono::high_resolution_clock;
    using MS = std::chrono::milliseconds;
    using TimeStamp = Clock::time_point;

    class HttpRequest;

    std::function<void(void)> b;
    std::function<void(void)> a;

    class Timer
    {
    private:
        TimeStamp m_expiredTime;
        TimeoutCallback m_callBack;
        bool m_delete;

    public:
        Timer(const TimeStamp &when, const TimeoutCallback &cb)
                : m_expiredTime(when), m_callBack(cb), m_delete(false) {}

        ~Timer() {}

        void del() {
            m_delete = true;
        }

        bool isDeleted() const {
            return m_delete;
        }

        void runCallBack() {
            m_callBack();
        }

        TimeStamp getExpiredTime() const {
            return m_expiredTime;
        }
    };

    struct cmp {
        bool operator()(Timer *lhs, Timer *rhs) {
            assert(lhs != nullptr && rhs != nullptr);
            return (lhs->getExpiredTime() > rhs->getExpiredTime());
        }
    };

    class TimerManager {
    private:
        using TimerQueue = std::priority_queue<Timer *, std::vector<Timer *>, cmp>;
        TimerQueue m_timerQueue;
        TimeStamp m_nowTime;
        std::mutex m_mtx;

    public:
        TimerManager()
                : m_nowTime(Clock::now()) {}

        ~TimerManager() {}

        void updateTime() {
            m_nowTime = Clock::now();
        }

        void addTimer(HttpRequest *req, int &time_out, const TimeoutCallback &cb);

        void delTimer(HttpRequest *req);

        void handleExpiredTimers();

        int getNearestExpiredTimer();
    };
}
#endif //MODERNCPP_TIMER_H
