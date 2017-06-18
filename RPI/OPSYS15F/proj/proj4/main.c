#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "vmemory.h"

/* functions */
int cleandir(const char *, int);
int find(char **, const char *, const int);

/* server constants */
#define BUFFER_SIZE 1024
#define SERVER_PORT 8765
#define MAX_CLIENTS 5

int main() {
	/* simulation variables */
	const int blocksize = 4096;
	const int n_blocks  = 128;
	const char storepath[9] = ".storage";

	/* Create the listener socket as TCP socket */
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	/* socket structures */
	struct sockaddr_in server;
	struct sockaddr_in client;

	server.sin_family = PF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(SERVER_PORT);

	int len = sizeof(server);
	int fromlen = sizeof(client);

	if(bind(sock, (struct sockaddr *)&server, len) < 0) {
		perror("bind() failed");
		exit(EXIT_FAILURE);
	}

	/* create/clean storage folder */
	struct stat st = {0};
	if(stat(storepath, &st) == -1) {
		mkdir(storepath, 0700);
	}
	else {
		cleandir(storepath, 1);
	}

	/* start up the vm */
	v_memory vm = vm_new(n_blocks, 'f');

	/* file trackers */
	int cur_idx = 0;
	char cur_id = 'A';
	char *ids = (char *)malloc(n_blocks * sizeof(char));
	char **files = (char **)malloc(n_blocks * sizeof(char *));

	/* all systems go! */
	listen(sock, MAX_CLIENTS);
	printf("Block size is %d\n", blocksize);
	printf("Number of blocks is %d\n", n_blocks);
	printf("Listening on port %d\n", SERVER_PORT);

	int pid;
	char buffer[BUFFER_SIZE];

	while (1) {
		int newsock = accept(sock, (struct sockaddr *)&client, (socklen_t*)&fromlen);
		printf("Received incoming connection from %s\n", inet_ntoa(client.sin_addr));

		/* handle socket in a child process,
			 allowing the parent process to immediately go
			 back to the accept() call */
		pid = fork();

		if (pid < 0) {
			perror("fork() failed");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0) {
			int n;

			do {
				n = recv(newsock, buffer, BUFFER_SIZE, 0);

				if (n < 0) perror("recv() failed");

				else if(n != 0) {
					/* receive commands */
					buffer[n] = '\0';
					char *cmd = NULL;
					sscanf(buffer, "%ms", &cmd);

					if(cmd != NULL) {
						printf("[process %d] Rcvd: %s", getpid(), buffer);

						/* === STORE === */
						if(strcmp(cmd, "STORE") == 0) {
							char *filename = NULL;
							int bytes = 0;
							sscanf(buffer, "STORE %ms %d", &filename, &bytes);

							/* get full relative path to new file */
							int len = strlen(storepath) + strlen(filename) + 2;
							char filepath[len];
							snprintf(filepath, len, "%s/%s", storepath, filename);

							/* handle arguments */
							if(filename == NULL || bytes == 0) {
								printf("[process %d] Sent: ERROR: TOO LITTLE ARGUMENTS\n", getpid());
								int nbytes = send(newsock, "ERROR: TOO LITTLE ARGUMENTS\n", 28, 0);
								fflush(NULL);
								if(nbytes != 28) perror("send() failed");
							}
							else if(bytes < 1) {
								printf("[process %d] Sent: ERROR: INVALID FILE SIZE\n", getpid());
								int nbytes = send(newsock, "ERROR: INVALID FILE SIZE\n", 25, 0);
								fflush(NULL);
								if(nbytes != 25) perror("send() failed");
							}
							else if(access(filepath, F_OK) != -1) {
								printf("[process %d] Sent: ERROR: FILE EXISTS\n", getpid());
								int nbytes = send(newsock, "ERROR: FILE EXISTS\n", 19, 0);
								fflush(NULL);
								if(nbytes != 19) perror("send() failed");
							}
							else {
								/* receive file contents */
								char *filebuf = (char *)calloc(bytes, sizeof(char));
								recv(newsock, filebuf, bytes, 0);


								/* vm calculations */
								int nblocks, nclusters;
								nblocks = (bytes + blocksize - 1) / blocksize;
								nclusters = vm_add(&vm, bytes, blocksize);

								if(nclusters != 0) {
									/* write to file */
									FILE *fp;
									if((fp = fopen(filepath, "w")) == NULL) perror("fopen() failed");
									ftruncate(fileno(fp), bytes);
									rewind(fp);
									fprintf(fp, "%s", filebuf);
									fclose(fp);

									/* track file */
									ids[cur_idx] = cur_id++;
									if((int)cur_id > 90) cur_id = 'A'; // reset after Z
									files[cur_idx] = (char *)malloc((strlen(filename) + 1) * sizeof(char));
									strcpy(files[cur_idx], filename);
									++cur_idx;

									/* output */
									char plural1[2] = { 0 };
									char plural2[2] = { 0 };
									char plural3[2] = { 0 };
									if(bytes > 1) plural1[0] = 's';
									if(nblocks > 1) plural2[0] = 's';
									if(nclusters > 1) plural3[0] = 's';
									printf("[process %d] Stored file '%c' (%d byte%s; %d block%s; %d cluster%s)\n", getpid(), vm.id - 1, bytes, plural1,  nblocks, plural2, nclusters, plural3);
									printf("[process %d] Simulated Clustered Disk Space Allocation\n", getpid());
									vm_print(vm);
									int nbytes = send(newsock, "ACK\n", 4, 0);
									fflush(NULL);
									if(nbytes != 4) perror("send() failed");
								}
								else {
									/* error handling */
									printf("[process %d] Sent: ERROR: INSUFFICIENT DISK SPACE\n", getpid());
									int nbytes = send(newsock, "ERROR: INSUFFICIENT DISK SPACE\n", 31, 0);
									fflush(NULL);
									if(nbytes != 31) perror("send() failed");
								}

								free(filebuf);
							}

							/* cleanup */
							if(filename != NULL) free(filename);
						}
						/* === READ === */
						else if(strcmp(cmd, "READ") == 0) {
							char *fpath = NULL;
							int offset = -1, length = -1;
							sscanf(buffer, "READ %ms %d %d", &fpath, &offset, &length);

							/* handle arguments */
							if(fpath == NULL || offset == -1 || length == -1) {
								printf("[process %d] Sent: ERROR: TOO LITTLE ARGUMENTS\n", getpid());
								int nbytes = send(newsock, "ERROR: TOO LITTLE ARGUMENTS\n", 28, 0);
								fflush(NULL);
								if(nbytes != 28) perror("send() failed");
							}
							else if(offset < 0 || length < 1) {
								printf("[process %d] Sent: ERROR: INVALID BYTE RANGE\n", getpid());
								int nbytes = send(newsock, "ERROR: INVALID BYTE RANGE\n", 27, 0);
								fflush(NULL);
								if(nbytes != 27) perror("send() failed");
							}
							else {
								int len = strlen(storepath) + strlen(fpath) + 2;
								char filepath[len];
								snprintf(filepath, len, "%s/%s", storepath, fpath);

								/* check existence */
								/* note: suffers from TOCTOU (time of check, time of use) race condition */
								if(access(filepath, F_OK) != -1) {
									FILE *fp;
									int filesize;
									if((fp = fopen(filepath, "rb")) == NULL) perror("fopen failed");
									fseek(fp, 0L, SEEK_END);
									filesize = ftell(fp);

									/* check length and offset against file */
									if(offset >= filesize || length > filesize) {
										printf("[process %d] Sent: ERROR: INVALID BYTE RANGE\n", getpid());
										int nbytes = send(newsock, "ERROR: INVALID BYTE RANGE\n", 27, 0);
										fflush(NULL);
										if(nbytes != 27) perror("send() failed");
									}
									else {
										/* read file */
										char *buf = (char *)calloc(length + 1, sizeof(char));
										fseek(fp, offset, SEEK_SET);
										fread(buf, length, 1, fp);
										buf[length] = 0;

										/* calculate vm info */
										int idx = find(files, fpath, cur_idx);
										int nblocks = 1, offset_rem, rdbytes;
										rdbytes = length; // store length
										offset_rem = offset % blocksize; // get the offset within a block
										while((offset_rem + rdbytes) > blocksize) {
											/* basically, cut down on rdbytes to calculate blocks */
											++nblocks;
											rdbytes -= blocksize;
										}

										/* output */
										char plural1[2] = { 0 };
										char plural2[2] = { 0 };
										if(length > 1) plural1[0] = 's';
										if(nblocks > 1) plural2[0] = 's';
										printf("[process %d] Sent: ACK %d\n", getpid(), length);
										printf("[process %d] Sent: %d byte%s (from %d '%c' block%s) from offset %d\n", getpid(), length, plural1, nblocks, ids[idx], plural2, offset);
										int nbytes = send(newsock, buf, length, 0);
										fflush(NULL);
										if(nbytes != length) perror("send() failed");
										free(buf);
									}

									fclose(fp);
								}
								else {
									printf("[process %d] Sent: ERROR: NO SUCH FILE\n", getpid());
									int nbytes = send(newsock, "ERROR: NO SUCH FILE\n", 20, 0);
									fflush(NULL);
									if(nbytes != 20) perror("send() failed");
								}
							}
						}

						/* === DELETE === */
						else if(strcmp(cmd, "DELETE") == 0) {
							char *fpath = NULL;
							sscanf(buffer, "DELETE %ms", &fpath);

							/* handle arguments */
							if(fpath == NULL) {
								printf("[process %d] Sent: ERROR: TOO LITTLE ARGUMENTS\n", getpid());
								int nbytes = send(newsock, "ERROR: TOO LITTLE ARGUMENTS\n", 28, 0);
								fflush(NULL);
								if(nbytes != 28) perror("send() failed");
							}
							else {
								int len = strlen(storepath) + strlen(fpath) + 2;
								char filepath[len];
								snprintf(filepath, len, "%s/%s", storepath, fpath);

								/* check existence */
								/* note: suffers from TOCTOU (time of check, time of use) race condition */
								if(access(filepath, F_OK) != -1) {
									unlink(filepath);

									/* calculate vm info */
									int idx = find(files, fpath, cur_idx);
									int blocks = vm_del(&vm, ids[idx]);

									/* output */
									char plural[2] = { 0 };
									if(blocks > 1) plural[0] = 's';
									printf("[process %d] Deleted %s file '%c' (deallocated %d block%s)\n", getpid(), fpath, ids[idx], blocks, plural);
									printf("[process %d] Simulated Clustered Disk Space Allocation\n", getpid());
									vm_print(vm);
									printf("[process %d] Sent: ACK\n", getpid());
									int nbytes = send(newsock, "ACK\n", 4, 0);
									fflush(NULL);
									if(nbytes != 4) perror("send() failed");
								}
								else {
									/* handle errors */
									printf("[process %d] Sent: ERROR: NO SUCH FILE\n", getpid());
									int nbytes = send(newsock, "ERROR: NO SUCH FILE\n", 20, 0);
									fflush(NULL);
									if(nbytes != 20) perror("send() failed");
								}

								/* cleanup */
								free(fpath);
							}
						}

						/* === DIR === */
						else if(strcmp(cmd, "DIR") == 0) {
							struct dirent **list;
							int i, n_files = 0;

							/* calculate number of files */
							n_files = scandir(storepath, &list, 0, alphasort);
							if(n < 0) perror("scandir() failed");
							else {
								char plural[2] = { 0 };
								if(n_files-2 > 1) plural[0] = 's';
								printf("[process %d] Sent: %d filename%s\n", getpid(), n_files-2, plural);

								/* scan and send to client */
								char tmp[10];
								sprintf(tmp, "%d\n", n_files-2);
								int nbytes = send(newsock, tmp, strlen(tmp), 0);
								fflush(NULL);
								if(nbytes != strlen(tmp)) perror("send() failed");

								for(i = 0; i < n_files; ++i) {
									if(!(!strcmp(list[i]->d_name, ".") || !strcmp(list[i]->d_name, ".."))) {
										int len = strlen(list[i]->d_name) + 2; // make room for \n\0
										char buf[len];
										strcpy(buf, list[i]->d_name);
										strcat(buf, "\n");
										nbytes = send(newsock, buf, len, 0);
										fflush(NULL);
										if(nbytes != len) perror("send() failed");
									}

									free(list[i]);
								}

								free(list);
							}
						}
						else {
							printf("[process %d] Sent: ERROR: COMMAND NOT FOUND\n", getpid());
							n = send(newsock, "ERROR: COMMAND NOT FOUND\n", 25, 0);
							fflush(NULL);
							if(n != 25) perror("send() failed");
						}

						/* cleanup */
						free(cmd);
					}
				}
			} while (n > 0);

			printf("[process %d] Client closed its socket....terminating\n", getpid());

			close(newsock);
			exit(EXIT_SUCCESS);
		} /* child terminates here! */
		else {
			/* parent simply closes the socket (endpoint) */
			close(newsock);
		}
	}

	/* cleanup */
	int i;
	for(i = 0; i < cur_idx; ++i) {
		free(files[i]);
	}
	free(ids);
	free(files);
	vm_free(&vm);
	close(sock);

	return EXIT_SUCCESS;
}

/*
 * cleandir()
 * recursively delete all files/folders in a given folder
 * del = true: do not delete containing folder
 * del = false: delete containing folder
 */
int cleandir(const char *path, int del) {
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = -1;
	if(d) {
		struct dirent *p;
		r = 0;
		while(!r && (p=readdir(d))) {
			int r2 = -1;
			char *buf;
			size_t len;
			if(!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;
			len = path_len + strlen(p->d_name) + 2;
			buf = (char *)malloc(len);
			if(buf) {
				struct stat st;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if(!stat(buf, &st)) {
					if(S_ISDIR(st.st_mode)) r2 = cleandir(buf, del & 0);
					else r2 = unlink(buf);
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if(!r && !del) r = rmdir(path);
	return r;
}

/*
 * find()
 * returns index of a string in an array
 */
int find(char **array, const char *word, const int idx) {
	int i;
	for(i = 0; i < idx; ++i) {
		if(!strcmp(array[i], word)) return i;
	}
	return -1;
}
