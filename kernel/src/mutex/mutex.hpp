#ifndef MUTEX_HPP
#define MUTEX_HPP 1

struct __mutex;
typedef __mutex* mutex_t;

mutex_t create_mutex();
void destroy_mutex(mutex_t mutexp);
void mutex_lock(mutex_t mutexp);
void mutex_unlock(mutex_t mutexp);

#endif
