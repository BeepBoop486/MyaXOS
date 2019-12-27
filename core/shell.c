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
					//TODO: cd
				}
			}
		}
	}
}
