#include <system.h>
#include <multiboot.h>
#include <ext2.h>

/*
 * kernel entry point
 */
int main(struct multiboot *mboot_ptr) {

	/* Realing memory to the end of the multiboot modules */
	if (mboot_ptr->mods_count > 0) {
		uint32_t module_start = *((uint32_t*)mboot_ptr->mods_addr);
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
	kprintf("Opening /etc/kernel/hello.txt... ");
	fs_node_t * test_file = kopen("/etc/kernel/hello.txt", NULL);
	if (!test_file) {
		kprintf("Couldn't find hello.txt\n");
	}
	kprintf("Found at inode %d\n", test_file->inode);
	char buffer[256];
	uint32_t bytes_read;
	bytes_read = read_fs(test_file, 0, 255, &buffer);
	kprintf("cat /etc/kernel/hello.txt\n");
	uint32_t i = 0;
	for(i = 0; i < bytes_read; ++i) {
		kprintf("%c", buffer[i]);
	}
	close_fs(test_file);
	free(test_file);
	test_file = kopen("/usr/docs/README.txt", NULL);
	char * bufferb = malloc(test_file->length + 200);
	bytes_read = read_fs(test_file, 100, test_file->length, bufferb);
	for(i = 0; i < bytes_read; ++i) {
		kprintf("%c", (char)bufferb[i]);
		if(i % 500 == 0) {
			timer_wait(30);
		}
	}
	free(test_file);
	close_fs(test_file);

	return 0;
}
