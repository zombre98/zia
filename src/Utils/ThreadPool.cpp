#include <iostream>
#include "ThreadPool.hpp"

zia::ThreadPool::ThreadPool() : running_(true), isTasks_(false), maxThread_(std::thread::hardware_concurrency()) {
  for (size_t i = 0; i < maxThread_; ++i)
    threads_.emplace_back([this]() {
      while (running_) {
        std::unique_lock<std::mutex> lck(taskMutex_);
        taskCond_.wait(lck);

        if (isTasks_) {
          auto &task = tasks_.front();
          tasks_.pop();
          if (tasks_.size() == 0)
            isTasks_ = false;
          else
            taskCond_.notify_one();
          lck.unlock();
          task();
        }
      }
    });
}

zia::ThreadPool::~ThreadPool() {
  running_ = false;
  taskCond_.notify_all();
  for (auto &thread : threads_)
    thread.join();
}
