#include <iostream>
#include <tuple>
#include <variant>



// Паттерн Overloaded
// ----------------------------------------------------
template<typename... Ts>
struct Overloaded : Ts ... {
  using Ts::operator()...;
};

template<typename ...Ts>
Overloaded(Ts...)->Overloaded<Ts...>;
// ----------------------------------------------------


template<typename S, typename... Handlers>
struct StateMachine;

// tuple с перечислением типов состояний
template<typename... States, typename... Handlers>
struct StateMachine<std::tuple<States...>, Handlers...> : Handlers ... {
  using Handlers::operator()...;

  template<typename... H>
  StateMachine(H&& ... h) : Handlers(h) ... {}

  template<typename E>
  void OnEvent(E&& e) {
	std::visit([this, &e](auto* state_ptr) {
	  // Если в текущем состоянии можно позвать какое-либо правило, определенное прежде
	  // То вызваем, иначе - ничего не делаем (диагностический вывод)
	  if constexpr (std::is_invocable_v<StateMachine,
										decltype(*state_ptr),
										E&&>) {
		using ResultType = std::invoke_result_t<StateMachine,
												decltype(*state_ptr),
												E&&>;
		// Если правило возвращает void, то перехода нет
		if constexpr (std::is_same_v<ResultType, void>) {
		  (*this)(*state_ptr, std::forward<E>(e));
		  std::cout << "(no transition)" << std::endl;
		} else {
		  auto transition_to = (*this)(*state_ptr, std::forward<E>(e));
		  current_state = &std::get<typename ResultType::TargetState>(states);
		  std::cout << "(transitioned to " << current_state.index() << ")"
					<< std::endl;
		}
	  } else {
		std::cout << "(no rules invoked)" << std::endl;
	  }
	}, current_state);
  }

  std::tuple<States...> states;
  std::variant<States* ...> current_state = &std::get<0>(states);
};

template<typename S>
struct TransitionTo {
  using TargetState = S;
};

template<typename S, typename ...Handlers>
auto MakeStateMachine(Handlers&& ... h) {
  return StateMachine<S, std::decay_t<Handlers>...>{
	  std::forward<Handlers>(h)...};
}

struct StateOpen {};
struct StateClosed {};

struct Open {};
struct Knock {};
struct Close {};

[[noreturn]] auto GetDoor() {
  return MakeStateMachine<std::tuple<StateClosed, StateOpen>>(
	  [](StateClosed& s, Open event) -> TransitionTo<StateOpen> { return {}; },
	  [](StateClosed& s, Knock event) /* нет перехода */ {
		std::cout << "Knock-knock!" << std::endl;
	  },
	  [](StateOpen& s, Close event) -> TransitionTo<StateClosed> { return {}; }
  );
}

int main() {
  auto door = GetDoor();
  door.OnEvent(Knock{});
  door.OnEvent(Open{});
  door.OnEvent(Knock{});
  door.OnEvent(Close{});
  return 0;
}
