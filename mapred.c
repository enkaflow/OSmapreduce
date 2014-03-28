#include "mapred.h"
#include "tokenizer.h"
void splitInput(char **argv)
{
    /* Description: forks a new process and executes the shell script split.sh to split the input file into one file for each map worker
    ** Modifies: creates new input files in current directory
    ** Returns: nothing
    */
    int pid, status;

    switch(pid = fork())
    {
    case 0:
        execlp("./split.sh", "split.sh", argv[4], argv[2], NULL);
        perror("./split.sh");
        exit(EXIT_FAILURE);
    case -1:
        fprintf(stderr,"\nERROR: fork() failed to create new process");
        perror("fork");
        exit(EXIT_FAILURE);
    default:
        break;
    }
    while((pid = wait(&status)) != -1)
    {
        fprintf(stderr, "process %d: exits with %d\n",pid, WEXITSTATUS(status));
    }
    if(status < 0)
    {
        fprintf(stderr,"\nERROR: command failed");
    }
}
char *itoa(int num)
{
    /* Description: converts and int to a string
    ** Modifies: creates a string
    ** Returns: string representation of integer parameter
    */
	char const digit[] = "0123456789";
	int shifter = num;
	int count = 0;
	do
	{
		shifter = shifter/10;
		count++;
	}while(shifter);
	char *string = malloc(sizeof(char)*(count+1));
	string[count] = '\0';
	char *p = string + count;
	do
	{
		*--p = digit[num%10];
		num = num/10;

	}while(num);
	return string;
}
char *modifyFileName(char *fileName, int num)
{
    /* Description: modifies a file name by concatenating "." and the string representation of num to the input file name
    ** this is used for assigning a file pointer to each split input file
    ** Modifies: creates a new string (does not actually modify fileName)
    ** Returns: concatenated string
    */
    char *snum = itoa(num);
    int length = (strlen(fileName) + strlen(snum) + 1);
    char *string = malloc(sizeof(char)*length + 1);
    int i;
    for(i = 0; i < length; i++)
    {
        if(i < strlen(fileName))
        {
            string[i] = fileName[i];
        }
        else if(i > strlen(fileName))
        {
            string[i] = snum[i - (strlen(fileName)+1)];
        }
        else
        {
            string[i] = '.';
        }
    }
    string[length] = '\0';
    free(snum);
    return string;
}

void assignFilePtrs(FILE **inputs, int numFiles, char *fileName)
{
    /* Description: assigned each file pointer in FILE **inputs array to one of the split input files
    ** Modifies: FILE **inputs array
    ** Returns: nothing
    */
    int i;
    for(i = 0; i < numFiles; i ++)
    {
        char *string = modifyFileName(fileName, i);
        if((inputs[i] = fopen(string, "r")) == NULL)
        {
            fprintf(stderr, "ERROR: failed to open split file");
            exit(EXIT_FAILURE);
        }
        /*printf("\nassinged file pointer:%d to %s\n", i, string);*/
        free(string);

    }
}
char *makeLowerCase(char *string)
{
    int i = 0;
    char *lower = malloc(sizeof(char)*strlen(string));
    while(string[i]!= '\0')
    {
        lower[i] = tolower(string[i]);
        i++;
    }
    return lower;
}
KeyVal createKeyVal(char *key, int value)
{
    /*Desripcion: creates and new KeyVal struct*/
    KeyVal keyVal = malloc(sizeof(struct KeyVal_));
    keyVal->key = key;
    keyVal->value = value;
    return keyVal;
}

void cleanup(char *fileName, int numFiles, FILE **inputs, SortedListPtr *lists)
{
    /* Description: frees allocated memory and removes split input files
    */
    int i;
    for(i = 0; i < numFiles; i++)
    {
        char *string = modifyFileName(fileName, i);
        remove(string);
        free(string);
    }
    for(i = 0; i < numFiles; i++)
    {
        fclose(inputs[i]);
        SortedListIteratorPtr iter = SLCreateIterator(lists[i]);
        KeyVal curr;
        while((curr = (KeyVal)SLNextItem(iter)) != NULL)
        {
            free(curr->key);
            free(curr);
        }
        SLDestroy(lists[i]);
        SLDestroyIterator(iter);
    }
}

int compareStrings(void*currObj, void*newObj)
{
    KeyVal currKeyVal = (KeyVal)currObj;
    KeyVal newKeyVal = (KeyVal)newObj;
    return strcmp(currKeyVal->key, newKeyVal->key);
}

int hashfn(char * input, int reduce_workers)
{
    int hash = 5381;
    int c;

	while ((c = *input++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % reduce_workers;
}
void partition(SortedListPtr *mapLists, int numReds, Hash_Func hash)
{

}

/**************************************Create ArgPtrs*********************************************/
MapArgPtr createMapArgPtr(FILE *input, SortedListPtr list)
{
    MapArgPtr targs = malloc(sizeof(struct MapArgPtr_));
    targs->input = input;
    targs->list = list;
    return targs;
}
RedArgPtr createRedArgPtr(SortedListPtr *mapLists, SortedListPtr list, char *key, int numMaps, int numReds)
{
    RedArgPtr targs = malloc(sizeof(struct RedArgPtr_));
    targs->mapLists = mapLists;
    targs->list = list;
    targs->key = key;
    targs->numMaps = numMaps;
    targs->numReds = numReds;
    return targs;
}
PartArgPtr createPartArgPtr(SortedListPtr list, Hash_Func hash, int numReds)
{
    PartArgPtr targs = malloc(sizeof(struct PartArgPtr));
    targs->list = list;
    targs->hash = hash;
    targs->numReds = numReds;
    return targs;
}
/****************************Create Worker Threads*****************************/
void createMapWorkers(FILE **inputs, SortedListPtr *mapLists, int numMaps, Map_Func map)
{
    CompareFuncT cf = compareStrings;
    pthread_t mapid[numMaps];
    int i, err;
    for(i = 0; i < numMaps; i++)
    {
        mapLists[i] = SLCreate(cf);
        err = pthread_create(&mapid[i], NULL, map, (void*)createMapArgPtr(inputs[i], mapLists[i]));
        if(err != 0)
        {
            fprintf(stderr, "\nERROR: Failed to create map worker thread: %s", strerror(err));
            exit(EXIT_FAILURE);
        }
    }
    void *threadResult;
    for(i = 0; i < numMaps; i++)
    {
        pthread_join(mapid[i], &threadResult);
    }
    free(threadResult);
}

void createRedWorkers(SortedListPtr *mapLists, SortedListPtr *redLists, int numMaps, int numReds, Reduce_Func reduce)
{
    CompareFuncT cf = compareStrings;
    pthread_t redid[numReds];
    int i, err;
    for(i = 0; i < numReds; i++)
    {
        redLists[i] = SLCreate(cf);
        err = pthread_create(&redid[i], NULL, reduce, (void*)createRedArgPtr(mapLists, redLists[i], key, numMaps));
        if(err != 0)
        {
            fprintf(stderr, "\nERROR: Failed to create reduce worker thread: %s", strerror(err));
            exit(EXIT_FAILURE);
        }
    }
    void *threadResult;
    for(i = 0; i < numMaps; i++)
    {
        pthread_join(redid[i], &threadResult);
    }
    free(threadResult);
}
/*****************Map and Reduce Functions************************/
void *map_wordcount(void *targs)
{
    /* Description: reads an inputfile and adds a KeyVal (key, val) for every word to the MapWorker's list
    ** Parameter: void* casted MapArgPtr
    ** Modifies: mapLists[MapWokerId]
    ** Returns: NULL
    */
    MapArgPtr args = (MapArgPtr)targs;
    FILE *input = args->input;
    SortedListPtr list = args->list;
    char line[512];
    char *token;
    char *delims = " !/@#$%^&*()_+-,.;:[]{}<>\\|\'\"\n\r\t\?";
    TokenizerT *tk;

    while(fgets(line, sizeof(line), input))
    {
        tk = TKCreate(delims, line);
        while((token = TKGetNextToken(tk))!= NULL)
        {
            SLInsert(list, (void*)createKeyVal(makeLowerCase(token), 1));
            free(token);
        }
        TKDestroy(tk);
    }
    return targs;
}
void *map_sort(void *targs)
{
    return NULL;
}
void *reduce_wordcount(void *targs)
{
	/*
	 * Description: counts all instances of 'key' in lists, then places result into redLists.
	 * Parameters: map worker outputs (lists[]), reduce worker output structure (redLists[]), key to be reduced (key), and number of map workers aka # of entries in lists[] (numMaps)
	 * Modifies: redLists[threadID] ONLY
	 * Returns: nothing
	 *
	 */
    SortedListPtr *mapLists = ((RedArgPtr)targs)->mapLists;
    SortedListPtr list = ((RedArgPtr)targs)->list;
    char *key = ((RedArgPtr)targs)->key;
    int numMaps = ((RedArgPtr)targs)->numMaps;
    int numReds = ((RedArgPtr)targs)->numReds;
	int i;
	int keyCount=0;
	int hash;
	KeyVal reducedOut;
	SortedListIteratorPtr p;
	KeyVal thisKV;
	hash = hashfn(key, numReds);

	/*FETCH*/
	for(i=0; i<numMaps; i++)
	{ /* go through all map outputs*/
		p = SLCreateIterator(mapLists[i]);
		while((thisKV = (KeyVal)SLNextItem(p)) != NULL)
		{	/*go through each KeyVal pair in each map, compare hashes.*/
			if(thisKV->hashVal == hash)
				keyCount++;
		}
	}

	/*SORTED INSERT*/
	reducedOut = createKeyVal(key, keyCount);

	SLInsert(list, (void*)reducedOut);

	/*CLEAN UP*/
	SLDestroyIterator(p);

	return NULL;
}
void *reduce_sort(void *targs)
{
    return NULL;
}
