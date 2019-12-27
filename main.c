#include <system.h>
#include <multiboot.h>
#include <ext2.h>

/*
 * kernel entry point
 */
int main(struct multiboot *mboot_ptr) {

	/* Realing memory to the end of the multiboot modules */
	if (mboot_ptr->mods_count > 0) {
		uint32_t module_end   = *(uint32_t*)(mboot_ptr->mods_addr+4);
		kmalloc_startat(module_end);
	}
#if 0
	mboot_ptr = copy_multiboot(mboot_ptr);
#endif

	/* Initialize core modules */
	gdt_install();	/* Global descriptor table */
	idt_install();	/* IDT */
	isrs_install();	/* Interrupt service requests */
	irq_install();	/* Hardware interrupt requests */
	init_video();	/* VGA driver */

	/* Hardware drivers */
	timer_install();
	keyboard_install();

	/* Memory management */
	paging_install(mboot_ptr->mem_upper);
	heap_install();

	/* Kernel Version */
	settextcolor(12,0);
	kprintf("[%s %s]\n", KERNEL_UNAME, KERNEL_VERSION_STRING);

	/* Print multiboot information */
	dump_multiboot(mboot_ptr);

	uint32_t module_start = *((uint32_t*)mboot_ptr->mods_addr);
	uint32_t module_end   = *(uint32_t*)(mboot_ptr->mods_addr+4);

	initrd_mount(module_start, module_end);

	start_shell();

	return 0;
}

