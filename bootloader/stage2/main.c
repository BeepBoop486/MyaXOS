/*
 * Mr. Boots Stage 2 C Main
 */

/*
 * 16-bit bootloader
 */
__asm__(".code16gcc\n");

void kprint(short s)
{
	__asm__ __volatile__ ("movw %0, %%si\n"
						  "call _print" : : "l" ((short)s));
}

/*
 * Main entry point
 */
int main() {
	kprint((short)(int)"Welcome to C!\r\n");

	/* And that's it for now... */
	__asm__ __volatile__ ("hlt");
	while (1) {};

	/* Uh oh */
	return -1;
}
