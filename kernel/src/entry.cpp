#include <limine.h>
#include <cstddef>

extern "C" {
    int __cxa_atexit(void (*)(void *), void *, void *) { return 0; }
    void __cxa_pure_virtual() { while (1) { asm("hlt"); } }
    void *__dso_handle;
}

extern void (*__init_array[])();
extern void (*__init_array_end[])();

// Get the command line, basically we give the command line into the kernel entry (int main(int argc, char** argv))
__attribute__((section(".limine_requests")))
static volatile limine_executable_cmdline_request __entry_executable_cmdline_request = {
	.id = LIMINE_EXECUTABLE_CMDLINE_REQUEST_ID,
	.revision = 0,
};

extern int main(int argc, char** argv);

__attribute__((no_return))
extern "C" void kernel_entry() {
	if (!__entry_executable_cmdline_request.response ||
		!__entry_executable_cmdline_request.response->cmdline) {
		asm ("cli;hlt");
	}

    for (size_t i = 0; &__init_array[i] != __init_array_end; i++) {
        __init_array[i]();
    }

	// Since the command line is perfectly fine after the check we did above, then we can parse it and give it to main
	// We don't really need to copy the string and parse it, we can just replace spaces with zeroes

	// Get the length first so we can still iterate after replacing spaces with \0
	size_t cmdline_len = 0;
	while (__entry_executable_cmdline_request.response->cmdline[cmdline_len] != '\0') cmdline_len++;

	int argc = 1; // Start at 1 to account for the first argument before any space
	for (size_t i = 0; i < cmdline_len; i++) {
		char* charptr = &__entry_executable_cmdline_request.response->cmdline[i];
		if (*charptr == ' ') {
			*charptr = '\0';
			argc++;
		}
	}

	// Variable size array of argc*8 (argc of char*)
	char* arg_ptrs[argc];
	char* last_start = &__entry_executable_cmdline_request.response->cmdline[0];
	int j = 0;
	arg_ptrs[j++] = last_start; // First argument starts at the beginning

	for (size_t i = 0; i < cmdline_len; i++) {
		char* charptr = &__entry_executable_cmdline_request.response->cmdline[i];
		if (*charptr == '\0') {
			last_start = (char*)((uint64_t)charptr + 1);
			arg_ptrs[j++] = last_start;
		}
	}

	int ret = main(argc, arg_ptrs);

	if (ret == 0) {
		goto hcf;
	} else {
		asm ("cli;hlt");
	}

hcf: // Pray that RFL.IF is set
	while (1) {
		asm ("hlt");
	}

	__builtin_unreachable();
}
