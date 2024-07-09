#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "includes.h"

class thread_pool
{
public:
    thread_pool(size_t num_threads);
    ~thread_pool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

    void worker();
};

#endif