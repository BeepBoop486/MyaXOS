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

#if 0
	ext2_superblock_t * superblock = (ext2_superblock_t *)(module_start + 1024);
	kprintf("Magic is 0x%x\n", (int)superblock->magic);
	assert(superblock->magic == EXT2_SUPER_MAGIC);

	kprintf("Partition has %d inodes and %d blocks.\n", superblock->inodes_count, superblock->blocks_count);
	kprintf("%d blocks reserved for root\n", superblock->r_blocks_count);
	kprintf("%d blocks free\n", superblock->free_blocks_count);
	kprintf("%d free inodes\n", superblock->free_inodes_count);
	kprintf("Blocks contain %d bytes\n", 1024 << superblock->log_block_size);
	kprintf("Fragments contain %d bytes\n", 1024 << superblock->log_frag_size);
	kprintf("I am at block id: %d (should be 1 if this is a 1KB block)\n", superblock->first_data_block);
	kprintf("There are %d blocks in a group\n", superblock->blocks_per_group);
	kprintf("There are %d fragments in a group\n", superblock->frags_per_group);
	kprintf("There are %d inodes in a group\n", superblock->inodes_per_group);
	kprintf("Last mount: 0x%x\n", superblock->mtime);
	kprintf("Last write: 0x%x\n", superblock->wtime);
	kprintf("Mounts since verification: %d\n", superblock->mnt_count);
	kprintf("Must be verified in %d mounts\n", superblock->max_mnt_count - superblock->mnt_count);
	kprintf("Inodes are %d bytes\n", (int)superblock->inode_size);

	ext2_bgdescriptor_t * blockgroups = (ext2_bgdescriptor_t *)(module_start + 1024 + 1024);
	kprintf("First block group has %d free blocks, %d free inodes, %d used dirs\n",
			blockgroups->free_blocks_count,
			blockgroups->free_inodes_count,
			blockgroups->used_dirs_count);

	ext2_inodetable_t * inodetable = (ext2_inodetable_t *)(module_start + (1024 << superblock->log_block_size) * blockgroups->inode_table);
	uint32_t i;
	for (i = 0; i < superblock->inodes_per_group; ++i) {
		ext2_inodetable_t * inode = (ext2_inodetable_t *)((int)inodetable + (int)superblock->inode_size * i);
		if (inode->block[0] == 0)
			continue;
		kprintf("Inode %d starts at block %d,%d and is %d bytes (%d blocks). ", i, inode->block[0], inode->block[1], inode->size, inode->blocks);
		if (inode->mode & EXT2_S_IFDIR) {
			kprintf("is a directory\n");
			kprintf("File listing:\n");
			uint32_t dir_offset;
			dir_offset = 0;
			while (dir_offset < inode->size) {
				ext2_dir_t * d_ent = (ext2_dir_t *)(module_start + (1024 << superblock->log_block_size) * inode->block[0] + dir_offset);
				unsigned char * name = malloc(sizeof(unsigned char) * (d_ent->name_len + 1));
				memcpy(name, &d_ent->name, d_ent->name_len);
				name[d_ent->name_len] = '\0';
				kprintf("[%d] %s [%d]\n", dir_offset, name, d_ent->inode);
				if (name[0] == 'h' &&
					name[1] == 'e' &&
					name[2] == 'l' &&
					name[3] == 'l' &&
					name[4] == 'o') {
					kprintf("Found a file to read.\n");
					ext2_inodetable_t * inode_f = (ext2_inodetable_t *)((int)inodetable + (int)superblock->inode_size * (d_ent->inode -1));
					kprintf("Going to print %d bytes from block %d\n", inode_f->size, inode_f->block[0]);
					unsigned char * file_pointer = (unsigned char *)(module_start + (1024 << superblock->log_block_size) * inode_f->block[0]);
					unsigned int file_offset;
					for (file_offset = 0; file_offset < inode_f->size; ++file_offset) {
						kprintf("%c", file_pointer[file_offset]);
					}
				}

				free(name);
				dir_offset += d_ent->rec_len;
				if (d_ent->inode == 0)
					break;
			}
			break;
		}
		kprintf("\n");
	};
#endif

	return 0;
}
