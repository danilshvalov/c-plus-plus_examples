enum class ExecutionPolicy {
  OnSuccess,
  OnFailure,
  OnSuccessOrFailure
};

class UncaughtExceptionDetector {
 public:
  UncaughtExceptionDetector() : count_(std::uncaught_exceptions()) {}
  explicit operator bool() const noexcept {
    return std::uncaught_exceptions() > count_;
  }
 private:
  const size_t count_;
};

template<typename Func>
class ScopeGuard {
 private:
  Func func_;
  ExecutionPolicy policy_;
  UncaughtExceptionDetector detector_;
 public:
  explicit ScopeGuard(const Func &func, ExecutionPolicy policy) : func_(func), policy_(policy) {}
  explicit ScopeGuard(Func &&func, ExecutionPolicy policy) : func_(func), policy_(policy) {}
  ScopeGuard(ScopeGuard &&other) noexcept: func_(other.func_), policy_(other.policy_) {}
  ScopeGuard() = delete;
  ScopeGuard(const ScopeGuard &) = delete;
  ScopeGuard &operator=(const ScopeGuard &) = delete;
  ScopeGuard &operator=(ScopeGuard &&) = delete;
  ~ScopeGuard() {
    if (policy_ == ExecutionPolicy::OnSuccessOrFailure
        || (policy_ == ExecutionPolicy::OnSuccess && !detector_)
        || (policy_ == ExecutionPolicy::OnFailure && detector_)) {
      func_();
    }
  }
};