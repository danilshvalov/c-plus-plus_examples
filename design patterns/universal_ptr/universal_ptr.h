#pragma once
#include <cstddef>
#include <memory>
#include <type_traits>

template <typename T>
class UniversalPtr {
 public:
  constexpr UniversalPtr() noexcept = default;
  constexpr UniversalPtr(nullptr_t) noexcept {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U&, T&>>>
  UniversalPtr(U& ref) noexcept
      : ptr_(std::shared_ptr<T>(), std::addressof(ref)) {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  UniversalPtr(U* ptr) noexcept : ptr_(std::shared_ptr<T>(), ptr) {}

  template <typename U, typename Deleter,
            typename = std::enable_if_t<std::is_convertible_v<
                typename std::unique_ptr<U, Deleter>::pointer, T*>>>
  UniversalPtr(std::unique_ptr<U, Deleter>&& ptr) : ptr_(std::move(ptr)) {}

  template <typename U,
            typename = std::enable_if_t<std::is_convertible_v<U*, T*>>>
  UniversalPtr(const std::shared_ptr<U>& ptr) noexcept : ptr_(ptr) {}

  explicit operator bool() const noexcept { return ptr_.get() != nullptr; }

  T& operator*() const noexcept { return *ptr_; }

  T* operator->() const noexcept { return ptr_.get(); }

  T* get() const noexcept { return ptr_.get(); }

 private:
  std::shared_ptr<T> ptr_;
};

template <typename T, typename U>
bool operator==(const UniversalPtr<T>& lhs, const UniversalPtr<U>& rhs) {
  return lhs.get() == rhs.get();
}

template <typename T>
bool operator==(const UniversalPtr<T> type, nullptr_t) {
  return !type.get();
}

template <typename T>
bool operator==(nullptr_t, const UniversalPtr<T> type) {
  return !type.get();
}

template <typename T, typename U>
bool operator!=(const UniversalPtr<T>& lhs,
                const UniversalPtr<U>& rhs) noexcept {
  return !(lhs == rhs);
}

template <typename T>
bool operator!=(const UniversalPtr<T>& type, nullptr_t) noexcept {
  return !(type == nullptr);
}

template <typename T>
bool operator!=(nullptr_t, const UniversalPtr<T>& type) noexcept {
  return !(type == nullptr);
}