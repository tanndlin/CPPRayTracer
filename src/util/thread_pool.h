#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#define MULTITHEADING_ENABLED true

class ThreadPool {
   public:
    // Takes in num threads and callback
    ThreadPool() {}

    void Start() {
        const uint32_t num_threads = MULTITHEADING_ENABLED ? std::thread::hardware_concurrency() : 1;
        for (uint32_t ii = 0; ii < num_threads; ++ii) {
            threads.emplace_back(std::thread(&ThreadPool::ThreadLoop, this));
        }
    }

    void QueueJob(std::function<void()> job) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            jobs.push(std::move(job));
        }
        mutex_condition.notify_one();
    }

    bool busy() {
        bool poolbusy;
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            poolbusy = !jobs.empty();
        }
        return poolbusy;
    }

    int size() {
        std::unique_lock<std::mutex> lock(queue_mutex);
        return jobs.size();
    }

    void Stop() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            should_terminate = true;
        }
        mutex_condition.notify_all();
        for (std::thread& active_thread : threads) {
            active_thread.join();
        }
        threads.clear();
    }

   private:
    void ThreadLoop() {
        while (true) {
            std::function<void()> job;
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                mutex_condition.wait(lock, [this] {
                    return !jobs.empty() || should_terminate;
                });
                if (should_terminate) {
                    return;
                }
                job = jobs.front();
                jobs.pop();
            }
            job();
        }
    }

    bool should_terminate = false;            // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                   // Prevents data races to the job queue
    std::condition_variable mutex_condition;  // Allows threads to wait on new jobs or termination
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

#endif