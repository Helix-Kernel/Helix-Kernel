#include "mutex.hpp"
#include <cstdlib>

struct internal_mutex {
	bool locked;
};

mutex_t create_mutex() {
	mutex_t mtx = (mutex_t)malloc(sizeof(internal_mutex));
	internal_mutex* mtx_intrl = (internal_mutex*)mtx;
	mtx_intrl->locked = false;
	return mtx;
}

void destroy_mutex(mutex_t mutexp) {
	mutex_lock(mutexp);
	free(mutexp);
}

void mutex_lock(mutex_t mutexp) {
	internal_mutex* mtx_intrl = (internal_mutex*)mutexp;
	while (mtx_intrl->locked);
	mtx_intrl->locked = true;
}

void mutex_unlock(mutex_t mutexp) {
	internal_mutex* mtx_intrl = (internal_mutex*)mutexp;
	mtx_intrl->locked = false;
}
