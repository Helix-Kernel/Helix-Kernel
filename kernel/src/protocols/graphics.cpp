#include "graphics.hpp"

KernelGraphics gGfx;

__attribute__((section(".limine_requests")))
static volatile limine_framebuffer_request framebuffer_request = {
	.id = LIMINE_FRAMEBUFFER_REQUEST_ID,
	.revision = 0
};

KernelGraphics::KernelGraphics() {
	if (!framebuffer_request.response ||
		framebuffer_request.response->framebuffer_count < 1) {
		asm ("cli;hlt");
	}

	this->main_framebuffer = framebuffer_request.response->framebuffers[0];
}

KernelGraphics::~KernelGraphics() {
	// no-op
}

uint64_t KernelGraphics::get_width() {
	return this->main_framebuffer->width;
}

uint64_t KernelGraphics::get_height() {
	return this->main_framebuffer->height;
}

uint64_t KernelGraphics::get_pitch() {
	return this->main_framebuffer->pitch;
}

uint64_t KernelGraphics::get_bpp() {
	return this->main_framebuffer->bpp;
}

uint64_t KernelGraphics::get_stride() {
	return ((this->get_width() * this->get_bpp()) + 32) / 32;
}

uint64_t KernelGraphics::get_red_mask_size() {
	return this->main_framebuffer->red_mask_size;
}

uint64_t KernelGraphics::get_green_mask_size() {
	return this->main_framebuffer->green_mask_size;
}

uint64_t KernelGraphics::get_blue_mask_size() {
	return this->main_framebuffer->blue_mask_size;
}
	
uint64_t KernelGraphics::get_red_mask_shift() {
	return this->main_framebuffer->red_mask_shift;
}

uint64_t KernelGraphics::get_green_mask_shift() {
	return this->main_framebuffer->green_mask_shift;
}

uint64_t KernelGraphics::get_blue_mask_shift() {
	return this->main_framebuffer->blue_mask_shift;
}

uint32_t* KernelGraphics::get_framebuffer_ptr() {
	return reinterpret_cast<uint32_t*>(this->main_framebuffer->address);
}
