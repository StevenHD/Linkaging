//
// Created by hlhd on 2021/4/28.
//

#ifndef MODERNCPP_THREADPOOL_H
#define MODERNCPP_THREADPOOL_H

#include "../all.h"

namespace Linkaging
{

class ThreadPool
{
public:
    using JobFunction = std::function<void()>;

private:
    std::vector<std::thread> m_threads;
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::queue<JobFunction> m_jobs;
    bool m_isStop;

public:
    ThreadPool(int numWkrs);

    ~ThreadPool();

    void pushJob(const JobFunction &job);
};

}

#endif //MODERNCPP_THREADPOOL_H
