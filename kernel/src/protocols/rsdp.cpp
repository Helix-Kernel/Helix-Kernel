#include "rsdp.hpp"
#include <limine.h>

__attribute__((section(".limine_requests")))
static volatile limine_rsdp_request rsdp_request = {
	.id = LIMINE_RSDP_REQUEST_ID,
	.revision = 0
};

void* rsdp::get_rsdp() {
	return rsdp_request.response->address;
}
