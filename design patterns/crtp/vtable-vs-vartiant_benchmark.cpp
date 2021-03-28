#include <iostream>
#include <variant>
#include <benchmark/benchmark.h>
#include "utils.h"


using namespace std;

struct IVirtualRequest {
  virtual int Print() const = 0;
  virtual ~IVirtualRequest() = default;
};

struct VirtualRequest : public IVirtualRequest {
  int Print() const override {return 0;}
};

struct SecondVirtualRequest : public IVirtualRequest {
  int Print() const override {return 0;}
};

template<typename T>
struct ITemplateRequest {
  void Print() const {
    static_cast<T *>(this)->Print();
  }
};

struct TemplateRequest : public ITemplateRequest<TemplateRequest> {
  void Print() const {}
};

struct AnotherTemplateRequest : public ITemplateRequest<AnotherTemplateRequest> {
  void Print() const {}
};

using VirutalRequestHolder = IVirtualRequest *;
using TemplateRequestHolder = variant<TemplateRequest, AnotherTemplateRequest>;

void TestVirtualPointer(benchmark::State &state) {
  VirutalRequestHolder request = new VirtualRequest();
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(request->Print());)
  }
  delete request;
}

void TestVariant(benchmark::State &state) {
  TemplateRequestHolder request = TemplateRequest();
  for (auto _ : state) {
    REPEAT(benchmark::DoNotOptimize(visit([](auto &&req) { req.Print(); return 0; }, request));)
  }
}

BENCHMARK(TestVirtualPointer);
BENCHMARK(TestVariant);

BENCHMARK_MAIN();
