//? septumfunk 2024
#include "files.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

char **win_lsdir(const char *path, int *count) {
    WIN32_FIND_DATA file;
    HANDLE find = NULL;
    char fpath[2048];
    sprintf(fpath, "%s/*", path);

    char **array = NULL;

    if((find = FindFirstFile(fpath, &file)) == INVALID_HANDLE_VALUE)
        return NULL;

    while(FindNextFile(find, &file)) {
        if(strcmp(file.cFileName, ".") != 0 && strcmp(file.cFileName, "..") != 0 && file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            array = realloc(array, ++*count * sizeof(char *));
            array[*count-1] = calloc(1, strlen(file.cFileName) + 1);
            strcpy(array[*count-1], file.cFileName);
        }
    }

    FindClose(find);

    return array;
}
