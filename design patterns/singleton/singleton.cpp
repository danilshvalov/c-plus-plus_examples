#include "singleton.h"

struct Singleton::SingletonImpl {
  SingletonImpl() : value_(0) {}
  int value_;
};

Singleton::Singleton() : impl_(Impl()) {}

int &Singleton::Get() {
  return impl_.value_;
}

// will be constructed once
Singleton::SingletonImpl &Singleton::Impl() {
  static SingletonImpl inst;
  return inst;
}
