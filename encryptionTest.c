#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

#include "./libs/encryption.h"
#include "./libs/fileIO.h"

#define LOGIN_LINE_LEN 255

#define XOR_CIPHER "HYiC5dZC0jiMdeQByHcPXxY7sDATnwyX"

void readCommand(char buffer[]) {
  fgets(buffer, CMD_LEN, stdin);
  buffer[strlen(buffer)-1] = '\0'; // overwrite the line feed with null term
}

void main() {
  char *fileInput = NULL, *fileOutput = NULL;
  char filenameIn[LOGIN_LINE_LEN];
  char filenameOut[LOGIN_LINE_LEN];
  int fileSize;
  
  printf("Input filename to run through encryption test: ");  
  readCommand(filenameIn);
  
	printf("\n\nEncryption\n\n");
  fileInput = readFile(filenameIn);
  fileOutput = malloc(strlen(fileInput) + 1);
  printf("Encr Input: %s\n", fileOutput);
  transEncodeString(fileInput, fileOutput);
  printf("Encr Tran: %s\n", fileOutput);
  strcpy(fileInput, fileOutput);
  subEncodeString(fileInput, fileOutput, 6); 
  printf("Encr Subs: %s\n", fileOutput);
  strcpy(fileInput, fileOutput);
  XORCrypt(fileInput, fileOutput, XOR_CIPHER);
  printf("Encr XORd: %s\n", fileOutput);
  sprintf(filenameOut, "%s.crpt", filenameIn);
  writeFile(filenameOut, fileOutput);
  free(fileInput);
  free(fileOutput);

  printf("\n\nDecryption\n\n");
  fileInput = readFile(filenameOut);
  fileOutput = malloc(strlen(fileInput) + 1);
  
  printf("Decr Input: %s\n", fileInput);
  XORCrypt(fileInput, fileOutput, XOR_CIPHER);
  printf("Decr XORd: %s\n", fileOutput);
  strcpy(fileInput, fileOutput);
  subDecodeString(fileInput, fileOutput, 6);
  printf("Decr Subs: %s\n", fileOutput);
  strcpy(fileInput, fileOutput);
  transDecodeString(fileInput, fileOutput);
  printf("Decr Tran: %s\n", fileOutput);
  
  sprintf(filenameOut, "%s.decrpt", filenameIn);
  writeFile(filenameOut, fileOutput);
  
  free(fileInput);
  free(fileOutput);
  
  printf("\n\nComparing Results\n\n");
  fileInput = readFile(filenameIn);
  fileOutput = readFile(filenameOut);
  if (strcmp(fileInput, fileOutput)) {
    printf("Encryption failed!\n");
  } else {
    printf("Successfully encrypted and decrypted file!\n");
  }
  
  free(fileInput);
  free(fileOutput);
}