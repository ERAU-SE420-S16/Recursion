#ifndef __FILELISTGEN__
#define __FILELISTGEN__

#define FILELISTGEN_MAX_FILE_PATH_SIZE 255

typedef struct fileList
{
	char filePath[FILELISTGEN_MAX_FILE_PATH_SIZE];
	struct fileList *next;
} File_List;//My structure for the linked list that will be used in this program

File_List* GetFileList (char* dir);

#endif