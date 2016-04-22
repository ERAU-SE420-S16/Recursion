#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "./libs/fileIO.h"
#include "./libs/security.h"
#include "./libs/cJSON.h"

#define FILENAME "./.users"
#define LINE_LEN 255

#define CMD_LEN 255

void readCommand(char buffer[]) {
  fgets(buffer, CMD_LEN, stdin);
  buffer[strlen(buffer)-1] = '\0'; // overwrite the line feed with null term
}

void main() {
  cJSON *userObject = NULL;
  char cmd[LINE_LEN];
  do {
    userObject = authenticateUser(FILENAME);
    if (!userObject) {
      printf("Authentication Error!\n");
    } else {
      printf("User authenticated!\n");
      cJSON_Delete(userObject);
    }
    printf("Retest? (y/n) ");
    readCommand(cmd);
  } while (toupper(cmd[0]) == 'Y');
	exit(EXIT_SUCCESS); 
}