/**
 * Kevin Zhang (zhangk6)
 * Homework 2
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("ERROR: Invalid arguments\n");
		printf("USAGE: %s <input-file> <chunk-size>\n", argv[0]);
	}

	else {
		// open file
		int fd = open(argv[1], O_RDONLY);

		// handle errors and exit
		if(fd == -1) {
			perror("open() failed");
			return EXIT_FAILURE;
		}

		// get file size
		struct stat stats;
		if(lstat(argv[1], &stats)) {
			perror("lstat() failed");
			return EXIT_FAILURE;
		}

		off_t sz = stats.st_size;
		printf("PARENT: File '%s' contains %lld bytes\n", argv[1], (long long) sz);

		// calculate split...
		int buf = atoi(argv[2]);
		int split = ((int) sz + buf - 1) / buf;

		printf("PARENT: ... and will be processed via %d child processes\n", split);

		// ... and begin forking!
		int i;
		for(i = 0; i < split; ++i) {
			fflush(NULL);
			int pid = fork();

			// failure
			if(pid == -1) {
				perror("fork() failed");
				return EXIT_FAILURE;
			}

			// child fork
			else if(pid == 0) {
				printf("CHILD %d CHUNK: ", getpid());

				// read file
				char buffer[buf];
				int rc = read( fd, buffer, buf );

				if (rc == -1) {
					perror("read() failed");
					return EXIT_FAILURE;
				}

				buffer[rc] = '\0';
				printf("%s\n", buffer);

				return EXIT_SUCCESS;
			}

			// parent
			else {
				// wait for the child process to terminate
				int status;
				pid_t child_pid = wait(&status);

				// abnormal termination
				if(WIFSIGNALED(status)) {
					printf("PARENT: child %d terminated abnormally\n", (int) child_pid);
				}

				// other return code
				else if(WIFEXITED(status)) {
					int rc = WEXITSTATUS(status);
					if(rc != 0) printf("PARENT: child %d exited terminated with nonzero exit status %d\n", (int) child_pid, rc);
				}
			}
		}
	}

	return EXIT_SUCCESS;
}