#include "cstdio"
#include <extra/flanterm/flanterm.h>
#include <extra/flanterm/flanterm_backends/fb.h>
#include <protocols/graphics.hpp>

flanterm_context* ft_ctx;

void __cstdio_initialise_terminal() {
	ft_ctx = flanterm_fb_init(
		nullptr,
		nullptr,
		gGfx.get_framebuffer_ptr(), gGfx.get_width(), gGfx.get_height(), gGfx.get_pitch(),
		gGfx.get_red_mask_size(), gGfx.get_red_mask_shift(),
		gGfx.get_green_mask_size(), gGfx.get_green_mask_shift(),
		gGfx.get_blue_mask_size(), gGfx.get_blue_mask_shift(),
		nullptr,
		nullptr, nullptr,
		nullptr, nullptr,
		nullptr, nullptr,
		nullptr, 0, 0, 1,
		0, 0,
		0,
		0
	);
}

void putchar(char c) {
	flanterm_write(ft_ctx, &c, 1);
}
