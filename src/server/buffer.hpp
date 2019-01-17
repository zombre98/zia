#pragma once

#include <vector>

#include <iostream>

namespace nkpp {

class Buffer {
  using bufferContainer = std::vector<uint8_t>;
  static constexpr int32_t BUFFER_SIZE = 512;
public:
  Buffer() {
    buffer_.resize(BUFFER_SIZE);
  }

private:
  void eraseFront(size_t size) {
    buffer_.erase(buffer_.begin(), buffer_.begin() + size);
  }

public:
  template<typename T>
  void setData(T *ptr, size_t size) {
    buffer_.resize(size);
    ::memcpy(buffer_.data(), ptr, size);
  }

public:
  void clear() {
    buffer_.resize(BUFFER_SIZE);
    std::fill(buffer_.begin(), buffer_.end(), 0);
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
    buffer_.insert(buffer_.end(), &data, &data + sizeof(T));
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

  //std::string
  template <typename T, typename = typename std::enable_if_t<std::is_same<std::string, T>::value, T>>
  void write(std::string data) {
    write<size_t>(data.size());
    buffer_.insert(buffer_.end(), data.c_str(), data.c_str() + data.size());
  }

  template <typename T, typename = typename std::enable_if_t<std::is_same<std::string, T>::value, T>>
  std::string read() {
    std::string data;
    auto length = read<size_t>();

    data.resize(length);
    if (buffer_.size() < length)
      throw std::runtime_error("Bad Read Size");
    ::memcpy(&data[0], &buffer_[0], length);
    eraseFront(length);
    return data;
  }

private:
  bufferContainer buffer_;
};

}
