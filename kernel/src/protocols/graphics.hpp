#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP 1

#include <limine.h>
#include <cstdint>

class KernelGraphics {
private:
	limine_framebuffer* main_framebuffer;

public:
	KernelGraphics();
	~KernelGraphics();

	uint64_t get_width();
	uint64_t get_height();
	uint64_t get_pitch();
	uint64_t get_bpp();
	uint64_t get_stride();

	uint64_t get_red_mask_size();
	uint64_t get_green_mask_size();
	uint64_t get_blue_mask_size();
	
	uint64_t get_red_mask_shift();
	uint64_t get_green_mask_shift();
	uint64_t get_blue_mask_shift();
	
	uint32_t* get_framebuffer_ptr();
};

extern KernelGraphics gGfx;

#endif
