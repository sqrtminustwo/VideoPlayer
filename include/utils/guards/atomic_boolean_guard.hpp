#ifndef ATOMIC_BOOLEAN_GUARD_H
#define ATOMIC_BOOLEAN_GUARD_H

#include <atomic>

class AtomicBooleanGuard {
    std::atomic_bool &var;

  public:
    AtomicBooleanGuard(std::atomic_bool &);
    ~AtomicBooleanGuard();
};

#endif
