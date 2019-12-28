/**
 * Serial Port Driver
 */
#include <system.h>

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
#define SERIAL_PORT_C 0x3E8
#define SERIAL_PORT_D 0x2E8

void serial_handler(struct regs *r) {
	char serial = serial_recv();
	/*
	 * Fix the serial input assuming it is ascii
	 */
	switch (serial) {
		case 127:
			serial = 0x08;
			break;
		case 13:
			serial = '\n';
			break;
		default:
			break;
	}
	if (keyboard_buffer_handler) {
		keyboard_buffer_handler(serial);
	}
}

void serial_install() {
	/* We'll initialize the first serial port */
	outportb(SERIAL_PORT_A + 1, 0x00);
	outportb(SERIAL_PORT_A + 3, 0x80); /* Enable divisor mode */
	outportb(SERIAL_PORT_A + 0, 0x03); /* Div Low:  03 Set the port to 38400 bps */
	outportb(SERIAL_PORT_A + 1, 0x00); /* Div High: 00 */
	outportb(SERIAL_PORT_A + 3, 0x03);
	outportb(SERIAL_PORT_A + 2, 0xC7);
	outportb(SERIAL_PORT_A + 4, 0x0B);
	irq_install_handler(4, serial_handler); /* Install the serial input handler */
	outportb(SERIAL_PORT_A + 1, 0x01);      /* Enable interrupts on receive */
}

int serial_rcvd() {
	return inportb(SERIAL_PORT_A + 5) & 1;
}

char serial_recv() {
	while (serial_rcvd() == 0);
	return inportb(SERIAL_PORT_A);
}

int serial_transmit_empty() {
	return inportb(SERIAL_PORT_A + 5) & 0x20;
}

void serial_send(char out) {
	while (serial_transmit_empty() == 0);
	outportb(SERIAL_PORT_A, out);
}
