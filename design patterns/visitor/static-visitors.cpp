#include <iostream>

using namespace std;

class Pet {
 public:
  template<typename Visitable, typename Visitor>
  static void Accept(Visitable &visitable, Visitor &visitor) {
    visitor.Visit(visitable);
  }
};

class Cat : public Pet {
 public:
  using Pet::Pet;
};

class Dog : public Pet {
 public:
  using Pet::Pet;
};

class FeedingVisitor {
 public:
  void Visit(Cat &) {
    cout << "FeedingVisitor CAT" << endl;
  }
  void Visit(Dog &) {
    cout << "FeedingVisitor DOG" << endl;
  }
};

/* ---------------------------------------- */

class Point {
 public:
  explicit Point() : x_(10), y_(10) {}
  template<typename This, typename Visitor>
  static void Accept(This &t, Visitor &visitor) {
    visitor(t.x_);
    visitor(t.y_);
  }
 private:
  double x_;
  double y_;
};

class Line {
 public:
  explicit Line() = default;
  template<typename This, typename Visitor>
  static void Accept(This &t, Visitor &visitor) {
    visitor(t.p1);
    visitor(t.p2);
  }
 private:
  Point p1;
  Point p2;
};

template<typename T1, typename T2>
class Intersection {
 public:
  Intersection(T1 t1, T2 t2) : t1_(t1), t2_(t2) {}
  template<typename This, typename Visitor>
  static void Accept(This &t, Visitor &visitor) {
    visitor(t.t1_);
    visitor(t.t2_);
  }
 private:
  T1 t1_;
  T2 t2_;
};

class Stringify {
 public:
  void operator()(double value) {
    total_value_ += value;
    cout << "Visited: " << value << endl;
  }
  template<typename T>
  void operator()(const T& t) {
    T::Accept(t, *this);
  }
  void operator()(const Line& line) {
    cout << "Visit LINE" << endl;
    Line::Accept(line, *this);
  }
  void operator()(const Point& point) {
    cout << "Visit POINT" << endl;
    Point::Accept(point, *this);
  }
 private:
  double total_value_;
};

template<typename Visitor, typename T>
void Visit(Visitor& visitor, T& type) {
  visitor(type);
}

template<typename Visitor, typename T, typename ...Types>
void Visit(Visitor& visitor, T& type, Types& ...types) {
  visitor(type);
  Visit(visitor, types...);
}



int main() {
  Point p1;
  Point p2;
  Line line;

  Stringify visitor;

  Visit(visitor, p1);
  Visit(visitor, line);

  return 0;
}