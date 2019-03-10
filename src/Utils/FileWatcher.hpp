#include <utility>

#pragma once

#include <string>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <thread>

namespace zia::utils {

class FileWatcher {
  using chronoDelay = std::chrono::duration<int, std::milli>;
public:
  enum class State {
    CREATED, MODIFIED, DELETED
  };

public:
	/**
	 * Construct the filewatcher to lookup a directory
	 * @param path THe path to the directory to look
	 * @param delay The delay the Filewatcher needs to update the status of each files present in a directory
	 */
  FileWatcher(std::string path, chronoDelay delay) : path_(std::move(path)), delay_(delay), running_(true) {
    for (auto &file : std::filesystem::recursive_directory_iterator(path_))
      filesPaths_[file.path().string()] = std::filesystem::last_write_time(file);
  }

public:
	/**
	 * Start watching a directory
	 * @param callback The call when a files has been updated, with the path of the file as first arg and the state as second
	 */
  void watch(const std::function<void(const std::string &, FileWatcher::State)> &&callback) {
    while (running_) {
      std::this_thread::sleep_for(delay_);

      for (auto &file : filesPaths_) {
        if (!std::filesystem::exists(file.first)) {
          callback(file.first, FileWatcher::State::DELETED);
          auto it = filesPaths_.find(file.first);
          if (it != filesPaths_.end())
            filesPaths_.erase(it);
        }
      }
      for (auto &file : std::filesystem::recursive_directory_iterator(path_)) {
        auto lastWrite = std::filesystem::last_write_time(file);

        if (filesPaths_.find(file.path().string()) == filesPaths_.end()) {
          filesPaths_[file.path().string()] = lastWrite;
          callback(file.path().string(), FileWatcher::State::CREATED);
        } else if (filesPaths_[file.path().string()] != lastWrite) {
          filesPaths_[file.path().string()] = lastWrite;
          callback(file.path().string(), FileWatcher::State::MODIFIED);
        }
      }
    }
  }

  /**
   * Stop the watch
   */
  void stop() {
    running_ = false;
  }

private:
  std::string path_;
  chronoDelay delay_;
  bool running_;

  std::unordered_map<std::string, std::filesystem::file_time_type> filesPaths_;
};

// clang++ -std=c++17 -stdlib=libc++ -Wall -pedantic test_fs_watcher.cpp -o test_fs_watcher -lc++fs

}
