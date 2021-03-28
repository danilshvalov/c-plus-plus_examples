#include <iostream>
#include <system_error>

enum class http_error {
  continue_request = 100,
  switching_protocols = 101,
  ok = 200,
  gateway_timeout = 504,
  version_not_supported = 505
};

class http_category_impl : public std::error_category {
  const char *name() const noexcept override {
    return "http";
  }
// public:
  std::string message(int ev) const override {
    switch (static_cast<http_error>(ev)) {
      case http_error::continue_request:return "Continue";
      case http_error::switching_protocols:return "Switching protocols";
      case http_error::ok:return "Ok";
      case http_error::gateway_timeout:return "Gateway time-out";
      case http_error::version_not_supported:return "HTTP version not supported";
      default:return "Unknown HTTP error";
    }
  }

  std::error_condition default_error_condition(int ev) const noexcept override {
    switch (static_cast<http_error>(ev)) {
      case http_error::gateway_timeout:return std::errc::network_down;
      default:return std::error_condition{ev, *this};
    }
  }

//  bool equivalent(const std::error_code &code, int condition) const noexcept override {
//    return true;
//  }
  friend std::string Call(int ev);

};

const std::error_category &http_category() {
  static http_category_impl instance;
  return instance;
}

std::error_code make_error_code(http_error e) {
  return std::error_code{static_cast<int>(e), http_category()};
}

std::error_condition make_error_condition(http_error e) {
  return std::error_condition{static_cast<int>(e), http_category()};
}

template<>
struct std::is_error_code_enum<http_error> : public std::true_type {};

std::string Call(int ev) {
  return http_category().message(ev);
}

void Test() {
  throw std::system_error(http_error::gateway_timeout);
}

int main() {
  try {
    Test();
  } catch (const std::system_error &e) {
    std::cout << e.what() << std::endl;
  }

  std::cout << Call(101) << " " << Call(506) << std::endl;

  return 0;
}
