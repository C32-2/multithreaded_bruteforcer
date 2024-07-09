#include "thread_pool.h"

thread_pool::thread_pool(size_t num_threads) : stop(false)
{
    for (size_t i = 0; i < num_threads; ++i)
    {
        workers.emplace_back([this] { this->worker(); });
    }
}

thread_pool::~thread_pool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& worker : workers)
    {
        if (worker.joinable())
        {
            worker.join();
        }
    }
}

void thread_pool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(task);
    }
    condition.notify_one();
}

void thread_pool::worker()
{
    while (true)
    {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(this->queue_mutex);
            this->condition.wait(lock, [this] { return this->stop || !this->tasks.empty(); });
            if (this->stop && this->tasks.empty())
            {
                return;
            }
            task = std::move(this->tasks.front());
            this->tasks.pop();
        }
        task();
    }
}