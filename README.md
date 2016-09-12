ThreadPool
==========

A simple C++11 Thread Pool implementation.

Basic usage:
```c++
// create thread pool with 4 worker threads
ThreadPool pool(4);

// enqueue and store future
auto result = pool.enqueue([](int answer) { return answer; }, 42);

// get result from future
std::cout << result.get() << std::endl;

```

Yche's Understanding
--------------------
---
Original Author's contributions: Make use of 
- move syntax and perfect forward(e.g, emplace_back usage), supported in C++11
- lambda expression and function container, supported in C++11
- thread and other utilities(condition_variable, mutex, unique_lock) wrapper, suppported in C++11
- future design pattern, supported in C++11