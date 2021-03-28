#pragma once

#include <future>
#include <optional>
#include <utility>

template <typename F>
class LazyValueHolder {
 private:
  F compute_function_;
  mutable std::optional<decltype(compute_function_())> cache_;
  mutable std::once_flag is_init_flag_;

 public:
  explicit LazyValueHolder(F&& compute_function)
      : compute_function_(std::move(compute_function)) {}
  explicit operator const decltype(compute_function_()) & () const {
    std::call_once(is_init_flag_, [this] { cache_ = compute_function_(); });
    return *cache_;
  }
  const decltype(compute_function_())& operator*() const {
    return static_cast<const decltype(compute_function_())&>(*this);
  }
};