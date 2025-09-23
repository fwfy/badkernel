#include <stdio.h>
#include <sys/mount.h>
#include <unistd.h>

int main() {
    // Boilerplate to set up stdin, stderr, and stdout.
	int onefd = open("/dev/console", O_RDONLY, 0);
	dup2(onefd, 0);
	int twofd = open("/dev/console", O_RDWR, 0);
	dup2(twofd, 1);
	dup2(twofd, 2);
	if (onefd > 2) close(onefd);
	if (twofd > 2) close(twofd);
    printf("init: We have set up stdin, stderr, and stdout! Hello, world!\n");
    
    // And some more boilerplate to ensure we have /proc mounted
    printf("init: Now going to mount /proc...\n");
	int result = mount("none", "/proc", "proc", MS_SILENT, NULL);
	if (result != 0) {
        printf("init: FAILED TO MOUNT /proc!!! We getting the fuck out of here now.\n");
        return 1;
    } else {
        printf("init: /proc is mounted! Now launching main executable!");
        char *const argv[] = {"apple"};
        char *const envp[] = {0};
        execve("/sbin/apple", argv, envp);
    }
	return 0;
}