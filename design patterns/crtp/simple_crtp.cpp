#include <iostream>

using namespace std;

template<typename Derived>
class Base {
 public:
  void f() {
    static_cast<Derived *>(this)->f_impl();
  }
 private:
  void f_impl() {
    cout << "Base::f_impl()" << endl;
  }
};

class Derived : public Base<Derived> {};

class DerivedWithImpl : public Base<DerivedWithImpl> {
 private:
  friend Base<DerivedWithImpl>;
  void f_impl() {
    cout << "DerivedWithImpl::f_impl()" << endl;
  }
};

int main() {
  DerivedWithImpl derived_with_impl;
  Derived derived;

  derived_with_impl.f();
  derived.f();
  return 0;
}
