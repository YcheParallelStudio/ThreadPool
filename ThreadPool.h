#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

using namespace std;

class ThreadPool {
public:
    ThreadPool(size_t);

    template<class F, class... Args>
    auto enqueue(F &&f, Args &&... args) -> future<typename result_of<F(Args...)>::type>;

    ~ThreadPool();

private:
    vector<thread> workers_;
    queue<function<void()> > tasks_;

    mutex queue_mutex_;
    condition_variable condition_;
    bool stop_flag_;
};

// the constructor just launches some amount of workers_
ThreadPool::ThreadPool(size_t threads) : stop_flag_(false) {
    for (size_t i = 0; i < threads; ++i)
        workers_.emplace_back(
                [this]() {
                    for (;;) {
                        function<void()> task;
                        {
                            unique_lock<mutex> lock(queue_mutex_);
                            condition_.wait(lock, [this] { return stop_flag_ || !tasks_.empty(); });
                            if (stop_flag_ && tasks_.empty())
                                return;
                            task = move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                }
        );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&... args) -> future<typename result_of<F(Args...)>::type> {
    using return_type = typename result_of<F(Args...)>::type;

    auto task = make_shared<packaged_task<return_type()>>(bind(forward<F>(f), forward<Args>(args)...));

    future<return_type> res = task->get_future();
    {
        unique_lock<mutex> lock(queue_mutex_);
        // don't allow enqueueing after stopping the pool
        if (stop_flag_)
            throw runtime_error("enqueue on stopped ThreadPool");

        tasks_.emplace([task]() { (*task)(); });
    }
    condition_.notify_one();
    return res;
}

ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queue_mutex_);
        stop_flag_ = true;
    }
    condition_.notify_all();
    for (thread &worker: workers_)
        worker.join();
}

#endif
