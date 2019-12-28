MyaXOS
==========

MyaXOS is a microkernel made from scratch using C and some pieces are made in Assembly and makefile,
some bits of code were taken from Linux, toaruos and ReactOS, my main ain't C, so, if you see any error
or something that's deprecated that in a future could be a bug, feel free to tell me in the issues of this
repo or if you can directly fix it, create a pull request.

Testing
==========

Clone the repository and run

	make			# to build the kernel
	make initrd		# to create an initial RAM-disk image
	sudo make install	# to build the bootdisk
	sudo make run		# to run the operating system (qemu-system-x86_64 is required)
