#ifndef MAPRED_H
#define MAPRED_H
#include "sorted-list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <ctype.h>
#include <sys/wait.h>
#include <math.h>

struct KeyVal_
{
    char *key;
    int value;
    int hash;
};
typedef struct KeyVal_ *KeyVal;

struct ArgPtr_
{
    FILE *input;
    SortedListPtr list;
};
typedef struct ArgPtr_ *ArgPtr;

typedef void* (*Map_Func)(void *);
typedef void* (*Reduce_Func)(void *);
void splitInput(char **argv);
void assignFilePtrs(FILE **inputs, int numFiles, char *fileName);
void cleanup(char *fileName, int numFiles, FILE **inputs, SortedListPtr *lists);
void *map_wordcount(void *targ);
void *map_sort(void *targ);
void *reduce_wordcount(void *targ);
void *reduce_sort(void *targ);

char *modifyFileName(char *fileName, int num);
char *itoa(int num);
char *makeLowerCase(char *string);

ArgPtr createArgPtr(FILE *input, SortedListPtr list);

KeyVal createKeyVal(char *key, int value);

int compareStrings(void*currObj, void*newObj);
int hash(char * input, int reduce_workers);


#endif
