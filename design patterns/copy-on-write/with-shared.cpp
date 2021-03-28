#include <iostream>
#include <memory>
#include <type_traits>

template <typename T>
class CopyOnWriteWrapper {
 public:
  struct WriteProxy {
    T* operator->() { return ptr_; }

   private:
    friend class CopyOnWriteWrapper;
    WriteProxy(const WriteProxy&) = default;
    WriteProxy& operator=(const WriteProxy&) = delete;
    WriteProxy(T* ptr) : ptr_(ptr) {}

   private:
    T* ptr_;
  };

  const T& operator*() const { return *shared_; }
  const T* operator->() const { return shared_.get(); }

  WriteProxy operator--(int) {
    EnsureUnique();
    return {shared_.get()};
  }

  template <typename... Args,
            typename = std::enable_if_t<!std::is_abstract_v<T>>>
  CopyOnWriteWrapper(Args&&... args)
      : shared_(std::make_shared<T>(std::forward<Args>(args)...)) {}

  template <typename U, typename Deleter>
  CopyOnWriteWrapper(std::unique_ptr<U, Deleter> ptr)
      : shared_(std::move(ptr)) {}

  template <typename U>
  friend class CopyOnWriteWrapper;

  template <typename U>
  CopyOnWriteWrapper(CopyOnWriteWrapper<U>& rhs) : shared_(rhs.shared_) {}
  template <typename U>
  CopyOnWriteWrapper(const CopyOnWriteWrapper<U>& rhs) : shared_(rhs.shared_) {}

  CopyOnWriteWrapper(CopyOnWriteWrapper&&) = default;
  CopyOnWriteWrapper(const CopyOnWriteWrapper&) = default;

 private:
  template <typename U>
  struct CopyConstr {
    static auto Copy(const U& other) { return std::make_shared<U>(other); }
  };

  template <typename U>
  struct CloneConstr {
    static auto Copy(const U& other) { return other.Clone(); }
  };

  using CopyClass =
      typename std::conditional_t<!std::is_abstract_v<T> &&
                                      std::is_copy_constructible_v<T>,
                                  CopyConstr<T>, CloneConstr<T>>;

  void EnsureUnique() {
    if (shared_.use_count() > 1) {
      shared_ = CopyClass::Copy(*shared_);
    }
  }

 private:
  std::shared_ptr<T> shared_ = nullptr;
};

struct Shape {
  virtual ~Shape() = default;
  virtual std::shared_ptr<Shape> Clone() const = 0;
};

struct Circle : Shape {
  Circle(double radius = 5) : radius_(radius) {}
  Circle(const Circle& other) {
    radius_ = other.radius_;
    std::cout << "Circle::Circle(const Circle&)" << std::endl;
  }
  std::shared_ptr<Shape> Clone() const override {
    return std::make_shared<Circle>(*this);
  }

  double radius_ = 5;
};

struct TrivialType {
  TrivialType() = default;
  TrivialType(const TrivialType& other) {
    value = other.value;
    std::cout << "TrivialType::TrivialType(const TrivialType&)" << std::endl;
    ;
  }
  int value = 5;
};

int main() {
  CopyOnWriteWrapper<TrivialType> trivial;
  CopyOnWriteWrapper<Circle> circle;

  CopyOnWriteWrapper<TrivialType> other_trivial{trivial};
  CopyOnWriteWrapper<Circle> other_circle{circle};
  std::cout << "Construct types" << std::endl;

  std::cout << "Other Trivial value: " << other_trivial->value << std::endl;
  std::cout << "Other Circle value: " << other_circle->radius_ << std::endl;

  std::cout << "Change values" << std::endl;
  other_trivial--->value = 1;
  other_circle--->radius_ = 1;

  std::cout << "Other Trivial value: " << other_trivial->value << std::endl;
  std::cout << "Other Circle value: " << other_circle->radius_ << std::endl;
  return 0;
}