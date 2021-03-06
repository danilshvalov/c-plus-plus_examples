#include <iostream>

using namespace std;

class SomeClass {};

template<typename T, typename Base>
struct TypePrinter {
  template<typename U = Base>
  auto Print() -> decltype(U::Print) {
    static_cast<Base *>(this)->Print();
  }
  void Print() {
    cout << "Default Print" << endl;
  }
};

template<typename T, template<typename, typename> typename ...PolicyArgs>
struct FirstPtr : public PolicyArgs<T, FirstPtr<T, PolicyArgs>> ... {
  void Print() {
    cout << "Changed Print" << endl;
  }
};

template<typename T, template<typename, typename> typename ...PolicyArgs>
struct SecondPtr : public PolicyArgs<T, SecondPtr<T, PolicyArgs>> ... {
};

int main() {
  FirstPtr<SomeClass, TypePrinter> another_ptr;
  SecondPtr<SomeClass, TypePrinter> second_ptr;
  another_ptr.Print();
  second_ptr.Print();

  return 0;
}