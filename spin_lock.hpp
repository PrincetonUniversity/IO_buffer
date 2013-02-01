#ifndef SPIN_LOCK_FLO_HPP
#define SPIN_LOCK_FLO_HPP

#include<atomic>
#include<mutex>

class t_spin_lock{

public:
  void lock(){ while (busy.exchange(true)) {};};

  void unlock(){ busy = false;};

  t_spin_lock(): busy(false){};

private:

  std::atomic_bool busy;
};

//typedef t_spin_lock spin_mutex;
typedef std::mutex spin_mutex;

#endif
