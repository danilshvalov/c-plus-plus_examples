#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

template <typename S, typename... Handlers>
struct StateMachine;

template <typename... States, typename... Handlers>
struct StateMachine<std::tuple<States...>, Handlers...> : Handlers... {
  using Handlers::operator()...;

  template <typename... H>
  StateMachine(H&&... h) : Handlers(h)... {}

  template <typename E, typename... Args>
  void OnEvent(E&& e, Args&&... args) {
    std::visit(
        [this, &e, &args...](auto* state_ptr) {
          if constexpr (std::is_invocable_v<StateMachine, decltype(*state_ptr),
                                            E&&, Args&&...>) {
            using ResultType =
                std::invoke_result_t<StateMachine, decltype(*state_ptr), E&&,
                                     Args&&...>;
            if constexpr (std::is_same_v<ResultType, void>) {
              (*this)(*state_ptr, std::forward<E>(e),
                      std::forward<Args>(args)...);
            } else {
              (*this)(*state_ptr, std::forward<E>(e),
                      std::forward<Args>(args)...);
              current_state =
                  &std::get<typename ResultType::TargetState>(states);
            }
          }
        },
        current_state);
  }

  std::tuple<States...> states;
  std::variant<States*...> current_state = &std::get<0>(states);
};

template <typename S>
struct TransitionTo {
  using TargetState = S;
};

template <typename S, typename... Handlers>
auto MakeStateMachine(Handlers&&... h) {
  return StateMachine<S, std::decay_t<Handlers>...>{
      std::forward<Handlers>(h)...};
}

template <typename S, typename... Handlers>
auto MakeStateMachinePtr(Handlers&&... h) {
  return std::make_unique<StateMachine<S, std::decay_t<Handlers>...>>(
      std::forward<Handlers>(h)...);
}

struct MouseButtonPressed {};
struct KeyDown {};
struct StateRed {};
struct StateBlue {};

struct EmptyEvent {};

using EventsHolder = std::variant<MouseButtonPressed, KeyDown, EmptyEvent>;

EventsHolder ParseToEvent(sf::Event::EventType e) {
  switch (e) {
    case sf::Event::EventType::MouseButtonPressed:
      return MouseButtonPressed{};
    case sf::Event::EventType::KeyPressed:
      return KeyDown{};
    default:
      return EmptyEvent{};
  }
}

template <typename T>
class ColoredCircle : public sf::CircleShape {
 private:
  std::unique_ptr<T> listener_ = nullptr;

 public:
  template <typename... Args>
  ColoredCircle(std::unique_ptr<T> ptr, Args&&... args)
      : sf::CircleShape(std::forward<Args>(args)...),
        listener_(std::move(ptr)) {}
  template <typename E, typename... Args>
  auto OnEvent(E&& e) {
    return listener_->OnEvent(std::forward<E>(e), *this);
  }
};

template <typename T, typename... Args>
ColoredCircle(std::unique_ptr<T>, Args&&... args) -> ColoredCircle<T>;

template <typename T>
class Circle : public sf::CircleShape {
 private:
  T listener_;

 public:
  template <typename... Args>
  Circle(const T& listener, Args&&... args)
      : sf::CircleShape(std::forward<Args>(args)...), listener_(listener) {}

  template <typename E, typename... Args>
  auto OnEvent(E&& e) {
    return listener_.OnEvent(std::forward<E>(e), *this);
  }
};

auto GetEventListener() {
  using namespace std;
  return MakeStateMachine<std::tuple<StateBlue, StateRed>>(
      [](StateBlue&, MouseButtonPressed,
         sf::CircleShape& circle) -> TransitionTo<StateRed> {
        std::cout << "Blue -> Red" << std::endl;
        circle.setFillColor(sf::Color::Red);
        return {};
      },
      [](StateRed&, MouseButtonPressed,
         sf::CircleShape& circle) -> TransitionTo<StateBlue> {
        std::cout << "Red -> Blue" << std::endl;
        circle.setFillColor(sf::Color::Blue);
        return {};
      },
      [](auto&, MouseButtonPressed) { cout << "One more event" << endl; });
}

auto GetEventListenerPtr() {
  using namespace std;
  return MakeStateMachinePtr<std::tuple<StateBlue, StateRed>>(
      [](StateBlue&, MouseButtonPressed,
         sf::CircleShape& circle) -> TransitionTo<StateRed> {
        std::cout << "Blue -> Red" << std::endl;
        circle.setFillColor(sf::Color::Red);
        return {};
      },
      [](StateRed&, MouseButtonPressed,
         sf::CircleShape& circle) -> TransitionTo<StateBlue> {
        std::cout << "Red -> Blue" << std::endl;
        circle.setFillColor(sf::Color::Blue);
        return {};
      },
      [](auto&, MouseButtonPressed) { cout << "One more event" << endl; });
}

int main() {
  sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
  Circle circle = Circle{GetEventListener(), 100.f};
  circle.setFillColor(sf::Color::Cyan);

  while (window.isOpen()) {
    sf::Event event;
    while (window.pollEvent(event)) {
      EventsHolder parsed_event = ParseToEvent(event.type);
      std::visit([&circle](auto& e) { return circle.OnEvent(e); },
                 parsed_event);
      if (event.type == sf::Event::Closed) window.close();
    }

    window.clear();
    window.draw(circle);
    window.display();
  }

  return 0;
}
