#include <system.h>
#include <boot.h>
#include <ext2.h>

/*
 * kernel entry point
 */
 int main(struct multiboot *mboot_ptr, uint32_t mboot_mag)
 {
   enum BOOTMODE boot_mode = unknown; /* Boot mode */
 	char * ramdisk = NULL;
 	if (mboot_mag == MULTIBOOT_EAX_MAGIC) {
 		boot_mode = multiboot;
 		/* Realing memory to the end of the multiboot modules */
 		kmalloc_startat(0x200000);
 		if (mboot_ptr->flags & (1 << 3)) {
 			if (mboot_ptr->mods_count > 0) {
 				uint32_t module_start = *((uint32_t *) mboot_ptr->mods_addr);
 				uint32_t module_end = *(uint32_t *) (mboot_ptr->mods_addr + 4);
 				ramdisk = (char *)kmalloc(module_end - module_start);
 				memcpy(ramdisk, (char *)module_start, module_end - module_start);
 			}
 		} else {
			/**
			 * This ain't a multiboot attempt. We were probably loaded by
			 * Mr. Boots, out dedicated boot loader. Verify this...
			 */
			boot_mode = mrboots;
		}
 	}

 	/* Initialize core modules */
 	gdt_install();		/* Global descriptor table */
 	idt_install();		/* IDT */
 	isrs_install();		/* Interrupt service requests */
 	irq_install();		/* Hardware interrupt requests */
 	init_video();		/* VGA driver */

 	/* Hardware drivers */
 	timer_install();
 	keyboard_install();
	serial_install();

 	/* Memory management */
 	paging_install(mboot_ptr->mem_upper);
 	heap_install();

 	/* Kernel Version */
 	settextcolor(12, 0);
 	kprintf("[%s %s]\n", KERNEL_UNAME, KERNEL_VERSION_STRING);

 	if (boot_mode == multiboot) {
 		/* Print multiboot information */
 		dump_multiboot(mboot_ptr);

 		if (mboot_ptr->flags & (1 << 3)) {
 			if (mboot_ptr->mods_count > 0) {
 				initrd_mount((uintptr_t)ramdisk, 0);
 			}
 		}
 	}

 	start_shell();

 	return 0;
 }
