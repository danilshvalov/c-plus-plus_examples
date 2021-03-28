#pragma once

#include <utility>
#include <iostream>

template<typename T, typename Parameter = void>
class NamedType {
 private:
  T value_;
 public:
  explicit NamedType(const T& value) : value_(value) {}
  explicit NamedType(T&& value) : value_(std::move(value)) {}
  explicit NamedType() = default;
  operator const T&() const {
	return value_;
  }
  operator T&() {
	return value_;
  }
  template<typename U>
  friend auto operator<=>(const U& lhs, const NamedType& rhs) {
	return lhs <=> rhs.value_;
  };
  friend std::ostream& operator<<(std::ostream& out, const NamedType& type) {
	return out << type.value_;
  }
  friend std::istream& operator>>(std::istream& in, NamedType& type) {
	return in >> type.value_;
  }
};