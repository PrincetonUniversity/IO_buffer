#ifndef SPIN_LOCK_FLO_HPP
#define SPIN_LOCK_FLO_HPP

#include<atomic>

class spin_mutex{

public:
  void lock(){ while (busy.exchange(true)) {};};

  void unlock(){ busy = false;};

  spin_mutex(): busy(false){};

private:

  std::atomic_bool busy;
};

#endif
