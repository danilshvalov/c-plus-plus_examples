#include <iostream>
#include <array>


using namespace std;



/* ---------------------------------------- */

template<typename ...Types>
class Visitor;

template<typename T>
class Visitor<T> {
 public:
  virtual void Visit(T *) = 0;
  virtual ~Visitor() = default;
};

template<typename T, typename ...Types>
class Visitor<T, Types...> : public Visitor<Types...> {
 public:
  using Visitor<Types...>::Visit;
  virtual void Visit(T *) = 0;
};

/* ---------------------------------------- */

using GeometryVisitor = Visitor<class Rectangle, class Line, class Polyline>;

template<typename Object>
class VisitableObject;

class GeomertyObject {
 public:
  virtual void Accept(GeometryVisitor &visitor) = 0;
  virtual ~GeomertyObject() = default;
};

template<typename Object>
class VisitableObject : public GeomertyObject {
 public:
  using GeomertyObject::GeomertyObject;
  void Accept(GeometryVisitor &visitor) override {
    visitor.Visit(static_cast<Object *>(this));
  }
};

class Rectangle : public VisitableObject<Rectangle> {
};

class Line : public VisitableObject<Line> {
  using VisitableObject<Line>::VisitableObject;
};

class Polyline : public VisitableObject<Polyline> {
  using VisitableObject<Polyline>::VisitableObject;
};

/* ---------------------------------------- */

template<typename Base, typename ...>
class LambdaVisitor;

template<typename Base, typename T1, typename F1>
class LambdaVisitor<Base, Visitor<T1>, F1> : private F1, public Base {
 public:
  explicit LambdaVisitor(F1 &&func) : F1(move(func)) {}
  explicit LambdaVisitor(const F1 &func) : F1(func) {}
  void Visit(T1 *type) override {
    return F1::operator()(type);
  }
};

template<typename Base, typename T1, typename ...T, typename F1, typename ...F>
class LambdaVisitor<Base, Visitor<T1, T...>, F1, F...> : private F1, public LambdaVisitor<Base, Visitor<T...>, F...> {
 public:
  explicit LambdaVisitor(F1 &&f1, F &&... f)
      : F1(move(f1)), LambdaVisitor<Base, Visitor<T...>, F...>(std::forward<F>(f)...) {}
  explicit LambdaVisitor(const F1 &f1, const F &... f)
      : F1(f1), LambdaVisitor<Base, Visitor<T...>, F...>(std::forward<F>(f)...) {}
  void Visit(T1 *type) override {
    return F1::operator()(type);
  }
};

template<typename Base, typename ...F>
auto MakeLambdaVisitor(F &&...f) {
  return LambdaVisitor<Base, Base, F...>(std::forward<F>(f)...);
}

/* ---------------------------------------- */


class TypeVisitor : public GeometryVisitor {
 public:
  void Visit(Rectangle *rect) override {
    cout << "Visit Rectangle" << endl;
  }
  void Visit(Line *line) override {
    cout << "Visit Line" << endl;
  }
  void Visit(Polyline *polyline) override {
    cout << "Visit Polyline" << endl;
  }
};

/* ---------------------------------------- */

class VisitorBase {
 public:
  virtual ~VisitorBase() = default;
};

template<typename Visitable>
class AcyclicVisitor {
 public:
  virtual void Visit(Visitable *) = 0;
};

class Pet {
 public:
  virtual void Accept(VisitorBase &) = 0;
  virtual ~Pet() = default;
};

template<typename Base, typename Visitable>
class VisitableBase : public Base {
 public:
  using Base::Base;
  void Accept(VisitorBase &visitor) override {
    if (auto *pv = dynamic_cast<AcyclicVisitor<Visitable> *>(&visitor); pv) {
      pv->Visit(static_cast<Visitable *>(this));
    } else {
//      assert(false);
    }
  }
};

template<typename Visitable>
using PetVisitable = VisitableBase<Pet, Visitable>;

template<typename ...>
struct Visitors;

template<typename V>
struct Visitors<V> : public AcyclicVisitor<V> {};

template<typename V1, typename ...V>
struct Visitors<V1, V...> : public AcyclicVisitor<V1>, public Visitors<V...> {};

class Cat : public PetVisitable<Cat> {
  using PetVisitable<Cat>::PetVisitable;
};

class Dog : public PetVisitable<Dog> {
  using PetVisitable<Dog>::PetVisitable;
};

class FeedingVisitor : public VisitorBase, public Visitors<Cat, Dog> {
 public:
  void Visit(Cat *) override {
    cout << "FeedingVisitor CAT" << endl;
  }
  void Visit(Dog *) override {
    cout << "FeedingVisitor DOG" << endl;
  }
};

int main() {
  Rectangle rect;
  Line line;

  TypeVisitor visitor;
  line.Accept(visitor);
  rect.Accept(visitor);

  auto lambda_visitor = MakeLambdaVisitor<GeometryVisitor>([](Rectangle *) { cout << "Rectangle visit" << endl; },
                                                           [](Line *) { cout << "Line visit" << endl; },
                                                           [](Polyline *) { cout << "Polyline visit" << endl; });

  line.Accept(lambda_visitor);
  rect.Accept(lambda_visitor);


  /* ---------------------------------------- */

  Cat cat;
  Dog dog;
  FeedingVisitor feeding_visitor;
  cat.Accept(feeding_visitor);
  dog.Accept(feeding_visitor);


  return 0;
}