#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <algorithm>
#include <numeric>
#include <any>
#include <variant>

using namespace std;

template<typename T>
struct Request {
  template<typename U = T>
  auto Print() -> decltype(U::Print) {
    return static_cast<U *>(this)->Print();
  }
  void Print() const {
    cout << "Simple Request" << endl;
  }
  template<typename U = T>
  static auto ParseFrom() -> decltype(U::ParseFrom) {
    return U::ParseFrom();
  }
  static T ParseFrom() {
    cout << "Simple Parse" << endl;
    return T{};
  }
};

struct InterestRequest : public Request<InterestRequest> {
  int value = 10;
  void Print() const {
    cout << value << endl;
  }
  static InterestRequest ParseFrom() {
    cout << "Interest Parse" << endl;
    return InterestRequest{};
  }
};

struct SomeRequest : public Request<SomeRequest> {
  string value = "Some...";
  void Print() const {
    cout << value << endl;
  }
  static SomeRequest ParseFrom() {
    cout << "Some Parse" << endl;
    return SomeRequest{};
  }
};

struct WithOutPrintRequest : public Request<WithOutPrintRequest> {

};

template<typename ...Ts>
using RequestHolder = variant<Ts...>;

template<typename ...Ts>
void ProcessRequests(const vector<RequestHolder<Ts...>> &requests) {
  for (const auto &request : requests) {
    visit([&](auto &&req) { req.Print(); }, request);
  }
}

template<typename ...Ts>
vector<RequestHolder<Ts...>> CreateRequestHolder(Ts ...args) {
  return vector<RequestHolder<Ts...>>{move(args)...};
}

int main() {
  InterestRequest interest_request = InterestRequest::ParseFrom();
  SomeRequest some_request = SomeRequest::ParseFrom();
  WithOutPrintRequest with_out_print_request = WithOutPrintRequest::ParseFrom();

  auto requests = CreateRequestHolder(interest_request, some_request, with_out_print_request);

  ProcessRequests(requests);
  return 0;
}