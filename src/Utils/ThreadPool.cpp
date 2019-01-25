#include <iostream>
#include "ThreadPool.hpp"


zia::ThreadPool::~ThreadPool() {
  running_ = false;
  taskCond_.notify_all();
  for (auto &thread : threads_)
    thread.join();
}
