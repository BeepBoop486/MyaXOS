#ifndef __SYSTEM_H
#define __SYSTEM_H

/* Types */

#define NULL ((void *)0UL)

typedef unsigned long uintptr_t;
typedef long size_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;

/* Unimportant Kernel Strings */
#define KERNEL_UNAME "MyaXOS"
#define KERNEL_VERSION_STRING "0.0.1"


#define CHAR_BIT 8
#define INT32_MAX 0x7fffffffL
#define UINT32_MAX 0xffffffffL

extern void * sbrk(uintptr_t increment);

/* Kernel Main */
extern void *memcpy(void * restrict dest, const void * restrict src, size_t count);
extern void *memset(void *dest, int val, size_t count);
extern unsigned short *memsetw(unsigned short *dest, unsigned short val, int count);
extern int strlen(const char *str);
extern unsigned char inportb (unsigned short _port);
extern void outportb (unsigned short _port, unsigned char _data);

/* Panic */
#define HALT_AND_CATCH_FIRE(mesg) halt_and_catch_fire(mesg, __FILE__, __LINE__)
#define ASSERT(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))
#define assert(statement) ((statement) ? (void)0 : assert_failed(__FILE__, __LINE__, #statement))
void halt_and_catch_fire(char * error_message, const char * file, int line);
void assert_failed(const char *file, uint32_t line, const char *desc);

/* VGA driver */
extern void cls();
extern void putch(unsigned char c);
extern void puts(char *str);
extern void settextcolor(unsigned char forecolor, unsigned char backcolor);
extern void resettextcolor();
extern void init_video();

/* GDT */
extern void gdt_install();
extern void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);

/* IDT */
extern void idt_install();
extern void idt_set_gate(unsigned char num, unsigned long base, unsigned short sel, unsigned char flags);

/* Registers */
struct regs {
    unsigned int gs, fs, es, ds;
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
};

typedef void (*irq_handler_t)(struct regs *);

/* ISRS */
extern void isrs_install();
extern void isrs_install_handler(int isrs, irq_handler_t);
extern void isrs_uninstall_handler(int isrs);

/* Interrupt Handlers */
extern void irq_install();
extern void irq_install_handler(int irq, irq_handler_t);
extern void irq_uninstall_handler(int irq);

/* Timer */
extern void timer_install();
extern long timer_ticks;
extern void timer_wait(int ticks);

/* Keyboard */
extern void keyboard_install();
extern void keyboard_wait();

/* kprintf */
extern void kprintf(const char *fmt, ...);

/* Memory Management */
extern uintptr_t placement_pointer;
extern void kmalloc_startat(uintptr_t address);
extern uintptr_t kmalloc_real(size_t size, int align, uintptr_t * phys);
extern uintptr_t kmalloc(size_t size);
extern uintptr_t kvmalloc(size_t size);
extern uintptr_t kmalloc_p(size_t size, uintptr_t * phys);
extern uintptr_t kvmalloc_p(size_t size, uintptr_t * phys);

typedef struct page {
	uint32_t present : 1;
	uint32_t rw      : 1;
	uint32_t user    : 1;
	uint32_t accessed: 1;
	uint32_t dirty   : 1;
	uint32_t unused  : 7;
	uint32_t frame   : 20;
} page_t;

typedef struct page_table {
	page_t pages[1024];
} page_table_t;

typedef struct page_directory {
	page_table_t *tables[1024]; /* 1024 pointers to page tables... */
	uintptr_t physical_tables[1024]; /* Physical addresses of the tables */
	uintptr_t physical_address; /* The physical address of physical_tables */
} page_directory_t;

page_directory_t * kernel_directory;
page_directory_t * current_directory;

extern void paging_install(uint32_t memsize);
extern void switch_page_directory(page_directory_t *new);
extern page_t *get_page(uintptr_t address, int make, page_directory_t *dir);
extern void page_fault(struct regs *r);

void heap_install();

/* klmalloc */
void * __attribute__ ((malloc)) malloc(size_t size);
void * __attribute__ ((malloc)) realloc(void * ptr, size_t size);
void * __attribute__ ((malloc)) calloc(size_t nmemb, size_t size);


#endif
