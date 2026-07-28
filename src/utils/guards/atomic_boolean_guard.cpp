#include "utils/guards/atomic_boolean_guard.hpp"

AtomicBooleanGuard::AtomicBooleanGuard(std::atomic_bool &var) : var{var} {
    var.store(true, std::memory_order_release);
}
AtomicBooleanGuard::~AtomicBooleanGuard() {
    var.store(false, std::memory_order_release);
    var.notify_one();
}
