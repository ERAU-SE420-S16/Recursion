#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include "FileListGen.h"

void __fileListGen_AddToList(File_List** rootNode, char* fileName) {	
	File_List * tmpNode = (File_List *)malloc(sizeof(File_List));/*allocates an extra node for the list*/
	strcpy((*rootNode) -> filePath, fileName);
	tmpNode -> next = (*rootNode); /*Goes back to the last input on the list*/
	(*rootNode) = tmpNode;
}

void __fileListGen_GetFileListHelper (File_List** rootNode, char* dir) {
    DIR *dfd;
	struct dirent *dp;
	 if (NULL == (dfd = opendir (dir))) 
    {
        fprintf(stderr, "Error : Failed to open input directory\n");

        exit(1);
    }
	if (dfd = opendir(dir)) {
		char filename_qfd[FILELISTGEN_MAX_FILE_PATH_SIZE];
		while ((dp = readdir(dfd)) != NULL) {
			sprintf(filename_qfd , "%s/%s", dir, dp->d_name) ;
			if (strcmp(dp->d_name,"..") && strcmp(dp->d_name,".")) {
				if ( dp->d_type == DT_DIR ) {
				  __fileListGen_GetFileListHelper(rootNode, filename_qfd);
				} else if (dp->d_type == DT_REG) {
				  printf("file: %s\n", filename_qfd);
				  __fileListGen_AddToList(rootNode,filename_qfd);
				}
			}
		}
	}
}

File_List* GetFileList (char* dir) {
    File_List* rootNode =(File_List *)malloc(sizeof(File_List));
	rootNode->next = NULL;
	__fileListGen_GetFileListHelper(&rootNode, dir);
	return rootNode;
}

printList(File_List* rootNode)
{
	while(rootNode)
	{
		printf("File : %s \n", rootNode ->filePath);
		rootNode = rootNode->next;
	}
}

int main (void)
{
	char dirPath[255];
	char userResponse;
	File_List* rootNode = NULL;
	int done = 0;
	do{
	printf("Enter dir path: ");
	scanf(" %s", dirPath);
	
	rootNode = GetFileList(dirPath);
	
	printf("Are you done? (y/n)");
	scanf(" %c", &userResponse);
	
	if(toupper(userResponse)  == 'Y')
	{
		done = 1;
		exit(1);
	}
	while (rootNode)
	{
		File_List* currentNode = rootNode;
		rootNode = rootNode->next;
		free(currentNode);
	}
	
	  }while(!done);
	
	
	return 1;
}
