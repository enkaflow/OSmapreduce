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
    int hashVal;
};
typedef struct KeyVal_ *KeyVal;

struct MapArgPtr_
{
    FILE *input;
    SortedListPtr list;
};
typedef struct MapArgPtr_ *MapArgPtr;

struct RedArgPtr_
{
    SortedListPtr *mapLists;
    SortedListPtr list;
    char *key;
    int numMaps;
    int numReds;
};
typedef struct RedArgPtr_ *RedArgPtr;

typedef void* (*Map_Func)(void *);
typedef void* (*Reduce_Func)(void *);

void createMapWorkers(FILE **inputs, SortedListPtr *mapLists, int numMaps, Map_Func map);
void createRedWorkers(SortedListPtr *mapLists, SortedListPtr *redLists, int numMaps, int numReds, Reduce_Func reduce);
void splitInput(char **argv);
void assignFilePtrs(FILE **inputs, int numFiles, char *fileName);
void cleanup(char *fileName, int numFiles, FILE **inputs, SortedListPtr *lists);
void *map_wordcount(void *targs);
void *map_sort(void *targs);
void *reduce_wordcount(void *targs);
void *reduce_sort(void *targs);

char *modifyFileName(char *fileName, int num);
char *itoa(int num);
char *makeLowerCase(char *string);

MapArgPtr createMapArgPtr(FILE *input, SortedListPtr list);
RedArgPtr createRedArgPtr(SortedListPtr *mapLists, SortedListPtr list, char *key, int numMaps, int numReds);
KeyVal createKeyVal(char *key, int value);

int compareStrings(void*currObj, void*newObj);
int hashfn(char * input, int reduce_workers);


#endif
