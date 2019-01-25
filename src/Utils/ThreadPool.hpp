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
	/**
	 * Construct the Threadpool with hardware maximum threads possible
	 */
  ThreadPool() : running_(true), isTasks_(false), maxThread_(std::thread::hardware_concurrency()) {
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
  ThreadPool(const ThreadPool &) = delete;
  ThreadPool &operator=(const ThreadPool &) = delete;
  ~ThreadPool();

public:
  /**
   * Add a Task to the threadpool and return it as a shared_future
   * @tparam T The type of the returned by the task
   * @param task The task to execute
   * @return
   */
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