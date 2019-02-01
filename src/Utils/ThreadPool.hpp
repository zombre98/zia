#pragma once

#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <memory>
#include <future>
#include <functional>

namespace zia {

class ThreadPool {
public:
  ThreadPool();
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ~ThreadPool();

public:
  template<typename T>
  std::shared_future<T> addTask(const std::function<T()> &&task) {
    std::unique_lock<std::mutex> lck(taskMutex_);
    auto *taskPackaged = new std::packaged_task<T()>(std::move(task));

    tasks_.push([taskPackaged]() {
      (*taskPackaged)();
      delete taskPackaged;
    });
    isTasks_ = true;
    taskCond_.notify_one();
    return taskPackaged->get_future();
  }

private:
  std::atomic_bool running_;
  std::mutex taskMutex_;
  std::condition_variable taskCond_;
  std::queue<std::function<void()>> tasks_;
  std::atomic_bool isTasks_;
  size_t maxThread_;
  std::vector<std::thread> threads_;
};

}