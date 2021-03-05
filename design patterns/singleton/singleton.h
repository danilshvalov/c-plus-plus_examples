#pragma once

class Singleton {
 public:
  Singleton();
  int& Get();
 private:
  struct SingletonImpl;
  static SingletonImpl& Impl();
  SingletonImpl& impl_;
};

