#pragma once

#include <vector>
#include <iostream>

namespace nkpp {

class Buffer {
  using bufferContainer = std::vector<uint8_t>;
  static constexpr int32_t BUFFER_SIZE = 1024;
public:
  /**
   * Buffer Constructor
   */
  Buffer() : cursor_(0) {
    buffer_.resize(BUFFER_SIZE);
  }

private:
  /**
   * Erase the front of the buffer
   * @param size the size to erase
   */
  void eraseFront(size_t size) {
    buffer_.erase(buffer_.begin(), buffer_.begin() + size);
    cursor_ -= size;
  }

public:
  /**
   * set in raw the data of the buffer
   * @tparam T The type of the pointer to copy for scalar type only
   * @param ptr the pointer where to start the copy
   * @param size the size of the copy
   */
  template<typename T, typename = typename std::enable_if_t<std::is_scalar<T>::value, T>>
  void setData(T *ptr, size_t size) {
    buffer_.resize(size);
    ::memcpy(buffer_.data(), ptr, size);
    cursor_ += size;
  }

public:
  /**
   * Clear the buffer
   */
  void clear() {
    cursor_ = 0;
    buffer_.clear();
    buffer_.resize(BUFFER_SIZE);
  }

  /**
   * Return the Container of the buffer as const reference
   * @return
   */
  const bufferContainer &getBufferContainer() const {
    return buffer_;
  }

  /**
   * Return the Container of the buffer as reference
   * @return
   */
  bufferContainer &getBufferContainer() {
    return buffer_;
  }

public:
	/**
	 * Write data into the buffer
	 * @tparam T Arithmetic type of the data
	 * @param data The data to write into the buffer
	 */
  template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
  void write(T data) {
    buffer_.insert(buffer_.begin() + cursor_, &data, &data + sizeof(T));
    cursor_ += sizeof(T);
  }

  /**
   * Read data from the buffer and return as a arithmetic type
   * @tparam T Arytmetic Type of the data to read
   * @return
   */
  template <typename T, typename = typename std::enable_if_t<std::is_arithmetic<T>::value, T>>
  T read() {
    T data;

    if (buffer_.size() < sizeof(T))
      throw std::runtime_error("Bad Read Size");
    std::copy(buffer_.begin(), buffer_.begin() + sizeof(T), &data);
    eraseFront(sizeof(T));
    return data;
  }

  /**
   * Get the data wrote into the buffer
   * @return
   */
  size_t getWroteSize() const noexcept {
    return cursor_;
  }

  void setCursor(size_t newSize) noexcept {
    cursor_ = newSize;
  }

  /**
   * Write data into the buffer as an Std::String specialisation
   * @tparam T std::string
   * @param data the std::string to write into the buffer
   */
  template <typename T, typename = typename std::enable_if_t<std::is_same<std::string, T>::value, T>>
  void write(std::string data) {
    buffer_.insert(buffer_.begin() + cursor_, data.c_str(), data.c_str() + data.size());
    cursor_ += data.size();
  }

  /**
   * Read data from the buffer and return it as an std::string
   * @tparam T
   * @return
   */
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
