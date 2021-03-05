#pragma once

#include <vector>
#include <array>
#include <set>
#include <map>
#include <array>
#include <unordered_set>
#include <unordered_map>
#include <iostream>

template<typename Container>
std::ostream &PrintArray(std::ostream &out, const Container &container) {
  out << "{";
  bool is_first = true;
  for (const auto &item : container) {
    if (!is_first) {
      out << ", ";
    }
    is_first = false;
    out << item;
  }
  return out << "}";
}

template<typename Container>
std::ostream &PrintDict(std::ostream &out, const Container &container) {
  out << "{";
  bool is_first = true;
  for (const auto &[key, value] : container) {
    if (!is_first) {
      out << ", ";
    }
    is_first = false;
    out << "{" << key << ": " << value << "}";
  }
  return out << "}";
}

template<typename Container>
std::ostream &PrintSet(std::ostream &out, const Container &container) {
  out << "(";
  bool is_first = true;
  for (const auto &item : container) {
    if (!is_first) {
      out << ", ";
    }
    is_first = false;
    out << item;
  }
  return out << ")";
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &container) {
  return PrintArray(out, container);
}

template<typename T, size_t N>
std::ostream &operator<<(std::ostream &out, const std::array<T, N> &container) {
  return PrintArray(out, container);
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::unordered_set<T> &container) {
  return PrintSet(out, container);
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const std::set<T> &container) {
  return PrintSet(out, container);
}

template<typename K, typename V>
std::ostream &operator<<(std::ostream &out, const std::map<K, V> &container) {
  return PrintDict(out, container);
}

template<typename K, typename V>
std::ostream &operator<<(std::ostream &out, const std::unordered_map<K, V> &container) {
  return PrintDict(out, container);
}
