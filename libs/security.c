#include  <stdbool.h>
#include	<stdio.h>
#include	<stdlib.h>
#include  <sys/ioctl.h>
#include	<sys/types.h>
#include	<sys/wait.h>
#include	<string.h>
#include  <termios.h>
#include	<unistd.h>
#include  <crypt.h>
#include <ctype.h>

#include "security.h"

#include "cJSON.h"
#include "fileIO.h"

#define __SECURITY_CMD_LEN 255

unsigned long seed[2];
char salt[] = "$1$........";
const char *const seedchars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
														  
void __security_readCommand(char buffer[]) {
  fgets(buffer, __SECURITY_CMD_LEN, stdin);
  buffer[strlen(buffer)-1] = '\0';  // overwrite the line feed with null term
}

void __security_generateSalt(void) {
  /* Generate a (not very) random seed.
    You should do it better than this... */
  seed[0] = time(NULL);
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);
  
  /* Turn it into printable characters from ‘seedchars’. */
  int i;
  for (i = 0; i < 8; i++)
  salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];
}

cJSON* __security_loadUserDB(const char* userFile) { 
  char *strUserJson;
  cJSON* data = NULL;
  strUserJson = readFile(userFile);
  if (strlen(strUserJson) > 0) data = cJSON_Parse(strUserJson);
  if (!data) data = cJSON_CreateObject();
	return data;
}

bool __security_saveUserDB(const char* userFile, cJSON* userDatabase) {
  char *strUserJson = cJSON_Print(userDatabase);  
  return writeFile(strUserJson, strUserJson);
}

cJSON* authenticateUser(const char* userFile) {
	cJSON *userDatabase = NULL;
	cJSON *userObject   = NULL;
	cJSON *userRetObj   = NULL;
	cJSON *passwordObj  = NULL;
  bool userValid = false;
  int loginAttempts = 0;
  char commandLine[__SECURITY_CMD_LEN];
	char username[__SECURITY_CMD_LEN];
	char password[__SECURITY_CMD_LEN];
  
  userDatabase = __security_loadUserDB(userFile);
  if (!userDatabase) {
    return NULL;
  }
  printf("Please type a username: ");
  __security_readCommand(username);
  userObject = cJSON_GetObjectItem(userDatabase, username);
  if (userObject) {
    printf("User Exists!\nPlease enter your password: ");
    passwordObj = cJSON_GetObjectItem(userObject, "password");
    while (true) {
      __security_readCommand(commandLine);
      strcpy(password, commandLine);
      if (!strcmp(crypt(commandLine, passwordObj->valuestring), passwordObj->valuestring)) {
        userValid = true;
        break;
      } else if (loginAttempts < 2) {
        printf("Wrong password! %d attempt(s) left.\nPlease enter your password: ", 1 - loginAttempts);
      } else {
        printf("Wrong password!\nDo you want to reset your password (y/n)? ");
        __security_readCommand(commandLine);
        if (toupper(commandLine[0]) == 'Y') {
          cJSON *recovery = cJSON_GetObjectItem(userObject, "recovery");
          if (recovery && cJSON_GetArraySize(recovery) > 0) {          
            int promptNum;
            for (promptNum = 0; promptNum < cJSON_GetArraySize(recovery); promptNum++) {        
              cJSON *prompt = cJSON_GetArrayItem(recovery, promptNum);
              char *question = cJSON_GetObjectItem(prompt, "question")->valuestring;
              char *answer = cJSON_GetObjectItem(prompt, "answer")->valuestring;
              printf("Question %d: %s\nAnswer: ", promptNum+1, question);
              __security_readCommand(commandLine);
              if (strcmp(answer, crypt(commandLine, answer))) {
                userValid = false;
                break;
              }
              userValid = true;
            }
            if (userValid) {
							char commandLine[__SECURITY_CMD_LEN];
						  printf("Please type a new password: ");
						  __security_readCommand(commandLine);
							__security_generateSalt();
						  passwordObj = cJSON_CreateString(crypt(commandLine, salt));
						  cJSON_ReplaceItemInObject(userObject, "password", passwordObj);
						  __security_saveUserDB(userFile, userDatabase);
            } else {
              printf("User recovery failed.\n");
            }
          } else {
            printf("User has no method of recovering password.\n");
          }
        }
        break;
      }	    
      loginAttempts++;
    }
  } else {
    userObject = cJSON_CreateObject();
    printf("User not found.\nCreating new User.\nPlease type a password: ");
    __security_readCommand(commandLine);
    
    __security_generateSalt();
    cJSON_AddStringToObject(userObject, "password", crypt(commandLine, salt)); 
    
    cJSON *recovery = cJSON_CreateArray();
    cJSON *prompt1  = cJSON_CreateObject();
    cJSON *prompt2  = cJSON_CreateObject();
    cJSON *prompt3  = cJSON_CreateObject();
    
    printf("Please enter password recovery question 1: ");
    __security_readCommand(commandLine);
    cJSON_AddStringToObject(prompt1, "question", commandLine);    
    printf("Please enter password recovery response 1: ");
    __security_readCommand(commandLine);
    __security_generateSalt();
    cJSON_AddStringToObject(prompt1, "answer", crypt(commandLine, salt));    
    cJSON_AddItemToArray(recovery, prompt1);
    
    printf("Please enter password recovery question 2: ");    
    __security_readCommand(commandLine);
    cJSON_AddStringToObject(prompt2, "question", commandLine);    
    printf("Please enter password recovery response 2: ");
    __security_readCommand(commandLine);
    __security_generateSalt();
    cJSON_AddStringToObject(prompt2, "answer", crypt(commandLine, salt));    
    cJSON_AddItemToArray(recovery, prompt2);
    
    printf("Please enter password recovery question 3: ");    
    __security_readCommand(commandLine);
    cJSON_AddStringToObject(prompt3, "question", commandLine);    
    printf("Please enter password recovery response 3: ");
    __security_readCommand(commandLine);
    __security_generateSalt();
    cJSON_AddStringToObject(prompt3, "answer", crypt(commandLine, salt));    
    cJSON_AddItemToArray(recovery, prompt3);
    
    cJSON_AddItemToObject(userObject, "recovery", recovery);    
    cJSON_AddItemToObject(userDatabase, username, userObject);
    
    userValid = true;
  }
	__security_saveUserDB(userFile, userDatabase);
	if (userValid) {
		userRetObj = cJSON_CreateObject();
		cJSON_AddStringToObject(userRetObj, "username", username);
		cJSON_AddStringToObject(userRetObj, "password", password);
		cJSON_AddItemToObject(userRetObj, "data", cJSON_Duplicate(userObject, 1));
  }
	cJSON_Delete(userDatabase);
  return userRetObj;
}

void updateUser(const char* userFile, cJSON* userObject) {
  cJSON *userDatabase = NULL;
  if (!userObject) return;
  cJSON* userCopy = cJSON_Duplicate(userObject, 1);
  userDatabase = __security_loadUserDB(userFile);
  cJSON *username = cJSON_GetObjectItem(userCopy, "username");
  cJSON *data     = cJSON_GetObjectItem(userCopy, "data");
  cJSON_ReplaceItemInObject(userDatabase, username->valuestring, data);
	__security_saveUserDB(userFile, userDatabase);
	cJSON_Delete(userDatabase);
}