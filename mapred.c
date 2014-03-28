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
    KeyVal keyVal = malloc(sizeof(struct KeyVal_));
    keyVal->key = key;
    keyVal->value = value;
    return keyVal;
}
void *map_wordcount(void *targ)
{
    ArgPtr args = (ArgPtr)targ;
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
    return targ;
}
void *map_sort(void *targ)
{
    return NULL;
}
void *reduce_wordcount(void *targ)
{
    return NULL;
}
void *reduce_sort(void *targ)
{
    return NULL;
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
ArgPtr createArgPtr(FILE *input, SortedListPtr list)
{
    ArgPtr targs = malloc(sizeof(struct ArgPtr_));
    targs->input = input;
    targs->list = list;
    return targs;
}

int compareStrings(void*currObj, void*newObj)
{
    KeyVal currKeyVal = (KeyVal)currObj;
    KeyVal newKeyVal = (KeyVal)newObj;
    return strcmp(currKeyVal->key, newKeyVal->key);
}

int hash(char * input, int reduce_workers) //djb2 hashfn, known to provide good results for strings
{
    int hash = 5381;
    int c;

	while ((c = *input++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % reduce_workers;
}
