#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <time.h>

#include "./libs/filelistGen.h"

#define CMD_LEN 255

void readCommand(char buffer[]) {
  fgets(buffer, CMD_LEN, stdin);
  buffer[strlen(buffer)-1] = '\0'; // overwrite the line feed with null term
}

unsigned long
printList(FileList* rootNode) {
  unsigned long count = 0;
	while(rootNode) {
    count++;
		printf("File : %s \n", rootNode->filePath);
		rootNode = rootNode->next;
	}
  return count;
}

int main (void) {
	char dirPath[CMD_LEN];
	char userResponse[CMD_LEN];
	FileList* rootNode = NULL;
	struct timespec start_time, end_time;
	do {
    printf("Enter dir path: ");
    readCommand(dirPath);
    clock_gettime(CLOCK_REALTIME, &start_time);
    rootNode = GetFileList(dirPath);
    clock_gettime(CLOCK_REALTIME, &end_time);
    if (rootNode == NULL) {
      printf("Invalid path used.\n");
    } else {
      unsigned long count = printList(rootNode);
      float time = (end_time.tv_nsec/1E6) - (start_time.tv_nsec/1E6);
      printf("Iterated %lu files in %f ms\n", count, time);
    }
    printf("Are you done? (y/n) ");
    readCommand(userResponse);
  } while (toupper(userResponse[0]) != 'Y');
  printf("Success!");
	exit(EXIT_SUCCESS);
}
