#pragma once

#include <vector>
#include <iostream>

namespace nkpp {

class Buffer {
  using bufferContainer = std::vector<uint8_t>;
  static constexpr int32_t BUFFER_SIZE = 1024;
public:
  Buffer() : cursor_(0) {
    buffer_.resize(BUFFER_SIZE);
  }

private:
  void eraseFront(size_t size) {
    buffer_.erase(buffer_.begin(), buffer_.begin() + size);
    cursor_ -= size;
  }

public:
  template<typename T>
  void setData(T *ptr, size_t size) {
    buffer_.resize(size);
    ::memcpy(buffer_.data(), ptr, size);
    cursor_ += size;
  }

public:
  void clear() {
    cursor_ = 0;
    buffer_.clear();
    buffer_.resize(BUFFER_SIZE);
  }

  const bufferContainer &getBufferContainer() const {
    return buffer_;
  }

  bufferContainer &getBufferContainer() {
    return buffer_;
  }

public:
  template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
  void write(T data) {
    buffer_.insert(buffer_.begin() + cursor_, &data, &data + sizeof(T));
    cursor_ += sizeof(T);
  }

  template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
  T read() {
    T data;

    if (buffer_.size() < sizeof(T))
      throw std::runtime_error("Bad Read Size");
    std::copy(buffer_.begin(), buffer_.begin() + sizeof(T), &data);
    eraseFront(sizeof(T));
    return data;
  }

  size_t getWroteSize() const noexcept {
    return cursor_;
  }

  void serCursor(size_t newSize) noexcept {
    cursor_ = newSize;
  }

  //std::string
  template <typename T, typename = typename std::enable_if_t<std::is_same<std::string, T>::value, T>>
  void write(std::string data) {
    // write<size_t>(data.size());
    buffer_.insert(buffer_.begin() + cursor_, data.c_str(), data.c_str() + data.size());
    cursor_ += data.size();
  }

  template <typename T, typename = typename std::enable_if_t<std::is_same<std::string, T>::value, T>>
  std::string read() {
    std::string data;

    data.resize(cursor_);
    ::memcpy(&data[0], &buffer_[0], cursor_);
    eraseFront(buffer_.size());
    return data;
  }

private:
  bufferContainer buffer_;
  size_t cursor_;
};

}
