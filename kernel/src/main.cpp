#include <cstdio>

int main(int argc, char** argv) {
	(void)argc;(void)argv;

	__cstdio_initialise_terminal();

	for (int i = 0; i < argc; i++) {
		printf("ARGC#%d: %s\r\n", i, argv[i]);
	}

	return 0;
}
