#include <system.h>
#include <fs.h>

void start_shell() {
	char path[1024];
	fs_node_t * node = fs_root;
	path[0] = '/';
	path[1] = '\0';
	while(1) {
		char buffer[1024];
		int size;
		kprintf("MyaXOS %s> ", path);
		size = kgets((char *)&buffer, 1023);
		if(size < 1) {
			continue;
		} else {
			/**
			 * Tokenize the command
			 */
			char * pch;
			char * cmd;
			char * save;
			pch = strtok_r(buffer, " ", &save);
			cmd = pch;
			char * argv[1024];
			int tokenid = 0;
			while (pch != NULL) {
				argv[tokenid] = (char *)pch;
				++tokenid;
				pch = strtok_r(NULL, " ", &save);
			}
			argv[tokenid] = NULL;
			
			/**
			 * Execute command
			 */
			if(!strcmp(cmd, "cd")) {
				if(tokenid < 2) {
					kprintf("cd: argument expected\n");
					continue;
				} else {
					if(argv[1][0] == '/') {
						fs_node_t * chd = kopen(argv[1], 0);

						if(chd) {
							node = chd;
							memcpy(path, argv[1], strlen(argv[1]));
							path[strlen(argv[1])] = '\0';
						} else {
							kprintf("cd: couldn't change directory\n");
						}
					}
				}
			} else if(!strcmp(cmd, "cat")) {
				if(tokenid < 2) {
					kprintf("cat: argument expected\n");
					continue;
				} else {
					if(argv[1][0] == '/') {
						fs_node_t * file = kopen(argv[1],0);

						if(!file) {
							kprintf("cat: could not open file `%s`\n", argv[1]);
							continue;
						}

						char *bufferb = malloc(file->length + 200);
						size_t bytes_read = read_fs(file, 0, file->length, (uint8_t *)bufferb);
						size_t i = 0;
						
						for(i = 0; i < bytes_read; ++i) {
							writech(bufferb[i]);
						}
						free(bufferb);
						close_fs(file);

					}
				}
			}
		}
	}
}
