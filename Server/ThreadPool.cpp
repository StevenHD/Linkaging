//
// Created by hlhd on 2021/4/28.
//

#include "ThreadPool.h"

namespace Linkaging
{

ThreadPool::ThreadPool(int numWkrs) : m_isStop(false)
{
    numWkrs = (numWkrs <= 0) ? 1 : numWkrs;
    for (int i = 0; i < numWkrs; ++i)
        m_threads.emplace_back([this]()     // this是什么意思？
        {
            while (1)
            {
                JobFunction func;
                {
                    std::unique_lock<std::mutex> lck(m_mtx);
                    while (!m_isStop && m_jobs.empty())
                    {
                        m_cv.wait(lck);
                    }

                    if (m_jobs.empty() && m_isStop)
                    {
                        printf("[ThreadPool::TheadPool] thread_id = %lu return\n", pthread_self());
                        return;
                    }

                    func = m_jobs.front();
                    m_jobs.pop();
                }

                if (func)
                {
                    printf("[ThreadPool::ThreadPool] thread_id = %lu get a job\n", pthread_self());
                    func();
                    printf("[ThreadPool::ThreadPool] thread_id = %lu job finish\n", pthread_self());
                }
            }
        });
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lck(m_mtx);    // ??
        m_isStop = true;
    }

    m_cv.notify_all();
    for (auto &thread : m_threads)
    {
        thread.join();
    }
}

void ThreadPool::pushJob(const JobFunction &job)
{
    {
        std::unique_lock<std::mutex> lck(m_mtx);
        m_jobs.push(job);
    }

    printf("[ThreadPool::pushJob] push new job\n");
    m_cv.notify_one();
}

}