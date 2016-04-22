#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "./libs/fileIO.h"
#include "./libs/security.h"
#include "./libs/cJSON.h"
#include "./libs/encryption.h"

#define FILENAME "./.users"
#define LINE_LEN 255

void readCommand(char buffer[]) {
  fgets(buffer, CMD_LEN, stdin);
  buffer[strlen(buffer)-1] = '\0'; // overwrite the line feed with null term
}

void fileCrypt(char* filenameIn, char* password) { 
  int fileLen;
  int filenameLen = strlen(filenameIn);
  int passLen = strlen(password); 
  char *fileInputTemp, *fileInput, *fileOutput;
  char *filenameOut;
  
  fileInputTemp = readFile(filenameIn);
  fileLen = strlen(fileInputTemp);
  
  filenameOut = (char*) malloc(strlen(filenameIn) + 6);    
  fileInput  = (char*) malloc(passLen + fileLen + 1); 
  fileOutput = (char*) malloc(passLen + fileLen + 1); 
    
  if (strstr(filenameIn, "crpt" )) {
    printf("Running decryption protocol on %s!\n", filenameIn);
    strcpy(fileInput, fileInputTemp);
    
    XORCrypt(fileInput, fileOutput, password);			
    strcpy(fileInput, fileOutput);	
    subDecodeString(fileInput, fileOutput, 6); 		  		
    strcpy(fileInput, fileOutput);
    transDecodeString(fileInput, fileOutput);
    
    memset(fileInput, 0, passLen+1);
    strncpy(fileInput, fileOutput, passLen);
    if (strcmp(fileInput, password)) {
      printf("This file cannot be decrypted with your password. You do not have authority!\n");
    } else {
      printf("File decrypted. Writing back data!\n");
      memset(filenameOut, 0, filenameLen);
      strncpy(filenameOut, filenameIn, filenameLen-5);
      writeFile(filenameOut, &fileOutput[passLen]);
      remove(filenameIn);
    }
  } else {
    printf("Running Encryption protocol on %s!\n", filenameIn);
    sprintf(fileInput, "%s%s", password, fileInputTemp); 
    
    transEncodeString(fileInput, fileOutput);
    strcpy(fileInput, fileOutput);	  
    subEncodeString(fileInput, fileOutput, 6); 		  
    strcpy(fileInput, fileOutput);	  
    XORCrypt(fileInput, fileOutput, password);
    
    sprintf(filenameOut, "%s.crpt", filenameIn);
    printf("File encrypted. Writing back data!\n");
    writeFile(filenameOut, fileOutput);
    remove(filenameIn);
  }
  free (fileInputTemp);
  free (fileInput);
  free (fileOutput);
}

void main() {
  cJSON *userObject = NULL;
  char *password;
  char *fileInputTemp, *fileInput, *fileOutput;
  char filenameIn[LINE_LEN];
  char filenameOut[LINE_LEN];
  int filenameLen, passLen, fileLen;
  
  userObject = authenticateUser(FILENAME);
  if (!userObject) {
    printf("error\n");
		exit(EXIT_FAILURE); 
  } else {
		printf("Authenticated\n");
  }

	password = cJSON_GetObjectItem(userObject, "password")->valuestring; // password->valuestring
  
  while (1) {
	  printf("Please enter the filename that you wish to run encryption on (.crpt files will decrypt): ");
	  readCommand(filenameIn);
    fileCrypt(filenameIn, password);
  }
  
	exit(EXIT_SUCCESS); 
}